/*
 * Copyright (c) 2020 Cossack Labs Limited
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

#ifndef THEMISPP_SECURE_CELL_TOKEN_PROTECT_HPP
#define THEMISPP_SECURE_CELL_TOKEN_PROTECT_HPP

#include <cassert>
#include <vector>

#include <themis/themis.h>

#include "exception.hpp"
#include "impl/input_buffer.hpp"
#include "impl/secret_bytes.hpp"
#include "secure_cell_data.hpp"

namespace themispp
{

namespace impl
{

/**
 * Secure Cell in Token Protect mode secured by a master key
 */
template <typename Allocator = std::allocator<uint8_t> >
class secure_cell_token_protect_with_key
{
public:
    /**
     * Result of decryption
     */
    typedef std::vector<uint8_t, Allocator> output_data;

    /**
     * Result of encryption
     */
    typedef secure_cell_data<output_data> output_pair;

    /// @name Constructors
    /// @{

    /**
     * Make a new Secure Cell in Token Protect mode secured with a master key
     *
     * @param [in]  key     a non-empty container with master key
     *
     * STL-compatible contiguous containers of `uint8_t` and fixed-size C arrays
     * are supported as input for master key. The container must not be empty.
     *
     * @returns New Secure Cell with a copy of the master key.
     *
     * @throws themispp::exception_t if the master key is empty.
     */
    template <typename Key>
    explicit secure_cell_token_protect_with_key(const Key& key)
        : m_master_key(input_bytes(key))
    {
        if (m_master_key.empty()) {
            throw themispp::exception_t("Secure Cell: key cannot be empty", THEMIS_INVALID_PARAMETER);
        }
    }

#if __cplusplus >= 201103L
    /**
     * Make a new Secure Cell in Token Protect mode secured with a master key
     *
     * @param [in]  key     a non-empty vector with master key
     *
     * The vector must not be empty.
     *
     * @returns New Secure Cell using existing master key buffer.
     *
     * @throws themispp::exception_t if the master key is empty.
     */
    explicit secure_cell_token_protect_with_key(std::vector<uint8_t, Allocator>&& key)
        : m_master_key(std::move(key))
    {
        if (m_master_key.empty()) {
            throw themispp::exception_t("Secure Cell: key cannot be empty", THEMIS_INVALID_PARAMETER);
        }
    }
#endif

    /// @}
    /// @name Data encryption
    /// @{

    /**
     * Encrypt data.
     *
     * @param [in] plaintext    non-empty data to be encrypted
     *
     * STL-compatible contiguous containers of `uint8_t` and fixed-size C arrays
     * are supported as input. Plaintext must not be empty.
     *
     * Data is encrypted and authentication token is produced separately. Encrypted data has
     * the same length as the original. You will need to provide the token during decryption
     * so maintain the association between the data and the token.
     *
     * @see encrypt(const Plaintext& plaintext, const Context& context) const
     *
     * This method is equivalent to using an empty associated context.
     *
     * @returns An object containing encrypted data and authentication token.
     *
     * @throws themispp::exception_t if `plaintext` is empty.
     * @throws themispp::exception_t on encryption failure.
     */
    template <typename Plaintext>
    output_pair encrypt(const Plaintext& plaintext) const
    {
        return encrypt(input_bytes(plaintext), input_buffer());
    }

    /**
     * Encrypt data with context.
     *
     * @param [in] plaintext    non-empty data to be encrypted
     * @param [in] context      optional associated context, may be omitted
     *
     * STL-compatible contiguous containers of `uint8_t` and fixed-size C arrays
     * are supported as input. Plaintext must not be empty.
     *
     * Data is encrypted and authentication token is produced separately. Encrypted data has
     * the same length as the original. You will need to provide the token during decryption
     * so maintain the association between the data and the token.
     *
     * The context, if provided, is cryptographically mixed with the data, but is not included
     * into the resulting encrypted message. You will have to provide the same context again
     * during decryption. Usually this is some plaintext data associated associated with the
     * encrypted data, such as database row number, protocol message ID, etc.
     *
     * @returns An object containing encrypted data and authentication token.
     *
     * @throws themispp::exception_t if `plaintext` is empty.
     * @throws themispp::exception_t on encryption failure.
     */
    template <typename Plaintext, typename Context>
    output_pair encrypt(const Plaintext& plaintext, const Context& context) const
    {
        return encrypt(input_bytes(plaintext), input_bytes(context));
    }

    /// @}
    /// @name Data decryption
    /// @{

    /**
     * Decrypt data.
     *
     * @param [in] encrypted    data to be decrypted
     * @param [in] token        authentication token
     *
     * STL-compatible contiguous containers of `uint8_t` and fixed-size C arrays
     * are supported as input. Encrypted data and token cannot be empty.
     *
     * Secure Cell validates association with the context data, decrypts the message,
     * and verifies data integrity using the provided authentication token.
     *
     * @see decrypt(const Encrypted& encrypted, const Token& token, const Context& context) const
     *
     * This method is equivalent to using an empty associated context.
     *
     * @returns Newly allocated container with decrypted data if everything goes well.
     *
     * @throws themispp::exception_t if data cannot be decrypted. Usually this means that either
     * the data or the token has been tampered with, or the master key or associated context
     * are not the same as were used to encrypt the data.
     *
     * @throws themispp::exception_t if `encrypted` or `token` is empty.
     */
    template <typename Encrypted, typename Token>
    output_data decrypt(const Encrypted& encrypted, const Token& token) const
    {
        return decrypt(input_bytes(encrypted), input_bytes(token), input_buffer());
    }

    /**
     * Decrypt data with context.
     *
     * @param [in] encrypted    data to be decrypted
     * @param [in] token        authentication token
     * @param [in] context      associated context, may be empty
     *
     * STL-compatible contiguous containers of `uint8_t` and fixed-size C arrays
     * are supported as input. Encrypted data and token cannot be empty.
     *
     * Secure Cell validates association with the context data, decrypts the message,
     * and verifies data integrity using the provided authentication token.
     *
     * You need to provide the same context as it was used during encryption (or use null
     * or empty context if there was no context). The token also must be the one produced
     * during encryption.
     *
     * @returns Newly allocated container with decrypted data if everything goes well.
     *
     * @throws themispp::exception_t if data cannot be decrypted. Usually this means that either
     * the data or the token has been tampered with, or the master key or associated context
     * are not the same as were used to encrypt the data.
     *
     * @throws themispp::exception_t if `encrypted` or `token` is empty.
     */
    template <typename Encrypted, typename Token, typename Context>
    output_data decrypt(const Encrypted& encrypted, const Token& token, const Context& context) const
    {
        return decrypt(input_bytes(encrypted), input_bytes(token), input_bytes(context));
    }

    /// @}

private:
    output_pair encrypt(input_buffer plaintext, input_buffer context) const
    {
        if (plaintext.empty()) {
            throw themispp::exception_t("Secure Cell: encrypt: plaintext cannot be empty",
                                        THEMIS_INVALID_PARAMETER);
        }
        output_pair result;
        size_t token_size = 0;
        size_t encrypted_size = 0;
        themis_status_t res = THEMIS_FAIL;
        res = themis_secure_cell_encrypt_token_protect(m_master_key.data(),
                                                       m_master_key.size(),
                                                       context.data(),
                                                       context.size(),
                                                       plaintext.data(),
                                                       plaintext.size(),
                                                       0,
                                                       &token_size,
                                                       0,
                                                       &encrypted_size);
        if (res != THEMIS_BUFFER_TOO_SMALL) {
            throw themispp::exception_t("Secure Cell: encrypt: cannot compute output size", res);
        }
        assert(token_size != 0);
        assert(encrypted_size != 0);
        result.first.resize(encrypted_size);
        result.second.resize(token_size);
        res = themis_secure_cell_encrypt_token_protect(m_master_key.data(),
                                                       m_master_key.size(),
                                                       context.data(),
                                                       context.size(),
                                                       plaintext.data(),
                                                       plaintext.size(),
                                                       &result.second.front(),
                                                       &token_size,
                                                       &result.first.front(),
                                                       &encrypted_size);
        if (res != THEMIS_SUCCESS) {
            throw themispp::exception_t("Secure Cell: encrypt: cannot encrypt data", res);
        }
        assert(encrypted_size == result.first.size());
        assert(token_size == result.second.size());
        return result;
    }

    output_data decrypt(input_buffer encrypted, input_buffer token, input_buffer context) const
    {
        if (encrypted.empty()) {
            throw themispp::exception_t("Secure Cell: decrypt: encrypted data cannot be empty",
                                        THEMIS_INVALID_PARAMETER);
        }
        if (token.empty()) {
            throw themispp::exception_t("Secure Cell: decrypt: authentication token cannot be empty",
                                        THEMIS_INVALID_PARAMETER);
        }
        output_data decrypted;
        size_t decrypted_size = 0;
        themis_status_t res = THEMIS_FAIL;
        res = themis_secure_cell_decrypt_token_protect(m_master_key.data(),
                                                       m_master_key.size(),
                                                       context.data(),
                                                       context.size(),
                                                       encrypted.data(),
                                                       encrypted.size(),
                                                       token.data(),
                                                       token.size(),
                                                       0,
                                                       &decrypted_size);
        if (res != THEMIS_BUFFER_TOO_SMALL) {
            throw themispp::exception_t("Secure Cell: decrypt: cannot compute output size", res);
        }
        assert(decrypted_size != 0);
        decrypted.resize(decrypted_size);
        res = themis_secure_cell_decrypt_token_protect(m_master_key.data(),
                                                       m_master_key.size(),
                                                       context.data(),
                                                       context.size(),
                                                       encrypted.data(),
                                                       encrypted.size(),
                                                       token.data(),
                                                       token.size(),
                                                       &decrypted.front(),
                                                       &decrypted_size);
        if (res != THEMIS_SUCCESS) {
            throw themispp::exception_t("Secure Cell: decrypt: cannot decrypt data", res);
        }
        assert(decrypted_size == decrypted.size());
        return decrypted;
    }

private:
    impl::secret_bytes<Allocator> m_master_key;
};

} // namespace impl

typedef impl::secure_cell_token_protect_with_key<> secure_cell_token_protect_with_key;

} // namespace themispp

#endif // THEMISPP_SECURE_CELL_TOKEN_PROTECT_HPP
