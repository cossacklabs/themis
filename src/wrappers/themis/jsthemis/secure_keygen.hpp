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

#ifndef JSTHEMIS_KEY_PAIR_HPP_
#define JSTHEMIS_KEY_PAIR_HPP_

#include <vector>

#include <nan.h>

#include <themis/themis.h>

namespace jsthemis
{

class KeyPair : public Nan::ObjectWrap
{
public:
    static void Init(v8::Local<v8::Object> exports);

private:
    explicit KeyPair(const std::vector<uint8_t>& private_key, const std::vector<uint8_t>& public_key);
    explicit KeyPair();
    ~KeyPair();

    static void New(const Nan::FunctionCallbackInfo<v8::Value>& args);
    static void private_key(const Nan::FunctionCallbackInfo<v8::Value>& args);
    static void public_key(const Nan::FunctionCallbackInfo<v8::Value>& args);

    static Nan::Persistent<v8::Function> constructor;

    std::vector<uint8_t> private_key_;
    std::vector<uint8_t> public_key_;
};

themis_status_t ValidateKey(const std::vector<uint8_t>& key);
bool IsValidKey(const std::vector<uint8_t>& key);
bool IsPrivateKey(const std::vector<uint8_t>& key);
bool IsPublicKey(const std::vector<uint8_t>& key);

class SymmetricKey : public Nan::ObjectWrap
{
public:
    static void Init(v8::Local<v8::Object> exports);

private:
    static void New(const Nan::FunctionCallbackInfo<v8::Value>& args);

    static v8::Local<v8::Object> CopyIntoBuffer(const std::vector<uint8_t>& buffer);
    static v8::Local<v8::Object> CopyIntoBuffer(v8::Local<v8::Value> buffer);

    static Nan::Persistent<v8::Function> constructor;
};

} // namespace jsthemis

#endif /* JSTHEMIS_KEY_PAIR_HPP_ */
