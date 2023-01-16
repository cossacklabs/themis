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
#include "soter/soter_sign_rsa.h"

#include "soter/soter_api.h"
#include "soter/soter_t.h"

SOTER_PRIVATE_API
soter_status_t soter_sign_init(soter_sign_ctx_t* ctx,
                               soter_sign_alg_t algId,
                               const void* private_key,
                               const size_t private_key_length,
                               const void* public_key,
                               const size_t public_key_length)
{
    if (!ctx) {
        return SOTER_INVALID_PARAMETER;
    }
    ctx->alg = algId;
    switch (ctx->alg) {
    case SOTER_SIGN_rsa_pss_pkcs8:
        return soter_sign_init_rsa_pss_pkcs8(ctx, private_key, private_key_length, public_key, public_key_length);
    case SOTER_SIGN_ecdsa_none_pkcs8:
        return soter_sign_init_ecdsa_none_pkcs8(ctx,
                                                private_key,
                                                private_key_length,
                                                public_key,
                                                public_key_length);
    default:
        return SOTER_INVALID_PARAMETER;
    }
    return SOTER_INVALID_PARAMETER;
}

SOTER_PRIVATE_API
soter_status_t soter_verify_init(soter_sign_ctx_t* ctx,
                                 soter_sign_alg_t algId,
                                 const void* private_key,
                                 const size_t private_key_length,
                                 const void* public_key,
                                 const size_t public_key_length)
{
    if (!ctx) {
        return SOTER_INVALID_PARAMETER;
    }
    ctx->alg = algId;
    switch (ctx->alg) {
    case SOTER_SIGN_rsa_pss_pkcs8:
        return soter_verify_init_rsa_pss_pkcs8(ctx,
                                               private_key,
                                               private_key_length,
                                               public_key,
                                               public_key_length);
    case SOTER_SIGN_ecdsa_none_pkcs8:
        return soter_verify_init_ecdsa_none_pkcs8(ctx,
                                                  private_key,
                                                  private_key_length,
                                                  public_key,
                                                  public_key_length);
    default:
        return SOTER_INVALID_PARAMETER;
    }
    return SOTER_INVALID_PARAMETER;
}

soter_status_t soter_sign_export_key(soter_sign_ctx_t* ctx, void* key, size_t* key_length, bool isprivate)
{
    if (!ctx) {
        return SOTER_INVALID_PARAMETER;
    }
    switch (ctx->alg) {
    case SOTER_SIGN_rsa_pss_pkcs8:
        return soter_sign_export_key_rsa_pss_pkcs8(ctx, key, key_length, isprivate);
    case SOTER_SIGN_ecdsa_none_pkcs8:
        if (isprivate) {
            return soter_sign_export_private_key_ecdsa_none_pkcs8(ctx, key, key_length);
        } else {
            return soter_sign_export_public_key_ecdsa_none_pkcs8(ctx, true, key, key_length);
        }
    default:
        return SOTER_INVALID_PARAMETER;
    }
    return SOTER_INVALID_PARAMETER;
}

soter_status_t soter_sign_export_private_key(const soter_sign_ctx_t* ctx, void* key, size_t* key_length)
{
    if (!ctx) {
        return SOTER_INVALID_PARAMETER;
    }
    switch (ctx->alg) {
    case SOTER_SIGN_rsa_pss_pkcs8:
        return soter_sign_export_key_rsa_pss_pkcs8(ctx, key, key_length, true);
    case SOTER_SIGN_ecdsa_none_pkcs8:
        return soter_sign_export_private_key_ecdsa_none_pkcs8(ctx, key, key_length);
    default:
        return SOTER_INVALID_PARAMETER;
    }
    return SOTER_INVALID_PARAMETER;
}

soter_status_t soter_sign_export_public_key(const soter_sign_ctx_t* ctx,
                                            bool compressed,
                                            void* key,
                                            size_t* key_length)
{
    if (!ctx) {
        return SOTER_INVALID_PARAMETER;
    }
    switch (ctx->alg) {
    case SOTER_SIGN_rsa_pss_pkcs8:
        return soter_sign_export_key_rsa_pss_pkcs8(ctx, key, key_length, false);
    case SOTER_SIGN_ecdsa_none_pkcs8:
        return soter_sign_export_public_key_ecdsa_none_pkcs8(ctx, compressed, key, key_length);
    default:
        return SOTER_INVALID_PARAMETER;
    }
    return SOTER_INVALID_PARAMETER;
}

soter_status_t soter_sign_update(soter_sign_ctx_t* ctx, const void* data, const size_t data_length)
{
    if (!ctx || !data || !data_length) {
        return SOTER_INVALID_PARAMETER;
    }
    switch (ctx->alg) {
    case SOTER_SIGN_rsa_pss_pkcs8:
        return soter_sign_update_rsa_pss_pkcs8(ctx, data, data_length);
    case SOTER_SIGN_ecdsa_none_pkcs8:
        return soter_sign_update_ecdsa_none_pkcs8(ctx, data, data_length);
    default:
        return SOTER_INVALID_PARAMETER;
    }
    return SOTER_INVALID_PARAMETER;
}

soter_status_t soter_verify_update(soter_sign_ctx_t* ctx, const void* data, const size_t data_length)
{
    if (!ctx || !data || !data_length) {
        return SOTER_INVALID_PARAMETER;
    }
    switch (ctx->alg) {
    case SOTER_SIGN_rsa_pss_pkcs8:
        return soter_verify_update_rsa_pss_pkcs8(ctx, data, data_length);
    case SOTER_SIGN_ecdsa_none_pkcs8:
        return soter_verify_update_ecdsa_none_pkcs8(ctx, data, data_length);
    default:
        return SOTER_INVALID_PARAMETER;
    }
    return SOTER_INVALID_PARAMETER;
}

soter_status_t soter_sign_final(soter_sign_ctx_t* ctx, void* signature, size_t* signature_length)
{
    if (!ctx) {
        return SOTER_INVALID_PARAMETER;
    }
    switch (ctx->alg) {
    case SOTER_SIGN_rsa_pss_pkcs8:
        return soter_sign_final_rsa_pss_pkcs8(ctx, signature, signature_length);
    case SOTER_SIGN_ecdsa_none_pkcs8:
        return soter_sign_final_ecdsa_none_pkcs8(ctx, signature, signature_length);
    default:
        return SOTER_INVALID_PARAMETER;
    }
    return SOTER_INVALID_PARAMETER;
}

soter_status_t soter_verify_final(soter_sign_ctx_t* ctx, const void* signature, const size_t signature_length)
{
    if (!ctx || !signature || !signature_length) {
        return SOTER_INVALID_PARAMETER;
    }
    switch (ctx->alg) {
    case SOTER_SIGN_rsa_pss_pkcs8:
        return soter_verify_final_rsa_pss_pkcs8(ctx, signature, signature_length);
    case SOTER_SIGN_ecdsa_none_pkcs8:
        return soter_verify_final_ecdsa_none_pkcs8(ctx, signature, signature_length);
    default:
        return SOTER_INVALID_PARAMETER;
    }
    return SOTER_INVALID_PARAMETER;
}

soter_status_t soter_sign_cleanup(soter_sign_ctx_t* ctx)
{
    if (!ctx) {
        return SOTER_INVALID_PARAMETER;
    }
    switch (ctx->alg) {
    case SOTER_SIGN_rsa_pss_pkcs8:
        return soter_sign_cleanup_rsa_pss_pkcs8(ctx);
    case SOTER_SIGN_ecdsa_none_pkcs8:
        return soter_sign_cleanup_ecdsa_none_pkcs8(ctx);
    default:
        return SOTER_INVALID_PARAMETER;
    }
    return SOTER_INVALID_PARAMETER;
}

soter_status_t soter_verify_cleanup(soter_sign_ctx_t* ctx)
{
    if (!ctx) {
        return SOTER_INVALID_PARAMETER;
    }
    switch (ctx->alg) {
    case SOTER_SIGN_rsa_pss_pkcs8:
        return soter_verify_cleanup_rsa_pss_pkcs8(ctx);
    case SOTER_SIGN_ecdsa_none_pkcs8:
        return soter_verify_cleanup_ecdsa_none_pkcs8(ctx);
    default:
        return SOTER_INVALID_PARAMETER;
    }
    return SOTER_INVALID_PARAMETER;
}

soter_sign_ctx_t* soter_sign_create(soter_sign_alg_t alg,
                                    const void* private_key,
                                    const size_t private_key_length,
                                    const void* public_key,
                                    const size_t public_key_length)
{
    soter_sign_ctx_t* ctx = calloc(sizeof(soter_sign_ctx_t), 1);
    if (!ctx) {
        return NULL;
    }
    if (soter_sign_init(ctx, alg, private_key, private_key_length, public_key, public_key_length)
        != SOTER_SUCCESS) {
        soter_sign_cleanup(ctx);
        free(ctx);
        return NULL;
    }
    return ctx;
}

soter_sign_ctx_t* soter_verify_create(soter_sign_alg_t alg,
                                      const void* private_key,
                                      const size_t private_key_length,
                                      const void* public_key,
                                      const size_t public_key_length)
{
    soter_sign_ctx_t* ctx = calloc(sizeof(soter_sign_ctx_t), 1);
    if (!ctx) {
        return NULL;
    }
    if (soter_verify_init(ctx, alg, private_key, private_key_length, public_key, public_key_length)
        != SOTER_SUCCESS) {
        soter_verify_destroy(ctx);
        return NULL;
    }
    return ctx;
}

soter_status_t soter_sign_destroy(soter_sign_ctx_t* ctx)
{
    if (!ctx) {
        return SOTER_INVALID_PARAMETER;
    }
    soter_sign_cleanup(ctx);
    free(ctx);
    return SOTER_SUCCESS;
}

soter_status_t soter_verify_destroy(soter_sign_ctx_t* ctx)
{
    if (!ctx) {
        return SOTER_INVALID_PARAMETER;
    }
    soter_verify_cleanup(ctx);
    free(ctx);
    return SOTER_SUCCESS;
}

soter_sign_alg_t soter_sign_get_alg_id(soter_sign_ctx_t* ctx)
{
    if (!ctx) {
        return SOTER_INVALID_PARAMETER;
    }
    return ctx->alg;
}

soter_sign_alg_t soter_verify_get_alg_id(soter_verify_ctx_t* ctx)
{
    if (!ctx) {
        return SOTER_INVALID_PARAMETER;
    }
    return ctx->alg;
}
