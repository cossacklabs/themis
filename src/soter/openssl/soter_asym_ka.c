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

#include "soter/soter_asym_ka.h"

#include <openssl/ec.h>

#include "soter/openssl/soter_engine.h"
#include "soter/soter_api.h"
#include "soter/soter_ec_key.h"

static int soter_alg_to_curve_nid(soter_asym_ka_alg_t alg)
{
    switch (alg) {
    case SOTER_ASYM_KA_EC_P256:
        return NID_X9_62_prime256v1;
    default:
        return 0;
    }
}

SOTER_PRIVATE_API
soter_status_t soter_asym_ka_init(soter_asym_ka_t* asym_ka_ctx, soter_asym_ka_alg_t alg)
{
    soter_status_t res = SOTER_FAIL;
    EVP_PKEY_CTX* param_ctx = NULL;
    int nid = soter_alg_to_curve_nid(alg);

    if ((!asym_ka_ctx) || (0 == nid)) {
        return SOTER_INVALID_PARAMETER;
    }

    param_ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, NULL);
    if (!param_ctx) {
        res = SOTER_NO_MEMORY;
        goto err;
    }

    if (1 != EVP_PKEY_paramgen_init(param_ctx)) {
        res = SOTER_FAIL;
        goto err;
    }
    if (1 != EVP_PKEY_CTX_set_ec_paramgen_curve_nid(param_ctx, nid)) {
        res = SOTER_FAIL;
        goto err;
    }
    if (1 != EVP_PKEY_paramgen(param_ctx, &asym_ka_ctx->param)) {
        res = SOTER_FAIL;
        goto err;
    }

    res = SOTER_SUCCESS;

err:
    EVP_PKEY_CTX_free(param_ctx);

    return res;
}

SOTER_PRIVATE_API
soter_status_t soter_asym_ka_cleanup(soter_asym_ka_t* asym_ka_ctx)
{
    if (!asym_ka_ctx) {
        return SOTER_INVALID_PARAMETER;
    }
    if (asym_ka_ctx->param) {
        EVP_PKEY_free(asym_ka_ctx->param);
        asym_ka_ctx->param = NULL;
    }
    if (asym_ka_ctx->pkey) {
        EVP_PKEY_free(asym_ka_ctx->pkey);
        asym_ka_ctx->pkey = NULL;
    }
    return SOTER_SUCCESS;
}

soter_asym_ka_t* soter_asym_ka_create(soter_asym_ka_alg_t alg)
{
    soter_status_t status;
    soter_asym_ka_t* ctx = calloc(1, sizeof(*ctx));
    if (!ctx) {
        return NULL;
    }

    status = soter_asym_ka_init(ctx, alg);
    if (SOTER_SUCCESS == status) {
        return ctx;
    }

    free(ctx);
    return NULL;
}

soter_status_t soter_asym_ka_destroy(soter_asym_ka_t* asym_ka_ctx)
{
    soter_status_t status;

    if (!asym_ka_ctx) {
        return SOTER_INVALID_PARAMETER;
    }

    status = soter_asym_ka_cleanup(asym_ka_ctx);
    if (SOTER_SUCCESS == status) {
        free(asym_ka_ctx);
        return SOTER_SUCCESS;
    }

    return status;
}

soter_status_t soter_asym_ka_gen_key(soter_asym_ka_t* asym_ka_ctx)
{
    soter_status_t res = SOTER_FAIL;
    EVP_PKEY_CTX* pkey_ctx = NULL;

    if (!asym_ka_ctx || !asym_ka_ctx->param) {
        return SOTER_INVALID_PARAMETER;
    }

    pkey_ctx = EVP_PKEY_CTX_new(asym_ka_ctx->param, NULL);
    if (!pkey_ctx) {
        return SOTER_NO_MEMORY;
    }

    if (EVP_PKEY_keygen_init(pkey_ctx) != 1) {
        res = SOTER_FAIL;
        goto err;
    }

    if (EVP_PKEY_keygen(pkey_ctx, &asym_ka_ctx->pkey) != 1) {
        res = SOTER_FAIL;
        goto err;
    }

    res = SOTER_SUCCESS;

err:
    EVP_PKEY_CTX_free(pkey_ctx);

    return res;
}

soter_status_t soter_asym_ka_import_key(soter_asym_ka_t* asym_ka_ctx, const void* key, size_t key_length)
{
    const soter_container_hdr_t* hdr = key;

    if ((!asym_ka_ctx) || (!key)) {
        return SOTER_INVALID_PARAMETER;
    }

    if (key_length < sizeof(soter_container_hdr_t)) {
        return SOTER_INVALID_PARAMETER;
    }

    /*
     * soter_ec_{priv,pub}_key_to_engine_specific() expect EVP_PKEY of EVP_PKEY_EC type
     * to be already allocated and non-NULL. We might be importing it anew, or we might be
     * replacing previously generated key pair.
     */
    if (asym_ka_ctx->pkey) {
        if (EVP_PKEY_base_id(asym_ka_ctx->pkey) != EVP_PKEY_EC) {
            return SOTER_INVALID_PARAMETER;
        }
    } else {
        asym_ka_ctx->pkey = EVP_PKEY_new();
        if (!asym_ka_ctx->pkey) {
            return SOTER_NO_MEMORY;
        }

        if (EVP_PKEY_set_type(asym_ka_ctx->pkey, EVP_PKEY_EC) != 1) {
            EVP_PKEY_free(asym_ka_ctx->pkey);
            asym_ka_ctx->pkey = NULL;
            return SOTER_FAIL;
        }
    }

    switch (hdr->tag[0]) {
    case 'R':
        return soter_ec_priv_key_to_engine_specific(hdr,
                                                    key_length,
                                                    ((soter_engine_specific_ec_key_t**)&asym_ka_ctx->pkey));
    case 'U':
        return soter_ec_pub_key_to_engine_specific(hdr,
                                                   key_length,
                                                   ((soter_engine_specific_ec_key_t**)&asym_ka_ctx->pkey));
    default:
        return SOTER_INVALID_PARAMETER;
    }
}

soter_status_t soter_asym_ka_export_key(soter_asym_ka_t* asym_ka_ctx,
                                        void* key,
                                        size_t* key_length,
                                        bool isprivate)
{
    if (!asym_ka_ctx || !asym_ka_ctx->pkey) {
        return SOTER_INVALID_PARAMETER;
    }
    if (EVP_PKEY_base_id(asym_ka_ctx->pkey) != EVP_PKEY_EC) {
        return SOTER_INVALID_PARAMETER;
    }

    if (isprivate) {
        return soter_engine_specific_to_ec_priv_key((const soter_engine_specific_ec_key_t*)
                                                        asym_ka_ctx->pkey,
                                                    (soter_container_hdr_t*)key,
                                                    key_length);
    }

    return soter_engine_specific_to_ec_pub_key((const soter_engine_specific_ec_key_t*)asym_ka_ctx->pkey,
                                               true,
                                               (soter_container_hdr_t*)key,
                                               key_length);
}

soter_status_t soter_asym_ka_derive(soter_asym_ka_t* asym_ka_ctx,
                                    const void* peer_key,
                                    size_t peer_key_length,
                                    void* shared_secret,
                                    size_t* shared_secret_length)
{
    soter_status_t res = SOTER_FAIL;
    EVP_PKEY* peer_pkey = NULL;
    EVP_PKEY_CTX* derive_ctx = NULL;
    size_t out_length = 0;

    if (!asym_ka_ctx || !asym_ka_ctx->pkey) {
        return SOTER_INVALID_PARAMETER;
    }
    if (!peer_key || peer_key_length == 0 || !shared_secret_length) {
        return SOTER_INVALID_PARAMETER;
    }
    if (EVP_PKEY_base_id(asym_ka_ctx->pkey) != EVP_PKEY_EC) {
        return SOTER_INVALID_PARAMETER;
    }

    peer_pkey = EVP_PKEY_new();
    if (NULL == peer_pkey) {
        return SOTER_NO_MEMORY;
    }

    res = soter_ec_pub_key_to_engine_specific((const soter_container_hdr_t*)peer_key,
                                              peer_key_length,
                                              ((soter_engine_specific_ec_key_t**)&peer_pkey));
    if (SOTER_SUCCESS != res) {
        goto err;
    }

    derive_ctx = EVP_PKEY_CTX_new(asym_ka_ctx->pkey, NULL);
    if (!derive_ctx) {
        res = SOTER_NO_MEMORY;
        goto err;
    }

    if (1 != EVP_PKEY_derive_init(derive_ctx)) {
        res = SOTER_FAIL;
        goto err;
    }

    if (1 != EVP_PKEY_derive_set_peer(derive_ctx, peer_pkey)) {
        res = SOTER_FAIL;
        goto err;
    }

    if (1 != EVP_PKEY_derive(derive_ctx, NULL, &out_length)) {
        res = SOTER_FAIL;
        goto err;
    }

    if (!shared_secret || out_length > *shared_secret_length) {
        *shared_secret_length = out_length;
        res = SOTER_BUFFER_TOO_SMALL;
        goto err;
    }

    if (1 != EVP_PKEY_derive(derive_ctx, (unsigned char*)shared_secret, shared_secret_length)) {
        res = SOTER_FAIL;
        goto err;
    }

    res = SOTER_SUCCESS;

err:
    EVP_PKEY_free(peer_pkey);
    EVP_PKEY_CTX_free(derive_ctx);

    return res;
}
