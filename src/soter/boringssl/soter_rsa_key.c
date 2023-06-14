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

#include "soter/soter_rsa_key.h"

#include <string.h>

#include <openssl/bn.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>

#include "soter/soter_portable_endian.h"

static size_t rsa_pub_key_size(unsigned mod_size)
{
    switch (mod_size) {
    case 128: /* 1024 */
        return sizeof(soter_rsa_pub_key_1024_t);
    case 256: /* 2048 */
        return sizeof(soter_rsa_pub_key_2048_t);
    case 512: /* 4096 */
        return sizeof(soter_rsa_pub_key_4096_t);
    case 1024: /* 8192 */
        return sizeof(soter_rsa_pub_key_8192_t);
    default:
        return 0;
    }
}

static size_t rsa_priv_key_size(unsigned mod_size)
{
    switch (mod_size) {
    case 128: /* 1024 */
        return sizeof(soter_rsa_priv_key_1024_t);
    case 256: /* 2048 */
        return sizeof(soter_rsa_priv_key_2048_t);
    case 512: /* 4096 */
        return sizeof(soter_rsa_priv_key_4096_t);
    case 1024: /* 8192 */
        return sizeof(soter_rsa_priv_key_8192_t);
    default:
        return 0;
    }
}

static char* rsa_pub_key_tag(unsigned mod_size)
{
    switch (mod_size) {
    case 128: /* 1024 */
        return RSA_PUB_KEY_TAG(1024);
    case 256: /* 2048 */
        return RSA_PUB_KEY_TAG(2048);
    case 512: /* 4096 */
        return RSA_PUB_KEY_TAG(4096);
    case 1024: /* 8192 */
        return RSA_PUB_KEY_TAG(8192);
    default:
        return NULL;
    }
}

static char* rsa_priv_key_tag(unsigned mod_size)
{
    switch (mod_size) {
    case 128: /* 1024 */
        return RSA_PRIV_KEY_TAG(1024);
    case 256: /* 2048 */
        return RSA_PRIV_KEY_TAG(2048);
    case 512: /* 4096 */
        return RSA_PRIV_KEY_TAG(4096);
    case 1024: /* 8192 */
        return RSA_PRIV_KEY_TAG(8192);
    default:
        return NULL;
    }
}

static bool is_mod_size_supported(unsigned mod_size)
{
    switch (mod_size) {
    case 128:
    case 256:
    case 512:
    case 1024:
        return true;
    default:
        return false;
    }
}

static soter_status_t bignum_to_bytes(const BIGNUM* bn, uint8_t* to, size_t to_length)
{
    size_t bn_size = (size_t)BN_num_bytes(bn);
    size_t bytes_copied;

    if (bn_size > to_length) {
        return SOTER_FAIL;
    }

    bytes_copied = BN_bn2bin(bn, to + (to_length - bn_size));

    if (bytes_copied != bn_size) {
        return SOTER_FAIL;
    }

    memset(to, 0, to_length - bn_size);

    return SOTER_SUCCESS;
}

soter_status_t soter_engine_specific_to_rsa_pub_key(const soter_engine_specific_rsa_key_t* engine_key,
                                                    soter_container_hdr_t* key,
                                                    size_t* key_length)
{
    EVP_PKEY* pkey = (EVP_PKEY*)engine_key;
    RSA* rsa;
    soter_status_t res;
    unsigned rsa_mod_size;
    size_t output_length;
    uint32_t* pub_exp;

    if (!key_length) {
        return SOTER_INVALID_PARAMETER;
    }

    if (EVP_PKEY_RSA != EVP_PKEY_id(pkey)) {
        return SOTER_INVALID_PARAMETER;
    }

    rsa = EVP_PKEY_get1_RSA((EVP_PKEY*)pkey);
    if (NULL == rsa) {
        return SOTER_FAIL;
    }

    rsa_mod_size = RSA_size(rsa);
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

    pub_exp = (uint32_t*)((unsigned char*)(key + 1) + rsa_mod_size);
    if (BN_is_word(RSA_get0_e(rsa), RSA_F4)) {
        *pub_exp = htobe32(RSA_F4);
    } else if (BN_is_word(RSA_get0_e(rsa), RSA_3)) {
        *pub_exp = htobe32(RSA_3);
    } else {
        res = SOTER_INVALID_PARAMETER;
        goto err;
    }

    res = bignum_to_bytes(RSA_get0_n(rsa), (unsigned char*)(key + 1), rsa_mod_size);
    if (SOTER_SUCCESS != res) {
        goto err;
    }

    memcpy(key->tag, rsa_pub_key_tag(rsa_mod_size), SOTER_CONTAINER_TAG_LENGTH);
    key->size = htobe32(output_length);
    soter_update_container_checksum(key);
    *key_length = output_length;
    res = SOTER_SUCCESS;

err:
    /* Free extra reference on RSA object provided by EVP_PKEY_get1_RSA */
    RSA_free(rsa);

    return res;
}

soter_status_t soter_engine_specific_to_rsa_priv_key(const soter_engine_specific_rsa_key_t* engine_key,
                                                     soter_container_hdr_t* key,
                                                     size_t* key_length)
{
    EVP_PKEY* pkey = (EVP_PKEY*)engine_key;
    RSA* rsa;
    soter_status_t res;
    unsigned rsa_mod_size;
    size_t output_length;
    uint32_t* pub_exp;
    unsigned char* curr_bn = (unsigned char*)(key + 1);

    if (!key_length) {
        return SOTER_INVALID_PARAMETER;
    }

    if (EVP_PKEY_RSA != EVP_PKEY_id(pkey)) {
        return SOTER_INVALID_PARAMETER;
    }

    rsa = EVP_PKEY_get1_RSA((EVP_PKEY*)pkey);
    if (NULL == rsa) {
        return SOTER_FAIL;
    }

    rsa_mod_size = RSA_size(rsa);
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

    pub_exp = (uint32_t*)(curr_bn + ((rsa_mod_size * 4) + (rsa_mod_size / 2)));
    if (BN_is_word(RSA_get0_e(rsa), RSA_F4)) {
        *pub_exp = htobe32(RSA_F4);
    } else if (BN_is_word(RSA_get0_e(rsa), RSA_3)) {
        *pub_exp = htobe32(RSA_3);
    } else {
        res = SOTER_INVALID_PARAMETER;
        goto err;
    }

    /* Private exponent */
    res = bignum_to_bytes(RSA_get0_d(rsa), curr_bn, rsa_mod_size);
    if (SOTER_SUCCESS != res) {
        goto err;
    }
    curr_bn += rsa_mod_size;

    /* p */
    res = bignum_to_bytes(RSA_get0_p(rsa), curr_bn, rsa_mod_size / 2);
    if (SOTER_SUCCESS != res) {
        goto err;
    }
    curr_bn += rsa_mod_size / 2;

    /* q */
    res = bignum_to_bytes(RSA_get0_q(rsa), curr_bn, rsa_mod_size / 2);
    if (SOTER_SUCCESS != res) {
        goto err;
    }
    curr_bn += rsa_mod_size / 2;

    /* dp */
    res = bignum_to_bytes(RSA_get0_dmp1(rsa), curr_bn, rsa_mod_size / 2);
    if (SOTER_SUCCESS != res) {
        goto err;
    }
    curr_bn += rsa_mod_size / 2;

    /* dq */
    res = bignum_to_bytes(RSA_get0_dmq1(rsa), curr_bn, rsa_mod_size / 2);
    if (SOTER_SUCCESS != res) {
        goto err;
    }
    curr_bn += rsa_mod_size / 2;

    /* qp */
    res = bignum_to_bytes(RSA_get0_iqmp(rsa), curr_bn, rsa_mod_size / 2);
    if (SOTER_SUCCESS != res) {
        goto err;
    }
    curr_bn += rsa_mod_size / 2;

    /* modulus */
    res = bignum_to_bytes(RSA_get0_n(rsa), curr_bn, rsa_mod_size);
    if (SOTER_SUCCESS != res) {
        goto err;
    }

    memcpy(key->tag, rsa_priv_key_tag(rsa_mod_size), SOTER_CONTAINER_TAG_LENGTH);
    key->size = htobe32(output_length);
    soter_update_container_checksum(key);
    *key_length = output_length;
    res = SOTER_SUCCESS;

err:
    /* Free extra reference on RSA object provided by EVP_PKEY_get1_RSA */
    RSA_free(rsa);

    //	if (SOTER_SUCCESS != res)
    //	{
    //		/* Zero output memory to avoid leaking private key information */
    //		memset(key, 0, *key_length);
    //	}

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
    RSA* rsa = NULL;
    BIGNUM* rsa_n = NULL;
    BIGNUM* rsa_e = NULL;
    EVP_PKEY* pkey = (EVP_PKEY*)(*engine_key);
    const uint32_t* pub_exp;

    if (key_length != be32toh(key->size)) {
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

    rsa = RSA_new();
    rsa_n = BN_new();
    rsa_e = BN_new();
    if (!rsa || !rsa_n || !rsa_e) {
        err = SOTER_NO_MEMORY;
        goto free_exponents;
    }

    if (!BN_set_word(rsa_e, be32toh(*pub_exp))) {
        goto free_exponents;
    }

    if (!BN_bin2bn((const unsigned char*)(key + 1), rsa_mod_size, rsa_n)) {
        goto free_exponents;
    }

    /* RSA_set0_key() transfers ownership over exponents to "rsa" */
    if (!RSA_set0_key(rsa, rsa_n, rsa_e, NULL)) {
        goto free_exponents;
    }

    /* EVP_PKEY_assign_RSA() transfers ownership over "rsa" to "pkey" */
    if (!EVP_PKEY_assign_RSA(pkey, rsa)) {
        goto free_rsa;
    }

    return SOTER_SUCCESS;

free_exponents:
    BN_free(rsa_n);
    BN_free(rsa_e);
free_rsa:
    RSA_free(rsa);
    return err;
}

soter_status_t soter_rsa_priv_key_to_engine_specific(const soter_container_hdr_t* key,
                                                     size_t key_length,
                                                     soter_engine_specific_rsa_key_t** engine_key)
{
    soter_status_t err = SOTER_FAIL;
    int rsa_mod_size;
    RSA* rsa = NULL;
    BIGNUM* rsa_e = NULL;
    BIGNUM* rsa_d = NULL;
    BIGNUM* rsa_p = NULL;
    BIGNUM* rsa_q = NULL;
    BIGNUM* rsa_dmp1 = NULL;
    BIGNUM* rsa_dmq1 = NULL;
    BIGNUM* rsa_iqmp = NULL;
    BIGNUM* rsa_n = NULL;
    EVP_PKEY* pkey = (EVP_PKEY*)(*engine_key);
    const uint32_t* pub_exp;
    const unsigned char* curr_bn = (const unsigned char*)(key + 1);

    if (key_length != be32toh(key->size)) {
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

    pub_exp = (const uint32_t*)(curr_bn + ((rsa_mod_size * 4) + (rsa_mod_size / 2)));
    ;
    switch (be32toh(*pub_exp)) {
    case RSA_3:
    case RSA_F4:
        break;
    default:
        return SOTER_INVALID_PARAMETER;
    }

    rsa = RSA_new();
    rsa_e = BN_new();
    rsa_d = BN_new();
    rsa_p = BN_new();
    rsa_q = BN_new();
    rsa_dmp1 = BN_new();
    rsa_dmq1 = BN_new();
    rsa_iqmp = BN_new();
    rsa_n = BN_new();
    if (!rsa || !rsa_e || !rsa_d || !rsa_p || !rsa_q || !rsa_dmp1 || !rsa_dmq1 || !rsa_iqmp || !rsa_n) {
        err = SOTER_NO_MEMORY;
        goto free_exponents;
    }

    if (!BN_set_word(rsa_e, be32toh(*pub_exp))) {
        goto free_exponents;
    }

    /* Private exponent */
    if (!BN_bin2bn(curr_bn, rsa_mod_size, rsa_d)) {
        goto free_exponents;
    }
    curr_bn += rsa_mod_size;

    /* p */
    if (!BN_bin2bn(curr_bn, rsa_mod_size / 2, rsa_p)) {
        goto free_exponents;
    }
    curr_bn += rsa_mod_size / 2;

    /* q */
    if (!BN_bin2bn(curr_bn, rsa_mod_size / 2, rsa_q)) {
        goto free_exponents;
    }
    curr_bn += rsa_mod_size / 2;

    /* dp */
    if (!BN_bin2bn(curr_bn, rsa_mod_size / 2, rsa_dmp1)) {
        goto free_exponents;
    }
    curr_bn += rsa_mod_size / 2;

    /* dq */
    if (!BN_bin2bn(curr_bn, rsa_mod_size / 2, rsa_dmq1)) {
        goto free_exponents;
    }
    curr_bn += rsa_mod_size / 2;

    /* qp */
    if (!BN_bin2bn(curr_bn, rsa_mod_size / 2, rsa_iqmp)) {
        goto free_exponents;
    }
    curr_bn += rsa_mod_size / 2;

    /* modulus */
    if (!BN_bin2bn(curr_bn, rsa_mod_size, rsa_n)) {
        goto free_exponents;
    }

    /* If at least one CRT parameter is zero, free them */
    if (BN_is_zero(rsa_p) || BN_is_zero(rsa_q) || BN_is_zero(rsa_dmp1) || BN_is_zero(rsa_dmq1)
        || BN_is_zero(rsa_iqmp)) {
        BN_free(rsa_p);
        rsa_p = NULL;

        BN_free(rsa_q);
        rsa_q = NULL;

        BN_free(rsa_dmp1);
        rsa_dmp1 = NULL;

        BN_free(rsa_dmq1);
        rsa_dmq1 = NULL;

        BN_free(rsa_iqmp);
        rsa_iqmp = NULL;
    }

    /* RSA_set0_*() functions transfer ownership over bignums to "rsa" */
    if (!RSA_set0_key(rsa, rsa_n, rsa_e, rsa_d)) {
        goto free_exponents;
    }
    if (!RSA_set0_factors(rsa, rsa_p, rsa_q)) {
        goto free_factors;
    }
    if (!RSA_set0_crt_params(rsa, rsa_dmp1, rsa_dmq1, rsa_iqmp)) {
        goto free_crt_params;
    }

    /* EVP_PKEY_assign_RSA() transfers ownership over "rsa" to "pkey" */
    if (!EVP_PKEY_assign_RSA(pkey, rsa)) {
        goto free_rsa;
    }

    return SOTER_SUCCESS;

free_exponents:
    BN_free(rsa_n);
    BN_free(rsa_e);
    BN_free(rsa_d);
free_factors:
    BN_free(rsa_p);
    BN_free(rsa_q);
free_crt_params:
    BN_free(rsa_dmp1);
    BN_free(rsa_dmq1);
    BN_free(rsa_iqmp);
free_rsa:
    RSA_free(rsa);
    return err;
}
