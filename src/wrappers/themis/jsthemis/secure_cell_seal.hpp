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

#ifndef JSTHEMIS_SECURE_CELL_SEAL_HPP_
#define JSTHEMIS_SECURE_CELL_SEAL_HPP_

#include <vector>

#include <nan.h>

namespace jsthemis
{

class SecureCellSeal : public Nan::ObjectWrap
{
public:
    static void Init(v8::Local<v8::Object> exports);

private:
    friend class SecureCellSealWithPassphrase;

    explicit SecureCellSeal(std::vector<uint8_t>&& key);
    ~SecureCellSeal();

    static void New(const Nan::FunctionCallbackInfo<v8::Value>& args);
    static void WithKey(const Nan::FunctionCallbackInfo<v8::Value>& args);
    static void WithPassphrase(const Nan::FunctionCallbackInfo<v8::Value>& args);

    static void encrypt(const Nan::FunctionCallbackInfo<v8::Value>& args);
    static void decrypt(const Nan::FunctionCallbackInfo<v8::Value>& args);

    static Nan::Persistent<v8::Function> constructor;
    static Nan::Persistent<v8::FunctionTemplate> klass;

private:
    std::vector<uint8_t> key_;
};

class SecureCellSealWithPassphrase : public Nan::ObjectWrap
{
public:
    static void Init(v8::Local<v8::Object> exports);

private:
    friend class SecureCellSeal;

    explicit SecureCellSealWithPassphrase(std::vector<uint8_t>&& passphrase);
    ~SecureCellSealWithPassphrase();

    static void New(const Nan::FunctionCallbackInfo<v8::Value>& args);

    static void encrypt(const Nan::FunctionCallbackInfo<v8::Value>& args);
    static void decrypt(const Nan::FunctionCallbackInfo<v8::Value>& args);

    static Nan::Persistent<v8::Function> constructor;

private:
    std::vector<uint8_t> m_passphrase;
};

} // namespace jsthemis

#endif /* JSTHEMIS_SECURE_CELL_SEAL_HPP_ */
