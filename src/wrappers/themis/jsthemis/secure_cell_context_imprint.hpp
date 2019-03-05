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

#ifndef JSTHEMIS_SECURE_CELL_CONTEXT_IMPRINT_HPP_
#define JSTHEMIS_SECURE_CELL_CONTEXT_IMPRINT_HPP_

#include <vector>

#include <nan.h>

namespace jsthemis
{

class SecureCellContextImprint : public Nan::ObjectWrap
{
public:
    static void Init(v8::Handle<v8::Object> exports);

private:
    explicit SecureCellContextImprint(const std::vector<uint8_t>& key);
    ~SecureCellContextImprint();

    static void New(const Nan::FunctionCallbackInfo<v8::Value>& args);
    static void encrypt(const Nan::FunctionCallbackInfo<v8::Value>& args);
    static void decrypt(const Nan::FunctionCallbackInfo<v8::Value>& args);

    static Nan::Persistent<v8::Function> constructor;

    std::vector<uint8_t> key_;
};

} // namespace jsthemis
#endif /* JSTHEMIS_SECURE_CELL_CONTEXT_IMPRINT_HPP_ */
