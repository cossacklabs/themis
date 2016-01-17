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
    size_t length=0;
    const uint8_t* context=NULL;
    size_t context_length=0;
    if(args.Length()==2){
      context = (const uint8_t*)(node::Buffer::Data(args[1]));
      context_length = node::Buffer::Length(args[1]);
    }
    if(themis_secure_cell_encrypt_seal(&(obj->key_)[0], obj->key_.size(), context, context_length, (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), NULL, &length)!=THEMIS_BUFFER_TOO_SMALL){
      ThrowException(v8::Exception::Error(v8::String::New("secure cell seal encrypt (length determination) error")));
      return scope.Close(v8::Undefined());
    }
    uint8_t* data=new uint8_t[length];
    if(themis_secure_cell_encrypt_seal(&(obj->key_)[0], obj->key_.size(), context, context_length, (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), data, &length)!=THEMIS_SUCCESS){
      ThrowException(v8::Exception::Error(v8::String::New("secure cell seal encrypt error")));
      delete data;
      return scope.Close(v8::Undefined());
    }
    node::Buffer *buffer = node::Buffer::New((const char*)(data), length);
    delete data;
    return scope.Close(buffer->handle_);
  }

  v8::Handle<v8::Value> SecureCellSeal::decrypt(const v8::Arguments& args) {
    v8::HandleScope scope;
    SecureCellSeal* obj = node::ObjectWrap::Unwrap<SecureCellSeal>(args.This());
    size_t length=0;
    const uint8_t* context=NULL;
    size_t context_length=0;
    if(args.Length()==2){
      context = (const uint8_t*)(node::Buffer::Data(args[1]));
      context_length = node::Buffer::Length(args[1]);
    }
    if(themis_secure_cell_decrypt_seal(&(obj->key_)[0], obj->key_.size(), context, context_length, (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), NULL, &length)!=THEMIS_BUFFER_TOO_SMALL){
      ThrowException(v8::Exception::Error(v8::String::New("secure cell seal encrypt (length determination) error")));
      return scope.Close(v8::Undefined());
    }
    uint8_t* data=new uint8_t[length];
    if(themis_secure_cell_decrypt_seal(&(obj->key_)[0], obj->key_.size(), context, context_length, (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), data, &length)!=THEMIS_SUCCESS){
      ThrowException(v8::Exception::Error(v8::String::New("secure cell seal encrypt error")));
      delete data;
      return scope.Close(v8::Undefined());
    }
    node::Buffer *buffer = node::Buffer::New((const char*)(data), length);
    delete data;
    return scope.Close(buffer->handle_);
  }

  
} //end jsthemis
