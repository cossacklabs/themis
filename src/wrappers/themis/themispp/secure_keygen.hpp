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

#ifndef THEMISPP_SECURE_KEYGEN_HPP_
#define THEMISPP_SECURE_KEYGEN_HPP_

#include <cstring>
#include <vector>

#include <themis/themis.h>

#include "exception.hpp"

namespace themispp
{

enum asym_algs { EC, RSA };

static const size_t max_key_length = 10 * 1024;

template <asym_algs alg_t_p, size_t max_key_length_t_p = max_key_length>
class secure_key_pair_generator_t
{
public:
    secure_key_pair_generator_t()
        : private_key(max_key_length_t_p)
        , public_key(max_key_length_t_p)
    {
        gen();
    }

    void gen()
    {
        themis_status_t status = THEMIS_FAIL;
        size_t private_key_length = max_key_length_t_p;
        size_t public_key_length = max_key_length_t_p;
        switch (alg_t_p) {
        case EC:
            status = themis_gen_ec_key_pair(&private_key[0],
                                            &private_key_length,
                                            &public_key[0],
                                            &public_key_length);
            if (status != THEMIS_SUCCESS) {
                throw themispp::exception_t("Themis failed to generate EC key pair", status);
            }
            break;
        case RSA:
            status = themis_gen_rsa_key_pair(&private_key[0],
                                             &private_key_length,
                                             &public_key[0],
                                             &public_key_length);
            if (status != THEMIS_SUCCESS) {
                throw themispp::exception_t("Themis failed to generate RSA key pair", status);
            }
            break;
        default:
            throw themispp::exception_t("Themis failed generate key pair: unsupported algorithm");
        }
        private_key.resize(private_key_length);
        public_key.resize(public_key_length);
    }

    const std::vector<uint8_t>& get_priv()
    {
        return private_key;
    }
    const std::vector<uint8_t>& get_pub()
    {
        return public_key;
    }

private:
    std::vector<uint8_t> private_key;
    std::vector<uint8_t> public_key;
};

inline themis_status_t validate_key(const std::vector<uint8_t>& key)
{
    if (key.empty()) {
        return THEMIS_INVALID_PARAMETER;
    }
    return themis_is_valid_asym_key(&key[0], key.size());
}

inline bool is_valid_key(const std::vector<uint8_t>& key)
{
    return validate_key(key) == THEMIS_SUCCESS;
}

inline bool is_private_key(const std::vector<uint8_t>& key)
{
    if (!key.empty()) {
        themis_key_kind_t kind = themis_get_asym_key_kind(&key[0], key.size());
        switch (kind) {
        case THEMIS_KEY_EC_PRIVATE:
        case THEMIS_KEY_RSA_PRIVATE:
            return true;
        default:
            break;
        }
    }
    return false;
}

inline bool is_public_key(const std::vector<uint8_t>& key)
{
    if (!key.empty()) {
        themis_key_kind_t kind = themis_get_asym_key_kind(&key[0], key.size());
        switch (kind) {
        case THEMIS_KEY_EC_PUBLIC:
        case THEMIS_KEY_RSA_PUBLIC:
            return true;
        default:
            break;
        }
    }
    return false;
}

} // namespace themispp

#endif
