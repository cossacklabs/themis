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
#include <openssl/err.h>
#include <openssl/evp.h>

#include "soter/openssl/soter_ecdsa_common.h"
#include "soter/openssl/soter_engine.h"
#include "soter/soter_ec_key.h"

soter_status_t soter_sign_init_ecdsa_none_pkcs8(soter_sign_ctx_t* ctx,
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

    if ((!private_key) && (!public_key)) {
        err = soter_ec_gen_key(&ctx->pkey);
        if (err != SOTER_SUCCESS) {
            goto free_pkey;
        }
    } else {
        ctx->pkey = EVP_PKEY_new();
        if (!ctx->pkey) {
            err = SOTER_NO_MEMORY;
            goto free_pkey;
        }

        if (!EVP_PKEY_set_type(ctx->pkey, EVP_PKEY_EC)) {
            err = SOTER_FAIL;
            goto free_pkey;
        }

        if (private_key != NULL) {
            err = soter_ec_import_key(ctx->pkey, private_key, private_key_length);
            if (err != SOTER_SUCCESS) {
                goto free_pkey;
            }
        }
        if (public_key != NULL) {
            err = soter_ec_import_key(ctx->pkey, public_key, public_key_length);
            if (err != SOTER_SUCCESS) {
                goto free_pkey;
            }
        }
    }

    ctx->md_ctx = EVP_MD_CTX_create();
    if (!(ctx->md_ctx)) {
        err = SOTER_NO_MEMORY;
        goto free_pkey;
    }

    if (EVP_DigestSignInit(ctx->md_ctx, NULL, EVP_sha256(), NULL, ctx->pkey) != 1) {
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

soter_status_t soter_sign_export_key_ecdsa_none_pkcs8(soter_sign_ctx_t* ctx,
                                                      void* key,
                                                      size_t* key_length,
                                                      bool isprivate)
{
    return soter_ec_export_key(ctx, key, key_length, isprivate);
}

soter_status_t soter_sign_update_ecdsa_none_pkcs8(soter_sign_ctx_t* ctx,
                                                  const void* data,
                                                  const size_t data_length)
{
    if (EVP_DigestSignUpdate(ctx->md_ctx, data, data_length) != 1) {
        return SOTER_FAIL;
    }
    return SOTER_SUCCESS;
}

soter_status_t soter_sign_final_ecdsa_none_pkcs8(soter_sign_ctx_t* ctx,
                                                 void* signature,
                                                 size_t* signature_length)
{
    EVP_PKEY* pkey = ctx->pkey;
    if (!pkey) {
        return SOTER_INVALID_PARAMETER;
    }
    if (EVP_PKEY_base_id(pkey) != EVP_PKEY_EC) {
        return SOTER_INVALID_PARAMETER;
    } /* TODO: need review */
    soter_status_t res = SOTER_SUCCESS;
    if (!signature || (*signature_length) < (size_t)EVP_PKEY_size(pkey)) {
        (*signature_length) = (size_t)EVP_PKEY_size(pkey);
        res = SOTER_BUFFER_TOO_SMALL;
    } else {
        if (EVP_DigestSignFinal(ctx->md_ctx, signature, signature_length) != 1) {
            res = SOTER_INVALID_SIGNATURE;
        }
    }
    return res;
}

soter_status_t soter_sign_cleanup_ecdsa_none_pkcs8(soter_sign_ctx_t* ctx)
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
    if (ctx->pkey_ctx) {
        EVP_PKEY_CTX_free(ctx->pkey_ctx);
        ctx->pkey_ctx = NULL;
    }
    return SOTER_SUCCESS;
}
