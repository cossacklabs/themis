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

#include <node.h>
#include <node_buffer.h>
#include <themis/themis.h>
#include <vector>
#include "secure_cell_context_imprint.hpp"

namespace jsthemis {

  v8::Persistent<v8::Function> SecureCellContextImprint::constructor;

  SecureCellContextImprint::SecureCellContextImprint(const std::vector<uint8_t>& key) :
    key_(key){}

  SecureCellContextImprint::~SecureCellContextImprint() {}

  void SecureCellContextImprint::Init(v8::Handle<v8::Object> exports) {
    // Prepare constructor template
    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(SecureCellContextImprint::New);
    tpl->SetClassName(v8::String::NewSymbol("SecureCellContextImprint"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    // Prototype
    tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("encrypt"), v8::FunctionTemplate::New(SecureCellContextImprint::encrypt)->GetFunction());
    tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("decrypt"), v8::FunctionTemplate::New(SecureCellContextImprint::decrypt)->GetFunction());
    constructor = v8::Persistent<v8::Function>::New(tpl->GetFunction());
    exports->Set(v8::String::NewSymbol("SecureCellContextImprint"), constructor);
  }

  v8::Handle<v8::Value> SecureCellContextImprint::New(const v8::Arguments& args) {
    v8::HandleScope scope;

    if (args.IsConstructCall()) {
      std::vector<uint8_t> key((uint8_t*)(node::Buffer::Data(args[0])), (uint8_t*)(node::Buffer::Data(args[0])+node::Buffer::Length(args[0])));
      SecureCellContextImprint* obj = new SecureCellContextImprint(key);
      obj->Wrap(args.This());
      return args.This();
    } else {
      const int argc = 1;
      v8::Local<v8::Value> argv[argc] = { args[0]};
      return scope.Close(constructor->NewInstance(argc, argv));
    }
  }

  v8::Handle<v8::Value> SecureCellContextImprint::encrypt(const v8::Arguments& args) {
    v8::HandleScope scope;
    SecureCellContextImprint* obj = node::ObjectWrap::Unwrap<SecureCellContextImprint>(args.This());
    size_t length=0;
    const uint8_t* context=(const uint8_t*)(node::Buffer::Data(args[1]));
    size_t context_length=node::Buffer::Length(args[1]);
    if(themis_secure_cell_encrypt_context_imprint(&(obj->key_)[0], obj->key_.size(), (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), context, context_length, NULL, &length)!=THEMIS_BUFFER_TOO_SMALL){
      ThrowException(v8::Exception::Error(v8::String::New("Secure Cell (Context Imprint) failed  encrypting")));
      return scope.Close(v8::Undefined());
    }
    uint8_t* data=new uint8_t[length];
    if(themis_secure_cell_encrypt_context_imprint(&(obj->key_)[0], obj->key_.size(), (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), context, context_length, data, &length)!=THEMIS_SUCCESS){
      ThrowException(v8::Exception::Error(v8::String::New("Secure Cell (Context Imprint) failed  encrypting")));
      delete[] data;
      return scope.Close(v8::Undefined());
    }
    node::Buffer *buffer = node::Buffer::New((const char*)(data), length);
    delete[] data;
    return scope.Close(buffer->handle_);
  }

  v8::Handle<v8::Value> SecureCellContextImprint::decrypt(const v8::Arguments& args) {
    v8::HandleScope scope;
    SecureCellContextImprint* obj = node::ObjectWrap::Unwrap<SecureCellContextImprint>(args.This());
    size_t length=0;
    const uint8_t* context=(const uint8_t*)(node::Buffer::Data(args[1]));
    size_t context_length=node::Buffer::Length(args[1]);
    if(themis_secure_cell_decrypt_context_imprint(&(obj->key_)[0], obj->key_.size(), (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), context, context_length, NULL, &length)!=THEMIS_BUFFER_TOO_SMALL){
      ThrowException(v8::Exception::Error(v8::String::New("Secure Cell (Context Imprint) failed  decrypting")));
      return scope.Close(v8::Undefined());
    }
    uint8_t* data=new uint8_t[length];
    if(themis_secure_cell_decrypt_context_imprint(&(obj->key_)[0], obj->key_.size(), (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), context, context_length, data, &length)!=THEMIS_SUCCESS){
      ThrowException(v8::Exception::Error(v8::String::New("Secure Cell (Context Imprint) failed  decrypting")));
      delete[] data;
      return scope.Close(v8::Undefined());
    }
    node::Buffer *buffer = node::Buffer::New((const char*)(data), length);
    delete[] data;
    return scope.Close(buffer->handle_);
  }

} //end jsthemis
