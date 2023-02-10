/*
 * Copyright (c) 2015 Cossack Labs Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "soter/soter_ec_key.h"

#include <string.h>

#include <openssl/bn.h>
#include <openssl/ec.h>
#include <openssl/evp.h>
#ifdef THEMIS_EXPERIMENTAL_OPENSSL_3_SUPPORT
#include <openssl/core_names.h>
#endif

#include "soter/soter_portable_endian.h"

static bool is_curve_supported(int curve)
{
    switch (curve) {
    case NID_X9_62_prime256v1:
    case NID_secp384r1:
    case NID_secp521r1:
        return true;
    default:
        return false;
    }
}

static bool is_curve_supported_2(const char* curve)
{
    if (strcmp(curve, "prime256v1" /* P-256 */) == 0) {
        return true;
    } else if (strcmp(curve, "secp384r1" /* P-384 */) == 0) {
        return true;
    } else if (strcmp(curve, "secp521r1" /* P-521 */) == 0) {
        return true;
    } else {
        return false;
    }
}

/* Input size directly since public key type structures may be aligned to word boundary */
static size_t ec_pub_key_size(int curve, bool compressed)
{
    switch (curve) {
    case NID_X9_62_prime256v1: /* P-256 */
        return sizeof(soter_container_hdr_t)
               + (compressed ? EC_PUB_SIZE(256) : EC_PUB_UNCOMPRESSED_SIZE(256));
    case NID_secp384r1: /* P-384 */
        return sizeof(soter_container_hdr_t)
               + (compressed ? EC_PUB_SIZE(384) : EC_PUB_UNCOMPRESSED_SIZE(384));
    case NID_secp521r1: /* P-521 */
        return sizeof(soter_container_hdr_t)
               + (compressed ? EC_PUB_SIZE(521) : EC_PUB_UNCOMPRESSED_SIZE(521));
    default:
        return 0;
    }
}

/* Input size directly since public key type structures may be aligned to word boundary */
static size_t ec_pub_key_size_2(const char* curve, bool compressed)
{
    if (strcmp(curve, "prime256v1" /* P-256 */) == 0) {
        return sizeof(soter_container_hdr_t)
               + (compressed ? EC_PUB_SIZE(256) : EC_PUB_UNCOMPRESSED_SIZE(256));
    } else if (strcmp(curve, "secp384r1" /* P-384 */) == 0) {
        return sizeof(soter_container_hdr_t)
               + (compressed ? EC_PUB_SIZE(384) : EC_PUB_UNCOMPRESSED_SIZE(384));
    } else if (strcmp(curve, "secp521r1" /* P-521 */) == 0) {
        return sizeof(soter_container_hdr_t)
               + (compressed ? EC_PUB_SIZE(521) : EC_PUB_UNCOMPRESSED_SIZE(521));
    } else {
        return 0;
    }
}

static size_t ec_priv_key_size(int curve)
{
    switch (curve) {
    case NID_X9_62_prime256v1: /* P-256 */
        return sizeof(soter_ec_priv_key_256_t);
    case NID_secp384r1: /* P-384 */
        return sizeof(soter_ec_priv_key_384_t);
    case NID_secp521r1: /* P-521 */
        return sizeof(soter_ec_priv_key_521_t);
    default:
        return 0;
    }
}

#ifndef THEMIS_EXPERIMENTAL_OPENSSL_3_SUPPORT
static char* ec_pub_key_tag(int curve)
{
    switch (curve) {
    case NID_X9_62_prime256v1: /* P-256 */
        return EC_PUB_KEY_TAG(256);
    case NID_secp384r1: /* P-384 */
        return EC_PUB_KEY_TAG(384);
    case NID_secp521r1: /* P-521 */
        return EC_PUB_KEY_TAG(521);
    default:
        return NULL;
    }
}
#else
static char* ec_pub_key_tag(const char* curve)
{
    if (strcmp(curve, "prime256v1" /* P-256 */) == 0) {
        return EC_PUB_KEY_TAG(256);
    } else if (strcmp(curve, "secp384r1" /* P-384 */) == 0) {
        return EC_PUB_KEY_TAG(384);
    } else if (strcmp(curve, "secp521r1" /* P-521 */) == 0) {
        return EC_PUB_KEY_TAG(521);
    } else {
        return NULL;
    }
}
#endif

#ifndef THEMIS_EXPERIMENTAL_OPENSSL_3_SUPPORT
static char* ec_priv_key_tag(int curve)
{
    switch (curve) {
    case NID_X9_62_prime256v1: /* P-256 */
        return EC_PRIV_KEY_TAG(256);
    case NID_secp384r1: /* P-384 */
        return EC_PRIV_KEY_TAG(384);
    case NID_secp521r1: /* P-521 */
        return EC_PRIV_KEY_TAG(521);
    default:
        return NULL;
    }
}
#else
static char* ec_priv_key_tag(const char* curve)
{
    if (strcmp(curve, "prime256v1" /* P-256 */) == 0) {
        return EC_PRIV_KEY_TAG(256);
    } else if (strcmp(curve, "secp384r1" /* P-384 */) == 0) {
        return EC_PRIV_KEY_TAG(384);
    } else if (strcmp(curve, "secp521r1" /* P-521 */) == 0) {
        return EC_PRIV_KEY_TAG(521);
    } else {
        return NULL;
    }
}
#endif

static size_t bn_encode(const BIGNUM* bn, uint8_t* buffer, size_t length)
{
    int bn_size = BN_num_bytes(bn);
    if (length < (size_t)bn_size) {
        return 0;
    }
    memset(buffer, 0, length - bn_size);
    return (length - bn_size) + BN_bn2bin(bn, buffer + (length - bn_size));
}

soter_status_t soter_engine_specific_to_ec_pub_key(const soter_engine_specific_ec_key_t* engine_key,
                                                   bool compressed,
                                                   soter_container_hdr_t* key,
                                                   size_t* key_length)
{
    EVP_PKEY* pkey = (EVP_PKEY*)engine_key;
    soter_status_t res;
    size_t output_length;
#ifndef THEMIS_EXPERIMENTAL_OPENSSL_3_SUPPORT
    EC_KEY* ec;
    const EC_GROUP* group;
    const EC_POINT* Q;
    int curve;
#else
    char group_str[16];
    const char* param_name;
    size_t serialized_len;
#endif

    if ((!key_length) || (EVP_PKEY_EC != EVP_PKEY_id(pkey))) {
        return SOTER_INVALID_PARAMETER;
    }

#ifndef THEMIS_EXPERIMENTAL_OPENSSL_3_SUPPORT
    ec = EVP_PKEY_get1_EC_KEY((EVP_PKEY*)pkey);
    if (NULL == ec) {
        return SOTER_FAIL;
    }

    // Curve identifier as EC_GROUP*
    group = EC_KEY_get0_group(ec);
    if (NULL == group) {
        res = SOTER_INVALID_PARAMETER;
        goto err;
    }

    // Curve identifier as int (NID_X9_62_prime256v1, NID_secp384r1, NID_secp521r1 etc)
    curve = EC_GROUP_get_curve_name(group);
    if (!is_curve_supported(curve)) {
        res = SOTER_INVALID_PARAMETER;
        goto err;
    }

    output_length = ec_pub_key_size(curve, compressed);
#else
    // Curve identifier as string ("prime256v1", "secp384r1", "secp521r1" etc)
    if (!EVP_PKEY_get_utf8_string_param(pkey,
                                        OSSL_PKEY_PARAM_GROUP_NAME /* "group" */,
                                        group_str,
                                        sizeof(group_str),
                                        NULL)) {
        res = SOTER_INVALID_PARAMETER;
        goto err;
    }

    if (!is_curve_supported_2(group_str)) {
        res = SOTER_INVALID_PARAMETER;
        goto err;
    }

    output_length = ec_pub_key_size_2(group_str, compressed);
#endif
    if ((!key) || (output_length > *key_length)) {
        *key_length = output_length;
        res = SOTER_BUFFER_TOO_SMALL;
        goto err;
    }

    *key_length = output_length;

#ifndef THEMIS_EXPERIMENTAL_OPENSSL_3_SUPPORT
    Q = EC_KEY_get0_public_key(ec);
    if (NULL == Q) {
        res = SOTER_INVALID_PARAMETER;
        goto err;
    }

    if ((output_length - sizeof(soter_container_hdr_t))
        != EC_POINT_point2oct(group,
                              Q,
                              compressed ? POINT_CONVERSION_COMPRESSED : POINT_CONVERSION_UNCOMPRESSED,
                              (unsigned char*)(key + 1),
                              output_length - sizeof(soter_container_hdr_t),
                              NULL)) {
        res = SOTER_FAIL;
        goto err;
    }
#else
    if (compressed) {
        param_name = OSSL_PKEY_PARAM_PUB_KEY; // "pub"
    } else {
        param_name = OSSL_PKEY_PARAM_ENCODED_PUBLIC_KEY; // "encoded-pub-key"
    }

    if (!EVP_PKEY_get_octet_string_param(pkey,
                                         param_name,
                                         (unsigned char*)(key + 1),
                                         output_length - sizeof(soter_container_hdr_t),
                                         &serialized_len)) {
        res = SOTER_INVALID_PARAMETER;
        goto err;
    }

    if (serialized_len != output_length - sizeof(soter_container_hdr_t)) {
        res = SOTER_INVALID_PARAMETER;
        goto err;
    }
#endif

#ifndef THEMIS_EXPERIMENTAL_OPENSSL_3_SUPPORT
    memcpy(key->tag, ec_pub_key_tag(curve), SOTER_CONTAINER_TAG_LENGTH);
#else
    memcpy(key->tag, ec_pub_key_tag(group_str), SOTER_CONTAINER_TAG_LENGTH);
#endif
    key->size = htobe32(output_length);
    soter_update_container_checksum(key);
    *key_length = output_length;
    res = SOTER_SUCCESS;

err:
#ifndef THEMIS_EXPERIMENTAL_OPENSSL_3_SUPPORT
    /* Free extra reference on EC_KEY object provided by EVP_PKEY_get1_EC_KEY */
    EC_KEY_free(ec);
#endif

    return res;
}

soter_status_t soter_engine_specific_to_ec_priv_key(const soter_engine_specific_ec_key_t* engine_key,
                                                    soter_container_hdr_t* key,
                                                    size_t* key_length)
{
    EVP_PKEY* pkey = (EVP_PKEY*)engine_key;
    const bool compressed = true;
    soter_status_t res;
    size_t output_length;
#ifndef THEMIS_EXPERIMENTAL_OPENSSL_3_SUPPORT
    EC_KEY* ec;
    const EC_GROUP* group;
    const BIGNUM* d;
    int curve;
#else
    char group_str[16];
    BIGNUM* d = NULL;
#endif

    if ((!key_length) || (EVP_PKEY_EC != EVP_PKEY_id(pkey))) {
        return SOTER_INVALID_PARAMETER;
    }

#ifndef THEMIS_EXPERIMENTAL_OPENSSL_3_SUPPORT
    ec = EVP_PKEY_get1_EC_KEY((EVP_PKEY*)pkey);
    if (NULL == ec) {
        return SOTER_FAIL;
    }

    group = EC_KEY_get0_group(ec);
    if (NULL == group) {
        res = SOTER_INVALID_PARAMETER;
        goto err;
    }

    curve = EC_GROUP_get_curve_name(group);
    if (!is_curve_supported(curve)) {
        res = SOTER_INVALID_PARAMETER;
        goto err;
    }

    /*
     * Note that we use a buffer suitable for a public key to store a private
     * key. This was a historical mistake, now preserved for compatibility.
     */
    output_length = ec_pub_key_size(curve, compressed);
    if ((!key) || (output_length > *key_length)) {
        *key_length = output_length;
        res = SOTER_BUFFER_TOO_SMALL;
        goto err;
    }

    *key_length = output_length;

    d = EC_KEY_get0_private_key(ec);
    if (NULL == d) {
        res = SOTER_INVALID_PARAMETER;
        goto err;
    }
#else
    // Curve identifier as string ("prime256v1", "secp384r1", "secp521r1" etc)
    if (!EVP_PKEY_get_utf8_string_param(pkey,
                                        OSSL_PKEY_PARAM_GROUP_NAME /* "group" */,
                                        group_str,
                                        sizeof(group_str),
                                        NULL)) {
        res = SOTER_INVALID_PARAMETER;
        goto err;
    }

    if (!is_curve_supported_2(group_str)) {
        res = SOTER_INVALID_PARAMETER;
        goto err;
    }

    /*
     * Note that we use a buffer suitable for a public key to store a private
     * key. This was a historical mistake, now preserved for compatibility.
     */
    output_length = ec_pub_key_size_2(group_str, compressed);
    if ((!key) || (output_length > *key_length)) {
        *key_length = output_length;
        res = SOTER_BUFFER_TOO_SMALL;
        goto err;
    }

    *key_length = output_length;

    if (!EVP_PKEY_get_bn_param(pkey,
                               OSSL_PKEY_PARAM_PRIV_KEY /* "priv" */,
                               &d)) {
        res = SOTER_INVALID_PARAMETER;
        goto err;
    }
#endif

    if ((output_length - sizeof(soter_container_hdr_t))
        != bn_encode(d, (unsigned char*)(key + 1), output_length - sizeof(soter_container_hdr_t))) {
        res = SOTER_FAIL;
        goto err;
    }

#ifndef THEMIS_EXPERIMENTAL_OPENSSL_3_SUPPORT
    memcpy(key->tag, ec_priv_key_tag(curve), SOTER_CONTAINER_TAG_LENGTH);
#else
    memcpy(key->tag, ec_priv_key_tag(group_str), SOTER_CONTAINER_TAG_LENGTH);
#endif
    key->size = htobe32(output_length);
    soter_update_container_checksum(key);
    *key_length = output_length;
    res = SOTER_SUCCESS;

err:
#ifndef THEMIS_EXPERIMENTAL_OPENSSL_3_SUPPORT
    /* Free extra reference on EC_KEY object provided by EVP_PKEY_get1_EC_KEY */
    EC_KEY_free(ec);
#endif

    return res;
}

soter_status_t soter_ec_pub_key_to_engine_specific(const soter_container_hdr_t* key,
                                                   size_t key_length,
                                                   soter_engine_specific_ec_key_t** engine_key)
{
#ifndef THEMIS_EXPERIMENTAL_OPENSSL_3_SUPPORT
    int curve;
    EC_KEY* ec = NULL;
    const EC_GROUP* group;
    EC_POINT* Q = NULL;
    EVP_PKEY* pkey = (EVP_PKEY*)(*engine_key);
#else
    const char *curve_str;
    OSSL_PARAM params[3] = { [2] = OSSL_PARAM_END };
    EVP_PKEY_CTX* ctx = NULL;
#endif
    const bool compressed = true;
    soter_status_t res;

    if ((!key) || (key_length < sizeof(soter_container_hdr_t))) {
        return SOTER_INVALID_PARAMETER;
    }

    if (key_length != be32toh(key->size)) {
        return SOTER_INVALID_PARAMETER;
    }

    /* Validate tag */
    if (memcmp(key->tag, EC_PUB_KEY_PREF, strlen(EC_PUB_KEY_PREF)) != 0) {
        return SOTER_INVALID_PARAMETER;
    }

    if (SOTER_SUCCESS != soter_verify_container_checksum(key)) {
        return SOTER_DATA_CORRUPT;
    }

#ifndef THEMIS_EXPERIMENTAL_OPENSSL_3_SUPPORT
    switch (key->tag[3]) {
    case EC_SIZE_TAG_256:
        curve = NID_X9_62_prime256v1;
        break;
    case EC_SIZE_TAG_384:
        curve = NID_secp384r1;
        break;
    case EC_SIZE_TAG_521:
        curve = NID_secp521r1;
        break;
    default:
        return SOTER_INVALID_PARAMETER;
    }

    /* Encoded public key cannot be smaller than this */
    if (key_length < ec_pub_key_size(curve, compressed)) {
        return SOTER_INVALID_PARAMETER;
    }

    ec = EC_KEY_new_by_curve_name(curve);
    if (NULL == ec) {
        return SOTER_FAIL;
    }

    group = EC_KEY_get0_group(ec);
    if (NULL == group) {
        res = SOTER_FAIL;
        goto err;
    }

    Q = EC_POINT_new(group);
    if (NULL == group) {
        res = SOTER_NO_MEMORY;
        goto err;
    }

    if (1
        != EC_POINT_oct2point(group,
                              Q,
                              (const unsigned char*)(key + 1),
                              (int)(key_length - sizeof(soter_container_hdr_t)),
                              NULL)) {
        res = SOTER_INVALID_PARAMETER;
        goto err;
    }

    if (1 != EC_KEY_set_public_key(ec, Q)) {
        res = SOTER_FAIL;
        goto err;
    }

    EC_POINT_free(Q);
    Q = NULL;

    if (EVP_PKEY_assign_EC_KEY(pkey, ec)) {
        res = SOTER_SUCCESS;
        ec = NULL;
    } else {
        res = SOTER_FAIL;
    }
#else
    switch (key->tag[3]) {
    case EC_SIZE_TAG_256:
        curve_str = "prime256v1";
        break;
    case EC_SIZE_TAG_384:
        curve_str = "secp384r1";
        break;
    case EC_SIZE_TAG_521:
        curve_str = "secp521r1";
        break;
    default:
        return SOTER_INVALID_PARAMETER;
    }

    params[0] = OSSL_PARAM_construct_utf8_string(OSSL_PKEY_PARAM_GROUP_NAME /* "group" */,
                                                 (char*)curve_str,
                                                 strlen(curve_str));

    params[1] = OSSL_PARAM_construct_octet_string(OSSL_PKEY_PARAM_PUB_KEY /* "pub" */,
                                                  (unsigned char*)(key + 1),
                                                  (int)(key_length - sizeof(soter_container_hdr_t)));

    ctx = EVP_PKEY_CTX_new_from_name(NULL, "EC", NULL);
    if (ctx == NULL) {
        res = SOTER_FAIL;
        goto err;
    }

    if (!EVP_PKEY_fromdata_init(ctx)) {
        res = SOTER_FAIL;
        goto err;
    }

    if (!EVP_PKEY_fromdata(ctx,
                           (EVP_PKEY**)engine_key,
                           EVP_PKEY_PUBLIC_KEY,
                           params)) {
        res = SOTER_FAIL;
        goto err;
    }

    res = SOTER_SUCCESS;
#endif

err:

#ifndef THEMIS_EXPERIMENTAL_OPENSSL_3_SUPPORT
    if (Q) {
        EC_POINT_free(Q);
    }

    if (ec) {
        EC_KEY_free(ec);
    }
#else
    EVP_PKEY_CTX_free(ctx);
#endif

    return res;
}

soter_status_t soter_ec_priv_key_to_engine_specific(const soter_container_hdr_t* key,
                                                    size_t key_length,
                                                    soter_engine_specific_ec_key_t** engine_key)
{
    int curve;
    EC_KEY* ec = NULL;
    const EC_GROUP* group;
    BIGNUM* d = NULL;
    EVP_PKEY* pkey = (EVP_PKEY*)(*engine_key);
    soter_status_t res;

    if (key_length != be32toh(key->size)) {
        return SOTER_INVALID_PARAMETER;
    }

    /* Validate tag */
    if (memcmp(key->tag, EC_PRIV_KEY_PREF, strlen(EC_PRIV_KEY_PREF)) != 0) {
        return SOTER_INVALID_PARAMETER;
    }

    if (SOTER_SUCCESS != soter_verify_container_checksum(key)) {
        return SOTER_DATA_CORRUPT;
    }

    switch (key->tag[3]) {
    case EC_SIZE_TAG_256:
        curve = NID_X9_62_prime256v1;
        break;
    case EC_SIZE_TAG_384:
        curve = NID_secp384r1;
        break;
    case EC_SIZE_TAG_521:
        curve = NID_secp521r1;
        break;
    default:
        return SOTER_INVALID_PARAMETER;
    }

    if (key_length < ec_priv_key_size(curve)) {
        return SOTER_INVALID_PARAMETER;
    }

    ec = EC_KEY_new_by_curve_name(curve);
    if (NULL == ec) {
        return SOTER_FAIL;
    }

    group = EC_KEY_get0_group(ec);
    if (NULL == group) {
        res = SOTER_FAIL;
        goto err;
    }

    d = BN_bin2bn((const unsigned char*)(key + 1), (int)(key_length - sizeof(soter_container_hdr_t)), NULL);
    if (NULL == d) {
        res = SOTER_NO_MEMORY;
        goto err;
    }

    if (1 != EC_KEY_set_private_key(ec, d)) {
        res = SOTER_FAIL;
        goto err;
    }

    BN_clear_free(d);
    d = NULL;

    if (EVP_PKEY_assign_EC_KEY(pkey, ec)) {
        res = SOTER_SUCCESS;
        ec = NULL;
    } else {
        res = SOTER_FAIL;
    }

err:

    if (d) {
        BN_clear_free(d);
    }

    if (ec) {
        EC_KEY_free(ec);
    }

    return res;
}
