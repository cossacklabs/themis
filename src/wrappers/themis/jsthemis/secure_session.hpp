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

#include <nan.h>
#include <vector>
#include <themis/themis.h>

namespace jsthemis{

  class SecureSession : public Nan::ObjectWrap {
  public:
    static void Init(v8::Handle<v8::Object> exports);

  private:
    explicit SecureSession(const std::vector<uint8_t>& id, const std::vector<uint8_t>& private_key, v8::Local<v8::Function> get_pub_by_id_callback);
    ~SecureSession();

    static void New(const Nan::FunctionCallbackInfo<v8::Value>& args);
    static void connectRequest(const Nan::FunctionCallbackInfo<v8::Value>& args);
    static void wrap(const Nan::FunctionCallbackInfo<v8::Value>& args);
    static void unwrap(const Nan::FunctionCallbackInfo<v8::Value>& args);
    static void isEstablished(const Nan::FunctionCallbackInfo<v8::Value>& args);

    static Nan::Persistent<v8::Function> constructor;

  private:
    secure_session_t* session_;
    secure_session_user_callbacks_t callback_;
  public:
    Nan::Callback id_to_pub_key_callback_;
  };

}
#endif /* JSTHEMIS_SECURE_SESSION_HPP_ */
