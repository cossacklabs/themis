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

#ifndef JSTHEMIS_COMMON_HPP_
#define JSTHEMIS_COMMON_HPP_

#include <vector>

#include <stdint.h>

#include <nan.h>

#define RETURN_BUFFER(buf, length)                                                           \
    v8::Local<v8::Object> globalObj = v8::Context::GetCurrent()->Global();                   \
    v8::Local<v8::Function> bufferConstructor = v8::Local<v8::Function>::Cast(               \
        globalObj->Get(v8::String::New("Buffer")));                                          \
    v8::Local<v8::Value> constructorArgs[3] = {(buf)->handle_,                               \
                                               v8::Integer::New(length),                     \
                                               v8::Integer::New(0)};                         \
    v8::Local<v8::Object> actualBuffer = bufferConstructor->NewInstance(3, constructorArgs); \
    return scope.Close(actualBuffer)

static inline void assign_buffer_to_vector(std::vector<uint8_t>& vector,
                                           const v8::Local<v8::Value>& buffer)
{
    const uint8_t* data = reinterpret_cast<const uint8_t*>(node::Buffer::Data(buffer));
    size_t length = node::Buffer::Length(buffer);
    vector.assign(data, data + length);
}

#endif /* JSTHEMIS_COMMON_HPP_ */
