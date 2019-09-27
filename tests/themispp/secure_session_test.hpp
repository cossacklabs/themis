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

#ifndef THEMISPP_SECURE_SESSION_TEST_HPP_
#define THEMISPP_SECURE_SESSION_TEST_HPP_

#include <string.h>

#if __cplusplus >= 201103L
#include <unordered_map>
#endif

#include <common/sput.h>

#include <themispp/secure_session.hpp>

#include "utils.hpp"

// Allow usage of non-owning Secure Session constructor for testing
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

// We need to specialize std::hash for std::vector in order to use it with
// std::unordered_map later. Use boost::hash_combine to combine hash values.
#if __cplusplus >= 201103L
namespace
{
template <class T>
inline void hash_combine(std::size_t& seed, T const& v)
{
    seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}
} // anonymous namespace

namespace std
{
template <typename T>
struct hash<vector<T>> {
    typedef vector<T> argument_type;
    typedef std::size_t result_type;
    result_type operator()(argument_type const& in) const
    {
        size_t size = in.size();
        size_t seed = 0;
        for (size_t i = 0; i < size; i++) {
            hash_combine(seed, in[i]);
        }
        return seed;
    }
};
} // namespace std
#endif

namespace themispp
{
namespace secure_session_test
{

static const std::vector<uint8_t> client_id = as_bytes("client");
static const std::vector<uint8_t> server_id = as_bytes("server");

// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays): C++03 compatbility
const uint8_t server_priv[] = {0x52, 0x45, 0x43, 0x32, 0x00, 0x00, 0x00, 0x2d, 0x6c,
                               0x07, 0x99, 0x10, 0x00, 0x49, 0xbe, 0x76, 0x15, 0x5e,
                               0xeb, 0x8b, 0x68, 0xb5, 0x7f, 0x0f, 0xc4, 0xfd, 0xb5,
                               0x16, 0x41, 0x74, 0x7c, 0xf4, 0xe3, 0xed, 0xd7, 0xaf,
                               0x02, 0x7e, 0x95, 0x7d, 0xcf, 0x6d, 0x9b, 0x4e, 0x4a};
static const std::vector<uint8_t> server_private_key = as_bytes(server_priv);

// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays): C++03 compatbility
const uint8_t server_pub[] = {0x55, 0x45, 0x43, 0x32, 0x00, 0x00, 0x00, 0x2d, 0x16,
                              0x2d, 0x68, 0x68, 0x03, 0x96, 0x1a, 0xcd, 0xd1, 0x5b,
                              0x01, 0xea, 0x18, 0xb1, 0x63, 0x83, 0x2b, 0xe6, 0x85,
                              0x8d, 0x5e, 0xbf, 0xd2, 0xb0, 0xc2, 0xe1, 0x2a, 0xa3,
                              0x24, 0xeb, 0x37, 0xd3, 0xe7, 0xc7, 0xa2, 0xab, 0xdd};
static const std::vector<uint8_t> server_public_key = as_bytes(server_pub);

// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays): C++03 compatbility
const uint8_t client_priv[] = {0x52, 0x45, 0x43, 0x32, 0x00, 0x00, 0x00, 0x2d, 0xf2,
                               0x6a, 0x06, 0x8c, 0x00, 0x5b, 0xd7, 0x35, 0x5c, 0x53,
                               0x41, 0x89, 0xcd, 0x08, 0xa2, 0x30, 0xd2, 0x0b, 0x32,
                               0x4c, 0x74, 0x87, 0xc9, 0x0c, 0x2e, 0x9c, 0x86, 0xbf,
                               0x40, 0xf6, 0xea, 0xac, 0x8a, 0x75, 0x63, 0x67, 0xbc};
static const std::vector<uint8_t> client_private_key = as_bytes(client_priv);

// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays): C++03 compatbility
const uint8_t client_pub[] = {0x55, 0x45, 0x43, 0x32, 0x00, 0x00, 0x00, 0x2d, 0xba,
                              0x94, 0x97, 0xf7, 0x03, 0x06, 0x4f, 0x4c, 0xe0, 0x82,
                              0x74, 0xe3, 0xe9, 0x23, 0xa3, 0xac, 0x57, 0x1f, 0x1e,
                              0xf8, 0xf0, 0x66, 0x85, 0xee, 0xa3, 0x4c, 0x4f, 0xd5,
                              0xb9, 0x85, 0xc2, 0xde, 0x39, 0x49, 0x8d, 0x99, 0xe1};
static const std::vector<uint8_t> client_public_key = as_bytes(client_pub);

class callback : public themispp::secure_session_callback_interface_t
{
public:
    std::vector<uint8_t> get_pub_key_by_id(const std::vector<uint8_t>& id)
    {
        if (id == client_id) {
            return client_public_key;
        }
        if (id == server_id) {
            return server_public_key;
        }
        return std::vector<uint8_t>(0);
    }
};

static void secure_session_invalid_arguments()
{
    std::vector<uint8_t> empty;
    callback callbacks;

    try {
        themispp::secure_session_t client(empty, client_private_key, &callbacks);
        sput_fail_unless(false, "empty client ID", __LINE__);
    } catch (const themispp::exception_t&) {
        sput_fail_unless(true, "empty client ID", __LINE__);
    }

    try {
        themispp::secure_session_t client(client_id, empty, &callbacks);
        sput_fail_unless(false, "empty private key", __LINE__);
    } catch (const themispp::exception_t&) {
        sput_fail_unless(true, "empty private key", __LINE__);
    }

    try {
        themispp::secure_session_t client(client_id, client_private_key, 0);
        sput_fail_unless(false, "empty callback", __LINE__);
    } catch (const themispp::exception_t&) {
        sput_fail_unless(true, "empty callbacks", __LINE__);
    }
}

static void secure_session_test()
{
    std::vector<uint8_t> message = as_bytes("the test message");

    callback client_callbacks;
    themispp::secure_session_t client(client_id, client_private_key, &client_callbacks);

    callback server_callbacks;
    themispp::secure_session_t server(server_id, server_private_key, &server_callbacks);

    std::vector<uint8_t> control_msg1 = client.init();
    std::vector<uint8_t> control_msg2 = server.unwrap(control_msg1);
    std::vector<uint8_t> control_msg3 = client.unwrap(control_msg2);
    std::vector<uint8_t> control_msg4 = server.unwrap(control_msg3);
    std::vector<uint8_t> control_msg5 = client.unwrap(control_msg4);
    sput_fail_unless(server.is_established(), "server ready", __LINE__);
    sput_fail_unless(client.is_established(), "client ready", __LINE__);

    std::vector<uint8_t> msg1 = client.wrap(message);
    std::vector<uint8_t> msg2 = server.unwrap(msg1);
    sput_fail_unless(msg2 == message, "server get message", __LINE__);

    std::vector<uint8_t> msg3 = server.wrap(message);
    std::vector<uint8_t> msg4 = client.unwrap(msg3);
    sput_fail_unless(msg4 == message, "client get message", __LINE__);
}

static void secure_session_uninitialized()
{
    themispp::secure_session_t client;

    bool init_throws = false;
    try {
        client.init();
    } catch (const themispp::exception_t&) {
        init_throws = true;
    }
    sput_fail_unless(init_throws, "using uninitialized session", __LINE__);
}

#if __cplusplus >= 201103L
static void secure_session_moved()
{
    callback client_callbacks;
    themispp::secure_session_t client(client_id, client_private_key, &client_callbacks);

    std::unordered_map<std::vector<uint8_t>, themispp::secure_session_t> clients;
    clients.emplace(client_id, std::move(client));

    bool init_throws = false;
    try {
        // NOLINTNEXTLINE: intentional use-after-move
        client.init();
    } catch (const themispp::exception_t&) {
        init_throws = true;
    }
    sput_fail_unless(init_throws, "deny using moved-out session", __LINE__);

    std::vector<uint8_t> control_msg1 = clients.at(client_id).init();
    sput_fail_if(control_msg1.empty(), "initializing moved session", __LINE__);

    client = std::move(clients.at(client_id));

    sput_fail_if(client.is_established(), "using moved session again", __LINE__);
}

static void secure_session_ownership()
{
    auto client_callbacks_shared = std::make_shared<callback>();
    auto client_callbacks_unique = std::unique_ptr<callback>(new callback());

    themispp::secure_session_t client_shared(client_id,
                                             client_private_key,
                                             std::move(client_callbacks_shared));
    themispp::secure_session_t client_unique(client_id,
                                             client_private_key,
                                             std::move(client_callbacks_unique));

    themispp::secure_session_t client_shared_moved = std::move(client_shared);
    themispp::secure_session_t client_unique_moved = std::move(client_unique);

    sput_fail_if(client_shared_moved.is_established(), "using shared session", __LINE__);
    sput_fail_if(client_unique_moved.is_established(), "using unique session", __LINE__);

    bool shared_throws = false;
    bool unique_throws = false;
    try {
        // NOLINTNEXTLINE: intentional use-after-move
        client_shared.is_established();
    } catch (const themispp::exception_t&) {
        shared_throws = true;
    }
    try {
        // NOLINTNEXTLINE: intentional use-after-move
        client_unique.is_established();
    } catch (const themispp::exception_t&) {
        unique_throws = true;
    }
    sput_fail_unless(shared_throws, "using shared session after move", __LINE__);
    sput_fail_unless(unique_throws, "using unique session after move", __LINE__);
}
#else
static void secure_session_moved()
{
    sput_fail_if(false, "move semantics (disabled for C++03)", __LINE__);
}

static void secure_session_ownership()
{
    sput_fail_if(false, "ownership transfer (disabled for C++03)", __LINE__);
}
#endif

inline int run_secure_session_test()
{
    sput_enter_suite("ThemisPP secure session test");
    sput_run_test(secure_session_invalid_arguments, "secure_session_invalid_arguments", __FILE__);
    sput_run_test(secure_session_test, "secure_session_test", __FILE__);
    sput_run_test(secure_session_uninitialized, "secure_session_uninitialized", __FILE__);
    sput_run_test(secure_session_moved, "secure_session_moved", __FILE__);
    sput_run_test(secure_session_ownership, "secure_session_ownership", __FILE__);
    return 0;
}

} // namespace secure_session_test
} // namespace themispp
#endif

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif
