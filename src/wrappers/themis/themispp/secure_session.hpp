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

#ifndef THEMISPP_SECURE_SESSION_HPP_
#define THEMISPP_SECURE_SESSION_HPP_

#include <cstring>
#include <vector>

#if __cplusplus >= 201103L
#include <memory>
#endif

#include <themis/themis.h>

#include "exception.hpp"

namespace themispp
{

class secure_session_callback_interface_t
{
public:
    typedef std::vector<uint8_t> data_t;
    virtual data_t get_pub_key_by_id(const data_t& id) = 0;
    virtual void send(const data_t& data)
    {
        UNUSED(data);
        throw themispp::exception_t("Secure Session failed sending, send callback not set");
    }
    virtual const data_t& receive()
    {
        throw themispp::exception_t("Secure Session failed receiving, receive callback not set");
    }
#if __cplusplus >= 201103L
    virtual ~secure_session_callback_interface_t() = default;
#else
    virtual ~secure_session_callback_interface_t()
    {
    }
#endif
};

static inline secure_session_callback_interface_t* as_callback_interface(void* user_data)
{
    return static_cast<secure_session_callback_interface_t*>(user_data);
}

inline ssize_t send_callback(const uint8_t* data, size_t data_length, void* user_data)
{
    try {
        secure_session_callback_interface_t* callbacks = as_callback_interface(user_data);
        std::vector<uint8_t> data_vec(data, data + data_length); // NOLINT
        callbacks->send(data_vec);
    } catch (...) {
        return -1;
    }
    return ssize_t(data_length);
}

inline ssize_t receive_callback(uint8_t* data, size_t data_length, void* user_data)
{
    try {
        secure_session_callback_interface_t* callbacks = as_callback_interface(user_data);
        std::vector<uint8_t> received_data = callbacks->receive();
        if (received_data.empty() || received_data.size() > data_length) {
            return -1;
        }
        memcpy(data, &received_data[0], received_data.size());
        return ssize_t(received_data.size());
    } catch (...) {
    }
    return -1;
}

inline int get_public_key_for_id_callback(
    const void* id, size_t id_length, void* key_buffer, size_t key_buffer_length, void* user_data)
{
    secure_session_callback_interface_t* callbacks = as_callback_interface(user_data);
    std::vector<uint8_t> client_id(static_cast<const uint8_t*>(id),
                                   static_cast<const uint8_t*>(id) + id_length); // NOLINT
    std::vector<uint8_t> pubk = callbacks->get_pub_key_by_id(client_id);
    if (pubk.empty()) {
        return THEMIS_FAIL;
    }
    if (pubk.size() > key_buffer_length) {
        return THEMIS_BUFFER_TOO_SMALL;
    }
    std::memcpy(key_buffer, &pubk[0], pubk.size());
    return THEMIS_SUCCESS;
}

class secure_session_t
{
public:
    typedef std::vector<uint8_t> data_t;

    secure_session_t()
        : _session(NULL)
        , _callback(NULL)
    {
    }

#if __cplusplus >= 201103L
    DEPRECATED("please use std::shared_ptr<secure_session_callback_interface_t> constructor instead")
#endif
    secure_session_t(const data_t& id, const data_t& priv_key, secure_session_callback_interface_t* callbacks)
        : _session(NULL)
        , _callback(NULL)
        , _res(0)
    {
        initialize_session(id, priv_key, callbacks);
    }

    virtual ~secure_session_t()
    {
        if (_session) {
            secure_session_destroy(_session);
            _session = NULL;
        }
        delete _callback;
        _callback = NULL;
    }

#if __cplusplus >= 201103L
    secure_session_t(const data_t& id,
                     const data_t& priv_key,
                     std::shared_ptr<secure_session_callback_interface_t>&& callbacks)
        : _session(nullptr)
        , _callback(nullptr)
        , _res(0)
        , _interface(std::move(callbacks))
    {
        initialize_session(id, priv_key, _interface.get());
    }

    secure_session_t(const secure_session_t&) = delete;
    secure_session_t& operator=(const secure_session_t&) = delete;

    secure_session_t(secure_session_t&& other) noexcept
    {
        _session = other._session;
        _callback = other._callback;
        _res = std::move(other._res);
        _interface = std::move(other._interface);
        other._session = nullptr;
        other._callback = nullptr;
    }

    secure_session_t& operator=(secure_session_t&& other) noexcept
    {
        if (this != &other) {
            if (_session) {
                secure_session_destroy(_session);
            }
            delete _callback;
            _session = other._session;
            _callback = other._callback;
            _res = std::move(other._res);
            _interface = std::move(other._interface);
            other._session = nullptr;
            other._callback = nullptr;
        }
        return *this;
    }
#else
private:
    secure_session_t(const secure_session_t&);
    secure_session_t& operator=(const secure_session_t&);

public:
#endif

    const data_t& unwrap(const std::vector<uint8_t>& data)
    {
        if (!_session) {
            throw themispp::exception_t("uninitialized Secure Session");
        }
        if (data.empty()) {
            throw themispp::exception_t("Secure Session failed to unwrap message: data must be non-empty");
        }
        themis_status_t status = THEMIS_FAIL;
        size_t unwrapped_data_length = 0;
        status = secure_session_unwrap(_session, &data[0], data.size(), NULL, &unwrapped_data_length);
        if (status == THEMIS_SUCCESS) {
            _res.resize(0);
            return _res;
        }
        if (status != THEMIS_BUFFER_TOO_SMALL) {
            throw themispp::exception_t("Secure Session failed to unwrap message", status);
        }
        _res.resize(unwrapped_data_length);
        status = secure_session_unwrap(_session, &data[0], data.size(), &_res[0], &unwrapped_data_length);
        if (status == THEMIS_SSESSION_SEND_OUTPUT_TO_PEER) {
            return _res;
        }
        if (status != THEMIS_SUCCESS) {
            throw themispp::exception_t("Secure Session failed to unwrap message", status);
        }
        return _res;
    }

    const data_t& wrap(const std::vector<uint8_t>& data)
    {
        if (!_session) {
            throw themispp::exception_t("uninitialized Secure Session");
        }
        if (data.empty()) {
            throw themispp::exception_t("Secure Session failed to wrap message: data must be non-empty");
        }
        themis_status_t status = THEMIS_FAIL;
        size_t wrapped_message_length = 0;
        status = secure_session_wrap(_session, &data[0], data.size(), NULL, &wrapped_message_length);
        if (status != THEMIS_BUFFER_TOO_SMALL) {
            throw themispp::exception_t("Secure Session failed to wrap message", status);
        }
        _res.resize(wrapped_message_length);
        status = secure_session_wrap(_session, &data[0], data.size(), &_res[0], &wrapped_message_length);
        if (status != THEMIS_SUCCESS) {
            throw themispp::exception_t("Secure Session failed to wrap message", status);
        }
        return _res;
    }

    const data_t& init()
    {
        if (!_session) {
            throw themispp::exception_t("uninitialized Secure Session");
        }
        themis_status_t status = THEMIS_FAIL;
        size_t init_data_length = 0;
        status = secure_session_generate_connect_request(_session, NULL, &init_data_length);
        if (status != THEMIS_BUFFER_TOO_SMALL) {
            throw themispp::exception_t("Secure Session failed to initialize", status);
        }
        _res.resize(init_data_length);
        status = secure_session_generate_connect_request(_session, &_res.front(), &init_data_length);
        if (status != THEMIS_SUCCESS) {
            throw themispp::exception_t("Secure Session failed to initialize", status);
        }
        return _res;
    }

    bool is_established() const
    {
        if (!_session) {
            throw themispp::exception_t("uninitialized Secure Session");
        }
        return secure_session_is_established(_session);
    }

    void connect()
    {
        if (!_session) {
            throw themispp::exception_t("uninitialized Secure Session");
        }
        themis_status_t status = secure_session_connect(_session);
        if (status != THEMIS_SUCCESS) {
            throw themispp::exception_t("Secure Session failed to connect", status);
        }
    }

    const data_t& receive()
    {
        static const size_t max_message_size = 2048;
        if (!_session) {
            throw themispp::exception_t("uninitialized Secure Session");
        }
        _res.resize(max_message_size);
        ssize_t recv_size = secure_session_receive(_session, &_res[0], _res.size());
        if (recv_size <= 0) {
            throw themispp::exception_t("Secure Session failed to receive message",
                                        THEMIS_SSESSION_TRANSPORT_ERROR);
        }
        _res.resize(recv_size);
        return _res;
    }

    void send(const data_t& data)
    {
        if (!_session) {
            throw themispp::exception_t("uninitialized Secure Session");
        }
        if (data.empty()) {
            throw themispp::exception_t("Secure Session failed to send message: data must be non-empty");
        }
        ssize_t send_size = secure_session_send(_session, &data[0], data.size());
        if (send_size <= 0) {
            throw themispp::exception_t("Secure Session failed to send message",
                                        THEMIS_SSESSION_TRANSPORT_ERROR);
        }
    }

private:
    void initialize_session(const data_t& id,
                            const data_t& priv_key,
                            secure_session_callback_interface_t* callbacks)
    {
        if (id.empty()) {
            throw themispp::exception_t("Secure Session construction failed: client ID must be non-empty");
        }
        if (priv_key.empty()) {
            throw themispp::exception_t(
                "Secure Session construction failed: private key must be non-empty");
        }
        if (!callbacks) {
            throw themispp::exception_t("Secure Session construction failed: callbacks must be non-NULL");
        }
        // NOLINTNEXTLINE(cppcoreguidelines-owning-memory): C++03 compatibility
        _callback = new secure_session_user_callbacks_t();
        _callback->get_public_key_for_id = themispp::get_public_key_for_id_callback;
        _callback->send_data = themispp::send_callback;
        _callback->receive_data = themispp::receive_callback;
        _callback->state_changed = NULL;
        _callback->user_data = callbacks;
        _session = secure_session_create(&id[0], id.size(), &priv_key[0], priv_key.size(), _callback);
        if (!_session) {
            delete _callback;
            throw themispp::exception_t("Secure Session construction failed");
        }
    }

private:
    ::secure_session_t* _session;
    ::secure_session_user_callbacks_t* _callback;
    std::vector<uint8_t> _res;
#if __cplusplus >= 201103L
    std::shared_ptr<secure_session_callback_interface_t> _interface;
#endif
};

} // namespace themispp

#endif
