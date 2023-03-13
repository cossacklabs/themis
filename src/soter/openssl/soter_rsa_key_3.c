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

#include "soter/soter_rsa_key.h"

#include <string.h>

#include <openssl/bn.h>
#include <openssl/core_names.h>
#include <openssl/evp.h>
#include <openssl/param_build.h>
#include <openssl/rsa.h>

#include "soter/openssl/soter_bignum_utils.h"
#include "soter/openssl/soter_rsa_key_utils.h"
#include "soter/soter_portable_endian.h"
#include "soter/soter_wipe.h"

soter_status_t soter_engine_specific_to_rsa_pub_key(const soter_engine_specific_rsa_key_t* engine_key,
                                                    soter_container_hdr_t* key,
                                                    size_t* key_length)
{
    EVP_PKEY* pkey = (EVP_PKEY*)engine_key;
    soter_status_t res;
    int rsa_mod_size;
    size_t output_length;
    uint32_t* pub_exp;
    // Maximum supported RSA key is 8192 bits (1024 bytes)
    unsigned char bignum_buf[RSA_KEY_BYTES_MAX];
    BIGNUM* bignum = NULL;

    if (!key_length) {
        return SOTER_INVALID_PARAMETER;
    }

    if (EVP_PKEY_RSA != EVP_PKEY_id(pkey)) {
        return SOTER_INVALID_PARAMETER;
    }

    // Relevant pkey params
    // OSSL_PKEY_PARAM_BITS, "bits", int
    // OSSL_PKEY_PARAM_RSA_N, "n", big uint
    // OSSL_PKEY_PARAM_RSA_E, "e", big uint

    // See https://docs.cossacklabs.com/themis/spec/asymmetric-keypairs/rsa/ for key layout

    if (!EVP_PKEY_get_int_param(pkey, OSSL_PKEY_PARAM_BITS, &rsa_mod_size)) {
        res = SOTER_FAIL;
        goto err;
    }
    rsa_mod_size = (rsa_mod_size + 7) / 8;

    if (!is_mod_size_supported(rsa_mod_size)) {
        res = SOTER_INVALID_PARAMETER;
        goto err;
    }

    output_length = rsa_pub_key_size(rsa_mod_size);
    if ((!key) || (output_length > *key_length)) {
        *key_length = output_length;
        res = SOTER_BUFFER_TOO_SMALL;
        goto err;
    }

    if (!get_bn_param(pkey, OSSL_PKEY_PARAM_RSA_N, bignum_buf, sizeof(bignum_buf), &bignum)) {
        res = SOTER_FAIL;
        goto err;
    }

    if (BN_bn2binpad(bignum, (unsigned char*)(key + 1), rsa_mod_size) == -1) {
        res = SOTER_FAIL;
        goto err;
    }

    if (!get_bn_param(pkey, OSSL_PKEY_PARAM_RSA_E, bignum_buf, sizeof(bignum_buf), &bignum)) {
        res = SOTER_FAIL;
        goto err;
    }

    pub_exp = (uint32_t*)((unsigned char*)(key + 1) + rsa_mod_size);
    if (BN_is_word(bignum, RSA_F4)) {
        *pub_exp = htobe32(RSA_F4);
    } else if (BN_is_word(bignum, RSA_3)) {
        *pub_exp = htobe32(RSA_3);
    } else {
        res = SOTER_INVALID_PARAMETER;
        goto err;
    }

    memcpy(key->tag, rsa_pub_key_tag(rsa_mod_size), SOTER_CONTAINER_TAG_LENGTH);
    key->size = htobe32(output_length);
    soter_update_container_checksum(key);
    *key_length = output_length;
    res = SOTER_SUCCESS;

err:
    BN_free(bignum);

    return res;
}

soter_status_t soter_engine_specific_to_rsa_priv_key(const soter_engine_specific_rsa_key_t* engine_key,
                                                     soter_container_hdr_t* key,
                                                     size_t* key_length)
{
    EVP_PKEY* pkey = (EVP_PKEY*)engine_key;
    soter_status_t res;
    int rsa_mod_size;
    size_t output_length;
    uint32_t* pub_exp;
    // Maximum supported RSA key is 8192 bits (1024 bytes)
    unsigned char bignum_buf[RSA_KEY_BYTES_MAX];
    BIGNUM* bignum = NULL;
    unsigned char* curr_bn;

    if (!key_length) {
        return SOTER_INVALID_PARAMETER;
    }

    if (EVP_PKEY_RSA != EVP_PKEY_id(pkey)) {
        return SOTER_INVALID_PARAMETER;
    }

    // Relevant pkey params
    // OSSL_PKEY_PARAM_BITS, "bits", int
    // OSSL_PKEY_PARAM_RSA_D, "d", big uint
    // OSSL_PKEY_PARAM_RSA_FACTOR1, "rsa-factor1", big uint
    // OSSL_PKEY_PARAM_RSA_FACTOR2, "rsa-factor2", big uint
    // OSSL_PKEY_PARAM_RSA_EXPONENT1, "rsa-exponent1", big uint
    // OSSL_PKEY_PARAM_RSA_EXPONENT2, "rsa-exponent2", big uint
    // OSSL_PKEY_PARAM_RSA_COEFFICIENT1, "rsa-coefficient1", big uint
    // OSSL_PKEY_PARAM_RSA_N, "n", big uint
    // OSSL_PKEY_PARAM_RSA_E, "e", big uint

    // See https://docs.cossacklabs.com/themis/spec/asymmetric-keypairs/rsa/ for key layout

    if (!EVP_PKEY_get_int_param(pkey, OSSL_PKEY_PARAM_BITS, &rsa_mod_size)) {
        res = SOTER_FAIL;
        goto err;
    }
    rsa_mod_size = (rsa_mod_size + 7) / 8;

    if (!is_mod_size_supported(rsa_mod_size)) {
        res = SOTER_INVALID_PARAMETER;
        goto err;
    }

    output_length = rsa_priv_key_size(rsa_mod_size);
    if ((!key) || (output_length > *key_length)) {
        *key_length = output_length;
        res = SOTER_BUFFER_TOO_SMALL;
        goto err;
    }

    curr_bn = (unsigned char*)(key + 1);

    /* Private exponent */
    if (!get_bn_param(pkey, OSSL_PKEY_PARAM_RSA_D, bignum_buf, sizeof(bignum_buf), &bignum)) {
        res = SOTER_FAIL;
        goto err;
    }

    if (BN_bn2binpad(bignum, curr_bn, rsa_mod_size) == -1) {
        res = SOTER_FAIL;
        goto clear_key;
    }
    curr_bn += rsa_mod_size;

    /* p */
    if (!get_bn_param(pkey, OSSL_PKEY_PARAM_RSA_FACTOR1, bignum_buf, sizeof(bignum_buf), &bignum)) {
        res = SOTER_FAIL;
        goto clear_key;
    }

    if (BN_bn2binpad(bignum, curr_bn, rsa_mod_size / 2) == -1) {
        res = SOTER_FAIL;
        goto clear_key;
    }
    curr_bn += rsa_mod_size / 2;

    /* q */
    if (!get_bn_param(pkey, OSSL_PKEY_PARAM_RSA_FACTOR2, bignum_buf, sizeof(bignum_buf), &bignum)) {
        res = SOTER_FAIL;
        goto clear_key;
    }

    if (BN_bn2binpad(bignum, curr_bn, rsa_mod_size / 2) == -1) {
        res = SOTER_FAIL;
        goto clear_key;
    }
    curr_bn += rsa_mod_size / 2;

    /* dp */
    if (!get_bn_param(pkey, OSSL_PKEY_PARAM_RSA_EXPONENT1, bignum_buf, sizeof(bignum_buf), &bignum)) {
        res = SOTER_FAIL;
        goto clear_key;
    }

    if (BN_bn2binpad(bignum, curr_bn, rsa_mod_size / 2) == -1) {
        res = SOTER_FAIL;
        goto clear_key;
    }
    curr_bn += rsa_mod_size / 2;

    /* dq */
    if (!get_bn_param(pkey, OSSL_PKEY_PARAM_RSA_EXPONENT2, bignum_buf, sizeof(bignum_buf), &bignum)) {
        res = SOTER_FAIL;
        goto clear_key;
    }

    if (BN_bn2binpad(bignum, curr_bn, rsa_mod_size / 2) == -1) {
        res = SOTER_FAIL;
        goto clear_key;
    }
    curr_bn += rsa_mod_size / 2;

    /* qp */
    if (!get_bn_param(pkey, OSSL_PKEY_PARAM_RSA_COEFFICIENT1, bignum_buf, sizeof(bignum_buf), &bignum)) {
        res = SOTER_FAIL;
        goto clear_key;
    }

    if (BN_bn2binpad(bignum, curr_bn, rsa_mod_size / 2) == -1) {
        res = SOTER_FAIL;
        goto clear_key;
    }
    curr_bn += rsa_mod_size / 2;

    /* modulus */
    if (!get_bn_param(pkey, OSSL_PKEY_PARAM_RSA_N, bignum_buf, sizeof(bignum_buf), &bignum)) {
        res = SOTER_FAIL;
        goto clear_key;
    }

    if (BN_bn2binpad(bignum, curr_bn, rsa_mod_size) == -1) {
        res = SOTER_FAIL;
        goto clear_key;
    }
    curr_bn += rsa_mod_size;

    /* public exponent */
    if (!get_bn_param(pkey, OSSL_PKEY_PARAM_RSA_E, bignum_buf, sizeof(bignum_buf), &bignum)) {
        res = SOTER_FAIL;
        goto clear_key;
    }

    pub_exp = (uint32_t*)curr_bn;
    if (BN_is_word(bignum, RSA_F4)) {
        *pub_exp = htobe32(RSA_F4);
    } else if (BN_is_word(bignum, RSA_3)) {
        *pub_exp = htobe32(RSA_3);
    } else {
        res = SOTER_INVALID_PARAMETER;
        goto clear_key;
    }

    memcpy(key->tag, rsa_priv_key_tag(rsa_mod_size), SOTER_CONTAINER_TAG_LENGTH);
    key->size = htobe32(output_length);
    soter_update_container_checksum(key);
    *key_length = output_length;
    res = SOTER_SUCCESS;

clear_key:
    if (res != SOTER_SUCCESS) {
        /* Zero output memory to avoid leaking private key information */
        soter_wipe(key, output_length);
    }

err:
    /* We did not use whole buffer, only `rsa_mod_size` bytes of it */
    soter_wipe(bignum_buf, rsa_mod_size);

    BN_clear_free(bignum);

    return res;
}

/* TODO: Maybe, basic container validation should be put in separate functions outside of
 * engine-specific code */
soter_status_t soter_rsa_pub_key_to_engine_specific(const soter_container_hdr_t* key,
                                                    size_t key_length,
                                                    soter_engine_specific_rsa_key_t** engine_key)
{
    soter_status_t err = SOTER_FAIL;
    int rsa_mod_size;
    BIGNUM* rsa_n = NULL;
    BIGNUM* rsa_e = NULL;
    const uint32_t* pub_exp;
    OSSL_PARAM* params = NULL;
    OSSL_PARAM_BLD* bld = NULL;
    EVP_PKEY_CTX* ctx = NULL;

    if ((!key) || key_length < sizeof(soter_container_hdr_t) || key_length != be32toh(key->size)) {
        return SOTER_INVALID_PARAMETER;
    }

    /* Validate tag */
    if (memcmp(key->tag, RSA_PUB_KEY_PREF, strlen(RSA_PUB_KEY_PREF)) != 0) {
        return SOTER_INVALID_PARAMETER;
    }

    if (SOTER_SUCCESS != soter_verify_container_checksum(key)) {
        return SOTER_DATA_CORRUPT;
    }

    switch (key->tag[3]) {
    case RSA_SIZE_TAG_1024:
        rsa_mod_size = 128;
        break;
    case RSA_SIZE_TAG_2048:
        rsa_mod_size = 256;
        break;
    case RSA_SIZE_TAG_4096:
        rsa_mod_size = 512;
        break;
    case RSA_SIZE_TAG_8192:
        rsa_mod_size = 1024;
        break;
    default:
        return SOTER_INVALID_PARAMETER;
    }

    if (key_length < rsa_pub_key_size(rsa_mod_size)) {
        return SOTER_INVALID_PARAMETER;
    }

    pub_exp = (const uint32_t*)((unsigned char*)(key + 1) + rsa_mod_size);
    switch (be32toh(*pub_exp)) {
    case RSA_3:
    case RSA_F4:
        break;
    default:
        return SOTER_INVALID_PARAMETER;
    }

    rsa_n = BN_new();
    rsa_e = BN_new();
    if (!rsa_n || !rsa_e) {
        err = SOTER_NO_MEMORY;
        goto out;
    }

    if (!BN_set_word(rsa_e, be32toh(*pub_exp))) {
        goto out;
    }

    if (!BN_bin2bn((const unsigned char*)(key + 1), rsa_mod_size, rsa_n)) {
        goto out;
    }

    bld = OSSL_PARAM_BLD_new();
    if (bld == NULL) {
        err = SOTER_NO_MEMORY;
        goto out;
    }

    if (!OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_RSA_N, rsa_n)) {
        err = SOTER_FAIL;
        goto out;
    }

    if (!OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_RSA_E, rsa_e)) {
        err = SOTER_FAIL;
        goto out;
    }

    params = OSSL_PARAM_BLD_to_param(bld);
    if (params == NULL) {
        err = SOTER_NO_MEMORY;
        goto out;
    }

    ctx = EVP_PKEY_CTX_new_from_name(NULL, "RSA", NULL);
    if (ctx == NULL) {
        err = SOTER_NO_MEMORY;
        goto out;
    }

    if (!EVP_PKEY_fromdata_init(ctx)) {
        err = SOTER_FAIL;
        goto out;
    }

    if (!EVP_PKEY_fromdata(ctx, (EVP_PKEY**)engine_key, EVP_PKEY_KEYPAIR, params)) {
        err = SOTER_FAIL;
        goto out;
    }

    err = SOTER_SUCCESS;

out:
    BN_free(rsa_n);
    BN_free(rsa_e);
    EVP_PKEY_CTX_free(ctx);
    OSSL_PARAM_free(params);
    OSSL_PARAM_BLD_free(bld);

    return err;
}

soter_status_t soter_rsa_priv_key_to_engine_specific(const soter_container_hdr_t* key,
                                                     size_t key_length,
                                                     soter_engine_specific_rsa_key_t** engine_key)
{
    soter_status_t err = SOTER_FAIL;
    int rsa_mod_size;
    BIGNUM* rsa_e = NULL;
    BIGNUM* rsa_d = NULL;
    BIGNUM* rsa_p = NULL;
    BIGNUM* rsa_q = NULL;
    BIGNUM* rsa_dmp1 = NULL;
    BIGNUM* rsa_dmq1 = NULL;
    BIGNUM* rsa_iqmp = NULL;
    BIGNUM* rsa_n = NULL;
    const uint32_t* pub_exp;
    const unsigned char* curr_bn;
    OSSL_PARAM* params = NULL;
    OSSL_PARAM_BLD* bld = NULL;
    EVP_PKEY_CTX* ctx = NULL;

    if ((!key) || key_length < sizeof(soter_container_hdr_t) || key_length != be32toh(key->size)) {
        return SOTER_INVALID_PARAMETER;
    }

    /* Validate tag */
    if (memcmp(key->tag, RSA_PRIV_KEY_PREF, strlen(RSA_PRIV_KEY_PREF)) != 0) {
        return SOTER_INVALID_PARAMETER;
    }

    if (SOTER_SUCCESS != soter_verify_container_checksum(key)) {
        return SOTER_DATA_CORRUPT;
    }

    switch (key->tag[3]) {
    case RSA_SIZE_TAG_1024:
        rsa_mod_size = 128;
        break;
    case RSA_SIZE_TAG_2048:
        rsa_mod_size = 256;
        break;
    case RSA_SIZE_TAG_4096:
        rsa_mod_size = 512;
        break;
    case RSA_SIZE_TAG_8192:
        rsa_mod_size = 1024;
        break;
    default:
        return SOTER_INVALID_PARAMETER;
    }

    if (key_length < rsa_priv_key_size(rsa_mod_size)) {
        return SOTER_INVALID_PARAMETER;
    }

    curr_bn = (const unsigned char*)(key + 1);

    pub_exp = (const uint32_t*)(curr_bn + ((rsa_mod_size * 4) + (rsa_mod_size / 2)));
    switch (be32toh(*pub_exp)) {
    case RSA_3:
    case RSA_F4:
        break;
    default:
        return SOTER_INVALID_PARAMETER;
    }

    rsa_e = BN_new();
    rsa_d = BN_new();
    rsa_p = BN_new();
    rsa_q = BN_new();
    rsa_dmp1 = BN_new();
    rsa_dmq1 = BN_new();
    rsa_iqmp = BN_new();
    rsa_n = BN_new();
    if (!rsa_e || !rsa_d || !rsa_p || !rsa_q || !rsa_dmp1 || !rsa_dmq1 || !rsa_iqmp || !rsa_n) {
        err = SOTER_NO_MEMORY;
        goto out;
    }

    if (!BN_set_word(rsa_e, be32toh(*pub_exp))) {
        goto out;
    }

    /* Private exponent */
    if (!BN_bin2bn(curr_bn, rsa_mod_size, rsa_d)) {
        goto out;
    }
    curr_bn += rsa_mod_size;

    if (!BN_bin2bn(curr_bn, rsa_mod_size / 2, rsa_p)) {
        goto out;
    }
    curr_bn += rsa_mod_size / 2;

    /* q */
    if (!BN_bin2bn(curr_bn, rsa_mod_size / 2, rsa_q)) {
        goto out;
    }
    curr_bn += rsa_mod_size / 2;

    /* dp */
    if (!BN_bin2bn(curr_bn, rsa_mod_size / 2, rsa_dmp1)) {
        goto out;
    }
    curr_bn += rsa_mod_size / 2;

    /* dq */
    if (!BN_bin2bn(curr_bn, rsa_mod_size / 2, rsa_dmq1)) {
        goto out;
    }
    curr_bn += rsa_mod_size / 2;

    /* qp */
    if (!BN_bin2bn(curr_bn, rsa_mod_size / 2, rsa_iqmp)) {
        goto out;
    }
    curr_bn += rsa_mod_size / 2;

    /* modulus */
    if (!BN_bin2bn(curr_bn, rsa_mod_size, rsa_n)) {
        goto out;
    }

    /* If at least one CRT parameter is zero, free them */
    if (BN_is_zero(rsa_p) || BN_is_zero(rsa_q) || BN_is_zero(rsa_dmp1) || BN_is_zero(rsa_dmq1)
        || BN_is_zero(rsa_iqmp)) {
        BN_clear_free(rsa_p);
        rsa_p = NULL;

        BN_clear_free(rsa_q);
        rsa_q = NULL;

        BN_clear_free(rsa_dmp1);
        rsa_dmp1 = NULL;

        BN_clear_free(rsa_dmq1);
        rsa_dmq1 = NULL;

        BN_clear_free(rsa_iqmp);
        rsa_iqmp = NULL;
    }

    bld = OSSL_PARAM_BLD_new();
    if (bld == NULL) {
        err = SOTER_NO_MEMORY;
        goto out;
    }

    if (!OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_RSA_D, rsa_d)) {
        err = SOTER_FAIL;
        goto out;
    }

    if (!OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_RSA_FACTOR1, rsa_p)) {
        err = SOTER_FAIL;
        goto out;
    }

    if (!OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_RSA_FACTOR2, rsa_q)) {
        err = SOTER_FAIL;
        goto out;
    }

    if (!OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_RSA_EXPONENT1, rsa_dmp1)) {
        err = SOTER_FAIL;
        goto out;
    }

    if (!OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_RSA_EXPONENT2, rsa_dmq1)) {
        err = SOTER_FAIL;
        goto out;
    }

    if (!OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_RSA_COEFFICIENT1, rsa_iqmp)) {
        err = SOTER_FAIL;
        goto out;
    }

    if (!OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_RSA_N, rsa_n)) {
        err = SOTER_FAIL;
        goto out;
    }

    if (!OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_RSA_E, rsa_e)) {
        err = SOTER_FAIL;
        goto out;
    }

    params = OSSL_PARAM_BLD_to_param(bld);
    if (params == NULL) {
        err = SOTER_NO_MEMORY;
        goto out;
    }

    ctx = EVP_PKEY_CTX_new_from_name(NULL, "RSA", NULL);
    if (ctx == NULL) {
        err = SOTER_NO_MEMORY;
        goto out;
    }

    if (!EVP_PKEY_fromdata_init(ctx)) {
        err = SOTER_FAIL;
        goto out;
    }

    if (!EVP_PKEY_fromdata(ctx, (EVP_PKEY**)engine_key, EVP_PKEY_KEYPAIR, params)) {
        err = SOTER_FAIL;
        goto out;
    }

    err = SOTER_SUCCESS;

out:
    BN_free(rsa_n);
    BN_free(rsa_e);
    BN_clear_free(rsa_d);
    BN_clear_free(rsa_p);
    BN_clear_free(rsa_q);
    BN_clear_free(rsa_dmp1);
    BN_clear_free(rsa_dmq1);
    BN_clear_free(rsa_iqmp);
    EVP_PKEY_CTX_free(ctx);
    OSSL_PARAM_free(params);
    OSSL_PARAM_BLD_free(bld);

    return err;
}

#endif /* OPENSSL_VERSION_NUMBER >= 0x30000000 */
