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

#ifndef THEMISPP_SECURE_CELL_CONTEXT_IMPRINT_HPP
#define THEMISPP_SECURE_CELL_CONTEXT_IMPRINT_HPP

#include <cassert>
#include <vector>

#include <themis/themis.h>

#include "exception.hpp"
#include "impl/input_buffer.hpp"
#include "impl/secret_bytes.hpp"

namespace themispp
{

namespace impl
{

/**
 * Secure Cell in Context Imprint mode secured by a master key
 */
template <typename Allocator = std::allocator<uint8_t> >
class secure_cell_context_imprint_with_key
{
public:
    /**
     * Result of encryption or decryption
     */
    typedef std::vector<uint8_t, Allocator> output_data;

    /// @name Constructors
    /// @{

    /**
     * Make a new Secure Cell in Context Imprint mode secured with a master key
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
    explicit secure_cell_context_imprint_with_key(const Key& key)
        : m_master_key(input_bytes(key))
    {
        if (m_master_key.empty()) {
            throw themispp::exception_t("Secure Cell: key cannot be empty", THEMIS_INVALID_PARAMETER);
        }
    }

#if __cplusplus >= 201103L
    /**
     * Make a new Secure Cell in Context Imprint mode secured with a master key
     *
     * @param [in]  key     a non-empty vector with master key
     *
     * The vector must not be empty.
     *
     * @returns New Secure Cell using existing master key buffer.
     *
     * @throws themispp::exception_t if the master key is empty.
     */
    explicit secure_cell_context_imprint_with_key(std::vector<uint8_t, Allocator>&& key)
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
     * Encrypt data with context.
     *
     * @param [in] plaintext    non-empty data to be encrypted
     * @param [in] context      non-empty associated context
     *
     * STL-compatible contiguous containers of `uint8_t` and fixed-size C arrays
     * are supported as input. Plaintext and context must not be empty.
     *
     * Data is encrypted and cryptographically mixed with the provided context. The context
     * is not included into encrypted message. You will have to provide the same context again
     * during decryption. Usually this is some plaintext data associated associated with the
     * encrypted data, such as database row number, protocol message ID, client name, etc.
     *
     * @returns Newly allocated container with encrypted data, same length as input.
     * Context Imprint mode does not include any additional data for integrity validation.
     *
     * @throws themispp::exception_t if `plaintext` or `context` is empty.
     * @throws themispp::exception_t on encryption failure.
     */
    template <typename Plaintext, typename Context>
    output_data encrypt(const Plaintext& plaintext, const Context& context) const
    {
        return encrypt(input_bytes(plaintext), input_bytes(context));
    }

    /// @}
    /// @name Data decryption
    /// @{

    /**
     * Decrypt data with context.
     *
     * @param [in] encrypted    data to be decrypted
     * @param [in] context      associated context
     *
     * STL-compatible contiguous containers of `uint8_t` and fixed-size C arrays
     * are supported as input. Encrypted data and context cannot be empty.
     *
     * Secure Cell decrypts the message in association with provided context.
     * You have to use the same context as it was used during encryption.
     *
     * Context Imprint mode does not validate integrity of the decrypted message.
     * It will successfully return garbage output if an incorrect key or
     * mismatched context has been used, or if the message data has been
     * corrupted or tampered with.
     *
     * @returns Newly allocated container with decrypted data. You should
     * validate its correctness and integrity.
     *
     * @throws themispp::exception_t if `encrypted` or `context` is empty.
     * @throws themispp::exception_t on encryption failure.
     */
    template <typename Encrypted, typename Context>
    output_data decrypt(const Encrypted& encrypted, const Context& context) const
    {
        return decrypt(input_bytes(encrypted), input_bytes(context));
    }

    /// @}

private:
    output_data encrypt(input_buffer plaintext, input_buffer context) const
    {
        if (plaintext.empty()) {
            throw themispp::exception_t("Secure Cell: encrypt: plaintext cannot be empty",
                                        THEMIS_INVALID_PARAMETER);
        }
        if (context.empty()) {
            throw themispp::exception_t("Secure Cell: encrypt: context cannot be empty",
                                        THEMIS_INVALID_PARAMETER);
        }
        output_data encrypted;
        size_t encrypted_size = 0;
        themis_status_t res = THEMIS_FAIL;
        res = themis_secure_cell_encrypt_context_imprint(m_master_key.data(),
                                                         m_master_key.size(),
                                                         plaintext.data(),
                                                         plaintext.size(),
                                                         context.data(),
                                                         context.size(),
                                                         0,
                                                         &encrypted_size);
        if (res != THEMIS_BUFFER_TOO_SMALL) {
            throw themispp::exception_t("Secure Cell: encrypt: cannot compute output size", res);
        }
        assert(encrypted_size != 0);
        encrypted.resize(encrypted_size);
        res = themis_secure_cell_encrypt_context_imprint(m_master_key.data(),
                                                         m_master_key.size(),
                                                         plaintext.data(),
                                                         plaintext.size(),
                                                         context.data(),
                                                         context.size(),
                                                         &encrypted.front(),
                                                         &encrypted_size);
        if (res != THEMIS_SUCCESS) {
            throw themispp::exception_t("Secure Cell: encrypt: cannot encrypt data", res);
        }
        assert(encrypted_size == encrypted.size());
        return encrypted;
    }

    output_data decrypt(input_buffer encrypted, input_buffer context) const
    {
        if (encrypted.empty()) {
            throw themispp::exception_t("Secure Cell: decrypt: encrypted data cannot be empty",
                                        THEMIS_INVALID_PARAMETER);
        }
        if (context.empty()) {
            throw themispp::exception_t("Secure Cell: decrypt: context cannot be empty",
                                        THEMIS_INVALID_PARAMETER);
        }
        output_data decrypted;
        size_t decrypted_size = 0;
        themis_status_t res = THEMIS_FAIL;
        res = themis_secure_cell_decrypt_context_imprint(m_master_key.data(),
                                                         m_master_key.size(),
                                                         encrypted.data(),
                                                         encrypted.size(),
                                                         context.data(),
                                                         context.size(),
                                                         0,
                                                         &decrypted_size);
        if (res != THEMIS_BUFFER_TOO_SMALL) {
            throw themispp::exception_t("Secure Cell: decrypt: cannot compute output size", res);
        }
        assert(decrypted_size != 0);
        decrypted.resize(decrypted_size);
        res = themis_secure_cell_decrypt_context_imprint(m_master_key.data(),
                                                         m_master_key.size(),
                                                         encrypted.data(),
                                                         encrypted.size(),
                                                         context.data(),
                                                         context.size(),
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

typedef impl::secure_cell_context_imprint_with_key<> secure_cell_context_imprint_with_key;

} // namespace themispp

#endif // THEMISPP_SECURE_CELL_CONTEXT_IMPRINT_HPP
