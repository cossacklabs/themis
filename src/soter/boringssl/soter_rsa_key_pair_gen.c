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

#include "soter/soter_rsa_key_pair_gen.h"

#include <string.h>

#include <openssl/evp.h>
#include <openssl/rsa.h>

#include "soter/boringssl/soter_engine.h"
#include "soter/boringssl/soter_rsa_common.h"

soter_rsa_key_pair_gen_t* soter_rsa_key_pair_gen_create(const unsigned key_length)
{
    SOTER_CHECK_PARAM_(rsa_key_length(key_length) > 0);
    soter_rsa_key_pair_gen_t* ctx = malloc(sizeof(soter_rsa_key_pair_gen_t));
    SOTER_CHECK_MALLOC_(ctx);
    SOTER_IF_FAIL_(soter_rsa_key_pair_gen_init(ctx, key_length) == SOTER_SUCCESS, free(ctx));
    return ctx;
}

soter_status_t soter_rsa_key_pair_gen_init(soter_rsa_key_pair_gen_t* ctx, const unsigned key_length)
{
    soter_status_t err = SOTER_FAIL;
    EVP_PKEY* pkey = NULL;

    pkey = EVP_PKEY_new();
    if (!pkey) {
        return SOTER_NO_MEMORY;
    }

    /* Only RSA supports asymmetric encryption */
    if (EVP_PKEY_set_type(pkey, EVP_PKEY_RSA) != 1) {
        goto free_pkey;
    }

    ctx->pkey_ctx = EVP_PKEY_CTX_new(pkey, NULL);
    if (!ctx->pkey_ctx) {
        err = SOTER_NO_MEMORY;
        goto free_pkey;
    }

    err = soter_rsa_gen_key(ctx->pkey_ctx, key_length);
    if (err != SOTER_SUCCESS) {
        goto free_pkey_ctx;
    }

    EVP_PKEY_free(pkey);
    return SOTER_SUCCESS;

free_pkey_ctx:
    EVP_PKEY_CTX_free(ctx->pkey_ctx);
    ctx->pkey_ctx = NULL;
free_pkey:
    EVP_PKEY_free(pkey);
    return err;
}

soter_status_t soter_rsa_key_pair_gen_cleanup(soter_rsa_key_pair_gen_t* ctx)
{
    SOTER_CHECK_PARAM(ctx);
    if (ctx->pkey_ctx) {
        EVP_PKEY_CTX_free(ctx->pkey_ctx);
        ctx->pkey_ctx = NULL;
    }
    return SOTER_SUCCESS;
}

soter_status_t soter_rsa_key_pair_gen_destroy(soter_rsa_key_pair_gen_t* ctx)
{
    SOTER_CHECK_PARAM(ctx);
    SOTER_CHECK(soter_rsa_key_pair_gen_cleanup(ctx) == SOTER_SUCCESS);
    free(ctx);
    return SOTER_SUCCESS;
}

soter_status_t soter_rsa_key_pair_gen_export_key(soter_rsa_key_pair_gen_t* ctx,
                                                 void* key,
                                                 size_t* key_length,
                                                 bool isprivate)
{
    EVP_PKEY* pkey;
    SOTER_CHECK_PARAM(ctx);
    pkey = EVP_PKEY_CTX_get0_pkey(ctx->pkey_ctx);
    SOTER_CHECK_PARAM(pkey);
    SOTER_CHECK_PARAM(EVP_PKEY_RSA == EVP_PKEY_id(pkey));
    if (isprivate) {
        return soter_engine_specific_to_rsa_priv_key((const soter_engine_specific_rsa_key_t*)pkey,
                                                     (soter_container_hdr_t*)key,
                                                     key_length);
    }

    return soter_engine_specific_to_rsa_pub_key((const soter_engine_specific_rsa_key_t*)pkey,
                                                (soter_container_hdr_t*)key,
                                                key_length);
}
