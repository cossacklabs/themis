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

#include "soter/soter_sign_rsa.h"

#include <openssl/evp.h>
#include <openssl/opensslv.h>
#include <openssl/rsa.h>
#if OPENSSL_VERSION_NUMBER >= 0x30000000
#include <openssl/core_names.h>
#endif

#include "soter/openssl/soter_engine.h"
#include "soter/openssl/soter_rsa_common.h"
#include "soter/soter_rsa_key.h"

soter_status_t soter_sign_init_rsa_pss_pkcs8(soter_sign_ctx_t* ctx,
                                             const void* private_key,
                                             const size_t private_key_length,
                                             const void* public_key,
                                             const size_t public_key_length)
{
    soter_status_t err = SOTER_FAIL;
    EVP_PKEY_CTX* md_pkey_ctx = NULL;

    /* soter_sign_ctx_t should be initialized only once */
    if (!ctx || ctx->pkey || ctx->md_ctx) {
        return SOTER_INVALID_PARAMETER;
    }

    if ((!private_key) && (!public_key)) {
        err = soter_rsa_gen_key(&ctx->pkey);
        if (err != SOTER_SUCCESS) {
            goto free_pkey;
        }
    } else {
        ctx->pkey = EVP_PKEY_new();
        if (!ctx->pkey) {
            err = SOTER_NO_MEMORY;
            goto free_pkey;
        }

        if (EVP_PKEY_set_type(ctx->pkey, EVP_PKEY_RSA) != 1) {
            err = SOTER_FAIL;
            goto free_pkey;
        }

        if (private_key != NULL) {
            err = soter_rsa_import_key(ctx->pkey, private_key, private_key_length);
            if (err != SOTER_SUCCESS) {
                goto free_pkey;
            }
        }
        if (public_key != NULL) {
            err = soter_rsa_import_key(ctx->pkey, public_key, public_key_length);
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

    /* md_pkey_ctx is owned by ctx->md_ctx */
    if (EVP_DigestSignInit(ctx->md_ctx, &md_pkey_ctx, EVP_sha256(), NULL, ctx->pkey) != 1) {
        goto free_md_ctx;
    }
    if (EVP_PKEY_CTX_set_rsa_padding(md_pkey_ctx, RSA_PKCS1_PSS_PADDING) != 1) {
        goto free_md_ctx;
    }
    if (EVP_PKEY_CTX_set_rsa_pss_saltlen(md_pkey_ctx, -2) != 1) {
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

soter_status_t soter_sign_export_key_rsa_pss_pkcs8(const soter_sign_ctx_t* ctx,
                                                   void* key,
                                                   size_t* key_length,
                                                   bool isprivate)
{
    if (!ctx) {
        return SOTER_INVALID_PARAMETER;
    }
    if (EVP_PKEY_base_id(ctx->pkey) != EVP_PKEY_RSA) {
        return SOTER_INVALID_PARAMETER;
    }
    return soter_rsa_export_key(ctx->pkey, key, key_length, isprivate);
}

soter_status_t soter_sign_update_rsa_pss_pkcs8(soter_sign_ctx_t* ctx,
                                               const void* data,
                                               const size_t data_length)
{
    if (!ctx || !ctx->pkey) {
        return SOTER_INVALID_PARAMETER;
    }
    if (!data || data_length == 0) {
        return SOTER_INVALID_PARAMETER;
    }
    if (EVP_PKEY_base_id(ctx->pkey) != EVP_PKEY_RSA) {
        return SOTER_INVALID_PARAMETER;
    }

    if (EVP_DigestSignUpdate(ctx->md_ctx, data, data_length) != 1) {
        return SOTER_FAIL;
    }
    return SOTER_SUCCESS;
}

soter_status_t soter_sign_final_rsa_pss_pkcs8(soter_sign_ctx_t* ctx, void* signature, size_t* signature_length)
{
    int key_size = 0;

    if (!ctx || !ctx->pkey) {
        return SOTER_INVALID_PARAMETER;
    }
    if (!signature_length) {
        return SOTER_INVALID_PARAMETER;
    }
    if (EVP_PKEY_base_id(ctx->pkey) != EVP_PKEY_RSA) {
        return SOTER_INVALID_PARAMETER;
    }

#if OPENSSL_VERSION_NUMBER >= 0x30000000
    // Even though EVP_PKEY_size() is not deprecated in OpenSSL 3, it works incorrectly (returns
    // zero or even crashes) when given a pkey generated with new EVP_PKEY_fromdata() function.
    // Using different method instead.
    if (!EVP_PKEY_get_int_param(ctx->pkey, OSSL_PKEY_PARAM_MAX_SIZE, &key_size)) {
        return SOTER_FAIL;
    }
#else
    key_size = EVP_PKEY_size(ctx->pkey);
#endif
    if (key_size <= 0) {
        return SOTER_FAIL;
    }
    if (!signature || (*signature_length) < (size_t)key_size) {
        (*signature_length) = (size_t)key_size;
        return SOTER_BUFFER_TOO_SMALL;
    }

    if (EVP_DigestSignFinal(ctx->md_ctx, signature, signature_length) != 1) {
        return SOTER_FAIL;
    }
    return SOTER_SUCCESS;
}

soter_status_t soter_sign_cleanup_rsa_pss_pkcs8(soter_sign_ctx_t* ctx)
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
