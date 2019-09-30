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

#ifndef THEMISPP_SECURE_CELL_
#define THEMISPP_SECURE_CELL_

#include <vector>

#include <themis/themis.h>

#include "exception.hpp"

namespace themispp
{

class secure_cell_t
{
public:
    typedef std::vector<uint8_t> data_t;

    secure_cell_t(data_t::const_iterator password_begin, data_t::const_iterator password_end)
        : _password(password_begin, password_end)
        , _res(0)
    {
        if (_password.empty()) {
            throw themispp::exception_t("Secure Cell must have non-empty password");
        }
    }

    secure_cell_t(const data_t& password)
        : _password(password.begin(), password.end())
        , _res(0)
    {
        if (_password.empty()) {
            throw themispp::exception_t("Secure Cell must have non-empty password");
        }
    }

#if __cplusplus >= 201103L
    virtual ~secure_cell_t() = default;
#else
    virtual ~secure_cell_t()
    {
    }
#endif

    virtual const data_t& encrypt(data_t::const_iterator data_begin,
                                  data_t::const_iterator data_end,
                                  data_t::const_iterator context_begin,
                                  data_t::const_iterator context_end) = 0;
    virtual const data_t& decrypt(data_t::const_iterator data_begin,
                                  data_t::const_iterator data_end,
                                  data_t::const_iterator context_begin,
                                  data_t::const_iterator context_end) = 0;

    const data_t& encrypt(const data_t& data, const data_t& context)
    {
        return encrypt(data.begin(), data.end(), context.begin(), context.end());
    }

    const data_t& encrypt(data_t::const_iterator data_begin,
                          data_t::const_iterator data_end,
                          const data_t& context)
    {
        return encrypt(data_begin, data_end, context.begin(), context.end());
    }

    const data_t& encrypt(const data_t& data,
                          data_t::const_iterator context_begin,
                          data_t::const_iterator context_end)
    {
        return encrypt(data.begin(), data.end(), context_begin, context_end);
    }

    const data_t& decrypt(const data_t& data, const data_t& context)
    {
        return decrypt(data.begin(), data.end(), context.begin(), context.end());
    }

    const data_t& decrypt(data_t::const_iterator data_begin,
                          data_t::const_iterator data_end,
                          const data_t& context)
    {
        return decrypt(data_begin, data_end, context.begin(), context.end());
    }

    const data_t& decrypt(const data_t& data,
                          data_t::const_iterator context_begin,
                          data_t::const_iterator context_end)
    {
        return decrypt(data.begin(), data.end(), context_begin, context_end);
    }

protected:
    data_t _password; // NOLINT: keep inheritance-based interface (for now)
    data_t _res;      // NOLINT
};

class secure_cell_optional_context_t : public secure_cell_t
{
public:
    secure_cell_optional_context_t(data_t::const_iterator password_begin,
                                   data_t::const_iterator password_end)
        : secure_cell_t(password_begin, password_end)
    {
    }

    secure_cell_optional_context_t(const data_t& password)
        : secure_cell_t(password)
    {
    }

    virtual const data_t& encrypt(data_t::const_iterator data_begin, data_t::const_iterator data_end)
    {
        data_t context(0);
        return secure_cell_t::encrypt(data_begin, data_end, context);
    }

    virtual const data_t& encrypt(const data_t& data)
    {
        return encrypt(data.begin(), data.end());
    }
    using secure_cell_t::encrypt;

    const data_t& decrypt(data_t::const_iterator data_begin, data_t::const_iterator data_end)
    {
        data_t context(0);
        return secure_cell_t::decrypt(data_begin, data_end, context);
    }

    const data_t& decrypt(const data_t& data)
    {
        return decrypt(data.begin(), data.end());
    }
    using secure_cell_t::decrypt;
};

class secure_cell_seal_t : public secure_cell_optional_context_t
{
public:
    secure_cell_seal_t(data_t::const_iterator password_begin, data_t::const_iterator password_end)
        : secure_cell_optional_context_t(password_begin, password_end)
    {
    }

    secure_cell_seal_t(const data_t& password)
        : secure_cell_optional_context_t(password)
    {
    }

    const data_t& encrypt(data_t::const_iterator data_begin,
                          data_t::const_iterator data_end,
                          data_t::const_iterator context_begin,
                          data_t::const_iterator context_end)
    {
        if (data_end <= data_begin) {
            throw themispp::exception_t(
                "Secure Cell (Seal) failed to encrypt message: data must be non-empty");
        }
        themis_status_t status = THEMIS_FAIL;
        const uint8_t* context_ptr = (context_end > context_begin) ? &(*context_begin) : NULL;
        const size_t context_len = (context_end > context_begin) ? (context_end - context_begin) : 0;
        size_t encrypted_length = 0;
        status = themis_secure_cell_encrypt_seal(&_password[0],
                                                 _password.size(),
                                                 context_ptr,
                                                 context_len,
                                                 &(*data_begin),
                                                 data_end - data_begin,
                                                 NULL,
                                                 &encrypted_length);
        if (status != THEMIS_BUFFER_TOO_SMALL) {
            throw themispp::exception_t("Secure Cell (Seal) failed to encrypt message", status);
        }
        _res.resize(encrypted_length);
        status = themis_secure_cell_encrypt_seal(&_password[0],
                                                 _password.size(),
                                                 context_ptr,
                                                 context_len,
                                                 &(*data_begin),
                                                 data_end - data_begin,
                                                 &_res[0],
                                                 &encrypted_length);
        if (status != THEMIS_SUCCESS) {
            throw themispp::exception_t("Secure Cell (Seal) failed to encrypt message", status);
        }
        return _res;
    }
    using secure_cell_optional_context_t::encrypt;

    const data_t& decrypt(data_t::const_iterator data_begin,
                          data_t::const_iterator data_end,
                          data_t::const_iterator context_begin,
                          data_t::const_iterator context_end)
    {
        if (data_end <= data_begin) {
            throw themispp::exception_t(
                "Secure Cell (Seal) failed to decrypt message: data must be non-empty");
        }
        themis_status_t status = THEMIS_FAIL;
        const uint8_t* context_ptr = (context_end > context_begin) ? &(*context_begin) : NULL;
        const size_t context_len = (context_end > context_begin) ? (context_end - context_begin) : 0;
        size_t decrypted_length = 0;
        status = themis_secure_cell_decrypt_seal(&_password[0],
                                                 _password.size(),
                                                 context_ptr,
                                                 context_len,
                                                 &(*data_begin),
                                                 data_end - data_begin,
                                                 NULL,
                                                 &decrypted_length);
        if (status != THEMIS_BUFFER_TOO_SMALL) {
            throw themispp::exception_t("Secure Cell (Seal) failed to decrypt message", status);
        }
        _res.resize(decrypted_length);
        status = themis_secure_cell_decrypt_seal(&_password[0],
                                                 _password.size(),
                                                 context_ptr,
                                                 context_len,
                                                 &(*data_begin),
                                                 data_end - data_begin,
                                                 &_res[0],
                                                 &decrypted_length);
        if (status != THEMIS_SUCCESS) {
            throw themispp::exception_t("Secure Cell (Seal) failed to decrypt message", status);
        }
        return _res;
    }
    using secure_cell_optional_context_t::decrypt;
};

class secure_cell_token_protect_t : public secure_cell_optional_context_t
{
public:
    secure_cell_token_protect_t(data_t::const_iterator password_begin, data_t::const_iterator password_end)
        : secure_cell_optional_context_t(password_begin, password_end)
        , _token(0)
    {
    }

    secure_cell_token_protect_t(const data_t& password)
        : secure_cell_optional_context_t(password)
        , _token(0)
    {
    }

    const data_t& encrypt(data_t::const_iterator data_begin,
                          data_t::const_iterator data_end,
                          data_t::const_iterator context_begin,
                          data_t::const_iterator context_end)
    {
        if (data_end <= data_begin) {
            throw themispp::exception_t(
                "Secure Cell (Token Protect) failed to encrypt message: data must be non-empty");
        }
        themis_status_t status = THEMIS_FAIL;
        const uint8_t* context_ptr = (context_end > context_begin) ? &(*context_begin) : NULL;
        const size_t context_len = (context_end > context_begin) ? (context_end - context_begin) : 0;
        size_t encrypted_length = 0;
        size_t token_length = 0;
        status = themis_secure_cell_encrypt_token_protect(&_password[0],
                                                          _password.size(),
                                                          context_ptr,
                                                          context_len,
                                                          &(*data_begin),
                                                          data_end - data_begin,
                                                          NULL,
                                                          &token_length,
                                                          NULL,
                                                          &encrypted_length);
        if (status != THEMIS_BUFFER_TOO_SMALL) {
            throw themispp::exception_t("Secure Cell (Token Protect) failed to encrypt message", status);
        }
        _res.resize(encrypted_length);
        _token.resize(token_length);
        status = themis_secure_cell_encrypt_token_protect(&_password[0],
                                                          _password.size(),
                                                          context_ptr,
                                                          context_len,
                                                          &(*data_begin),
                                                          data_end - data_begin,
                                                          &_token[0],
                                                          &token_length,
                                                          &_res[0],
                                                          &encrypted_length);
        if (status != THEMIS_SUCCESS) {
            throw themispp::exception_t("Secure Cell (Token Protect) failed to encrypt message", status);
        }
        return _res;
    }
    using secure_cell_optional_context_t::encrypt;

    const data_t& decrypt(data_t::const_iterator data_begin,
                          data_t::const_iterator data_end,
                          data_t::const_iterator context_begin,
                          data_t::const_iterator context_end)
    {
        if (data_end <= data_begin) {
            throw themispp::exception_t(
                "Secure Cell (Token Protect) failed to decrypt message: data must be non-empty");
        }
        if (_token.empty()) {
            throw themispp::exception_t("Secure Cell (Token Protect) failed to decrypt message: "
                                        "token must be non-empty (set with set_token())");
        }
        themis_status_t status = THEMIS_FAIL;
        const uint8_t* context_ptr = (context_end > context_begin) ? &(*context_begin) : NULL;
        const size_t context_len = (context_end > context_begin) ? (context_end - context_begin) : 0;
        size_t decrypted_length = 0;
        status = themis_secure_cell_decrypt_token_protect(&_password[0],
                                                          _password.size(),
                                                          context_ptr,
                                                          context_len,
                                                          &(*data_begin),
                                                          data_end - data_begin,
                                                          &_token[0],
                                                          _token.size(),
                                                          NULL,
                                                          &decrypted_length);
        if (status != THEMIS_BUFFER_TOO_SMALL) {
            throw themispp::exception_t("Secure Cell (Token Protect) failed to decrypt message", status);
        }
        _res.resize(decrypted_length);
        status = themis_secure_cell_decrypt_token_protect(&_password[0],
                                                          _password.size(),
                                                          context_ptr,
                                                          context_len,
                                                          &(*data_begin),
                                                          data_end - data_begin,
                                                          &_token[0],
                                                          _token.size(),
                                                          &_res[0],
                                                          &decrypted_length);
        if (status != THEMIS_SUCCESS) {
            throw themispp::exception_t("Secure Cell (Token Protect) failed to decrypt message", status);
        }
        return _res;
    }

    using secure_cell_optional_context_t::decrypt;

    const data_t& get_token()
    {
        return _token;
    }
    void set_token(const data_t& token)
    {
        _token = token;
    }

protected:
    data_t _token; // NOLINT: keep inheritance-based interface (for now)
};

class secure_cell_context_imprint_t : public secure_cell_t
{
public:
    secure_cell_context_imprint_t(const data_t& password)
        : secure_cell_t(password)
    {
    }

    const data_t& encrypt(data_t::const_iterator data_begin,
                          data_t::const_iterator data_end,
                          data_t::const_iterator context_begin,
                          data_t::const_iterator context_end)
    {
        if (data_end <= data_begin) {
            throw themispp::exception_t(
                "Secure Cell (Context Imprint) failed to encrypt message: data must be non-empty");
        }
        if (context_end <= context_begin) {
            throw themispp::exception_t("Secure Cell (Context Imprint) failed to encrypt message: "
                                        "context must be non-empty");
        }
        themis_status_t status = THEMIS_FAIL;
        size_t encrypted_length = 0;
        status = themis_secure_cell_encrypt_context_imprint(&_password[0],
                                                            _password.size(),
                                                            &(*context_begin),
                                                            context_end - context_begin,
                                                            &(*data_begin),
                                                            data_end - data_begin,
                                                            NULL,
                                                            &encrypted_length);
        if (status != THEMIS_BUFFER_TOO_SMALL) {
            throw themispp::exception_t("Secure Cell (Context Imprint) failed to encrypt message",
                                        status);
        }
        _res.resize(encrypted_length);
        status = themis_secure_cell_encrypt_context_imprint(&_password[0],
                                                            _password.size(),
                                                            &(*context_begin),
                                                            context_end - context_begin,
                                                            &(*data_begin),
                                                            data_end - data_begin,
                                                            &_res[0],
                                                            &encrypted_length);
        if (status != THEMIS_SUCCESS) {
            throw themispp::exception_t("Secure Cell (Context Imprint) failed to encrypt message",
                                        status);
        }
        return _res;
    }
    using secure_cell_t::encrypt;

    const data_t& decrypt(data_t::const_iterator data_begin,
                          data_t::const_iterator data_end,
                          data_t::const_iterator context_begin,
                          data_t::const_iterator context_end)
    {
        if (data_end <= data_begin) {
            throw themispp::exception_t(
                "Secure Cell (Context Imprint) failed to decrypt message: data must be non-empty");
        }
        if (context_end <= context_begin) {
            throw themispp::exception_t("Secure Cell (Context Imprint) failed to decrypt message: "
                                        "context must be non-empty");
        }
        themis_status_t status = THEMIS_FAIL;
        size_t decrypted_length = 0;
        status = themis_secure_cell_decrypt_context_imprint(&_password[0],
                                                            _password.size(),
                                                            &(*context_begin),
                                                            context_end - context_begin,
                                                            &(*data_begin),
                                                            data_end - data_begin,
                                                            NULL,
                                                            &decrypted_length);
        if (status != THEMIS_BUFFER_TOO_SMALL) {
            throw themispp::exception_t("Secure Cell (Context Imprint) failed to decrypt message",
                                        status);
        }
        _res.resize(decrypted_length);
        status = themis_secure_cell_decrypt_context_imprint(&_password[0],
                                                            _password.size(),
                                                            &(*context_begin),
                                                            context_end - context_begin,
                                                            &(*data_begin),
                                                            data_end - data_begin,
                                                            &_res[0],
                                                            &decrypted_length);
        if (status != THEMIS_SUCCESS) {
            throw themispp::exception_t("Secure Cell (Context Imprint) failed to decrypt message",
                                        status);
        }
        return _res;
    }
    using secure_cell_t::decrypt;
};

} // namespace themispp

#endif
