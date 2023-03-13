/*
* Copyright (c) 2023 Cossack Labs Limited
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

#if OPENSSL_VERSION_NUMBER >= 0x30000000

#include "soter/soter_ec_key.h"

#include <string.h>

#include <openssl/bn.h>
#include <openssl/core_names.h>
#include <openssl/evp.h>
#include <openssl/obj_mac.h>
#include <openssl/param_build.h>

#include "soter/openssl/soter_bignum_utils.h"
#include "soter/openssl/soter_ec_key_utils.h"
#include "soter/openssl/soter_engine.h"
#include "soter/soter_portable_endian.h"
#include "soter/soter_wipe.h"

soter_status_t soter_engine_specific_to_ec_pub_key(const soter_engine_specific_ec_key_t* engine_key,
                                                   bool compressed,
                                                   soter_container_hdr_t* key,
                                                   size_t* key_length)
{
    // FIXME: make `pkey` const if possible; even though we don't actually modify `pkey`,
    //        we set one property so later it gives us public key in proper form
    EVP_PKEY* pkey = (EVP_PKEY*)engine_key;
    soter_status_t res;
    size_t output_length;
    char curve_str[MAX_CURVE_NAME_LEN];
    int curve;
    const char* param_name;
    size_t serialized_len;

    if ((!key_length) || (EVP_PKEY_EC != EVP_PKEY_id(pkey))) {
        return SOTER_INVALID_PARAMETER;
    }

    // Curve identifier as string ("prime256v1", "secp384r1", "secp521r1" etc)
    if (!EVP_PKEY_get_utf8_string_param(pkey, OSSL_PKEY_PARAM_GROUP_NAME, curve_str, sizeof(curve_str), NULL)) {
        res = SOTER_INVALID_PARAMETER;
        goto err;
    }
    curve = OBJ_sn2nid(curve_str);
    if (curve == NID_undef) {
        // Should never happen since Numeric IDentifier exists for all supported curves
        res = SOTER_FAIL;
        goto err;
    }

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

#if OPENSSL_VERSION_MINOR == 0 && OPENSSL_VERSION_PATCH < 8
    // In OpenSSL <=3.0.7 param "pub" always contains compressed public key,
    // while uncompressed one is available in "encoded-pub-key"

    if (compressed) {
        param_name = OSSL_PKEY_PARAM_PUB_KEY;
    } else {
        param_name = OSSL_PKEY_PARAM_ENCODED_PUBLIC_KEY;
    }
#else
    // In OpenSSL >=3.0.8 param "pub" contains compressed or uncompressed public key
    // based on value of "point-format" param. "encoded-pub-key" is still available,
    // but we don't rely on it anymore

    if (!EVP_PKEY_set_utf8_string_param(pkey,
                                        OSSL_PKEY_PARAM_EC_POINT_CONVERSION_FORMAT,
                                        compressed ? OSSL_PKEY_EC_POINT_CONVERSION_FORMAT_COMPRESSED
                                                   : OSSL_PKEY_EC_POINT_CONVERSION_FORMAT_UNCOMPRESSED)) {
        res = SOTER_FAIL;
        goto err;
    }

    param_name = OSSL_PKEY_PARAM_PUB_KEY;
#endif

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

    memcpy(key->tag, ec_pub_key_tag(curve), SOTER_CONTAINER_TAG_LENGTH);
    key->size = htobe32(output_length);
    soter_update_container_checksum(key);
    *key_length = output_length;
    res = SOTER_SUCCESS;

err:

    return res;
}

soter_status_t soter_engine_specific_to_ec_priv_key(const soter_engine_specific_ec_key_t* engine_key,
                                                    soter_container_hdr_t* key,
                                                    size_t* key_length)
{
    const EVP_PKEY* pkey = (EVP_PKEY*)engine_key;
    const bool compressed = true;
    soter_status_t res;
    size_t output_length;
    char curve_str[MAX_CURVE_NAME_LEN];
    int curve;
    // Maximum supported key length is 521 bits (secp521r1), roughly 65.12 bytes, rounding to 66
    unsigned char bignum_buf[66];
    BIGNUM* bignum = NULL;

    if ((!key_length) || (EVP_PKEY_EC != EVP_PKEY_id(pkey))) {
        return SOTER_INVALID_PARAMETER;
    }

    // Curve identifier as string (SN_X9_62_prime256v1, SN_secp384r1, SN_secp521r1 etc)
    if (!EVP_PKEY_get_utf8_string_param(pkey, OSSL_PKEY_PARAM_GROUP_NAME, curve_str, sizeof(curve_str), NULL)) {
        res = SOTER_INVALID_PARAMETER;
        goto err;
    }
    curve = OBJ_sn2nid(curve_str);
    if (curve == NID_undef) {
        // Should never happen since Numeric IDentifier exists for all supported curves
        res = SOTER_FAIL;
        goto err;
    }

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

    if (!get_bn_param(pkey, OSSL_PKEY_PARAM_PRIV_KEY, bignum_buf, sizeof(bignum_buf), &bignum)) {
        res = SOTER_FAIL;
        goto err;
    }
    if (BN_bn2binpad(bignum, (unsigned char*)(key + 1), (int)(output_length - sizeof(soter_container_hdr_t)))
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
    soter_wipe(bignum_buf, sizeof(bignum_buf));
    BN_clear_free(bignum);

    return res;
}

soter_status_t soter_ec_pub_key_to_engine_specific(const soter_container_hdr_t* key,
                                                   size_t key_length,
                                                   soter_engine_specific_ec_key_t** engine_key)
{
    const char* curve_str;
    OSSL_PARAM params[3];
    EVP_PKEY_CTX* ctx = NULL;
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
        curve_str = SN_X9_62_prime256v1;
        break;
    case EC_SIZE_TAG_384:
        curve_str = SN_secp384r1;
        break;
    case EC_SIZE_TAG_521:
        curve_str = SN_secp521r1;
        break;
    default:
        return SOTER_INVALID_PARAMETER;
    }

    params[0] = OSSL_PARAM_construct_utf8_string(OSSL_PKEY_PARAM_GROUP_NAME,
                                                 (char*)curve_str,
                                                 strlen(curve_str));

    params[1] = OSSL_PARAM_construct_octet_string(OSSL_PKEY_PARAM_PUB_KEY,
                                                  (unsigned char*)(key + 1),
                                                  (int)(key_length - sizeof(soter_container_hdr_t)));

    params[2] = OSSL_PARAM_construct_end();

    ctx = EVP_PKEY_CTX_new_from_name(NULL, "EC", NULL);
    if (ctx == NULL) {
        res = SOTER_FAIL;
        goto err;
    }

    if (!EVP_PKEY_fromdata_init(ctx)) {
        res = SOTER_FAIL;
        goto err;
    }

    if (!EVP_PKEY_fromdata(ctx, (EVP_PKEY**)engine_key, EVP_PKEY_PUBLIC_KEY, params)) {
        res = SOTER_FAIL;
        goto err;
    }

    res = SOTER_SUCCESS;

err:

    EVP_PKEY_CTX_free(ctx);

    return res;
}

soter_status_t soter_ec_priv_key_to_engine_specific(const soter_container_hdr_t* key,
                                                    size_t key_length,
                                                    soter_engine_specific_ec_key_t** engine_key)
{
    const char* curve_str;
    int curve;
    OSSL_PARAM* params = NULL;
    OSSL_PARAM_BLD* bld = NULL;
    EVP_PKEY_CTX* ctx = NULL;
    BIGNUM* d = NULL;
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
        curve_str = SN_X9_62_prime256v1;
        curve = NID_X9_62_prime256v1;
        break;
    case EC_SIZE_TAG_384:
        curve_str = SN_secp384r1;
        curve = NID_secp384r1;
        break;
    case EC_SIZE_TAG_521:
        curve_str = SN_secp521r1;
        curve = NID_secp521r1;
        break;
    default:
        return SOTER_INVALID_PARAMETER;
    }

    if (key_length < ec_priv_key_size(curve)) {
        return SOTER_INVALID_PARAMETER;
    }

    bld = OSSL_PARAM_BLD_new();
    if (bld == NULL) {
        res = SOTER_NO_MEMORY;
        goto err;
    }

    if (!OSSL_PARAM_BLD_push_utf8_string(bld, OSSL_PKEY_PARAM_GROUP_NAME, curve_str, strlen(curve_str))) {
        res = SOTER_FAIL;
        goto err;
    }

    d = BN_bin2bn((const unsigned char*)(key + 1), (int)(key_length - sizeof(soter_container_hdr_t)), NULL);
    if (d == NULL) {
        res = SOTER_NO_MEMORY;
        goto err;
    }

    // Unfortunately, using on stack `OSSL_PARAM params[3]`
    // like in soter_ec_pub_key_to_engine_specific() and setting
    // params[1] = OSSL_PARAM_construct_BN(OSSL_PKEY_PARAM_PRIV_KEY,
    //                                     (unsigned char*)(key + 1) + 1,
    //                                     (int)(key_length - sizeof(soter_container_hdr_t) - 1)))
    // won't work properly. This is because private key number (32 bytes, not counting leading zero)
    // is stored in one byte order and EVP_PKEY_fromdata() will read it in different byte order. As
    // a result, `priv` property of generated `pkey` will have different value, with reversed bytes
    // order. One working solution was to do like this, deserialize value into `BIGINT*`, then use
    // param builder (OSSL_PARAM_BLD*) and OSSL_PARAM_BLD_push_BN(). Seems to have a slight overhead,
    // but uses APIs that were designed for such actions.
    // Another possible and working solution is to simply swap bytes into temporary buffer and use
    // it instead of provided one, then we can avoid using EVP_PKEY_BLD* at all.

    if (!OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_PRIV_KEY, d)) {
        res = SOTER_FAIL;
        goto err;
    }

    params = OSSL_PARAM_BLD_to_param(bld);
    if (params == NULL) {
        res = SOTER_NO_MEMORY;
        goto err;
    }

    ctx = EVP_PKEY_CTX_new_from_name(NULL, "EC", NULL);
    if (ctx == NULL) {
        res = SOTER_NO_MEMORY;
        goto err;
    }

    if (!EVP_PKEY_fromdata_init(ctx)) {
        res = SOTER_FAIL;
        goto err;
    }

    if (!EVP_PKEY_fromdata(ctx, (EVP_PKEY**)engine_key, EVP_PKEY_KEYPAIR, params)) {
        res = SOTER_FAIL;
        goto err;
    }

    res = SOTER_SUCCESS;

err:

    EVP_PKEY_CTX_free(ctx);
    OSSL_PARAM_free(params);
    OSSL_PARAM_BLD_free(bld);
    BN_clear_free(d);

    return res;
}

#endif /* OPENSSL_VERSION_NUMBER >= 0x30000000 */
