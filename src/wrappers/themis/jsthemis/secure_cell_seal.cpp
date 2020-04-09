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

#include "secure_cell_seal.hpp"

#include <utility>

#include <node_buffer.h>

#include <soter/soter.h>
#include <themis/themis.h>

#include "common.hpp"
#include "errors.hpp"

namespace jsthemis
{

Nan::Persistent<v8::Function> SecureCellSeal::constructor;
Nan::Persistent<v8::FunctionTemplate> SecureCellSeal::klass;

SecureCellSeal::SecureCellSeal(std::vector<uint8_t>&& key)
    : key_(std::move(key))
{
}

SecureCellSeal::~SecureCellSeal()
{
    soter_wipe(key_.data(), key_.size());
}

void SecureCellSeal::Init(v8::Local<v8::Object> exports)
{
    v8::Local<v8::String> className = Nan::New("SecureCellSeal").ToLocalChecked();
    // Prepare constructor template
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
    klass.Reset(tpl);
    tpl->SetClassName(className);
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    // Static methods
    Nan::SetMethod(tpl, "withKey", WithKey);
    Nan::SetMethod(tpl, "withPassphrase", WithPassphrase);
    // Prototype
    Nan::SetPrototypeMethod(tpl, "encrypt", encrypt);
    Nan::SetPrototypeMethod(tpl, "decrypt", decrypt);
    // Export constructor
    v8::Local<v8::Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(exports, className, function);
}

void SecureCellSeal::New(const Nan::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.IsConstructCall()) {
        if (args.Length() < 1) {
            ThrowParameterError("Secure Cell (Seal) constructor",
                                "not enough arguments, expected master key");
            args.GetReturnValue().SetUndefined();
            return;
        }
        if (!args[0]->IsUint8Array()) {
            ThrowTypeError("SecureCellSeal",
                           "master key is not a byte buffer, use ByteBuffer or Uint8Array");
            args.GetReturnValue().SetUndefined();
            return;
        }
        if (node::Buffer::Length(args[0]) == 0) {
            ThrowParameterError("Secure Cell (Seal) constructor", "master key is empty");
            args.GetReturnValue().SetUndefined();
            return;
        }
        std::vector<uint8_t> key((uint8_t*)(node::Buffer::Data(args[0])),
                                 (uint8_t*)(node::Buffer::Data(args[0]) + node::Buffer::Length(args[0])));
        SecureCellSeal* obj = new SecureCellSeal(std::move(key));
        obj->Wrap(args.This());
        args.GetReturnValue().Set(args.This());
    } else {
        WithKey(args);
    }
}

void SecureCellSeal::WithKey(const Nan::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.Length() != 1) {
        ThrowParameterError("SecureCellSeal", "incorrect argument count, expected symmetric key");
        args.GetReturnValue().SetUndefined();
        return;
    }
    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = {args[0]};
    v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
    Nan::MaybeLocal<v8::Object> cell = Nan::NewInstance(cons, argc, argv);
    if (!cell.IsEmpty()) {
        args.GetReturnValue().Set(cell.ToLocalChecked());
    }
}

void SecureCellSeal::WithPassphrase(const Nan::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.Length() != 1) {
        ThrowParameterError("SecureCellSeal", "incorrect argument count, expected passphrase");
        args.GetReturnValue().SetUndefined();
        return;
    }
    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = {args[0]};
    v8::Local<v8::Function> cons = Nan::New<v8::Function>(SecureCellSealWithPassphrase::constructor);
    Nan::MaybeLocal<v8::Object> cell = Nan::NewInstance(cons, argc, argv);
    if (!cell.IsEmpty()) {
        args.GetReturnValue().Set(cell.ToLocalChecked());
    }
}

void SecureCellSeal::encrypt(const Nan::FunctionCallbackInfo<v8::Value>& args)
{
    themis_status_t status = THEMIS_FAIL;
    SecureCellSeal* obj = Nan::ObjectWrap::Unwrap<SecureCellSeal>(args.This());
    if (args.Length() < 1) {
        ThrowParameterError("Secure Cell (Seal) failed to encrypt",
                            "not enough arguments, expected message");
        args.GetReturnValue().SetUndefined();
        return;
    }
    if (!args[0]->IsUint8Array()) {
        ThrowTypeError("SecureCellSeal", "message is not a byte buffer, use ByteBuffer or Uint8Array");
        args.GetReturnValue().SetUndefined();
        return;
    }
    if (node::Buffer::Length(args[0]) == 0) {
        ThrowParameterError("Secure Cell (Seal) failed to encrypt", "message is empty");
        args.GetReturnValue().SetUndefined();
        return;
    }
    size_t length = 0;
    const uint8_t* context = NULL;
    size_t context_length = 0;
    if (args.Length() == 2 && !args[1]->IsNull()) {
        if (!args[1]->IsUint8Array()) {
            ThrowTypeError("SecureCellSeal",
                           "context is not a byte buffer, use ByteBuffer or Uint8Array");
            args.GetReturnValue().SetUndefined();
            return;
        }
        context = (const uint8_t*)(node::Buffer::Data(args[1]));
        context_length = node::Buffer::Length(args[1]);
    }
    status = themis_secure_cell_encrypt_seal(&(obj->key_)[0],
                                             obj->key_.size(),
                                             context,
                                             context_length,
                                             (const uint8_t*)(node::Buffer::Data(args[0])),
                                             node::Buffer::Length(args[0]),
                                             NULL,
                                             &length);
    if (status != THEMIS_BUFFER_TOO_SMALL) {
        ThrowError("Secure Cell (Seal) failed to encrypt", status);
        args.GetReturnValue().SetUndefined();
        return;
    }
    uint8_t* data = (uint8_t*)(malloc(length));
    status = themis_secure_cell_encrypt_seal(&(obj->key_)[0],
                                             obj->key_.size(),
                                             context,
                                             context_length,
                                             (const uint8_t*)(node::Buffer::Data(args[0])),
                                             node::Buffer::Length(args[0]),
                                             data,
                                             &length);
    if (status != THEMIS_SUCCESS) {
        ThrowError("Secure Cell (Seal) failed to encrypt", status);
        free(data);
        args.GetReturnValue().SetUndefined();
        return;
    }
    args.GetReturnValue().Set(Nan::NewBuffer((char*)(data), length).ToLocalChecked());
}

void SecureCellSeal::decrypt(const Nan::FunctionCallbackInfo<v8::Value>& args)
{
    themis_status_t status = THEMIS_FAIL;
    SecureCellSeal* obj = Nan::ObjectWrap::Unwrap<SecureCellSeal>(args.This());
    if (args.Length() < 1) {
        ThrowParameterError("Secure Cell (Seal) failed to decrypt",
                            "not enough arguments, expected message");
        args.GetReturnValue().SetUndefined();
        return;
    }
    if (!args[0]->IsUint8Array()) {
        ThrowTypeError("SecureCellSeal", "message is not a byte buffer, use ByteBuffer or Uint8Array");
        args.GetReturnValue().SetUndefined();
        return;
    }
    if (node::Buffer::Length(args[0]) == 0) {
        ThrowParameterError("Secure Cell (Seal) failed to decrypt", "message is empty");
        args.GetReturnValue().SetUndefined();
        return;
    }
    size_t length = 0;
    const uint8_t* context = NULL;
    size_t context_length = 0;
    if (args.Length() == 2 && !args[1]->IsNull()) {
        if (!args[1]->IsUint8Array()) {
            ThrowTypeError("SecureCellSeal",
                           "context is not a byte buffer, use ByteBuffer or Uint8Array");
            args.GetReturnValue().SetUndefined();
            return;
        }
        context = (const uint8_t*)(node::Buffer::Data(args[1]));
        context_length = node::Buffer::Length(args[1]);
    }
    status = themis_secure_cell_decrypt_seal(&(obj->key_)[0],
                                             obj->key_.size(),
                                             context,
                                             context_length,
                                             (const uint8_t*)(node::Buffer::Data(args[0])),
                                             node::Buffer::Length(args[0]),
                                             NULL,
                                             &length);
    if (status != THEMIS_BUFFER_TOO_SMALL) {
        ThrowError("Secure Cell (Seal) failed to decrypt", status);
        args.GetReturnValue().SetUndefined();
        return;
    }
    uint8_t* data = (uint8_t*)malloc(length);
    status = themis_secure_cell_decrypt_seal(&(obj->key_)[0],
                                             obj->key_.size(),
                                             context,
                                             context_length,
                                             (const uint8_t*)(node::Buffer::Data(args[0])),
                                             node::Buffer::Length(args[0]),
                                             data,
                                             &length);
    if (status != THEMIS_SUCCESS) {
        ThrowError("Secure Cell (Seal) failed to decrypt", status);
        free(data);
        args.GetReturnValue().SetUndefined();
        return;
    }
    args.GetReturnValue().Set(Nan::NewBuffer((char*)(data), length).ToLocalChecked());
}

Nan::Persistent<v8::Function> SecureCellSealWithPassphrase::constructor;

SecureCellSealWithPassphrase::SecureCellSealWithPassphrase(std::vector<uint8_t>&& passphrase)
    : m_passphrase(std::move(passphrase))
{
}

SecureCellSealWithPassphrase::~SecureCellSealWithPassphrase()
{
    soter_wipe(m_passphrase.data(), m_passphrase.size());
}

void SecureCellSealWithPassphrase::Init(v8::Local<v8::Object> /*exports*/)
{
    v8::Local<v8::String> className = Nan::New("SecureCellSealWithPassphrase").ToLocalChecked();
    // Prepare constructor template
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(className);
    tpl->Inherit(Nan::New(SecureCellSeal::klass));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    // Prototype
    Nan::SetPrototypeMethod(tpl, "encrypt", encrypt);
    Nan::SetPrototypeMethod(tpl, "decrypt", decrypt);
    // Save constructor
    v8::Local<v8::Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    // SecureCellSealWithPassphrase is not exported. Keep it private for now.
}

void SecureCellSealWithPassphrase::New(const Nan::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.Length() != 1) {
        ThrowParameterError("SecureCellSeal", "incorrect argument count, expected passphrase");
        args.GetReturnValue().SetUndefined();
        return;
    }

    std::vector<uint8_t> passphrase;

    if (args[0]->IsUint8Array()) {
        if (node::Buffer::Length(args[0]) == 0) {
            ThrowParameterError("SecureCellSeal", "passphrase must not be empty");
            args.GetReturnValue().SetUndefined();
            return;
        }
        assign_buffer_to_vector(passphrase, args[0]);
    } else if (args[0]->IsString()) {
        ssize_t length = Nan::DecodeBytes(args[0], Nan::UTF8);
        if (length < 0) {
            ThrowParameterError("SecureCellSeal", "cannot encode passphrase in UTF-8");
            args.GetReturnValue().SetUndefined();
            return;
        }
        if (length == 0) {
            ThrowParameterError("SecureCellSeal", "passphrase must not be empty");
            args.GetReturnValue().SetUndefined();
            return;
        }
        passphrase.resize(length, 0);
        Nan::DecodeWrite(reinterpret_cast<char*>(passphrase.data()), passphrase.size(), args[0], Nan::UTF8);
    } else {
        ThrowTypeError("SecureCellSeal", "passphrase must be ByteBuffer, Uint8Array, or a string");
        args.GetReturnValue().SetUndefined();
        return;
    }

    SecureCellSealWithPassphrase* object = new SecureCellSealWithPassphrase(std::move(passphrase));
    object->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
}

void SecureCellSealWithPassphrase::encrypt(const Nan::FunctionCallbackInfo<v8::Value>& args)
{
    SecureCellSealWithPassphrase* self = Nan::ObjectWrap::Unwrap<SecureCellSealWithPassphrase>(
        args.This());

    if (args.Length() < 1 || args.Length() > 2) {
        ThrowParameterError("SecureCellSeal.encrypt",
                            "incorrect argument count, expected message with optional context");
        args.GetReturnValue().SetUndefined();
        return;
    }

    if (!args[0]->IsUint8Array()) {
        ThrowTypeError("SecureCellSeal.encrypt",
                       "message is not a byte buffer, use ByteBuffer or Uint8Array");
        args.GetReturnValue().SetUndefined();
        return;
    }
    const uint8_t* message_data = reinterpret_cast<const uint8_t*>(node::Buffer::Data(args[0]));
    size_t message_length = node::Buffer::Length(args[0]);
    if (message_length == 0) {
        ThrowParameterError("SecureCellSeal.encrypt", "message must not be empty");
        args.GetReturnValue().SetUndefined();
        return;
    }

    const uint8_t* context_data = NULL;
    size_t context_length = 0;
    if (args.Length() == 2 && !args[1]->IsNull()) {
        if (!args[1]->IsUint8Array()) {
            ThrowTypeError("SecureCellSeal.encrypt",
                           "context is not a byte buffer, use ByteBuffer or Uint8Array");
            args.GetReturnValue().SetUndefined();
            return;
        }
        context_data = reinterpret_cast<const uint8_t*>(node::Buffer::Data(args[1]));
        context_length = node::Buffer::Length(args[1]);
    }

    themis_status_t status = THEMIS_FAIL;
    size_t encrypted_length = 0;

    status = themis_secure_cell_encrypt_seal_with_passphrase(self->m_passphrase.data(),
                                                             self->m_passphrase.size(),
                                                             context_data,
                                                             context_length,
                                                             message_data,
                                                             message_length,
                                                             NULL,
                                                             &encrypted_length);
    if (status != THEMIS_BUFFER_TOO_SMALL) {
        ThrowError("SecureCellSeal.encrypt", status);
        args.GetReturnValue().SetUndefined();
        return;
    }

    uint8_t* encrypted_data = reinterpret_cast<uint8_t*>(malloc(encrypted_length));
    if (!encrypted_data) {
        ThrowError("SecureCellSeal.encrypt", THEMIS_FAIL);
        args.GetReturnValue().SetUndefined();
        return;
    }

    status = themis_secure_cell_encrypt_seal_with_passphrase(self->m_passphrase.data(),
                                                             self->m_passphrase.size(),
                                                             context_data,
                                                             context_length,
                                                             message_data,
                                                             message_length,
                                                             encrypted_data,
                                                             &encrypted_length);
    if (status != THEMIS_SUCCESS) {
        free(encrypted_data);
        ThrowError("SecureCellSeal.encrypt", status);
        args.GetReturnValue().SetUndefined();
        return;
    }

    Nan::MaybeLocal<v8::Object> encrypted = Nan::NewBuffer(reinterpret_cast<char*>(encrypted_data),
                                                           encrypted_length);
    args.GetReturnValue().Set(encrypted.ToLocalChecked());
}

void SecureCellSealWithPassphrase::decrypt(const Nan::FunctionCallbackInfo<v8::Value>& args)
{
    SecureCellSealWithPassphrase* self = Nan::ObjectWrap::Unwrap<SecureCellSealWithPassphrase>(
        args.This());

    if (args.Length() < 1 || args.Length() > 2) {
        ThrowParameterError("SecureCellSeal.decrypt",
                            "incorrect argument count, expected message with optional context");
        args.GetReturnValue().SetUndefined();
        return;
    }

    if (!args[0]->IsUint8Array()) {
        ThrowTypeError("SecureCellSeal.decrypt",
                       "message is not a byte buffer, use ByteBuffer or Uint8Array");
        args.GetReturnValue().SetUndefined();
        return;
    }
    const uint8_t* message_data = reinterpret_cast<const uint8_t*>(node::Buffer::Data(args[0]));
    size_t message_length = node::Buffer::Length(args[0]);
    if (message_length == 0) {
        ThrowParameterError("SecureCellSeal.decrypt", "message must not be empty");
        args.GetReturnValue().SetUndefined();
        return;
    }

    const uint8_t* context_data = NULL;
    size_t context_length = 0;
    if (args.Length() == 2 && !args[1]->IsNull()) {
        if (!args[1]->IsUint8Array()) {
            ThrowTypeError("SecureCellSeal.decrypt",
                           "context is not a byte buffer, use ByteBuffer or Uint8Array");
            args.GetReturnValue().SetUndefined();
            return;
        }
        context_data = reinterpret_cast<const uint8_t*>(node::Buffer::Data(args[1]));
        context_length = node::Buffer::Length(args[1]);
    }

    themis_status_t status = THEMIS_FAIL;
    size_t decrypted_length = 0;

    status = themis_secure_cell_decrypt_seal_with_passphrase(self->m_passphrase.data(),
                                                             self->m_passphrase.size(),
                                                             context_data,
                                                             context_length,
                                                             message_data,
                                                             message_length,
                                                             NULL,
                                                             &decrypted_length);
    if (status != THEMIS_BUFFER_TOO_SMALL) {
        ThrowError("SecureCellSeal.decrypt", status);
        args.GetReturnValue().SetUndefined();
        return;
    }

    uint8_t* decrypted_data = reinterpret_cast<uint8_t*>(malloc(decrypted_length));
    if (!decrypted_data) {
        ThrowError("SecureCellSeal.decrypt", THEMIS_FAIL);
        args.GetReturnValue().SetUndefined();
        return;
    }

    status = themis_secure_cell_decrypt_seal_with_passphrase(self->m_passphrase.data(),
                                                             self->m_passphrase.size(),
                                                             context_data,
                                                             context_length,
                                                             message_data,
                                                             message_length,
                                                             decrypted_data,
                                                             &decrypted_length);
    if (status != THEMIS_SUCCESS) {
        free(decrypted_data);
        ThrowError("SecureCellSeal.decrypt", status);
        args.GetReturnValue().SetUndefined();
        return;
    }

    Nan::MaybeLocal<v8::Object> decrypted = Nan::NewBuffer(reinterpret_cast<char*>(decrypted_data),
                                                           decrypted_length);
    args.GetReturnValue().Set(decrypted.ToLocalChecked());
}

} // namespace jsthemis
