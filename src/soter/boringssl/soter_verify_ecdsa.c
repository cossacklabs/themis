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

#include "soter/soter_sign_ecdsa.h"

#include <openssl/ecdsa.h>
#include <openssl/evp.h>

#include "soter/boringssl/soter_ecdsa_common.h"
#include "soter/boringssl/soter_engine.h"
#include "soter/soter_ec_key.h"

soter_status_t soter_verify_init_ecdsa_none_pkcs8(soter_sign_ctx_t* ctx,
                                                  const void* private_key,
                                                  const size_t private_key_length,
                                                  const void* public_key,
                                                  const size_t public_key_length)
{
    soter_status_t err = SOTER_FAIL;
    EVP_PKEY* pkey = NULL;

    pkey = EVP_PKEY_new();
    if (!pkey) {
        return SOTER_NO_MEMORY;
    }

    if (!EVP_PKEY_set_type(pkey, EVP_PKEY_EC)) {
        goto free_pkey;
    }

    ctx->pkey_ctx = EVP_PKEY_CTX_new(pkey, NULL);
    if (!(ctx->pkey_ctx)) {
        err = SOTER_NO_MEMORY;
        goto free_pkey;
    }

    /* TODO: Review needed */
    if ((private_key) && (private_key_length)) {
        err = soter_ec_import_key(pkey, private_key, private_key_length);
        if (err != SOTER_SUCCESS) {
            goto free_pkey_ctx;
        }
    }
    if ((public_key) && (public_key_length)) {
        err = soter_ec_import_key(pkey, public_key, public_key_length);
        if (err != SOTER_SUCCESS) {
            goto free_pkey_ctx;
        }
    }

    ctx->md_ctx = EVP_MD_CTX_create();
    if (!(ctx->md_ctx)) {
        err = SOTER_NO_MEMORY;
        goto free_pkey_ctx;
    }

    if (!EVP_DigestVerifyInit(ctx->md_ctx, NULL, EVP_sha256(), NULL, pkey)) {
        goto free_md_ctx;
    }

    EVP_PKEY_free(pkey);
    return SOTER_SUCCESS;

free_md_ctx:
    EVP_MD_CTX_destroy(ctx->md_ctx);
    ctx->md_ctx = NULL;
free_pkey_ctx:
    EVP_PKEY_CTX_free(ctx->pkey_ctx);
    ctx->pkey_ctx = NULL;
free_pkey:
    EVP_PKEY_free(pkey);
    return err;
}

soter_status_t soter_verify_update_ecdsa_none_pkcs8(soter_sign_ctx_t* ctx,
                                                    const void* data,
                                                    const size_t data_length)
{
    if (!EVP_DigestVerifyUpdate(ctx->md_ctx, data, data_length)) {
        return SOTER_FAIL;
    }
    return SOTER_SUCCESS;
}

/* TODO: Review needed */
soter_status_t soter_verify_final_ecdsa_none_pkcs8(soter_sign_ctx_t* ctx,
                                                   const void* signature,
                                                   const size_t signature_length)
{
    if (!ctx) {
        return SOTER_INVALID_PARAMETER;
    }
    EVP_PKEY* pkey = EVP_PKEY_CTX_get0_pkey(ctx->pkey_ctx);
    if (!pkey) {
        return SOTER_INVALID_PARAMETER;
    }

    switch (EVP_DigestVerifyFinal(ctx->md_ctx, (unsigned char*)signature, signature_length)) {
    case 0:
        return SOTER_INVALID_SIGNATURE;
    case 1:
        return SOTER_SUCCESS;
    default:
        return SOTER_INVALID_SIGNATURE;
    }
}

soter_status_t soter_verify_cleanup_ecdsa_none_pkcs8(soter_sign_ctx_t* ctx)
{
    if (!ctx) {
        return SOTER_INVALID_PARAMETER;
    }
    if (ctx->pkey_ctx) {
        EVP_PKEY_CTX_free(ctx->pkey_ctx);
        ctx->pkey_ctx = NULL;
    }
    if (ctx->md_ctx) {
        EVP_MD_CTX_destroy(ctx->md_ctx);
        ctx->md_ctx = NULL;
    }
    return SOTER_SUCCESS;
}
