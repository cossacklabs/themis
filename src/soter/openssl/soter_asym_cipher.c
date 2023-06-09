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

#include "soter/soter_asym_cipher.h"

#include <openssl/evp.h>
#include <openssl/opensslv.h>
#include <openssl/rsa.h>
#if OPENSSL_VERSION_NUMBER >= 0x30000000
#include <openssl/core_names.h>
#endif

#include "soter/openssl/soter_engine.h"
#include "soter/soter_api.h"
#include "soter/soter_rsa_key.h"

/* We use only SHA1 for now */
#define OAEP_HASH_SIZE 20

#ifndef SOTER_RSA_KEY_LENGTH
#define SOTER_RSA_KEY_LENGTH 2048
#endif

#if OPENSSL_VERSION_NUMBER >= 0x30000000
static bool get_rsa_mod_size(EVP_PKEY* pkey, int* rsa_mod_size)
{
    if (!EVP_PKEY_get_int_param(pkey, OSSL_PKEY_PARAM_BITS, rsa_mod_size)) {
        return false;
    }
    *rsa_mod_size = (*rsa_mod_size + 7) / 8;
    return true;
}
#else
static bool get_rsa_mod_size(EVP_PKEY* pkey, int* rsa_mod_size)
{
    RSA* rsa = EVP_PKEY_get0(pkey);
    if (!rsa) {
        return false;
    }
    *rsa_mod_size = RSA_size(rsa);
    return true;
}
#endif

#if OPENSSL_VERSION_NUMBER < 0x10100000L
static bool check_rsa_private_key(EVP_PKEY* pkey, EVP_PKEY_CTX* pkey_ctx)
{
    UNUSED(pkey_ctx);
    RSA* rsa = EVP_PKEY_get0(pkey);

    if (rsa->d == NULL) {
        return false;
    }

    return true;
}
#elif OPENSSL_VERSION_NUMBER < 0x30000000
static bool check_rsa_private_key(EVP_PKEY* pkey, EVP_PKEY_CTX* pkey_ctx)
{
    UNUSED(pkey_ctx);
    RSA* rsa = EVP_PKEY_get0(pkey);
    const BIGNUM* d = NULL;

    RSA_get0_key(rsa, NULL, NULL, &d);

    if (NULL == d) {
        return false;
    }

    return true;
}
#else /* OPENSSL_VERSION_NUMBER >= 0x30000000 */
static bool check_rsa_private_key(EVP_PKEY* pkey, EVP_PKEY_CTX* pkey_ctx)
{
    UNUSED(pkey);
    // EVP_PKEY_private_check was added in 3.0
    return EVP_PKEY_private_check(pkey_ctx) == 1;
}
#endif

soter_status_t soter_asym_cipher_import_key(soter_asym_cipher_t* asym_cipher_ctx,
                                            const void* key,
                                            size_t key_length)
{
    const soter_container_hdr_t* hdr = key;
    EVP_PKEY* pkey;

    if (!asym_cipher_ctx) {
        return SOTER_INVALID_PARAMETER;
    }

    pkey = EVP_PKEY_CTX_get0_pkey(asym_cipher_ctx->pkey_ctx);

    if (!pkey) {
        return SOTER_INVALID_PARAMETER;
    }

    if (EVP_PKEY_RSA != EVP_PKEY_id(pkey)) {
        /* We can only do asymmetric encryption with RSA algorithm */
        return SOTER_INVALID_PARAMETER;
    }

    if ((!key) || (key_length < sizeof(soter_container_hdr_t))) {
        return SOTER_INVALID_PARAMETER;
    }

    switch (hdr->tag[0]) {
    case 'R':
        return soter_rsa_priv_key_to_engine_specific(hdr,
                                                     key_length,
                                                     ((soter_engine_specific_rsa_key_t**)&pkey));
    case 'U':
        return soter_rsa_pub_key_to_engine_specific(hdr,
                                                    key_length,
                                                    ((soter_engine_specific_rsa_key_t**)&pkey));
    default:
        return SOTER_INVALID_PARAMETER;
    }
}

/* Padding is ignored. We use OAEP by default. Parameter is to support more paddings in the future
 */
SOTER_PRIVATE_API
soter_status_t soter_asym_cipher_init(soter_asym_cipher_t* asym_cipher,
                                      const void* key,
                                      const size_t key_length,
                                      soter_asym_cipher_padding_t pad)
{
    soter_status_t err = SOTER_FAIL;
    EVP_PKEY* pkey = NULL;

    if ((!asym_cipher) || (SOTER_ASYM_CIPHER_OAEP != pad)) {
        return SOTER_INVALID_PARAMETER;
    }

    pkey = EVP_PKEY_new();
    if (!pkey) {
        return SOTER_NO_MEMORY;
    }

    /* Only RSA supports asymmetric encryption */
    if (!EVP_PKEY_set_type(pkey, EVP_PKEY_RSA)) {
        goto free_pkey;
    }

    asym_cipher->pkey_ctx = EVP_PKEY_CTX_new(pkey, NULL);
    if (!(asym_cipher->pkey_ctx)) {
        err = SOTER_NO_MEMORY;
        goto free_pkey;
    }

    err = soter_asym_cipher_import_key(asym_cipher, key, key_length);
    if (err != SOTER_SUCCESS) {
        goto free_pkey_ctx;
    }

    EVP_PKEY_free(pkey);
    return SOTER_SUCCESS;

free_pkey_ctx:
    EVP_PKEY_CTX_free(asym_cipher->pkey_ctx);
    asym_cipher->pkey_ctx = NULL;
free_pkey:
    EVP_PKEY_free(pkey);
    return err;
}

SOTER_PRIVATE_API
soter_status_t soter_asym_cipher_cleanup(soter_asym_cipher_t* asym_cipher)
{
    if (!asym_cipher) {
        return SOTER_INVALID_PARAMETER;
    }

    if (asym_cipher->pkey_ctx) {
        EVP_PKEY_CTX_free(asym_cipher->pkey_ctx);
        asym_cipher->pkey_ctx = NULL;
    }

    return SOTER_SUCCESS;
}

soter_status_t soter_asym_cipher_encrypt(soter_asym_cipher_t* asym_cipher,
                                         const void* plain_data,
                                         size_t plain_data_length,
                                         void* cipher_data,
                                         size_t* cipher_data_length)
{
    EVP_PKEY* pkey;
    int rsa_mod_size;
    size_t output_length;

    if ((!asym_cipher) || (!plain_data) || (0 == plain_data_length) || (!cipher_data_length)) {
        return SOTER_INVALID_PARAMETER;
    }

    pkey = EVP_PKEY_CTX_get0_pkey(asym_cipher->pkey_ctx);

    if (!pkey) {
        return SOTER_INVALID_PARAMETER;
    }

    if (EVP_PKEY_RSA != EVP_PKEY_id(pkey)) {
        /* We can only do asymmetric encryption with RSA algorithm */
        return SOTER_INVALID_PARAMETER;
    }

    if (!get_rsa_mod_size(pkey, &rsa_mod_size)) {
        return SOTER_FAIL;
    }

    int oaep_max_payload_length = (rsa_mod_size - 2 - (2 * OAEP_HASH_SIZE));
    if (oaep_max_payload_length < 0 || plain_data_length > (size_t)oaep_max_payload_length) {
        /* The plaindata is too large for this key size */
        return SOTER_INVALID_PARAMETER;
    }

    /* Currently we support only OAEP padding for RSA encryption */
    /* TODO: should we support "no padding" or PKCS1.5 padding? */
    if (!EVP_PKEY_encrypt_init(asym_cipher->pkey_ctx)) {
        return SOTER_FAIL;
    }

    /* TODO: This function automatically sets SHA1 as MGF digest for OAEP padding. Should we change
     * it to SHA256? */
    if (1 > EVP_PKEY_CTX_ctrl(asym_cipher->pkey_ctx,
                              -1,
                              -1,
                              EVP_PKEY_CTRL_RSA_PADDING,
                              RSA_PKCS1_OAEP_PADDING,
                              NULL)) {
        return SOTER_FAIL;
    }

    /* We will check the size of the output buffer first */
    if (EVP_PKEY_encrypt(asym_cipher->pkey_ctx,
                         NULL,
                         &output_length,
                         (const unsigned char*)plain_data,
                         plain_data_length)) {
        if (output_length > *cipher_data_length) {
            *cipher_data_length = output_length;
            return SOTER_BUFFER_TOO_SMALL;
        }
    } else {
        return SOTER_FAIL;
    }

    if (EVP_PKEY_encrypt(asym_cipher->pkey_ctx,
                         (unsigned char*)cipher_data,
                         cipher_data_length,
                         (const unsigned char*)plain_data,
                         plain_data_length)) {
        if (cipher_data) {
            return SOTER_SUCCESS;
        }

        return SOTER_BUFFER_TOO_SMALL;
    }

    return SOTER_FAIL;
}

soter_status_t soter_asym_cipher_decrypt(soter_asym_cipher_t* asym_cipher,
                                         const void* cipher_data,
                                         size_t cipher_data_length,
                                         void* plain_data,
                                         size_t* plain_data_length)
{
    EVP_PKEY* pkey;
    int rsa_mod_size;
    size_t output_length;

    if ((!asym_cipher) || (!cipher_data) || (0 == cipher_data_length) || (!plain_data_length)) {
        return SOTER_INVALID_PARAMETER;
    }

    pkey = EVP_PKEY_CTX_get0_pkey(asym_cipher->pkey_ctx);

    if (!pkey) {
        return SOTER_INVALID_PARAMETER;
    }

    if (EVP_PKEY_RSA != EVP_PKEY_id(pkey)) {
        /* We can only do asymmetric encryption with RSA algorithm */
        return SOTER_INVALID_PARAMETER;
    }

    if (!get_rsa_mod_size(pkey, &rsa_mod_size)) {
        return SOTER_FAIL;
    }

    if (rsa_mod_size < 0 || cipher_data_length < (size_t)rsa_mod_size) {
        /* The cipherdata is too small for this key size */
        return SOTER_INVALID_PARAMETER;
    }

    /* we can only decrypt, if we have the private key */
    /* some versions of OpenSSL just crash, if you send RSA public key to EVP_PKEY_decrypt, so we do
     * checks here */
    if (!check_rsa_private_key(pkey, asym_cipher->pkey_ctx)) {
        return SOTER_INVALID_PARAMETER;
    }

    /* Currently we support only OAEP padding for RSA encryption */
    /* TODO: should we support "no padding" or PKCS1.5 padding? */
    if (!EVP_PKEY_decrypt_init(asym_cipher->pkey_ctx)) {
        return SOTER_FAIL;
    }

    /* TODO: This function automatically sets SHA1 as MGF digest for OAEP padding. Should we change
     * it to SHA256? */
    if (1 > EVP_PKEY_CTX_ctrl(asym_cipher->pkey_ctx,
                              -1,
                              -1,
                              EVP_PKEY_CTRL_RSA_PADDING,
                              RSA_PKCS1_OAEP_PADDING,
                              NULL)) {
        return SOTER_FAIL;
    }

    /* We will check the size of the output buffer first */
    if (EVP_PKEY_decrypt(asym_cipher->pkey_ctx,
                         NULL,
                         &output_length,
                         (const unsigned char*)cipher_data,
                         cipher_data_length)) {
        if (output_length > *plain_data_length) {
            *plain_data_length = output_length;
            return SOTER_BUFFER_TOO_SMALL;
        }
    } else {
        return SOTER_FAIL;
    }

    if (EVP_PKEY_decrypt(asym_cipher->pkey_ctx,
                         (unsigned char*)plain_data,
                         plain_data_length,
                         (const unsigned char*)cipher_data,
                         cipher_data_length)) {
        if (plain_data) {
            return SOTER_SUCCESS;
        }

        return SOTER_BUFFER_TOO_SMALL;
    }

    return SOTER_FAIL;
}

soter_asym_cipher_t* soter_asym_cipher_create(const void* key,
                                              const size_t key_length,
                                              soter_asym_cipher_padding_t pad)
{
    soter_status_t status;
    soter_asym_cipher_t* ctx = malloc(sizeof(soter_asym_cipher_t));
    if (!ctx) {
        return NULL;
    }
    ctx->pkey_ctx = NULL;

    status = soter_asym_cipher_init(ctx, key, key_length, pad);
    if (SOTER_SUCCESS == status) {
        return ctx;
    }

    soter_asym_cipher_destroy(ctx);
    return NULL;
}

soter_status_t soter_asym_cipher_destroy(soter_asym_cipher_t* asym_cipher)
{
    soter_status_t status;

    if (!asym_cipher) {
        return SOTER_INVALID_PARAMETER;
    }

    status = soter_asym_cipher_cleanup(asym_cipher);
    free(asym_cipher);
    if (SOTER_SUCCESS == status) {
        return SOTER_SUCCESS;
    }

    return status;
}
