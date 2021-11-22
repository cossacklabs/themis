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

#include "soter/openssl/soter_ecdsa_common.h"
#include "soter/openssl/soter_engine.h"
#include "soter/soter_ec_key.h"

soter_status_t soter_verify_init_ecdsa_none_pkcs8(soter_sign_ctx_t* ctx,
                                                  const void* private_key,
                                                  const size_t private_key_length,
                                                  const void* public_key,
                                                  const size_t public_key_length)
{
    soter_status_t err = SOTER_FAIL;

    /* soter_sign_ctx_t should be initialized only once */
    if (!ctx || ctx->pkey || ctx->md_ctx) {
        return SOTER_INVALID_PARAMETER;
    }

    ctx->pkey = EVP_PKEY_new();
    if (!ctx->pkey) {
        return SOTER_NO_MEMORY;
    }

    if (EVP_PKEY_set_type(ctx->pkey, EVP_PKEY_EC) != 1) {
        goto free_pkey;
    }

    /* TODO: Review needed */
    if ((private_key) && (private_key_length)) {
        err = soter_ec_import_key(ctx->pkey, private_key, private_key_length);
        if (err != SOTER_SUCCESS) {
            goto free_pkey;
        }
    }
    if ((public_key) && (public_key_length)) {
        err = soter_ec_import_key(ctx->pkey, public_key, public_key_length);
        if (err != SOTER_SUCCESS) {
            goto free_pkey;
        }
    }

    ctx->md_ctx = EVP_MD_CTX_create();
    if (!(ctx->md_ctx)) {
        err = SOTER_NO_MEMORY;
        goto free_pkey;
    }

    if (EVP_DigestVerifyInit(ctx->md_ctx, NULL, EVP_sha256(), NULL, ctx->pkey) != 1) {
        goto free_md_ctx;
    }

    return SOTER_SUCCESS;

free_md_ctx:
    EVP_MD_CTX_destroy(ctx->md_ctx);
    ctx->md_ctx = NULL;
free_pkey:
    EVP_PKEY_free(ctx->pkey);
    ctx->pkey = NULL;
    return err;
}

soter_status_t soter_verify_update_ecdsa_none_pkcs8(soter_sign_ctx_t* ctx,
                                                    const void* data,
                                                    const size_t data_length)
{
    if (!ctx || !ctx->pkey) {
        return SOTER_INVALID_PARAMETER;
    }
    if (!data || data_length == 0) {
        return SOTER_INVALID_PARAMETER;
    }
    if (EVP_PKEY_base_id(ctx->pkey) != EVP_PKEY_EC) {
        return SOTER_INVALID_PARAMETER;
    }

    if (EVP_DigestVerifyUpdate(ctx->md_ctx, data, data_length) != 1) {
        return SOTER_FAIL;
    }
    return SOTER_SUCCESS;
}

/* TODO: Review needed */
soter_status_t soter_verify_final_ecdsa_none_pkcs8(soter_sign_ctx_t* ctx,
                                                   const void* signature,
                                                   const size_t signature_length)
{
    if (!ctx || !ctx->pkey) {
        return SOTER_INVALID_PARAMETER;
    }
    if (!signature || signature_length == 0) {
        return SOTER_INVALID_PARAMETER;
    }
    if (EVP_PKEY_base_id(ctx->pkey) != EVP_PKEY_EC) {
        return SOTER_INVALID_PARAMETER;
    }

    if (EVP_DigestVerifyFinal(ctx->md_ctx, (unsigned char*)signature, signature_length) != 1) {
        return SOTER_INVALID_SIGNATURE;
    }

    return SOTER_SUCCESS;
}

soter_status_t soter_verify_cleanup_ecdsa_none_pkcs8(soter_sign_ctx_t* ctx)
{
    if (!ctx) {
        return SOTER_INVALID_PARAMETER;
    }
    if (ctx->pkey) {
        EVP_PKEY_free(ctx->pkey);
        ctx->pkey = NULL;
    }
    if (ctx->md_ctx) {
        EVP_MD_CTX_destroy(ctx->md_ctx);
        ctx->md_ctx = NULL;
    }
    return SOTER_SUCCESS;
}
