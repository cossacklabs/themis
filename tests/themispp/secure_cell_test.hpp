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
#include <vector>

#include <common/sput.h>

#include <themispp/secure_cell.hpp>
#include <themispp/secure_keygen.hpp>

#include "utils.hpp"

namespace themispp
{
namespace secure_cell_test
{

static const std::vector<uint8_t> password1 = as_bytes("password1");
static const std::vector<uint8_t> password2 = as_bytes("password2");

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
        sput_fail_unless(false, "empty password (seal)", __LINE__);
    } catch (const themispp::exception_t&) {
        sput_fail_unless(true, "empty password (seal)", __LINE__);
    }

    try {
        themispp::secure_cell_token_protect_t seal(empty);
        sput_fail_unless(false, "empty password (token protect)", __LINE__);
    } catch (const themispp::exception_t&) {
        sput_fail_unless(true, "empty password (token protect)", __LINE__);
    }

    try {
        themispp::secure_cell_context_imprint_t seal(empty);
        sput_fail_unless(false, "empty password (context imprint)", __LINE__);
    } catch (const themispp::exception_t&) {
        sput_fail_unless(true, "empty password (context imprint)", __LINE__);
    }
}

static void secure_cell_seal_test()
{
    // construction
    themispp::secure_cell_seal_t a1(password1.begin(), password1.end());
    themispp::secure_cell_seal_t a2(password1);
    themispp::secure_cell_seal_t b1(password2);

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
    themispp::secure_cell_seal_t a1(password1.begin(), password1.end());
    themispp::secure_cell_seal_t a2(password1);
    themispp::secure_cell_seal_t b1(password2);

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
    themispp::secure_cell_token_protect_t encrypter1(password1.begin(), password1.end());
    themispp::secure_cell_token_protect_t encrypter2(password1);
    themispp::secure_cell_token_protect_t encrypter3(password2);

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
    themispp::secure_cell_token_protect_t encrypter1(password1.begin(), password1.end());
    themispp::secure_cell_token_protect_t encrypter2(password1);
    themispp::secure_cell_token_protect_t encrypter3(password2);

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
    themispp::secure_cell_context_imprint_t encrypter1(password1);
    themispp::secure_cell_context_imprint_t encrypter2(password1);
    themispp::secure_cell_context_imprint_t encrypter3(password2);

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
}

} // namespace secure_cell_test
} // namespace themispp

#endif
