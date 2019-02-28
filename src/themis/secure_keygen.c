/*
 * Copyright (c) 2019 Cossack Labs Limited
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

#include "themis/secure_keygen.h"

#include <arpa/inet.h>
#include <string.h>

#include "soter/soter_container.h"
#include "soter/soter_ec_key.h"
#include "soter/soter_rsa_key.h"
#include "soter/soter_rsa_key_pair_gen.h"
#include "soter/soter_t.h"

#ifndef THEMIS_RSA_KEY_LENGTH
#define THEMIS_RSA_KEY_LENGTH RSA_KEY_LENGTH_2048
#endif
themis_status_t themis_gen_key_pair(soter_sign_alg_t alg, uint8_t* private_key,
                                    size_t* private_key_length, uint8_t* public_key,
                                    size_t* public_key_length)
{
    soter_sign_ctx_t* ctx = soter_sign_create(alg, NULL, 0, NULL, 0);
    THEMIS_CHECK(ctx != NULL);
    soter_status_t res = soter_sign_export_key(ctx, private_key, private_key_length, true);
    if (res != THEMIS_SUCCESS && res != THEMIS_BUFFER_TOO_SMALL) {
        soter_sign_destroy(ctx);
        return res;
    }
    soter_status_t res2 = soter_sign_export_key(ctx, public_key, public_key_length, false);
    if (res2 != THEMIS_SUCCESS && res2 != THEMIS_BUFFER_TOO_SMALL) {
        soter_sign_destroy(ctx);
        return res;
    }
    soter_sign_destroy(ctx);
    if (res == THEMIS_BUFFER_TOO_SMALL || res2 == THEMIS_BUFFER_TOO_SMALL) {
        return THEMIS_BUFFER_TOO_SMALL;
    }
    return THEMIS_SUCCESS;
}

themis_status_t themis_gen_rsa_key_pair(uint8_t* private_key, size_t* private_key_length,
                                        uint8_t* public_key, size_t* public_key_length)
{
    soter_rsa_key_pair_gen_t* key_pair_ctx = soter_rsa_key_pair_gen_create(THEMIS_RSA_KEY_LENGTH);
    THEMIS_CHECK(key_pair_ctx != NULL);
    soter_status_t res =
        soter_rsa_key_pair_gen_export_key(key_pair_ctx, private_key, private_key_length, true);
    if (res != THEMIS_SUCCESS && res != THEMIS_BUFFER_TOO_SMALL) {
        soter_rsa_key_pair_gen_destroy(key_pair_ctx);
        return res;
    }
    soter_status_t res2 =
        soter_rsa_key_pair_gen_export_key(key_pair_ctx, public_key, public_key_length, false);
    if (res2 != THEMIS_SUCCESS && res2 != THEMIS_BUFFER_TOO_SMALL) {
        soter_rsa_key_pair_gen_destroy(key_pair_ctx);
        return res2;
    }
    soter_rsa_key_pair_gen_destroy(key_pair_ctx);
    if (res == THEMIS_BUFFER_TOO_SMALL || res2 == THEMIS_BUFFER_TOO_SMALL) {
        return THEMIS_BUFFER_TOO_SMALL;
    }
    return THEMIS_SUCCESS;
}

themis_status_t themis_gen_ec_key_pair(uint8_t* private_key, size_t* private_key_length,
                                       uint8_t* public_key, size_t* public_key_length)
{
    return themis_gen_key_pair(SOTER_SIGN_ecdsa_none_pkcs8, private_key, private_key_length,
                               public_key, public_key_length);
}

themis_key_kind_t themis_get_asym_key_kind(const uint8_t* key, size_t length)
{
    const soter_container_hdr_t* container = (const void*)key;

    if (!key || (length < sizeof(soter_container_hdr_t))) {
        return THEMIS_KEY_INVALID;
    }

    if (!memcmp(container->tag, RSA_PRIV_KEY_PREF, strlen(RSA_PRIV_KEY_PREF))) {
        return THEMIS_KEY_RSA_PRIVATE;
    }
    if (!memcmp(container->tag, RSA_PUB_KEY_PREF, strlen(RSA_PUB_KEY_PREF))) {
        return THEMIS_KEY_RSA_PUBLIC;
    }
    if (!memcmp(container->tag, EC_PRIV_KEY_PREF, strlen(EC_PRIV_KEY_PREF))) {
        return THEMIS_KEY_EC_PRIVATE;
    }
    if (!memcmp(container->tag, EC_PUB_KEY_PREF, strlen(EC_PUB_KEY_PREF))) {
        return THEMIS_KEY_EC_PUBLIC;
    }

    return THEMIS_KEY_INVALID;
}

themis_status_t themis_is_valid_asym_key(const uint8_t* key, size_t length)
{
    const soter_container_hdr_t* container = (const void*)key;

    if (!key || (length < sizeof(soter_container_hdr_t))) {
        return THEMIS_INVALID_PARAMETER;
    }
    if (THEMIS_KEY_INVALID == themis_get_asym_key_kind(key, length)) {
        return THEMIS_INVALID_PARAMETER;
    }
    if (length != ntohl(container->size)) {
        return THEMIS_INVALID_PARAMETER;
    }
    if (SOTER_SUCCESS != soter_verify_container_checksum(container)) {
        return THEMIS_DATA_CORRUPT;
    }

    return THEMIS_SUCCESS;
}
