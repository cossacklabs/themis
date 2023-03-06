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

#include <openssl/opensslv.h>

#if OPENSSL_VERSION_NUMBER < 0x30000000

#include "soter/soter_ec_key.h"

#include <string.h>

#include <openssl/bn.h>
#include <openssl/ec.h>
#include <openssl/evp.h>

#include "soter/openssl/soter_bignum_utils.h"
#include "soter/openssl/soter_ec_key_utils.h"
#include "soter/soter_portable_endian.h"

soter_status_t soter_engine_specific_to_ec_pub_key(const soter_engine_specific_ec_key_t* engine_key,
                                                   bool compressed,
                                                   soter_container_hdr_t* key,
                                                   size_t* key_length)
{
    EVP_PKEY* pkey = (EVP_PKEY*)engine_key;
    soter_status_t res;
    size_t output_length;
    EC_KEY* ec;
    const EC_GROUP* group;
    const EC_POINT* Q;
    int curve;

    if ((!key_length) || (EVP_PKEY_EC != EVP_PKEY_id(pkey))) {
        return SOTER_INVALID_PARAMETER;
    }

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
    if ((!key) || (output_length > *key_length)) {
        *key_length = output_length;
        res = SOTER_BUFFER_TOO_SMALL;
        goto err;
    }

    *key_length = output_length;

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

    memcpy(key->tag, ec_pub_key_tag(curve), SOTER_CONTAINER_TAG_LENGTH);
    key->size = htobe32(output_length);
    soter_update_container_checksum(key);
    *key_length = output_length;
    res = SOTER_SUCCESS;

err:
    /* Free extra reference on EC_KEY object provided by EVP_PKEY_get1_EC_KEY */
    EC_KEY_free(ec);

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
    EC_KEY* ec;
    const EC_GROUP* group;
    const BIGNUM* d;
    int curve;

    if ((!key_length) || (EVP_PKEY_EC != EVP_PKEY_id(pkey))) {
        return SOTER_INVALID_PARAMETER;
    }

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

    if (BN_bn2binpad(d, (unsigned char*)(key + 1), (int)(output_length - sizeof(soter_container_hdr_t)))
        == -1) {
        res = SOTER_FAIL;
        goto err;
    }

    memcpy(key->tag, ec_priv_key_tag(curve), SOTER_CONTAINER_TAG_LENGTH);
    key->size = htobe32(output_length);
    soter_update_container_checksum(key);
    *key_length = output_length;
    res = SOTER_SUCCESS;

err:
    /* Free extra reference on EC_KEY object provided by EVP_PKEY_get1_EC_KEY */
    EC_KEY_free(ec);

    return res;
}

soter_status_t soter_ec_pub_key_to_engine_specific(const soter_container_hdr_t* key,
                                                   size_t key_length,
                                                   soter_engine_specific_ec_key_t** engine_key)
{
    int curve;
    EC_KEY* ec = NULL;
    const EC_GROUP* group;
    EC_POINT* Q = NULL;
    EVP_PKEY* pkey = (EVP_PKEY*)(*engine_key);
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

err:

    if (Q) {
        EC_POINT_free(Q);
    }

    if (ec) {
        EC_KEY_free(ec);
    }

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

#endif /* OPENSSL_VERSION_NUMBER < 0x30000000 */
