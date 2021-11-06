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

#include "soter/openssl/soter_ecdsa_common.h"

#include <openssl/ec.h>
#include <openssl/evp.h>

#include "soter/openssl/soter_engine.h"
#include "soter/soter_ec_key.h"

soter_status_t soter_ec_gen_key(EVP_PKEY** ppkey)
{
    soter_status_t res = SOTER_FAIL;
    EVP_PKEY* pkey = NULL;
    EVP_PKEY_CTX* pkey_ctx = NULL;
    EC_KEY* ec = NULL;

    if (!ppkey) {
        return SOTER_INVALID_PARAMETER;
    }
    pkey = *ppkey;
    if (!pkey) {
        return SOTER_INVALID_PARAMETER;
    }
    if (EVP_PKEY_EC != EVP_PKEY_id(pkey)) {
        return SOTER_INVALID_PARAMETER;
    }
    ec = EVP_PKEY_get0(pkey);
    if (NULL == ec) {
        return SOTER_INVALID_PARAMETER;
    }

    pkey_ctx = EVP_PKEY_CTX_new(pkey, NULL);
    if (!pkey_ctx) {
        res = SOTER_NO_MEMORY;
        goto err;
    }

    if (!EVP_PKEY_paramgen_init(pkey_ctx)) {
        res = SOTER_FAIL;
        goto err;
    }
    if (!EVP_PKEY_CTX_set_ec_paramgen_curve_nid(pkey_ctx, NID_X9_62_prime256v1)) {
        res = SOTER_FAIL;
        goto err;
    }
    if (!EVP_PKEY_paramgen(pkey_ctx, &pkey)) {
        res = SOTER_FAIL;
        goto err;
    }

    if (EC_KEY_generate_key(ec) != 1) {
        res = SOTER_FAIL;
        goto err;
    }

    res = SOTER_SUCCESS;

err:
    EVP_PKEY_CTX_free(pkey_ctx);

    return res;
}

soter_status_t soter_ec_import_key(EVP_PKEY* pkey, const void* key, const size_t key_length)
{
    const soter_container_hdr_t* hdr = key;
    if (!pkey || !key) {
        return SOTER_INVALID_PARAMETER;
    }
    if (key_length < sizeof(soter_container_hdr_t)) {
        return SOTER_INVALID_PARAMETER;
    }
    if (EVP_PKEY_EC != EVP_PKEY_id(pkey)) {
        return SOTER_INVALID_PARAMETER;
    }
    switch (hdr->tag[0]) {
    case 'R':
        return soter_ec_priv_key_to_engine_specific(hdr,
                                                    key_length,
                                                    ((soter_engine_specific_ec_key_t**)&pkey));
    case 'U':
        return soter_ec_pub_key_to_engine_specific(hdr,
                                                   key_length,
                                                   ((soter_engine_specific_ec_key_t**)&pkey));
    }
    return SOTER_INVALID_PARAMETER;
}

soter_status_t soter_ec_export_key(soter_sign_ctx_t* ctx, void* key, size_t* key_length, bool isprivate)
{
    EVP_PKEY* pkey = ctx->pkey;
    if (!pkey) {
        return SOTER_INVALID_PARAMETER;
    }
    if (isprivate) {
        return soter_engine_specific_to_ec_priv_key((const soter_engine_specific_ec_key_t*)pkey,
                                                    (soter_container_hdr_t*)key,
                                                    key_length);
    }
    return soter_engine_specific_to_ec_pub_key((const soter_engine_specific_ec_key_t*)pkey,
                                               (soter_container_hdr_t*)key,
                                               key_length);
}
