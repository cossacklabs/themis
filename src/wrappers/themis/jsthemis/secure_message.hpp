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

#ifndef JSTHEMIS_SECURE_MESSAGE_HPP_
#define JSTHEMIS_SECURE_MESSAGE_HPP_

#include <node.h>
#include <vector>

namespace jsthemis{

  class SecureMessage : public node::ObjectWrap {
  public:
    static void Init(v8::Handle<v8::Object> exports);

  private:
    explicit SecureMessage(const std::vector<uint8_t>& private_key, const std::vector<uint8_t>& peer_public_key);
    ~SecureMessage();

    static v8::Handle<v8::Value> New(const v8::Arguments& args);
    static v8::Handle<v8::Value> encrypt(const v8::Arguments& args);
    static v8::Handle<v8::Value> decrypt(const v8::Arguments& args);
    static v8::Handle<v8::Value> sign(const v8::Arguments& args);
    static v8::Handle<v8::Value> verify(const v8::Arguments& args);

    static v8::Persistent<v8::Function> constructor;

    std::vector<uint8_t> private_key_;
    std::vector<uint8_t> peer_public_key_;
  };

}
#endif /* JSTHEMIS_SECURE_MESSAGE_HPP_ */
