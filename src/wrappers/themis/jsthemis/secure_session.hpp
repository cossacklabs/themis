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

#ifndef JSTHEMIS_SECURE_SESSION_HPP_
#define JSTHEMIS_SECURE_SESSION_HPP_

#include <node.h>
#include <vector>
#include <themis/themis.h>

namespace jsthemis{

  class SecureSession : public node::ObjectWrap {
  public:
    static void Init(v8::Handle<v8::Object> exports);

  private:
    explicit SecureSession(const std::vector<uint8_t>& id, const std::vector<uint8_t>& private_key, v8::Local<v8::Function> get_pub_by_id_callback);
    ~SecureSession();

    static v8::Handle<v8::Value> New(const v8::Arguments& args);
    static v8::Handle<v8::Value> connectRequest(const v8::Arguments& args);
    static v8::Handle<v8::Value> wrap(const v8::Arguments& args);
    static v8::Handle<v8::Value> unwrap(const v8::Arguments& args);
    static v8::Handle<v8::Value> isEstablished(const v8::Arguments& args);

    static v8::Persistent<v8::Function> constructor;

  private:
    secure_session_t* session_;
    secure_session_user_callbacks_t callback_;
  public:
    v8::Persistent<v8::Function> id_to_pub_key_callback_;
  };

}
#endif /* JSTHEMIS_SECURE_SESSION_HPP_ */
