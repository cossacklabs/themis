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
#include "secure_cell_token_protect.hpp"

namespace jsthemis {

  v8::Persistent<v8::Function> SecureCellTokenProtect::constructor;

  SecureCellTokenProtect::SecureCellTokenProtect(const std::vector<uint8_t>& key) :
    key_(key){}

  SecureCellTokenProtect::~SecureCellTokenProtect() {}

  void SecureCellTokenProtect::Init(v8::Handle<v8::Object> exports) {
    // Prepare constructor template
    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(SecureCellTokenProtect::New);
    tpl->SetClassName(v8::String::NewSymbol("SecureCellTokenProtect"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    // Prototype
    tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("encrypt"), v8::FunctionTemplate::New(SecureCellTokenProtect::encrypt)->GetFunction());
    tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("decrypt"), v8::FunctionTemplate::New(SecureCellTokenProtect::decrypt)->GetFunction());
    constructor = v8::Persistent<v8::Function>::New(tpl->GetFunction());
    exports->Set(v8::String::NewSymbol("SecureCellTokenProtect"), constructor);
  }

  v8::Handle<v8::Value> SecureCellTokenProtect::New(const v8::Arguments& args) {
    v8::HandleScope scope;

    if (args.IsConstructCall()) {
      std::vector<uint8_t> key((uint8_t*)(node::Buffer::Data(args[0])), (uint8_t*)(node::Buffer::Data(args[0])+node::Buffer::Length(args[0])));
      SecureCellTokenProtect* obj = new SecureCellTokenProtect(key);
      obj->Wrap(args.This());
      return args.This();
    } else {
      const int argc = 1;
      v8::Local<v8::Value> argv[argc] = { args[0]};
      return scope.Close(constructor->NewInstance(argc, argv));
    }
  }

  v8::Handle<v8::Value> SecureCellTokenProtect::encrypt(const v8::Arguments& args) {
    v8::HandleScope scope;
    SecureCellTokenProtect* obj = node::ObjectWrap::Unwrap<SecureCellTokenProtect>(args.This());
    size_t length=0;
    size_t token_length=0;
    const uint8_t* context=NULL;
    size_t context_length=0;
    if(args.Length()==2){
      context = (const uint8_t*)(node::Buffer::Data(args[1]));
      context_length = node::Buffer::Length(args[1]);
    }
    if(themis_secure_cell_encrypt_token_protect(&(obj->key_)[0], obj->key_.size(), context, context_length, (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), NULL, &token_length, NULL, &length)!=THEMIS_BUFFER_TOO_SMALL){
      ThrowException(v8::Exception::Error(v8::String::New("secure cell token protect encrypt (length determination) error")));
      return scope.Close(v8::Undefined());
    }
    uint8_t* data=new uint8_t[length];
    uint8_t* token=new uint8_t[token_length];
    if(themis_secure_cell_encrypt_token_protect(&(obj->key_)[0], obj->key_.size(), context, context_length, (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), token, &token_length, data, &length)!=THEMIS_SUCCESS){
      ThrowException(v8::Exception::Error(v8::String::New("secure cell token protect encrypt error")));
      delete data;
      delete token;
      return scope.Close(v8::Undefined());
    }
    node::Buffer *buffer = node::Buffer::New((const char*)(data), length);
    node::Buffer *token_buffer = node::Buffer::New((const char*)(token), token_length);
    delete data;
    delete token;
    v8::Local<v8::Object> retobj = v8::Object::New();
    retobj->Set(v8::String::NewSymbol("data"), buffer->handle_);
    retobj->Set(v8::String::NewSymbol("token"), token_buffer->handle_);
    return scope.Close(retobj);
  }

  v8::Handle<v8::Value> SecureCellTokenProtect::decrypt(const v8::Arguments& args) {
    v8::HandleScope scope;
    SecureCellTokenProtect* obj = node::ObjectWrap::Unwrap<SecureCellTokenProtect>(args.This());
    size_t length=0;
    const uint8_t* context=NULL;
    size_t context_length=0;
    if(args.Length()==3){
      context = (const uint8_t*)(node::Buffer::Data(args[2]));
      context_length = node::Buffer::Length(args[2]);
    }
    if(themis_secure_cell_decrypt_token_protect(&(obj->key_)[0], obj->key_.size(), context, context_length, (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), (const uint8_t*)(node::Buffer::Data(args[1])), node::Buffer::Length(args[1]), NULL, &length)!=THEMIS_BUFFER_TOO_SMALL){
      ThrowException(v8::Exception::Error(v8::String::New("secure cell token protect decrypt (length determination) error")));
      return scope.Close(v8::Undefined());
    }
    uint8_t* data=new uint8_t[length];
    themis_status_t res=themis_secure_cell_decrypt_token_protect(&(obj->key_)[0], obj->key_.size(), context, context_length, (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), (const uint8_t*)(node::Buffer::Data(args[1])), node::Buffer::Length(args[1]), data, &length);
    if(res!=THEMIS_SUCCESS){
      ThrowException(v8::Exception::Error(v8::String::New("secure cell token protect decrypt error")));
      delete data;
      return scope.Close(v8::Undefined());
    }
    node::Buffer *buffer = node::Buffer::New((const char*)(data), length);
    delete data;
    return scope.Close(buffer->handle_);
  }

  
} //end jsthemis
