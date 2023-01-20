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

#include "soter/boringssl/soter_ecdsa_common.h"

#include <openssl/ec.h>
#include <openssl/evp.h>

#include "soter/boringssl/soter_engine.h"
#include "soter/soter_ec_key.h"

soter_status_t soter_ec_gen_key(EVP_PKEY_CTX* pkey_ctx)
{
    EVP_PKEY* pkey;
    EC_KEY* ec = NULL;
    if (!pkey_ctx) {
        return SOTER_INVALID_PARAMETER;
    }
    pkey = EVP_PKEY_CTX_get0_pkey(pkey_ctx);
    if (!pkey) {
        return SOTER_INVALID_PARAMETER;
    }
    if (EVP_PKEY_EC != EVP_PKEY_id(pkey)) {
        return SOTER_INVALID_PARAMETER;
    }
    ec = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
    if (!ec) {
        return SOTER_ENGINE_FAIL;
    }
    if (EC_KEY_generate_key(ec) != 1) {
        EC_KEY_free(ec);
        return SOTER_ENGINE_FAIL;
    }
    if (EVP_PKEY_assign_EC_KEY(pkey, ec) != 1) {
        EC_KEY_free(ec);
        return SOTER_ENGINE_FAIL;
    }
    return SOTER_SUCCESS;
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

soter_status_t soter_ec_export_private_key(const EVP_PKEY* pkey, void* key, size_t* key_length)
{
    if (!pkey) {
        return SOTER_INVALID_PARAMETER;
    }
    return soter_engine_specific_to_ec_priv_key((const soter_engine_specific_ec_key_t*)pkey,
                                                (soter_container_hdr_t*)key,
                                                key_length);
}

soter_status_t soter_ec_export_public_key(const EVP_PKEY* pkey, bool compressed, void* key, size_t* key_length)
{
    if (!pkey) {
        return SOTER_INVALID_PARAMETER;
    }
    return soter_engine_specific_to_ec_pub_key((const soter_engine_specific_ec_key_t*)pkey,
                                               compressed,
                                               (soter_container_hdr_t*)key,
                                               key_length);
}
