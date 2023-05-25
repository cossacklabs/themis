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

#include "secure_keygen.hpp"

#include <node_buffer.h>

#include <themis/themis.h>

#include "common.hpp"
#include "errors.hpp"

namespace jsthemis
{

Nan::Persistent<v8::Function> KeyPair::constructor;

KeyPair::KeyPair(const std::vector<uint8_t>& private_key, const std::vector<uint8_t>& public_key)
    : private_key_(private_key)
    , public_key_(public_key)
{
}

KeyPair::~KeyPair()
{
}

void KeyPair::Init(v8::Local<v8::Object> exports)
{
    v8::Local<v8::String> className = Nan::New("KeyPair").ToLocalChecked();
    // Prepare constructor template
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(KeyPair::New);
    tpl->SetClassName(className);
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    // Prototype
    Nan::SetPrototypeMethod(tpl, "private", private_key);
    Nan::SetPrototypeMethod(tpl, "public", public_key);
    // Export constructor
    v8::Local<v8::Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(exports, className, function);
}

void KeyPair::New(const Nan::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.IsConstructCall()) {
        if (args.Length() == 2) {
            if (!args[0]->IsUint8Array()) {
                ThrowTypeError("KeyPair",
                               "private key is not a byte buffer, use ByteBuffer or Uint8Array");
                return;
            }
            if (node::Buffer::Length(args[0]) == 0) {
                ThrowParameterError("Key Pair constructor", "private key is empty");
                return;
            }
            if (!args[1]->IsUint8Array()) {
                ThrowTypeError("KeyPair",
                               "public key is not a byte buffer, use ByteBuffer or Uint8Array");
                return;
            }
            if (node::Buffer::Length(args[1]) == 0) {
                ThrowParameterError("Key Pair constructor", "public key is empty");
                return;
            }
            std::vector<uint8_t> private_key((uint8_t*)(node::Buffer::Data(args[0])),
                                             (uint8_t*)(node::Buffer::Data(args[0])
                                                        + node::Buffer::Length(args[0])));
            std::vector<uint8_t> public_key((uint8_t*)(node::Buffer::Data(args[1])),
                                            (uint8_t*)(node::Buffer::Data(args[1])
                                                       + node::Buffer::Length(args[1])));
            KeyPair* obj = new KeyPair(private_key, public_key);
            obj->Wrap(args.This());
            args.GetReturnValue().Set(args.This());
        } else if (args.Length() == 0) {
            themis_status_t status = THEMIS_FAIL;
            size_t private_key_length, public_key_length;
            status = themis_gen_ec_key_pair(NULL, &private_key_length, NULL, &public_key_length);
            if (status != THEMIS_BUFFER_TOO_SMALL) {
                ThrowError("Key Pair generation failed", status);
                args.GetReturnValue().SetUndefined();
                return;
            }
            std::vector<uint8_t> prk(private_key_length);
            std::vector<uint8_t> puk(public_key_length);
            status = themis_gen_ec_key_pair(&prk[0], &private_key_length, &puk[0], &public_key_length);
            if (status != THEMIS_SUCCESS) {
                ThrowError("Key Pair generation failed", status);
                args.GetReturnValue().SetUndefined();
                return;
            }
            KeyPair* obj = new KeyPair(prk, puk);
            obj->Wrap(args.This());
            args.GetReturnValue().Set(args.This());
        } else {
            ThrowParameterError("Key Pair constructor",
                                "invalid argument count, expected no arguments or private and public keys");
            args.GetReturnValue().SetUndefined();
        }
    } else {
        const int argc = 2;
        v8::Local<v8::Value> argv[argc] = {args[0], args[1]};
        v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
        args.GetReturnValue().Set(Nan::NewInstance(cons, argc, argv).ToLocalChecked());
    }
}

void KeyPair::private_key(const Nan::FunctionCallbackInfo<v8::Value>& args)
{
    KeyPair* obj = Nan::ObjectWrap::Unwrap<KeyPair>(args.This());
    args.GetReturnValue().Set(
        Nan::CopyBuffer((char*)(&(obj->private_key_)[0]), obj->private_key_.size()).ToLocalChecked());
}

void KeyPair::public_key(const Nan::FunctionCallbackInfo<v8::Value>& args)
{
    KeyPair* obj = Nan::ObjectWrap::Unwrap<KeyPair>(args.This());
    args.GetReturnValue().Set(
        Nan::CopyBuffer((char*)(&(obj->public_key_)[0]), obj->public_key_.size()).ToLocalChecked());
}

themis_status_t ValidateKey(const std::vector<uint8_t>& key)
{
    if (key.empty()) {
        return false;
    }
    return themis_is_valid_asym_key(&key[0], key.size());
}

bool IsValidKey(const std::vector<uint8_t>& key)
{
    return ValidateKey(key) == THEMIS_SUCCESS;
}

bool IsPrivateKey(const std::vector<uint8_t>& key)
{
    if (!key.empty()) {
        themis_key_kind_t kind = themis_get_asym_key_kind(&key[0], key.size());
        switch (kind) {
        case THEMIS_KEY_EC_PRIVATE:
        case THEMIS_KEY_RSA_PRIVATE:
            return true;
        default:
            break;
        }
    }
    return false;
}

bool IsPublicKey(const std::vector<uint8_t>& key)
{
    if (!key.empty()) {
        themis_key_kind_t kind = themis_get_asym_key_kind(&key[0], key.size());
        switch (kind) {
        case THEMIS_KEY_EC_PUBLIC:
        case THEMIS_KEY_RSA_PUBLIC:
            return true;
        default:
            break;
        }
    }
    return false;
}

Nan::Persistent<v8::Function> SymmetricKey::constructor;

void SymmetricKey::Init(v8::Local<v8::Object> exports)
{
    v8::Local<v8::String> className = Nan::New("SymmetricKey").ToLocalChecked();

    // Prepare constructor template
    v8::Local<v8::FunctionTemplate> thisTemplate = Nan::New<v8::FunctionTemplate>(SymmetricKey::New);
    thisTemplate->SetClassName(className);

    // Export constructor
    v8::Local<v8::Function> function = Nan::GetFunction(thisTemplate).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(exports, className, function);
}

void SymmetricKey::New(const Nan::FunctionCallbackInfo<v8::Value>& args)
{
    // If not invoked as "new themis.SymmetricKey(...)" then reinvoke.
    if (!args.IsConstructCall()) {
        // We support at most one argument, pass it and ignore others.
        v8::Local<v8::Value> argv[1] = {args[0]};
        v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
        args.GetReturnValue().Set(Nan::NewInstance(cons, args.Length(), argv).ToLocalChecked());
        return;
    }

    // If invoked as "new themis.SymmetricKey()" then generate a new key.
    if (args.Length() == 0) {
        std::vector<uint8_t> buffer;

        size_t length = 0;
        themis_status_t status = themis_gen_sym_key(NULL, &length);
        if (status != THEMIS_BUFFER_TOO_SMALL) {
            ThrowError("Themis SymmetricKey", status);
            args.GetReturnValue().SetUndefined();
            return;
        }

        buffer.resize(length);
        status = themis_gen_sym_key(&buffer.front(), &length);
        if (status != THEMIS_SUCCESS) {
            ThrowError("Themis SymmetricKey", status);
            args.GetReturnValue().SetUndefined();
            return;
        }

        args.GetReturnValue().Set(CopyIntoBuffer(buffer));
        return;
    }

    // If invoked as "new themis.SymmetricKey(value)" then value must be
    // a byte buffer that we copy.
    v8::Local<v8::Value> value = args[0];
    if (!value->IsUint8Array()) {
        ThrowTypeError("SymmetricKey", "key is not a byte buffer (use Buffer or Uint8Array)");
        args.GetReturnValue().SetUndefined();
        return;
    }
    if (node::Buffer::Length(value) == 0) {
        ThrowParameterError("Themis SymmetricKey", "key is empty");
        args.GetReturnValue().SetUndefined();
        return;
    }

    args.GetReturnValue().Set(CopyIntoBuffer(value));
}

// TODO: return properly inherited instances of SymmetricKey
//
// Currently "new themis.SymmetricKey()" produces instances of Buffer.
// This works in practice (because JavaScript), but it may be unexpected
// as "key instanceof themis.SymmetricKey" returns false.
//
// Unfortunately, V8 does not make JavaScript prototype inheritance easier
// and I was not able to implement it correctly. It would be nice is someone
// made SymmetricKey constructor return SymmetricKey instances that inherit
// from Buffer and get all Node.js utities for free.

v8::Local<v8::Object> SymmetricKey::CopyIntoBuffer(const std::vector<uint8_t>& buffer)
{
    const char* data = NULL;
    if (!buffer.empty()) {
        data = reinterpret_cast<const char*>(&buffer.front());
    }
    uint32_t length = buffer.size();
    return Nan::CopyBuffer(data, length).ToLocalChecked();
}

v8::Local<v8::Object> SymmetricKey::CopyIntoBuffer(v8::Local<v8::Value> buffer)
{
    return Nan::CopyBuffer(node::Buffer::Data(buffer), node::Buffer::Length(buffer)).ToLocalChecked();
}

} // namespace jsthemis
