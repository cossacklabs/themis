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

#ifndef THEMISPP_CELL_TEST
#define THEMISPP_CELL_TEST

#include <stdint.h>
#include <string>
#if __cplusplus >= 201103L
#include <tuple>
#endif
#include <vector>

#include <common/sput.h>

#include <themispp/secure_cell.hpp>
#include <themispp/secure_keygen.hpp>

#include "utils.hpp"

// Allow usage of deprecated Secure Cell API for testing
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

namespace themispp
{
namespace secure_cell_test
{

static const std::vector<uint8_t> key1 = as_bytes(
    "\x68\x69\x65\x37\x68\x6f\x68\x33\x43\x65\x73\x75\x69\x79\x61\x65"
    "\x35\x7a\x65\x69\x4a\x65\x69\x42\x61\x69\x63\x65\x65\x6c\x65\x72");
static const std::vector<uint8_t> key2 = as_bytes(
    "\x6f\x65\x62\x39\x79\x61\x30\x61\x61\x43\x61\x79\x69\x30\x6f\x70"
    "\x68\x32\x6f\x69\x76\x33\x67\x69\x75\x7a\x65\x65\x77\x65\x6f\x35");

static const std::vector<uint8_t> message1 = as_bytes("secure cell test message1 (c)Cossacklabs");
static const std::vector<uint8_t> message2 = as_bytes("secure cell test message2 (c)Cossacklabs");

static const std::vector<uint8_t> context1 = as_bytes("secure cell test context1 message");
static const std::vector<uint8_t> context2 = as_bytes("secure cell test context2 message");

static void secure_cell_keygen_test()
{
    static const size_t default_size = 32;

    try {
        std::vector<uint8_t> key = themispp::gen_sym_key();
        sput_fail_unless(key.size() == default_size, "key generation (default)", __LINE__);
    } catch (const themispp::exception_t&) {
        sput_fail_unless(false, "key generation (default)", __LINE__);
    }

    try {
        std::vector<uint8_t> key;
        themispp::gen_sym_key(key);
        sput_fail_unless(key.size() == default_size, "key generation (in-place)", __LINE__);
    } catch (const themispp::exception_t&) {
        sput_fail_unless(false, "key generation (in-place)", __LINE__);
    }

    try {
        std::vector<uint8_t> key(8);
        themispp::gen_sym_key(key);
        sput_fail_unless(key.size() == 8, "key generation (custom)", __LINE__);
    } catch (const themispp::exception_t&) {
        sput_fail_unless(false, "key generation (custom)", __LINE__);
    }
}

static void secure_cell_construction_test()
{
    std::vector<uint8_t> empty;

    try {
        themispp::secure_cell_seal_t seal(empty);
        sput_fail_unless(false, "empty key (seal)", __LINE__);
    } catch (const themispp::exception_t&) {
        sput_fail_unless(true, "empty key (seal)", __LINE__);
    }

    try {
        themispp::secure_cell_token_protect_t seal(empty);
        sput_fail_unless(false, "empty key (token protect)", __LINE__);
    } catch (const themispp::exception_t&) {
        sput_fail_unless(true, "empty key (token protect)", __LINE__);
    }

    try {
        themispp::secure_cell_context_imprint_t seal(empty);
        sput_fail_unless(false, "empty key (context imprint)", __LINE__);
    } catch (const themispp::exception_t&) {
        sput_fail_unless(true, "empty key (context imprint)", __LINE__);
    }
}

static void secure_cell_seal_test()
{
    // construction
    themispp::secure_cell_seal_t a1(key1.begin(), key1.end());
    themispp::secure_cell_seal_t a2(key1);
    themispp::secure_cell_seal_t b1(key2);

    sput_fail_unless(a1.encrypt(message1) != a2.encrypt(message1),
                     "two different seal encryption with same keys can't be the same",
                     __LINE__);
    sput_fail_unless(a1.encrypt(message1) != b1.encrypt(message1),
                     "two different seal encryption with different keys can't be the same",
                     __LINE__);
    sput_fail_unless(a1.encrypt(message1) != a2.encrypt(message2),
                     "two different seal encryption with different messages can't be the same",
                     __LINE__);
    std::vector<uint8_t> r1 = a1.encrypt(message1);
    std::vector<uint8_t> r2 = a2.encrypt(message1.begin(), message1.end());
    std::vector<uint8_t> r3 = b1.encrypt(message1.begin(), message1.end());

    sput_fail_unless(r1 != r2, "two different seal encryption with same keys can't be the same", __LINE__);
    sput_fail_unless(a1.decrypt(r1) == a2.decrypt(r2.begin(), r2.end()), "decryption", __LINE__);
    try {
        a2.decrypt(r3.begin(), r3.end());
        sput_fail_unless(false, "decryption", __LINE__);
    } catch (themispp::exception_t& e) {
    }
}

static void secure_cell_seal_context_test()
{
    // construction
    themispp::secure_cell_seal_t a1(key1.begin(), key1.end());
    themispp::secure_cell_seal_t a2(key1);
    themispp::secure_cell_seal_t b1(key2);

    sput_fail_unless(a1.encrypt(message1, context1) != a2.encrypt(message1, context1),
                     "two different seal encryption with same keys and contexts can't be the same",
                     __LINE__);
    sput_fail_unless(a1.encrypt(message1, context1) != a2.encrypt(context2),
                     "two different seal encryption with same keys and different contexts can't be the same",
                     __LINE__);
    sput_fail_unless(a1.encrypt(message1, context1) != b1.encrypt(message1, context1),
                     "two different seal encryption with different keys and same context can't be the same",
                     __LINE__);

    sput_fail_unless(a1.encrypt(message1, context1) != a2.encrypt(message2, context1),
                     "two different seal encryption with different messages and same context can't be the same",
                     __LINE__);

    std::vector<uint8_t> r1 = a1.encrypt(message1, context1);
    std::vector<uint8_t> r2 = a2.encrypt(message1.begin(), message1.end(), context1);
    std::vector<uint8_t> r3 = b1.encrypt(message1.begin(), message1.end(), context1);

    sput_fail_unless(r1 != r2, "two different seal encryption with same keys can't be the same", __LINE__);
    sput_fail_unless(a1.decrypt(r1, context1) == a2.decrypt(r2.begin(), r2.end(), context1),
                     "decryption",
                     __LINE__);
    try {
        a2.decrypt(r3.begin(), r3.end(), context1);
        sput_fail_unless(false, "decryption", __LINE__);
    } catch (themispp::exception_t& e) {
    }
}

static void secure_cell_token_protect_test()
{
    themispp::secure_cell_token_protect_t encrypter1(key1.begin(), key1.end());
    themispp::secure_cell_token_protect_t encrypter2(key1);
    themispp::secure_cell_token_protect_t encrypter3(key2);

    sput_fail_unless(encrypter1.encrypt(message1) != encrypter2.encrypt(message1),
                     "two different encryption with same keys can't be the same",
                     __LINE__);

    sput_fail_unless(!encrypter1.get_token().empty(), "token can't be zero size after encryption", __LINE__);
    sput_fail_unless(!encrypter2.get_token().empty(), "token can't be zero size after encryption", __LINE__);

    sput_fail_unless(encrypter1.get_token() != encrypter2.get_token(),
                     "two different tokens with same keys can't be the same",
                     __LINE__);
    sput_fail_unless(encrypter1.encrypt(message1) != encrypter3.encrypt(message1),
                     "two different encryption with different keys can't be the same",
                     __LINE__);
    sput_fail_unless(encrypter1.get_token() != encrypter2.get_token(),
                     "two different tokens with different messages can't be the same",
                     __LINE__);

    std::vector<uint8_t> r1 = encrypter1.encrypt(message1);
    std::vector<uint8_t> r2 = encrypter2.encrypt(message1.begin(), message1.end());
    std::vector<uint8_t> r3 = encrypter3.encrypt(message1.begin(), message1.end());

    sput_fail_unless(r1 != r2, "two different encryption with same keys can't be the same", __LINE__);
    sput_fail_unless(encrypter1.decrypt(r1) == encrypter2.decrypt(r2.begin(), r2.end()),
                     "decryption",
                     __LINE__);
    secure_cell_t::data_t token1 = encrypter1.get_token();
    encrypter2.set_token(token1);
    sput_fail_unless(encrypter2.decrypt(r1.begin(), r1.end()) == message1,
                     "expected correct decryption",
                     __LINE__);
    try {
        encrypter2.decrypt(r3.begin(), r3.end());
        sput_fail_unless(false, "decryption", __LINE__);
    } catch (themispp::exception_t& e) {
    }
}

static void secure_cell_token_protect_context_test()
{
    themispp::secure_cell_token_protect_t encrypter1(key1.begin(), key1.end());
    themispp::secure_cell_token_protect_t encrypter2(key1);
    themispp::secure_cell_token_protect_t encrypter3(key2);

    sput_fail_unless(encrypter1.encrypt(message1, context1) != encrypter2.encrypt(message1, context1),
                     "two different encryption with same keys can't be the same",
                     __LINE__);
    secure_cell_t::data_t encrypted1 = encrypter1.encrypt(message1, context1);
    secure_cell_t::data_t encrypted2 = encrypter1.encrypt(message1, context2);
    sput_fail_unless(encrypted1 != encrypted2,
                     "two different encryption with same keys and different contexts can't be the same",
                     __LINE__);

    sput_fail_unless(!encrypter1.get_token().empty(), "token can't be zero size after encryption", __LINE__);
    sput_fail_unless(!encrypter2.get_token().empty(), "token can't be zero size after encryption", __LINE__);

    sput_fail_unless(encrypter1.get_token() != encrypter2.get_token(),
                     "two different tokens with same keys can't be the same",
                     __LINE__);
    sput_fail_unless(encrypter1.encrypt(message1, context1) != encrypter3.encrypt(message1, context1),
                     "two different encryption with different keys can't be the same",
                     __LINE__);
    sput_fail_unless(encrypter1.get_token() != encrypter2.get_token(),
                     "two different tokens with different messages can't be the same",
                     __LINE__);

    std::vector<uint8_t> r1 = encrypter1.encrypt(message1, context1);
    std::vector<uint8_t> r2 = encrypter2.encrypt(message1.begin(), message1.end(), context1);
    std::vector<uint8_t> r3 = encrypter3.encrypt(message1.begin(), message1.end(), context2);

    sput_fail_unless(r1 != r2, "two different encryption with same keys can't be the same", __LINE__);
    sput_fail_unless(encrypter1.decrypt(r1, context1)
                         == encrypter2.decrypt(r2.begin(), r2.end(), context1),
                     "decryption",
                     __LINE__);
    encrypter2.set_token(encrypter1.get_token());
    sput_fail_unless(encrypter2.decrypt(r1.begin(), r1.end(), context1) == message1,
                     "expected correct decryption",
                     __LINE__);
    try {
        encrypter2.decrypt(r3.begin(), r3.end(), context1);
        sput_fail_unless(false, "decryption", __LINE__);
    } catch (themispp::exception_t& e) {
    }
}

static void secure_cell_context_imprint_test()
{
    // construction
    themispp::secure_cell_context_imprint_t encrypter1(key1);
    themispp::secure_cell_context_imprint_t encrypter2(key1);
    themispp::secure_cell_context_imprint_t encrypter3(key2);

    sput_fail_unless(encrypter1.encrypt(message1, context1) == encrypter2.encrypt(message1, context1),
                     "two different encryption with same keys and context must be the same",
                     __LINE__);
    sput_fail_unless(encrypter1.encrypt(message1, context1) != encrypter2.encrypt(message1, context2),
                     "two different encryption with same keys and different context can't be the same",
                     __LINE__);

    sput_fail_unless(encrypter1.encrypt(message1, context1) != encrypter3.encrypt(message1, context1),
                     "two different encryption with different keys and same context can't be the same",
                     __LINE__);
    sput_fail_unless(encrypter1.encrypt(message1, context1) != encrypter2.encrypt(message2, context1),
                     "two different encryption with different messages and same context can't be the same",
                     __LINE__);
    std::vector<uint8_t> r1 = encrypter1.encrypt(message1, context1);
    std::vector<uint8_t> r2 = encrypter2.encrypt(message1.begin(), message1.end(), context1);
    std::vector<uint8_t> r3 = encrypter3.encrypt(message1.begin(), message1.end(), context2);

    sput_fail_unless(encrypter1.decrypt(r1, context1)
                         == encrypter2.decrypt(r2.begin(), r2.end(), context1),
                     "decryption",
                     __LINE__);
    try {
        std::vector<uint8_t> result = encrypter2.decrypt(r3.begin(), r3.end(), context1);
        sput_fail_unless(result != message2, "result should be the same as raw message", __LINE__);
    } catch (themispp::exception_t& e) {
    }
}

template <size_t N>
// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
static inline bool operator==(const std::vector<uint8_t>& lhs, const uint8_t (&rhs)[N])
{
    if (lhs.size() != N) {
        return false;
    }
    if (N == 0) {
        return true;
    }
    return !memcmp(&lhs.front(), &rhs[0], N);
}

template <size_t N>
// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
static inline bool operator==(const uint8_t (&lhs)[N], const std::vector<uint8_t>& rhs)
{
    return rhs == lhs;
}

template <size_t N>
// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
static inline bool operator!=(const uint8_t (&lhs)[N], const std::vector<uint8_t>& rhs)
{
    return !(lhs == rhs);
}

template <size_t N>
// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
static inline bool operator!=(const std::vector<uint8_t>& lhs, const uint8_t (&rhs)[N])
{
    return !(lhs == rhs);
}

namespace seal
{

template <typename SecureCell, typename Secret>
static inline void miscommunication(Secret secretA, Secret secretB)
{
    SecureCell alice_cell(secretA);
    SecureCell bob_cell(secretB);

    uint8_t message[] = "[redacted]";           // NOLINT(cppcoreguidelines-avoid-c-arrays)
    uint8_t contextA[] = "prison break plans";  // NOLINT(cppcoreguidelines-avoid-c-arrays)
    uint8_t contextB[] = "newspaper interview"; // NOLINT(cppcoreguidelines-avoid-c-arrays)

    std::vector<uint8_t> encrypted = alice_cell.encrypt(message, contextA);
    try {
        bob_cell.decrypt(encrypted, contextA);
        sput_fail_unless(false, "decrypt with different secret", __LINE__);
    } catch (const themispp::exception_t& e) {
        sput_fail_unless(true, "decrypt with different secret", __LINE__);
    }

    SecureCell bob_cell_2(secretA); // same as Alice has

    try {
        bob_cell_2.decrypt(encrypted, contextB);
        sput_fail_unless(false, "decrypt with different context", __LINE__);
    } catch (const themispp::exception_t& e) {
        sput_fail_unless(true, "decrypt with different context", __LINE__);
    }

    std::vector<uint8_t> decrypted = bob_cell_2.decrypt(encrypted, contextA);
    sput_fail_unless(decrypted == message, "matching secret and context", __LINE__);
}

template <typename SecureCell, typename Secret>
static inline void silly_arguments(Secret empty_secret, Secret ok_secret)
{
    try {
        SecureCell cell(empty_secret);
        sput_fail_unless(false, "empty secret not allowed", __LINE__);
    } catch (const themispp::exception_t& e) {
        sput_fail_unless(true, "empty secret not allowed", __LINE__);
    }

    SecureCell cell(ok_secret);

    try {
        cell.encrypt(std::vector<uint8_t>());
        sput_fail_unless(false, "encrypt: empty plaintext not allowed", __LINE__);
    } catch (const themispp::exception_t& e) {
        sput_fail_unless(true, "encrypt: empty plaintext not allowed", __LINE__);
    }

    try {
        cell.decrypt(std::vector<uint8_t>());
        sput_fail_unless(false, "decrypt: empty plaintext not allowed", __LINE__);
    } catch (const themispp::exception_t& e) {
        sput_fail_unless(true, "decrypt: empty plaintext not allowed", __LINE__);
    }

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
    uint8_t message[] = "important message";
    std::vector<uint8_t> context;

    std::vector<uint8_t> encrypted = cell.encrypt(message, context);
    std::vector<uint8_t> decrypted = cell.decrypt(encrypted);
    sput_fail_unless(decrypted == message, "empty context is no context", __LINE__);
}

template <typename SecureCell, typename Secret>
static inline void loss_of_patience(Secret secret)
{
    SecureCell cell(secret);

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
    uint8_t message[] = "important message";

    std::vector<uint8_t> encrypted = cell.encrypt(message);

    try {
        std::vector<uint8_t> truncated = encrypted;
        truncated.resize(truncated.size() - 1);
        cell.decrypt(truncated);
        sput_fail_unless(false, "fails to decrypt truncated message", __LINE__);
    } catch (const themispp::exception_t& e) {
        sput_fail_unless(true, "fails to decrypt truncated message", __LINE__);
    }

    try {
        std::vector<uint8_t> extended = encrypted;
        extended.resize(extended.size() + 1);
        cell.decrypt(extended);
        sput_fail_unless(false, "fails to decrypt extended message", __LINE__);
    } catch (const themispp::exception_t& e) {
        sput_fail_unless(true, "fails to decrypt extended message", __LINE__);
    }

    try {
        std::vector<uint8_t> corrupted = encrypted;
        // Dunno, flip every odd byte? That should do it
        for (size_t i = 0; i < corrupted.size(); i++) {
            corrupted[i] ^= (i % 2) ? 0xFF : 0x00;
        }
        cell.decrypt(corrupted);
        sput_fail_unless(false, "fails to decrypt corrupted message", __LINE__);
    } catch (const themispp::exception_t& e) {
        sput_fail_unless(true, "fails to decrypt corrupted message", __LINE__);
    }
}

static inline void master_key_showcase()
{
    std::vector<uint8_t> master_key = themispp::gen_sym_key();
    themispp::secure_cell_seal_with_key cell(master_key);

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
    uint8_t message[] = "important message";

    std::vector<uint8_t> encrypted = cell.encrypt(message);
    std::vector<uint8_t> decrypted = cell.decrypt(encrypted);

    sput_fail_unless(encrypted.size() > sizeof(message), "message includes auth token", __LINE__);
    sput_fail_unless(decrypted == message, "message decrypts into original data", __LINE__);
}

static inline void master_key_in_depth()
{
    std::vector<uint8_t> empty_key;
    std::vector<uint8_t> master_key_1 = themispp::gen_sym_key();
    std::vector<uint8_t> master_key_2 = themispp::gen_sym_key();
    miscommunication<secure_cell_seal_with_key>(master_key_1, master_key_2);
    silly_arguments<secure_cell_seal_with_key>(empty_key, master_key_1);
    loss_of_patience<secure_cell_seal_with_key>(master_key_2);
}

static inline void passphrase_showcase()
{
    themispp::secure_cell_seal_with_passphrase cell("secret");

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
    uint8_t message[] = "important message";

    std::vector<uint8_t> encrypted = cell.encrypt(message);
    std::vector<uint8_t> decrypted = cell.decrypt(encrypted);

    sput_fail_unless(encrypted.size() > sizeof(message), "message includes auth token", __LINE__);
    sput_fail_unless(decrypted == message, "message decrypts into original data", __LINE__);
}

static inline void passphrase_in_depth()
{
    miscommunication<secure_cell_seal_with_passphrase>("tango", "foxtrot");
    silly_arguments<secure_cell_seal_with_passphrase>("", "secret");
    loss_of_patience<secure_cell_seal_with_passphrase>(std::string("?"));
}

} // namespace seal

namespace token_protect
{

template <typename SecureCell, typename Secret>
static inline void miscommunication(Secret secretA, Secret secretB)
{
    SecureCell alice_cell(secretA);
    SecureCell bob_cell(secretB);

    uint8_t message[] = "[redacted]";           // NOLINT(cppcoreguidelines-avoid-c-arrays)
    uint8_t contextA[] = "prison break plans";  // NOLINT(cppcoreguidelines-avoid-c-arrays)
    uint8_t contextB[] = "newspaper interview"; // NOLINT(cppcoreguidelines-avoid-c-arrays)

    typename SecureCell::output_pair result = alice_cell.encrypt(message, contextA);
    const std::vector<uint8_t>& encrypted = result.encrypted();
    const std::vector<uint8_t>& token = result.token();

    try {
        bob_cell.decrypt(encrypted, token, contextA);
        sput_fail_unless(false, "decrypt with different secret", __LINE__);
    } catch (const themispp::exception_t& e) {
        sput_fail_unless(true, "decrypt with different secret", __LINE__);
    }

    SecureCell bob_cell_2(secretA); // same as Alice has

    try {
        std::vector<uint8_t> tokenB = alice_cell.encrypt(message, contextA).token();
        bob_cell_2.decrypt(encrypted, tokenB, contextA);
        sput_fail_unless(false, "decrypt with different token", __LINE__);
    } catch (const themispp::exception_t& e) {
        sput_fail_unless(true, "decrypt with different token", __LINE__);
    }

    try {
        bob_cell_2.decrypt(encrypted, token, contextB);
        sput_fail_unless(false, "decrypt with different context", __LINE__);
    } catch (const themispp::exception_t& e) {
        sput_fail_unless(true, "decrypt with different context", __LINE__);
    }

    std::vector<uint8_t> decrypted = bob_cell_2.decrypt(encrypted, token, contextA);
    sput_fail_unless(decrypted == message, "matching secret and context", __LINE__);
}

template <typename SecureCell, typename Secret>
static inline void silly_arguments(Secret empty_secret, Secret ok_secret)
{
    try {
        SecureCell cell(empty_secret);
        sput_fail_unless(false, "empty secret not allowed", __LINE__);
    } catch (const themispp::exception_t& e) {
        sput_fail_unless(true, "empty secret not allowed", __LINE__);
    }

    SecureCell cell(ok_secret);

    try {
        cell.encrypt(std::vector<uint8_t>());
        sput_fail_unless(false, "encrypt: empty plaintext not allowed", __LINE__);
    } catch (const themispp::exception_t& e) {
        sput_fail_unless(true, "encrypt: empty plaintext not allowed", __LINE__);
    }

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
    uint8_t message[] = "important message";
    std::vector<uint8_t> context;

    typename SecureCell::output_pair result = cell.encrypt(message, context);
    const std::vector<uint8_t>& encrypted = result.encrypted();
    const std::vector<uint8_t>& token = result.token();

    try {
        cell.decrypt(std::vector<uint8_t>(), token);
        sput_fail_unless(false, "decrypt: empty ciphertext not allowed", __LINE__);
    } catch (const themispp::exception_t& e) {
        sput_fail_unless(true, "decrypt: empty ciphertext not allowed", __LINE__);
    }
    try {
        cell.decrypt(encrypted, std::vector<uint8_t>());
        sput_fail_unless(false, "decrypt: empty token not allowed", __LINE__);
    } catch (const themispp::exception_t& e) {
        sput_fail_unless(true, "decrypt: empty token not allowed", __LINE__);
    }

    std::vector<uint8_t> decrypted = cell.decrypt(encrypted, token);
    sput_fail_unless(decrypted == message, "empty context is no context", __LINE__);
}

template <typename SecureCell, typename Secret>
static inline void loss_of_patience(Secret secret)
{
    SecureCell cell(secret);

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
    uint8_t message[] = "important message";

    typename SecureCell::output_pair result = cell.encrypt(message);
    const std::vector<uint8_t>& encrypted = result.encrypted();
    const std::vector<uint8_t>& token = result.token();

    try {
        std::vector<uint8_t> truncated = encrypted;
        truncated.resize(truncated.size() - 1);
        cell.decrypt(truncated, token);
        sput_fail_unless(false, "fails to decrypt truncated message", __LINE__);
    } catch (const themispp::exception_t& e) {
        sput_fail_unless(true, "fails to decrypt truncated message", __LINE__);
    }

    try {
        std::vector<uint8_t> extended = encrypted;
        extended.resize(extended.size() + 1);
        cell.decrypt(extended, token);
        sput_fail_unless(false, "fails to decrypt extended message", __LINE__);
    } catch (const themispp::exception_t& e) {
        sput_fail_unless(true, "fails to decrypt extended message", __LINE__);
    }

    try {
        std::vector<uint8_t> truncated = token;
        truncated.resize(truncated.size() - 1);
        cell.decrypt(encrypted, truncated);
        sput_fail_unless(false, "fails to decrypt truncated token", __LINE__);
    } catch (const themispp::exception_t& e) {
        sput_fail_unless(true, "fails to decrypt truncated token", __LINE__);
    }

    // Token Protect mode currently allows token buffer to be overlong
    try {
        std::vector<uint8_t> extended = token;
        extended.resize(extended.size() + 1);
        std::vector<uint8_t> decrypted = cell.decrypt(encrypted, extended);
        sput_fail_unless(message == decrypted, "extended token allowed", __LINE__);
    } catch (const themispp::exception_t& e) {
        sput_fail_unless(false, "extended token allowed", __LINE__);
    }

    try {
        std::vector<uint8_t> corrupted = encrypted;
        // Dunno, flip every odd byte? That should do it
        for (size_t i = 0; i < corrupted.size(); i++) {
            corrupted[i] ^= (i % 2) ? 0xFF : 0x00;
        }
        cell.decrypt(corrupted, token);
        sput_fail_unless(false, "fails to decrypt corrupted message", __LINE__);
    } catch (const themispp::exception_t& e) {
        sput_fail_unless(true, "fails to decrypt corrupted message", __LINE__);
    }

    try {
        std::vector<uint8_t> corrupted = token;
        // Dunno, flip every even byte? That should do it
        // TODO: fix Themis Core which segfaults if *odd* bytes are flipped
        for (size_t i = 0; i < corrupted.size(); i++) {
            corrupted[i] ^= (i % 2) ? 0x00 : 0xFF;
        }
        cell.decrypt(encrypted, corrupted);
        sput_fail_unless(false, "fails to decrypt corrupted token", __LINE__);
    } catch (const themispp::exception_t& e) {
        sput_fail_unless(true, "fails to decrypt corrupted token", __LINE__);
    }
}

static inline void master_key_showcase()
{
    std::vector<uint8_t> master_key = themispp::gen_sym_key();
    themispp::secure_cell_token_protect_with_key cell(master_key);

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
    uint8_t message[] = "important message";

#if __cplusplus >= 201703L
    auto [encrypted, token] = cell.encrypt(message);
#elif __cplusplus >= 201103L
    std::vector<uint8_t> encrypted;
    std::vector<uint8_t> token;
    std::tie(encrypted, token) = cell.encrypt(message);
#else
    themispp::secure_cell_token_protect_with_key::output_pair result = cell.encrypt(message);
    const std::vector<uint8_t>& encrypted = result.encrypted();
    const std::vector<uint8_t>& token = result.token();
#endif
    std::vector<uint8_t> decrypted = cell.decrypt(encrypted, token);

    sput_fail_unless(encrypted.size() == sizeof(message), "message keeps same size", __LINE__);
    sput_fail_unless(!token.empty(), "non-empty token is produced", __LINE__);
    sput_fail_unless(decrypted == message, "message decrypts into original data", __LINE__);
}

static inline void master_key_in_depth()
{
    std::vector<uint8_t> empty_key;
    std::vector<uint8_t> master_key_1 = themispp::gen_sym_key();
    std::vector<uint8_t> master_key_2 = themispp::gen_sym_key();
    miscommunication<secure_cell_token_protect_with_key>(master_key_1, master_key_2);
    silly_arguments<secure_cell_token_protect_with_key>(empty_key, master_key_1);
    loss_of_patience<secure_cell_token_protect_with_key>(master_key_2);
}

} // namespace token_protect

namespace context_imprint
{

static inline void miscommunication()
{
    std::vector<uint8_t> alice_key = themispp::gen_sym_key();
    std::vector<uint8_t> bob_key = themispp::gen_sym_key();

    themispp::secure_cell_context_imprint_with_key alice_cell(alice_key);
    themispp::secure_cell_context_imprint_with_key bob_cell(bob_key);

    uint8_t message[] = "[redacted]";           // NOLINT(cppcoreguidelines-avoid-c-arrays)
    uint8_t contextA[] = "prison break plans";  // NOLINT(cppcoreguidelines-avoid-c-arrays)
    uint8_t contextB[] = "newspaper interview"; // NOLINT(cppcoreguidelines-avoid-c-arrays)

    std::vector<uint8_t> encrypted = alice_cell.encrypt(message, contextA);
    {
        std::vector<uint8_t> decrypted = bob_cell.decrypt(encrypted, contextA);
        sput_fail_unless(decrypted != message, "decrypt with different secret", __LINE__);
    }

    themispp::secure_cell_context_imprint_with_key bob_cell_2(alice_key);
    {
        std::vector<uint8_t> decrypted = bob_cell_2.decrypt(encrypted, contextB);
        sput_fail_unless(decrypted != message, "decrypt with different context", __LINE__);
    }

    std::vector<uint8_t> decrypted = bob_cell_2.decrypt(encrypted, contextA);
    sput_fail_unless(decrypted == message, "matching secret and context", __LINE__);
}

static inline void silly_arguments()
{
    std::vector<uint8_t> empty;

    try {
        themispp::secure_cell_context_imprint_with_key cell(empty);
        sput_fail_unless(false, "empty secret not allowed", __LINE__);
    } catch (const themispp::exception_t& e) {
        sput_fail_unless(true, "empty secret not allowed", __LINE__);
    }

    std::vector<uint8_t> master_key = themispp::gen_sym_key();
    themispp::secure_cell_context_imprint_with_key cell(master_key);

    uint8_t message[] = "important message"; // NOLINT(cppcoreguidelines-avoid-c-arrays)
    uint8_t context[] = "important testing"; // NOLINT(cppcoreguidelines-avoid-c-arrays)

    try {
        cell.encrypt(empty, context);
        sput_fail_unless(false, "encrypt: empty plaintext not allowed", __LINE__);
    } catch (const themispp::exception_t& e) {
        sput_fail_unless(true, "encrypt: empty plaintext not allowed", __LINE__);
    }
    try {
        cell.encrypt(message, empty);
        sput_fail_unless(false, "encrypt: empty context not allowed", __LINE__);
    } catch (const themispp::exception_t& e) {
        sput_fail_unless(true, "encrypt: empty context not allowed", __LINE__);
    }

    try {
        cell.decrypt(empty, context);
        sput_fail_unless(false, "decrypt: empty ciphertext not allowed", __LINE__);
    } catch (const themispp::exception_t& e) {
        sput_fail_unless(true, "decrypt: empty ciphertext not allowed", __LINE__);
    }
    try {
        cell.decrypt(message, empty);
        sput_fail_unless(false, "decrypt: empty context not allowed", __LINE__);
    } catch (const themispp::exception_t& e) {
        sput_fail_unless(true, "decrypt: empty context not allowed", __LINE__);
    }
}

static inline void loss_of_patience()
{
    std::vector<uint8_t> master_key = themispp::gen_sym_key();
    themispp::secure_cell_context_imprint_with_key cell(master_key);

    uint8_t message[] = "important message"; // NOLINT(cppcoreguidelines-avoid-c-arrays)
    uint8_t context[] = "important testing"; // NOLINT(cppcoreguidelines-avoid-c-arrays)

    std::vector<uint8_t> encrypted = cell.encrypt(message, context);

    {
        std::vector<uint8_t> truncated = encrypted;
        truncated.resize(truncated.size() - 1);
        std::vector<uint8_t> decrypted = cell.decrypt(truncated, context);
        sput_fail_unless(decrypted != message, "decrypts truncated message into garbage", __LINE__);
    }

    {
        std::vector<uint8_t> extended = encrypted;
        extended.resize(extended.size() + 1);
        std::vector<uint8_t> decrypted = cell.decrypt(extended, context);
        sput_fail_unless(decrypted != message, "decrypts extended message into garbage", __LINE__);
    }

    {
        std::vector<uint8_t> corrupted = encrypted;
        // Dunno, flip every odd byte? That should do it
        for (size_t i = 0; i < corrupted.size(); i++) {
            corrupted[i] ^= (i % 2) ? 0xFF : 0x00;
        }
        std::vector<uint8_t> decrypted = cell.decrypt(corrupted, context);
        sput_fail_unless(decrypted != message, "decrypts corrupted message into garbage", __LINE__);
    }
}

static inline void showcase()
{
    std::vector<uint8_t> master_key = themispp::gen_sym_key();
    themispp::secure_cell_context_imprint_with_key cell(master_key);

    uint8_t message[] = "important message"; // NOLINT(cppcoreguidelines-avoid-c-arrays)
    uint8_t context[] = "important testing"; // NOLINT(cppcoreguidelines-avoid-c-arrays)

    std::vector<uint8_t> encrypted = cell.encrypt(message, context);
    std::vector<uint8_t> decrypted = cell.decrypt(encrypted, context);

    sput_fail_unless(encrypted.size() == sizeof(message), "message keeps same size", __LINE__);
    sput_fail_unless(decrypted == message, "message decrypts into original data", __LINE__);
}

static inline void in_depth()
{
    miscommunication();
    silly_arguments();
    loss_of_patience();
}

} // namespace context_imprint

inline void run_secure_cell_test()
{
    sput_enter_suite("ThemisPP secure cell seal mode test:");
    sput_run_test(secure_cell_keygen_test, "secure_cell_keygen_test", __FILE__);
    sput_run_test(secure_cell_construction_test, "secure_cell_construction_test", __FILE__);
    sput_run_test(secure_cell_seal_test, "secure_cell_seal_test", __FILE__);
    sput_run_test(secure_cell_seal_context_test, "secure_cell_seal_context_test", __FILE__);
    sput_run_test(secure_cell_token_protect_test, "secure_cell_token_protect_test", __FILE__);
    sput_run_test(secure_cell_token_protect_context_test,
                  "secure_cell_token_protect_context_test",
                  __FILE__);
    sput_run_test(secure_cell_context_imprint_test, "secure_cell_context_imprint_test", __FILE__);

    sput_enter_suite("Secure Cell, Seal mode, master key API");
    sput_run_test(seal::master_key_showcase, "happy path", __FILE__);
    sput_run_test(seal::master_key_in_depth, "error handling", __FILE__);

    sput_enter_suite("Secure Cell, Seal mode, passphrase API");
    sput_run_test(seal::passphrase_showcase, "happy path", __FILE__);
    sput_run_test(seal::passphrase_in_depth, "error handling", __FILE__);

    sput_enter_suite("Secure Cell, Token Protect mode, master key API");
    sput_run_test(token_protect::master_key_showcase, "happy path", __FILE__);
    sput_run_test(token_protect::master_key_in_depth, "error handling", __FILE__);

    sput_enter_suite("Secure Cell, Context Imprint mode");
    sput_run_test(context_imprint::showcase, "happy path", __FILE__);
    sput_run_test(context_imprint::in_depth, "error handling", __FILE__);
}

} // namespace secure_cell_test
} // namespace themispp

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

#endif
