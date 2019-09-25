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

#ifndef THEMISPP_SECURE_MESSAGE_HPP_
#define THEMISPP_SECURE_MESSAGE_HPP_

#include <vector>

#include <themis/themis.h>

#include "exception.hpp"
#include "secure_keygen.hpp"

namespace themispp
{

class secure_message_t
{
public:
    typedef std::vector<uint8_t> data_t;

    secure_message_t(data_t::const_iterator private_key_begin,
                     data_t::const_iterator private_key_end,
                     data_t::const_iterator peer_public_key_begin,
                     data_t::const_iterator peer_public_key_end)
        : private_key_(private_key_begin, private_key_end)
        , peer_public_key_(peer_public_key_begin, peer_public_key_end)
    {
        validate_keys();
    }

    secure_message_t(const data_t& private_key,
                     data_t::const_iterator peer_public_key_begin,
                     data_t::const_iterator peer_public_key_end)
        : private_key_(private_key.begin(), private_key.end())
        , peer_public_key_(peer_public_key_begin, peer_public_key_end)
    {
        validate_keys();
    }

    secure_message_t(data_t::const_iterator private_key_begin,
                     data_t::const_iterator private_key_end,
                     const data_t& peer_public_key)
        : private_key_(private_key_begin, private_key_end)
        , peer_public_key_(peer_public_key.begin(), peer_public_key.end())
    {
        validate_keys();
    }

    secure_message_t(const data_t& private_key, const data_t& peer_public_key)
        : private_key_(private_key.begin(), private_key.end())
        , peer_public_key_(peer_public_key.begin(), peer_public_key.end())
    {
        validate_keys();
    }

#if __cplusplus >= 201103L
    virtual ~secure_message_t() = default;
#else
    virtual ~secure_message_t()
    {
    }
#endif

    const data_t& encrypt(data_t::const_iterator data_begin, data_t::const_iterator data_end)
    {
        if (private_key_.empty()) {
            throw themispp::exception_t("Secure Message failed to encrypt message: private key not set");
        }
        if (peer_public_key_.empty()) {
            throw themispp::exception_t("Secure Message failed to encrypt message: public key not set");
        }
        if (data_end <= data_begin) {
            throw themispp::exception_t(
                "Secure Message failed to encrypt message: data must be non-empty");
        }
        themis_status_t status = THEMIS_FAIL;
        size_t encrypted_data_length = 0;
        status = themis_secure_message_encrypt(&private_key_[0],
                                               private_key_.size(),
                                               &peer_public_key_[0],
                                               peer_public_key_.size(),
                                               &(*data_begin),
                                               data_end - data_begin,
                                               NULL,
                                               &encrypted_data_length);
        if (status != THEMIS_BUFFER_TOO_SMALL) {
            throw themispp::exception_t("Secure Message failed to encrypt message", status);
        }
        res_.resize(encrypted_data_length);
        status = themis_secure_message_encrypt(&private_key_[0],
                                               private_key_.size(),
                                               &peer_public_key_[0],
                                               peer_public_key_.size(),
                                               &(*data_begin),
                                               data_end - data_begin,
                                               &res_[0],
                                               &encrypted_data_length);
        if (status != THEMIS_SUCCESS) {
            throw themispp::exception_t("Secure Message failed to encrypt message", status);
        }
        return res_;
    }

    const data_t& encrypt(const data_t& data)
    {
        return encrypt(data.begin(), data.end());
    }

    const data_t& decrypt(data_t::const_iterator data_begin, data_t::const_iterator data_end)
    {
        if (private_key_.empty()) {
            throw themispp::exception_t("Secure Message failed to decrypt message: private key not set");
        }
        if (peer_public_key_.empty()) {
            throw themispp::exception_t("Secure Message failed to decrypt message: public key not set");
        }
        if (data_end <= data_begin) {
            throw themispp::exception_t(
                "Secure Message failed to decrypt message: data must be non-empty");
        }
        themis_status_t status = THEMIS_FAIL;
        size_t decrypted_data_length = 0;
        status = themis_secure_message_decrypt(&private_key_[0],
                                               private_key_.size(),
                                               &peer_public_key_[0],
                                               peer_public_key_.size(),
                                               &(*data_begin),
                                               data_end - data_begin,
                                               NULL,
                                               &decrypted_data_length);
        if (status != THEMIS_BUFFER_TOO_SMALL) {
            throw themispp::exception_t("Secure Message failed to decrypt message", status);
        }
        res_.resize(decrypted_data_length);
        status = themis_secure_message_decrypt(&private_key_[0],
                                               private_key_.size(),
                                               &peer_public_key_[0],
                                               peer_public_key_.size(),
                                               &(*data_begin),
                                               data_end - data_begin,
                                               &res_[0],
                                               &decrypted_data_length);
        if (status != THEMIS_SUCCESS) {
            throw themispp::exception_t("Secure Message failed to decrypt message", status);
        }
        return res_;
    }

    const data_t& decrypt(const data_t& data)
    {
        return decrypt(data.begin(), data.end());
    }

    const data_t& sign(data_t::const_iterator data_begin, data_t::const_iterator data_end)
    {
        if (private_key_.empty()) {
            throw themispp::exception_t("Secure Message failed to sign message: private key not set");
        }
        if (data_end <= data_begin) {
            throw themispp::exception_t("Secure Message failed to sign message: data must be non-empty");
        }
        themis_status_t status = THEMIS_FAIL;
        size_t encrypted_data_length = 0;
        status = themis_secure_message_sign(&private_key_[0],
                                            private_key_.size(),
                                            &(*data_begin),
                                            data_end - data_begin,
                                            NULL,
                                            &encrypted_data_length);
        if (status != THEMIS_BUFFER_TOO_SMALL) {
            throw themispp::exception_t("Secure Message failed to sign message", status);
        }
        res_.resize(encrypted_data_length);
        status = themis_secure_message_sign(&private_key_[0],
                                            private_key_.size(),
                                            &(*data_begin),
                                            data_end - data_begin,
                                            &res_[0],
                                            &encrypted_data_length);
        if (status != THEMIS_SUCCESS) {
            throw themispp::exception_t("Secure Message failed to sign message", status);
        }
        return res_;
    }

    const data_t& sign(const data_t& data)
    {
        return sign(data.begin(), data.end());
    }

    const data_t& verify(data_t::const_iterator data_begin, data_t::const_iterator data_end)
    {
        if (peer_public_key_.empty()) {
            throw themispp::exception_t("Secure Message failed to verify signature: public key not set");
        }
        if (data_end <= data_begin) {
            throw themispp::exception_t(
                "Secure Message failed to verify signature: data must be non-empty");
        }
        themis_status_t status = THEMIS_FAIL;
        size_t decrypted_data_length = 0;
        status = themis_secure_message_verify(&peer_public_key_[0],
                                              peer_public_key_.size(),
                                              &(*data_begin),
                                              data_end - data_begin,
                                              NULL,
                                              &decrypted_data_length);
        if (status != THEMIS_BUFFER_TOO_SMALL) {
            throw themispp::exception_t("Secure Message failed to verify signature", status);
        }
        res_.resize(decrypted_data_length);
        status = themis_secure_message_verify(&peer_public_key_[0],
                                              peer_public_key_.size(),
                                              &(*data_begin),
                                              data_end - data_begin,
                                              &res_[0],
                                              &decrypted_data_length);
        if (status != THEMIS_SUCCESS) {
            throw themispp::exception_t("Secure Message failed to verify signature", status);
        }
        return res_;
    }

    const data_t& verify(const data_t& data)
    {
        return verify(data.begin(), data.end());
    }

private:
    data_t private_key_;
    data_t peer_public_key_;
    data_t res_;

private:
    void validate_keys()
    {
        if (!private_key_.empty()) {
            if (!is_valid_key(private_key_)) {
                throw themispp::exception_t("Secure Message: invalid private key");
            }
            if (!is_private_key(private_key_)) {
                throw themispp::exception_t("Secure Message: using public key instead of private key");
            }
        }
        if (!peer_public_key_.empty()) {
            if (!is_valid_key(peer_public_key_)) {
                throw themispp::exception_t("Secure Message: invalid public key");
            }
            if (!is_public_key(peer_public_key_)) {
                throw themispp::exception_t("Secure Message: using private key instead of public key");
            }
        }
    }
};

} // namespace themispp

#endif /* THEMISPP_SECURE_MESSAGE_HPP_ */
