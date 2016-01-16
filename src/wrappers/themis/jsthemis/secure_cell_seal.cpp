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
#include "secure_cell_seal.hpp"

namespace jsthemis {

  v8::Persistent<v8::Function> SecureCellSeal::constructor;

  SecureCellSeal::SecureCellSeal(const std::vector<uint8_t>& key) :
    key_(key){}

  SecureCellSeal::~SecureCellSeal() {}

  void SecureCellSeal::Init(v8::Handle<v8::Object> exports) {
    // Prepare constructor template
    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(New);
    tpl->SetClassName(v8::String::NewSymbol("SecureCellSeal"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    // Prototype
    tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("encrypt"), v8::FunctionTemplate::New(encrypt)->GetFunction());
    tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("decrypt"), v8::FunctionTemplate::New(decrypt)->GetFunction());
    constructor = v8::Persistent<v8::Function>::New(tpl->GetFunction());
    exports->Set(v8::String::NewSymbol("SecureCellSeal"), constructor);
  }

  v8::Handle<v8::Value> SecureCellSeal::New(const v8::Arguments& args) {
    v8::HandleScope scope;

    if (args.IsConstructCall()) {
      std::vector<uint8_t> key((uint8_t*)(node::Buffer::Data(args[0])), (uint8_t*)(node::Buffer::Data(args[0])+node::Buffer::Length(args[0])));
      SecureCellSeal* obj = new SecureCellSeal(key);
      obj->Wrap(args.This());
      return args.This();
    } else {
      const int argc = 1;
      v8::Local<v8::Value> argv[argc] = { args[0]};
      return scope.Close(constructor->NewInstance(argc, argv));
    }
  }

  v8::Handle<v8::Value> SecureCellSeal::encrypt(const v8::Arguments& args) {
    v8::HandleScope scope;
    SecureCellSeal* obj = node::ObjectWrap::Unwrap<SecureCellSeal>(args.This());
    size_t encrypted_length=0;
    if(themis_secure_message_wrap(&(obj->private_key_)[0], obj->private_key_.size(), &(obj->peer_public_key_)[0], obj->peer_public_key_.size(), (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), NULL, &encrypted_length)!=THEMIS_BUFFER_TOO_SMALL){
      ThrowException(v8::Exception::Error(v8::String::New("secure message encrypt (length determination) error")));
      return scope.Close(v8::Undefined());
    }
    uint8_t* encrypted_data=new uint8_t[encrypted_length];
    if(themis_secure_message_wrap(&(obj->private_key_)[0], obj->private_key_.size(), &(obj->peer_public_key_)[0], obj->peer_public_key_.size(), (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), encrypted_data, &encrypted_length)!=THEMIS_SUCCESS){
      ThrowException(v8::Exception::Error(v8::String::New("secure message encrypt error")));
      delete encrypted_data;
      return scope.Close(v8::Undefined());
    }
    node::Buffer *encrypted_buffer = node::Buffer::New((const char*)(encrypted_data), encrypted_length);
    delete encrypted_data;
    return scope.Close(encrypted_buffer->handle_);
  }

  v8::Handle<v8::Value> SecureCellSeal::decrypt(const v8::Arguments& args){
    v8::HandleScope scope;
    SecureCellSeal* obj = node::ObjectWrap::Unwrap<SecureCellSeal>(args.This());
    size_t decrypted_length=0;
    if(themis_secure_message_unwrap(&(obj->private_key_)[0], obj->private_key_.size(), &(obj->peer_public_key_)[0], obj->peer_public_key_.size(), (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), NULL, &decrypted_length)!=THEMIS_BUFFER_TOO_SMALL){
      ThrowException(v8::Exception::Error(v8::String::New("secure message decrypt (length determination) error")));
      return scope.Close(v8::Undefined());
    }
    uint8_t* decrypted_data=new uint8_t[decrypted_length];
    if(themis_secure_message_unwrap(&(obj->private_key_)[0], obj->private_key_.size(), &(obj->peer_public_key_)[0], obj->peer_public_key_.size(), (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), decrypted_data, &decrypted_length)!=THEMIS_SUCCESS){
      ThrowException(v8::Exception::Error(v8::String::New("secure message decrypt error")));
      delete decrypted_data;
      return scope.Close(v8::Undefined());
    }
    node::Buffer *decrypted_buffer = node::Buffer::New((const char*)(decrypted_data), decrypted_length);
    delete decrypted_data;
    return scope.Close(decrypted_buffer->handle_);
  }
} //end jsthemis
