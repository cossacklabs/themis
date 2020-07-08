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

#include "secure_message.hpp"

#include <node_buffer.h>

#include <themis/themis.h>

#include "common.hpp"
#include "errors.hpp"
#include "secure_keygen.hpp"

namespace jsthemis
{

Nan::Persistent<v8::Function> SecureMessage::constructor;

SecureMessage::SecureMessage(const std::vector<uint8_t>& private_key,
                             const std::vector<uint8_t>& peer_public_key)
    : private_key_(private_key)
    , peer_public_key_(peer_public_key)
{
}

SecureMessage::~SecureMessage()
{
}

void SecureMessage::Init(v8::Local<v8::Object> exports)
{
    v8::Local<v8::String> className = Nan::New("SecureMessage").ToLocalChecked();
    // Prepare constructor template
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(SecureMessage::New);
    tpl->SetClassName(className);
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    // Prototype
    Nan::SetPrototypeMethod(tpl, "encrypt", SecureMessage::encrypt);
    Nan::SetPrototypeMethod(tpl, "decrypt", SecureMessage::decrypt);
    Nan::SetPrototypeMethod(tpl, "sign", SecureMessage::sign);
    Nan::SetPrototypeMethod(tpl, "verify", SecureMessage::verify);
    // Export constructor
    v8::Local<v8::Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(exports, className, function);
}

void SecureMessage::New(const Nan::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.IsConstructCall()) {
        if (args.Length() < 2) {
            ThrowParameterError("Secure Message constructor",
                                "not enough arguments, expected private and public key");
            args.GetReturnValue().SetUndefined();
            return;
        }
        std::vector<uint8_t> private_key;
        if (!args[0]->IsNull()) {
            if (!args[0]->IsUint8Array()) {
                ThrowTypeError("SecureMessage",
                               "private key is not a byte buffer, use ByteBuffer or Uint8Array");
                args.GetReturnValue().SetUndefined();
                return;
            }
            assign_buffer_to_vector(private_key, args[0]);
        }
        std::vector<uint8_t> public_key;
        if (!args[1]->IsNull()) {
            if (!args[1]->IsUint8Array()) {
                ThrowTypeError("SecureMessage",
                               "public key is not a byte buffer, use ByteBuffer or Uint8Array");
                args.GetReturnValue().SetUndefined();
                return;
            }
            assign_buffer_to_vector(public_key, args[1]);
        }
        if (!ValidateKeys(private_key, public_key)) {
            args.GetReturnValue().SetUndefined();
            return;
        }
        SecureMessage* obj = new SecureMessage(private_key, public_key);
        obj->Wrap(args.This());
        args.GetReturnValue().Set(args.This());
        return;
    }
    const int argc = 2;
    v8::Local<v8::Value> argv[argc] = {args[0], args[1]};
    v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
    args.GetReturnValue().Set(Nan::NewInstance(cons, argc, argv).ToLocalChecked());
}

bool SecureMessage::ValidateKeys(const std::vector<uint8_t>& private_key,
                                 const std::vector<uint8_t>& public_key)
{
    if (private_key.empty() && public_key.empty()) {
        ThrowParameterError("SecureMessage", "private and public key cannot be both empty");
        return false;
    }
    if (!private_key.empty()) {
        if (!IsValidKey(private_key)) {
            ThrowParameterError("Secure Message constructor", "invalid private key");
            return false;
        }
        if (!IsPrivateKey(private_key)) {
            ThrowParameterError("Secure Message constructor", "using public key instead of private key");
            return false;
        }
    }
    if (!public_key.empty()) {
        if (!IsValidKey(public_key)) {
            ThrowParameterError("Secure Message constructor", "invalid public key");
            return false;
        }
        if (!IsPublicKey(public_key)) {
            ThrowParameterError("Secure Message constructor", "using private key instead of public key");
            return false;
        }
    }
    return true;
}

void SecureMessage::encrypt(const Nan::FunctionCallbackInfo<v8::Value>& args)
{
    themis_status_t status = THEMIS_FAIL;
    SecureMessage* obj = Nan::ObjectWrap::Unwrap<SecureMessage>(args.This());
    if (obj->private_key_.empty()) {
        ThrowParameterError("Secure Message failed to encrypt message", "private key is empty");
        args.GetReturnValue().SetUndefined();
        return;
    }
    if (obj->peer_public_key_.empty()) {
        ThrowParameterError("Secure Message failed to encrypt message", "public key is empty");
        args.GetReturnValue().SetUndefined();
        return;
    }
    if (args.Length() < 1) {
        ThrowParameterError("Secure Message failed to encrypt message",
                            "not enough arguments, expected message");
        args.GetReturnValue().SetUndefined();
        return;
    }
    if (!args[0]->IsUint8Array()) {
        ThrowTypeError("SecureMessage", "message is not a byte buffer, use ByteBuffer or Uint8Array");
        args.GetReturnValue().SetUndefined();
        return;
    }
    if (node::Buffer::Length(args[0]) == 0) {
        ThrowParameterError("Secure Message failed to encrypt message", "message is empty");
        args.GetReturnValue().SetUndefined();
        return;
    }
    size_t encrypted_length = 0;
    status = themis_secure_message_encrypt(&(obj->private_key_)[0],
                                           obj->private_key_.size(),
                                           &(obj->peer_public_key_)[0],
                                           obj->peer_public_key_.size(),
                                           (const uint8_t*)(node::Buffer::Data(args[0])),
                                           node::Buffer::Length(args[0]),
                                           NULL,
                                           &encrypted_length);
    if (status != THEMIS_BUFFER_TOO_SMALL) {
        ThrowError("Secure Message failed to encrypt message", status);
        args.GetReturnValue().SetUndefined();
        return;
    }
    uint8_t* encrypted_data = (uint8_t*)(malloc(encrypted_length));
    status = themis_secure_message_encrypt(&(obj->private_key_)[0],
                                           obj->private_key_.size(),
                                           &(obj->peer_public_key_)[0],
                                           obj->peer_public_key_.size(),
                                           (const uint8_t*)(node::Buffer::Data(args[0])),
                                           node::Buffer::Length(args[0]),
                                           encrypted_data,
                                           &encrypted_length);
    if (status != THEMIS_SUCCESS) {
        ThrowError("Secure Message failed to encrypt message", status);
        free(encrypted_data);
        args.GetReturnValue().SetUndefined();
        return;
    }
    args.GetReturnValue().Set(Nan::NewBuffer((char*)encrypted_data, encrypted_length).ToLocalChecked());
}

void SecureMessage::decrypt(const Nan::FunctionCallbackInfo<v8::Value>& args)
{
    themis_status_t status = THEMIS_FAIL;
    SecureMessage* obj = Nan::ObjectWrap::Unwrap<SecureMessage>(args.This());
    if (obj->private_key_.empty()) {
        ThrowParameterError("Secure Message failed to decrypt message", "private key is empty");
        args.GetReturnValue().SetUndefined();
        return;
    }
    if (obj->peer_public_key_.empty()) {
        ThrowParameterError("Secure Message failed to decrypt message", "public key is empty");
        args.GetReturnValue().SetUndefined();
        return;
    }
    if (args.Length() < 1) {
        ThrowParameterError("Secure Message failed to decrypt message",
                            "not enough arguments, expected message");
        args.GetReturnValue().SetUndefined();
        return;
    }
    if (!args[0]->IsUint8Array()) {
        ThrowTypeError("SecureMessage", "message is not a byte buffer, use ByteBuffer or Uint8Array");
        args.GetReturnValue().SetUndefined();
        return;
    }
    if (node::Buffer::Length(args[0]) == 0) {
        ThrowParameterError("Secure Message failed to decrypt message", "message is empty");
        args.GetReturnValue().SetUndefined();
        return;
    }
    size_t decrypted_length = 0;
    status = themis_secure_message_decrypt(&(obj->private_key_)[0],
                                           obj->private_key_.size(),
                                           &(obj->peer_public_key_)[0],
                                           obj->peer_public_key_.size(),
                                           (const uint8_t*)(node::Buffer::Data(args[0])),
                                           node::Buffer::Length(args[0]),
                                           NULL,
                                           &decrypted_length);
    if (status != THEMIS_BUFFER_TOO_SMALL) {
        ThrowError("Secure Message failed to decrypt message", status);
        args.GetReturnValue().SetUndefined();
        return;
    }
    uint8_t* decrypted_data = (uint8_t*)(malloc(decrypted_length));
    status = themis_secure_message_decrypt(&(obj->private_key_)[0],
                                           obj->private_key_.size(),
                                           &(obj->peer_public_key_)[0],
                                           obj->peer_public_key_.size(),
                                           (const uint8_t*)(node::Buffer::Data(args[0])),
                                           node::Buffer::Length(args[0]),
                                           decrypted_data,
                                           &decrypted_length);
    if (status != THEMIS_SUCCESS) {
        ThrowError("Secure Message failed to decrypt message", status);
        free(decrypted_data);
        args.GetReturnValue().SetUndefined();
        return;
    }
    args.GetReturnValue().Set(Nan::NewBuffer((char*)decrypted_data, decrypted_length).ToLocalChecked());
}

void SecureMessage::sign(const Nan::FunctionCallbackInfo<v8::Value>& args)
{
    themis_status_t status = THEMIS_FAIL;
    SecureMessage* obj = Nan::ObjectWrap::Unwrap<SecureMessage>(args.This());
    if (obj->private_key_.empty()) {
        ThrowParameterError("Secure Message failed to sign message", "private key is empty");
        args.GetReturnValue().SetUndefined();
        return;
    }
    if (args.Length() < 1) {
        ThrowParameterError("Secure Message failed to sign message",
                            "not enough arguments, expected message");
        args.GetReturnValue().SetUndefined();
        return;
    }
    if (!args[0]->IsUint8Array()) {
        ThrowTypeError("SecureMessage", "message is not a byte buffer, use ByteBuffer or Uint8Array");
        args.GetReturnValue().SetUndefined();
        return;
    }
    if (node::Buffer::Length(args[0]) == 0) {
        ThrowParameterError("Secure Message failed to sign message", "message is empty");
        args.GetReturnValue().SetUndefined();
        return;
    }
    size_t encrypted_length = 0;
    status = themis_secure_message_sign(&(obj->private_key_)[0],
                                        obj->private_key_.size(),
                                        (const uint8_t*)(node::Buffer::Data(args[0])),
                                        node::Buffer::Length(args[0]),
                                        NULL,
                                        &encrypted_length);
    if (status != THEMIS_BUFFER_TOO_SMALL) {
        ThrowError("Secure Message failed to sign message", status);
        args.GetReturnValue().SetUndefined();
        return;
    }
    uint8_t* encrypted_data = (uint8_t*)(malloc(encrypted_length));
    status = themis_secure_message_sign(&(obj->private_key_)[0],
                                        obj->private_key_.size(),
                                        (const uint8_t*)(node::Buffer::Data(args[0])),
                                        node::Buffer::Length(args[0]),
                                        encrypted_data,
                                        &encrypted_length);
    if (status != THEMIS_SUCCESS) {
        ThrowError("Secure Message failed to sign message", status);
        free(encrypted_data);
        args.GetReturnValue().SetUndefined();
        return;
    }
    args.GetReturnValue().Set(Nan::NewBuffer((char*)encrypted_data, encrypted_length).ToLocalChecked());
}

void SecureMessage::verify(const Nan::FunctionCallbackInfo<v8::Value>& args)
{
    themis_status_t status = THEMIS_FAIL;
    SecureMessage* obj = Nan::ObjectWrap::Unwrap<SecureMessage>(args.This());
    if (obj->peer_public_key_.empty()) {
        ThrowParameterError("Secure Message failed to verify signature", "public key is empty");
        args.GetReturnValue().SetUndefined();
        return;
    }
    if (args.Length() < 1) {
        ThrowParameterError("Secure Message failed to verify signature",
                            "not enough arguments, expected message");
        args.GetReturnValue().SetUndefined();
        return;
    }
    if (!args[0]->IsUint8Array()) {
        ThrowTypeError("SecureMessage", "message is not byte buffer, use ByteBuffer or Uint8Array");
        args.GetReturnValue().SetUndefined();
        return;
    }
    if (node::Buffer::Length(args[0]) == 0) {
        ThrowParameterError("Secure Message failed to verify signature", "message is empty");
        args.GetReturnValue().SetUndefined();
        return;
    }
    size_t decrypted_length = 0;
    status = themis_secure_message_verify(&(obj->peer_public_key_)[0],
                                          obj->peer_public_key_.size(),
                                          (const uint8_t*)(node::Buffer::Data(args[0])),
                                          node::Buffer::Length(args[0]),
                                          NULL,
                                          &decrypted_length);
    if (status != THEMIS_BUFFER_TOO_SMALL) {
        ThrowError("Secure Message failed to verify signature", status);
        args.GetReturnValue().SetUndefined();
        return;
    }
    uint8_t* decrypted_data = (uint8_t*)(malloc(decrypted_length));
    status = themis_secure_message_verify(&(obj->peer_public_key_)[0],
                                          obj->peer_public_key_.size(),
                                          (const uint8_t*)(node::Buffer::Data(args[0])),
                                          node::Buffer::Length(args[0]),
                                          decrypted_data,
                                          &decrypted_length);
    if (status != THEMIS_SUCCESS) {
        ThrowError("Secure Message failed to verify signature", status);
        free(decrypted_data);
        args.GetReturnValue().SetUndefined();
        return;
    }
    args.GetReturnValue().Set(Nan::NewBuffer((char*)decrypted_data, decrypted_length).ToLocalChecked());
}

} // namespace jsthemis
