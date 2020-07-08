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

#include "secure_cell_context_imprint.hpp"

#include <utility>

#include <node_buffer.h>

#include <soter/soter.h>
#include <themis/themis.h>

#include "errors.hpp"

namespace jsthemis
{

Nan::Persistent<v8::Function> SecureCellContextImprint::constructor;

SecureCellContextImprint::SecureCellContextImprint(std::vector<uint8_t>&& key)
    : key_(std::move(key))
{
}

SecureCellContextImprint::~SecureCellContextImprint()
{
    soter_wipe(key_.data(), key_.size());
}

void SecureCellContextImprint::Init(v8::Local<v8::Object> exports)
{
    v8::Local<v8::String> className = Nan::New("SecureCellContextImprint").ToLocalChecked();
    // Prepare constructor template
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(SecureCellContextImprint::New);
    tpl->SetClassName(className);
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    // Static methods
    Nan::SetMethod(tpl, "withKey", WithKey);
    // Prototype
    Nan::SetPrototypeMethod(tpl, "encrypt", encrypt);
    Nan::SetPrototypeMethod(tpl, "decrypt", decrypt);
    // Export constructor
    v8::Local<v8::Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(exports, className, function);
}

void SecureCellContextImprint::New(const Nan::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.IsConstructCall()) {
        if (args.Length() < 1) {
            ThrowParameterError("Secure Cell (Context Imprint) constructor",
                                "not enough arguments, expected master key");
            args.GetReturnValue().SetUndefined();
            return;
        }
        if (!args[0]->IsUint8Array()) {
            ThrowTypeError("SecureCellContextImprint", "master key is not a byte buffer");
            args.GetReturnValue().SetUndefined();
            return;
        }
        if (node::Buffer::Length(args[0]) == 0) {
            ThrowParameterError("Secure Cell (Context Imprint) constructor", "master key is empty");
            args.GetReturnValue().SetUndefined();
            return;
        }
        std::vector<uint8_t> key((uint8_t*)(node::Buffer::Data(args[0])),
                                 (uint8_t*)(node::Buffer::Data(args[0]) + node::Buffer::Length(args[0])));
        SecureCellContextImprint* obj = new SecureCellContextImprint(std::move(key));
        obj->Wrap(args.This());
        args.GetReturnValue().Set(args.This());
    } else {
        WithKey(args);
    }
}

void SecureCellContextImprint::WithKey(const Nan::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.Length() != 1) {
        ThrowParameterError("SecureCellContextImprint",
                            "incorrect argument count, expected symmetric key");
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

void SecureCellContextImprint::encrypt(const Nan::FunctionCallbackInfo<v8::Value>& args)
{
    themis_status_t status = THEMIS_FAIL;
    SecureCellContextImprint* obj = Nan::ObjectWrap::Unwrap<SecureCellContextImprint>(args.This());
    if (args.Length() < 2) {
        ThrowParameterError("Secure Cell (Context Imprint) failed to encrypt",
                            "not enough arguments, missing message and context");
        args.GetReturnValue().SetUndefined();
        return;
    }
    if (!args[0]->IsUint8Array()) {
        ThrowTypeError("SecureCellContextImprint",
                       "message is not a byte buffer, use ByteBuffer or Uint8Array");
        args.GetReturnValue().SetUndefined();
        return;
    }
    if (node::Buffer::Length(args[0]) == 0) {
        ThrowParameterError("Secure Cell (Context Imprint) failed to encrypt", "message is empty");
        args.GetReturnValue().SetUndefined();
        return;
    }
    if (!args[1]->IsUint8Array()) {
        ThrowTypeError("SecureCellContextImprint",
                       "context is not a byte buffer, use ByteBuffer or Uint8Array");
        args.GetReturnValue().SetUndefined();
        return;
    }
    if (node::Buffer::Length(args[1]) == 0) {
        ThrowParameterError("Secure Cell (Context Imprint) failed to encrypt", "context is empty");
        args.GetReturnValue().SetUndefined();
        return;
    }
    size_t length = 0;
    const uint8_t* context = (const uint8_t*)(node::Buffer::Data(args[1]));
    size_t context_length = node::Buffer::Length(args[1]);
    status = themis_secure_cell_encrypt_context_imprint(&(obj->key_)[0],
                                                        obj->key_.size(),
                                                        (const uint8_t*)(node::Buffer::Data(args[0])),
                                                        node::Buffer::Length(args[0]),
                                                        context,
                                                        context_length,
                                                        NULL,
                                                        &length);
    if (status != THEMIS_BUFFER_TOO_SMALL) {
        ThrowError("Secure Cell (Context Imprint) failed to encrypt", status);
        args.GetReturnValue().SetUndefined();
        return;
    }
    uint8_t* data = (uint8_t*)(malloc(length));
    status = themis_secure_cell_encrypt_context_imprint(&(obj->key_)[0],
                                                        obj->key_.size(),
                                                        (const uint8_t*)(node::Buffer::Data(args[0])),
                                                        node::Buffer::Length(args[0]),
                                                        context,
                                                        context_length,
                                                        data,
                                                        &length);
    if (status != THEMIS_SUCCESS) {
        ThrowError("Secure Cell (Context Imprint) failed to encrypt", status);
        free(data);
        args.GetReturnValue().SetUndefined();
        return;
    }
    args.GetReturnValue().Set(Nan::NewBuffer((char*)(data), length).ToLocalChecked());
}

void SecureCellContextImprint::decrypt(const Nan::FunctionCallbackInfo<v8::Value>& args)
{
    themis_status_t status = THEMIS_FAIL;
    SecureCellContextImprint* obj = Nan::ObjectWrap::Unwrap<SecureCellContextImprint>(args.This());
    if (args.Length() < 2) {
        ThrowParameterError("Secure Cell (Context Imprint) failed to decrypt",
                            "not enough arguments, expected message and context");
        args.GetReturnValue().SetUndefined();
        return;
    }
    if (!args[0]->IsUint8Array()) {
        ThrowTypeError("SecureCellContextImprint",
                       "message is not a byte buffer, use ByteBuffer or Uint8Array");
        args.GetReturnValue().SetUndefined();
        return;
    }
    if (node::Buffer::Length(args[0]) == 0) {
        ThrowParameterError("Secure Cell (Context Imprint) failed to decrypt", "message is empty");
        args.GetReturnValue().SetUndefined();
        return;
    }
    if (!args[1]->IsUint8Array()) {
        ThrowTypeError("SecureCellContextImprint",
                       "context is not a byte buffer, use ByteBuffer or Uint8Array");
        args.GetReturnValue().SetUndefined();
        return;
    }
    if (node::Buffer::Length(args[1]) == 0) {
        ThrowParameterError("Secure Cell (Context Imprint) failed to decrypt", "context is empty");
        args.GetReturnValue().SetUndefined();
        return;
    }
    size_t length = 0;
    const uint8_t* context = (const uint8_t*)(node::Buffer::Data(args[1]));
    size_t context_length = node::Buffer::Length(args[1]);
    status = themis_secure_cell_decrypt_context_imprint(&(obj->key_)[0],
                                                        obj->key_.size(),
                                                        (const uint8_t*)(node::Buffer::Data(args[0])),
                                                        node::Buffer::Length(args[0]),
                                                        context,
                                                        context_length,
                                                        NULL,
                                                        &length);
    if (status != THEMIS_BUFFER_TOO_SMALL) {
        ThrowError("Secure Cell (Context Imprint) failed to decrypt", status);
        args.GetReturnValue().SetUndefined();
        return;
    }
    uint8_t* data = (uint8_t*)(malloc(length));
    status = themis_secure_cell_decrypt_context_imprint(&(obj->key_)[0],
                                                        obj->key_.size(),
                                                        (const uint8_t*)(node::Buffer::Data(args[0])),
                                                        node::Buffer::Length(args[0]),
                                                        context,
                                                        context_length,
                                                        data,
                                                        &length);
    if (status != THEMIS_SUCCESS) {
        ThrowError("Secure Cell (Context Imprint) failed to decrypt", status);
        free(data);
        args.GetReturnValue().SetUndefined();
        return;
    }
    args.GetReturnValue().Set(Nan::NewBuffer((char*)(data), length).ToLocalChecked());
}

} // namespace jsthemis
