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

#ifndef THEMISPP_SECURE_CELL_SEAL_HPP
#define THEMISPP_SECURE_CELL_SEAL_HPP

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
 * Secure Cell in Seal mode secured by a master key
 */
template <typename Allocator = std::allocator<uint8_t> >
class secure_cell_seal_with_key
{
public:
    /**
     * Result of encryption or decryption
     */
    typedef std::vector<uint8_t, Allocator> output_data;

    /// @name Constructors
    /// @{

    /**
     * Make a new Secure Cell in Seal mode secured with a master key
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
    explicit secure_cell_seal_with_key(const Key& key)
        : m_master_key(input_bytes(key))
    {
        if (m_master_key.empty()) {
            throw themispp::exception_t("Secure Cell: key cannot be empty", THEMIS_INVALID_PARAMETER);
        }
    }

#if __cplusplus >= 201103L
    /**
     * Make a new Secure Cell in Seal mode secured with a master key
     *
     * @param [in]  key     a non-empty vector with master key
     *
     * The vector must not be empty.
     *
     * @returns New Secure Cell using existing master key buffer.
     *
     * @throws themispp::exception_t if the master key is empty.
     */
    explicit secure_cell_seal_with_key(std::vector<uint8_t, Allocator>&& key)
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
     * Data is encrypted and authentication token is appended to form a single sealed buffer.
     *
     * @see encrypt(const Plaintext& plaintext, const Context& context) const
     *
     * This method is equivalent to using an empty associated context.
     *
     * @returns Newly allocated container with encrypted data and authentication token.
     *
     * @throws themispp::exception_t if `plaintext` is empty.
     * @throws themispp::exception_t on encryption failure.
     */
    template <typename Plaintext>
    output_data encrypt(const Plaintext& plaintext) const
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
     * Data is encrypted and authentication token is appended to form a single sealed buffer.
     * The context, if provided, is cryptographically mixed with the data, but it not included
     * into the resulting encrypted message. You will have to provide the same context again
     * during decryption. Usually this is some plaintext data associated associated with the
     * encrypted data, such as database row number, protocol message ID, etc.
     *
     * @returns Newly allocated container with encrypted data and authentication token.
     *
     * @throws themispp::exception_t if `plaintext` is empty.
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
     * Decrypt data.
     *
     * @param [in] encrypted    data to be decrypted
     *
     * STL-compatible contiguous containers of `uint8_t` and fixed-size C arrays
     * are supported as input. Encrypted data cannot be empty.
     *
     * Secure Cell validates association with the context data, decrypts the message,
     * and verifies data integrity using authentication data embedded into the message.
     *
     * @see decrypt(const Encrypted& encrypted, const Context& context) const
     *
     * This method is equivalent to using an empty associated context.
     *
     * @returns Newly allocated container with decrypted data if everything goes well.
     *
     * @throws themispp::exception_t if data cannot be decrypted. Usually this means that either
     * the data has been tampered with, or the master key or associated context are not the
     * same as were used to encrypt the data.
     *
     * @throws themispp::exception_t if `encrypted` is empty.
     */
    template <typename Encrypted>
    output_data decrypt(const Encrypted& encrypted) const
    {
        return decrypt(input_bytes(encrypted), input_buffer());
    }

    /**
     * Decrypt data with context.
     *
     * @param [in] encrypted    data to be decrypted
     * @param [in] context      associated context, may be empty
     *
     * STL-compatible contiguous containers of `uint8_t` and fixed-size C arrays
     * are supported as input. Encrypted data cannot be empty.
     *
     * Secure Cell validates association with the context data, decrypts the message,
     * and verifies data integrity using authentication data embedded into the message.
     *
     * You need to provide the same context as it was used during encryption (or use
     * an empty context if there was no context).
     *
     * @returns Newly allocated container with decrypted data if everything goes well.
     *
     * @throws themispp::exception_t if data cannot be decrypted. Usually this means that either
     * the data has been tampered with, or the master key or associated context are not the
     * same as were used to encrypt the data.
     *
     * @throws themispp::exception_t if `encrypted` is empty.
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
        output_data encrypted;
        size_t encrypted_size = 0;
        themis_status_t res = THEMIS_FAIL;
        res = themis_secure_cell_encrypt_seal(m_master_key.data(),
                                              m_master_key.size(),
                                              context.data(),
                                              context.size(),
                                              plaintext.data(),
                                              plaintext.size(),
                                              0,
                                              &encrypted_size);
        if (res != THEMIS_BUFFER_TOO_SMALL) {
            throw themispp::exception_t("Secure Cell: encrypt: cannot compute output size", res);
        }
        assert(encrypted_size != 0);
        encrypted.resize(encrypted_size);
        res = themis_secure_cell_encrypt_seal(m_master_key.data(),
                                              m_master_key.size(),
                                              context.data(),
                                              context.size(),
                                              plaintext.data(),
                                              plaintext.size(),
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
        output_data decrypted;
        size_t decrypted_size = 0;
        themis_status_t res = THEMIS_FAIL;
        res = themis_secure_cell_decrypt_seal(m_master_key.data(),
                                              m_master_key.size(),
                                              context.data(),
                                              context.size(),
                                              encrypted.data(),
                                              encrypted.size(),
                                              0,
                                              &decrypted_size);
        if (res != THEMIS_BUFFER_TOO_SMALL) {
            throw themispp::exception_t("Secure Cell: decrypt: cannot compute output size", res);
        }
        assert(decrypted_size != 0);
        decrypted.resize(decrypted_size);
        res = themis_secure_cell_decrypt_seal(m_master_key.data(),
                                              m_master_key.size(),
                                              context.data(),
                                              context.size(),
                                              encrypted.data(),
                                              encrypted.size(),
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

/**
 * Secure Cell in Seal mode secured by a passphrase
 */
template <typename Allocator = std::allocator<uint8_t> >
class secure_cell_seal_with_passphrase
{
public:
    /**
     * Result of encryption or decryption
     */
    typedef std::vector<uint8_t, Allocator> output_data;

    /// @name Constructors
    /// @{

    /**
     * Make a new Secure Cell in Seal mode secured with a passphrase
     *
     * @param [in]  key     a non-empty container with passphrase
     *
     * STL-compatible multibyte string containers and well as null-terminated
     * C strings are supported. Raw byte input is also supported via containers
     * of `uint8_t`. Passphrase container must not be empty.
     *
     * @returns New Secure Cell with a copy of the passphrase.
     *
     * @throws themispp::exception_t if the passphrase is empty.
     */
    template <typename Passphrase>
    explicit secure_cell_seal_with_passphrase(const Passphrase& passphrase)
        : m_passphrase(input_string(passphrase))
    {
        if (m_passphrase.empty()) {
            throw themispp::exception_t("Secure Cell: passphrase cannot be empty",
                                        THEMIS_INVALID_PARAMETER);
        }
    }

#if __cplusplus >= 201103L
    /**
     * Make a new Secure Cell in Seal mode secured with a passphrase
     *
     * @param [in]  key     a non-empty vector with passphrase
     *
     * The vector must not be empty.
     *
     * @returns New Secure Cell using existing passphrase buffer.
     *
     * @throws themispp::exception_t if the passphrase is empty.
     */
    explicit secure_cell_seal_with_passphrase(std::vector<uint8_t, Allocator>&& passphrase)
        : m_passphrase(std::move(passphrase))
    {
        if (m_passphrase.empty()) {
            throw themispp::exception_t("Secure Cell: passphrase cannot be empty",
                                        THEMIS_INVALID_PARAMETER);
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
     * Data is encrypted and authentication token is appended to form a single sealed buffer.
     *
     * @see encrypt(const Plaintext& plaintext, const Context& context) const
     *
     * This method is equivalent to using an empty associated context.
     *
     * @returns Newly allocated container with encrypted data and authentication token.
     *
     * @throws themispp::exception_t if `plaintext` is empty.
     * @throws themispp::exception_t on encryption failure.
     */
    template <typename Plaintext>
    output_data encrypt(const Plaintext& plaintext) const
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
     * Data is encrypted and authentication token is appended to form a single sealed buffer.
     * The context, if provided, is cryptographically mixed with the data, but it not included
     * into the resulting encrypted message. You will have to provide the same context again
     * during decryption. Usually this is some plaintext data associated associated with the
     * encrypted data, such as database row number, protocol message ID, etc.
     *
     * @returns Newly allocated container with encrypted data and authentication token.
     *
     * @throws themispp::exception_t if `plaintext` is empty.
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
     * Decrypt data.
     *
     * @param [in] encrypted    data to be decrypted
     *
     * STL-compatible contiguous containers of `uint8_t` and fixed-size C arrays
     * are supported as input. Encrypted data cannot be empty.
     *
     * Secure Cell validates association with the context data, decrypts the message,
     * and verifies data integrity using authentication data embedded into the message.
     *
     * @see decrypt(const Encrypted& encrypted, const Context& context) const
     *
     * This method is equivalent to using an empty associated context.
     *
     * @returns Newly allocated container with decrypted data if everything goes well.
     *
     * @throws themispp::exception_t if data cannot be decrypted. Usually this means that either
     * the data has been tampered with, or the master key or associated context are not the
     * same as were used to encrypt the data.
     *
     * @throws themispp::exception_t if `encrypted` is empty.
     */
    template <typename Encrypted>
    output_data decrypt(const Encrypted& encrypted) const
    {
        return decrypt(input_bytes(encrypted), input_buffer());
    }

    /**
     * Decrypt data with context.
     *
     * @param [in] encrypted    data to be decrypted
     * @param [in] context      associated context, may be empty
     *
     * STL-compatible contiguous containers of `uint8_t` and fixed-size C arrays
     * are supported as input. Encrypted data cannot be empty.
     *
     * Secure Cell validates association with the context data, decrypts the message,
     * and verifies data integrity using authentication data embedded into the message.
     *
     * You need to provide the same context as it was used during encryption (or use
     * an empty context if there was no context).
     *
     * @returns Newly allocated container with decrypted data if everything goes well.
     *
     * @throws themispp::exception_t if data cannot be decrypted. Usually this means that either
     * the data has been tampered with, or the master key or associated context are not the
     * same as were used to encrypt the data.
     *
     * @throws themispp::exception_t if `encrypted` is empty.
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
        output_data encrypted;
        size_t encrypted_size = 0;
        themis_status_t res = THEMIS_FAIL;
        res = themis_secure_cell_encrypt_seal_with_passphrase(m_passphrase.data(),
                                                              m_passphrase.size(),
                                                              context.data(),
                                                              context.size(),
                                                              plaintext.data(),
                                                              plaintext.size(),
                                                              0,
                                                              &encrypted_size);
        if (res != THEMIS_BUFFER_TOO_SMALL) {
            throw themispp::exception_t("Secure Cell: encrypt: cannot compute output size", res);
        }
        assert(encrypted_size != 0);
        encrypted.resize(encrypted_size);
        res = themis_secure_cell_encrypt_seal_with_passphrase(m_passphrase.data(),
                                                              m_passphrase.size(),
                                                              context.data(),
                                                              context.size(),
                                                              plaintext.data(),
                                                              plaintext.size(),
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
        output_data decrypted;
        size_t decrypted_size = 0;
        themis_status_t res = THEMIS_FAIL;
        res = themis_secure_cell_decrypt_seal_with_passphrase(m_passphrase.data(),
                                                              m_passphrase.size(),
                                                              context.data(),
                                                              context.size(),
                                                              encrypted.data(),
                                                              encrypted.size(),
                                                              0,
                                                              &decrypted_size);
        if (res != THEMIS_BUFFER_TOO_SMALL) {
            throw themispp::exception_t("Secure Cell: decrypt: cannot compute output size", res);
        }
        assert(decrypted_size != 0);
        decrypted.resize(decrypted_size);
        res = themis_secure_cell_decrypt_seal_with_passphrase(m_passphrase.data(),
                                                              m_passphrase.size(),
                                                              context.data(),
                                                              context.size(),
                                                              encrypted.data(),
                                                              encrypted.size(),
                                                              &decrypted.front(),
                                                              &decrypted_size);
        if (res != THEMIS_SUCCESS) {
            throw themispp::exception_t("Secure Cell: decrypt: cannot decrypt data", res);
        }
        assert(decrypted_size == decrypted.size());
        return decrypted;
    }

private:
    impl::secret_bytes<Allocator> m_passphrase;
};

} // namespace impl

typedef impl::secure_cell_seal_with_key<> secure_cell_seal_with_key;
typedef impl::secure_cell_seal_with_passphrase<> secure_cell_seal_with_passphrase;

} // namespace themispp

#endif // THEMISPP_SECURE_CELL_SEAL_HPP
