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

#include <stdlib.h>
#include <string.h>

#include "soter/soter_container.h"
#include "soter/soter_ec_key.h"
#include "soter/soter_rand.h"
#include "soter/soter_rsa_key.h"
#include "soter/soter_rsa_key_pair_gen.h"
#include "soter/soter_t.h"
#include "soter/soter_wipe.h"

#include "themis/themis_portable_endian.h"

#ifndef THEMIS_RSA_KEY_LENGTH
#define THEMIS_RSA_KEY_LENGTH RSA_KEY_LENGTH_2048
#endif

/*
 * This is the default key length recommended for use with Secure Cell.
 * It will have enough randomness for AES-256 (normally used by Themis)
 * and is consistent with NIST recommendations for the next ten years,
 * as of 2020. See: https://www.keylength.com/en/4/
 */
#define THEMIS_SYM_KEY_LENGTH 32

/*
 * Historically Themis used compressed format for EC keys. This resulted
 * in a more compact representation, but is not optimal for performance.
 * Due to Hyrum's law, we can't change the default key format that easily,
 * so more efficient uncompressed representation can be used after opt-in.
 * (Note that we can import both representations without special actions.)
 */
static bool should_generate_compressed_ec_key_pairs(void)
{
    const char* uncompressed = getenv("THEMIS_GEN_EC_KEY_PAIR_UNCOMPRESSED");
    if (uncompressed != NULL && strcmp(uncompressed, "1") == 0) {
        return false;
    }
    return true;
}

static themis_status_t combine_key_generation_results(uint8_t* private_key,
                                                      const size_t* private_key_length,
                                                      themis_status_t private_result,
                                                      uint8_t* public_key,
                                                      const size_t* public_key_length,
                                                      themis_status_t public_result)
{
    if (private_result == THEMIS_SUCCESS && public_result == THEMIS_SUCCESS) {
        return THEMIS_SUCCESS;
    }

    if (private_result != THEMIS_BUFFER_TOO_SMALL) {
        soter_wipe(private_key, *private_key_length);
    }
    if (public_result != THEMIS_BUFFER_TOO_SMALL) {
        soter_wipe(public_key, *public_key_length);
    }

    if (private_result == THEMIS_BUFFER_TOO_SMALL || public_result == THEMIS_BUFFER_TOO_SMALL) {
        return THEMIS_BUFFER_TOO_SMALL;
    }

    return (private_result != THEMIS_SUCCESS) ? private_result : public_result;
}

themis_status_t themis_gen_key_pair(soter_sign_alg_t alg,
                                    uint8_t* private_key,
                                    size_t* private_key_length,
                                    uint8_t* public_key,
                                    size_t* public_key_length)
{
    themis_status_t private_result = THEMIS_FAIL;
    themis_status_t public_result = THEMIS_FAIL;
    soter_sign_ctx_t* ctx = NULL;
    bool compressed = true;

    if (!private_key_length || !public_key_length) {
        return THEMIS_INVALID_PARAMETER;
    }

    ctx = soter_sign_create(alg, NULL, 0, NULL, 0);
    if (!ctx) {
        return THEMIS_FAIL;
    }

    compressed = should_generate_compressed_ec_key_pairs();
    private_result = soter_sign_export_private_key(ctx, private_key, private_key_length);
    public_result = soter_sign_export_public_key(ctx, compressed, public_key, public_key_length);

    soter_sign_destroy(ctx);

    return combine_key_generation_results(private_key,
                                          private_key_length,
                                          private_result,
                                          public_key,
                                          public_key_length,
                                          public_result);
}

themis_status_t themis_gen_rsa_key_pair(uint8_t* private_key,
                                        size_t* private_key_length,
                                        uint8_t* public_key,
                                        size_t* public_key_length)
{
    themis_status_t private_result = THEMIS_FAIL;
    themis_status_t public_result = THEMIS_FAIL;
    soter_rsa_key_pair_gen_t* ctx = NULL;

    if (!private_key_length || !public_key_length) {
        return THEMIS_INVALID_PARAMETER;
    }

    ctx = soter_rsa_key_pair_gen_create(THEMIS_RSA_KEY_LENGTH);
    if (!ctx) {
        return THEMIS_FAIL;
    }

    private_result = soter_rsa_key_pair_gen_export_key(ctx, private_key, private_key_length, true);
    public_result = soter_rsa_key_pair_gen_export_key(ctx, public_key, public_key_length, false);

    soter_rsa_key_pair_gen_destroy(ctx);

    return combine_key_generation_results(private_key,
                                          private_key_length,
                                          private_result,
                                          public_key,
                                          public_key_length,
                                          public_result);
}

themis_status_t themis_gen_ec_key_pair(uint8_t* private_key,
                                       size_t* private_key_length,
                                       uint8_t* public_key,
                                       size_t* public_key_length)
{
    return themis_gen_key_pair(SOTER_SIGN_ecdsa_none_pkcs8,
                               private_key,
                               private_key_length,
                               public_key,
                               public_key_length);
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
    themis_key_kind_t kind = THEMIS_KEY_INVALID;

    if (!key || (length < sizeof(soter_container_hdr_t))) {
        return THEMIS_INVALID_PARAMETER;
    }

    kind = themis_get_asym_key_kind(key, length);
    if (kind == THEMIS_KEY_INVALID) {
        return THEMIS_INVALID_PARAMETER;
    }
    if (length != be32toh(container->size)) {
        return THEMIS_INVALID_PARAMETER;
    }
    if (SOTER_SUCCESS != soter_verify_container_checksum(container)) {
        return THEMIS_DATA_CORRUPT;
    }

    switch (kind) {
    case THEMIS_KEY_RSA_PRIVATE:
        return soter_rsa_priv_key_check_length(container, length);
    case THEMIS_KEY_RSA_PUBLIC:
        return soter_rsa_pub_key_check_length(container, length);
    case THEMIS_KEY_EC_PRIVATE:
        return soter_ec_priv_key_check_length(container, length);
    case THEMIS_KEY_EC_PUBLIC:
        return soter_ec_pub_key_check_length(container, length);
    default:
        return THEMIS_INVALID_PARAMETER;
    }

    return THEMIS_INVALID_PARAMETER;
}

themis_status_t themis_gen_sym_key(uint8_t* key, size_t* key_length)
{
    if (key_length == NULL) {
        return THEMIS_INVALID_PARAMETER;
    }

    if (key == NULL || *key_length == 0) {
        *key_length = THEMIS_SYM_KEY_LENGTH;
        return THEMIS_BUFFER_TOO_SMALL;
    }

    /* soter_rand() wipes the key on failure, soter_wipe() not needed */
    return soter_rand(key, *key_length);
}
