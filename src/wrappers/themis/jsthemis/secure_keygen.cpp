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
#include "common.hpp"
#include "secure_keygen.hpp"

namespace jsthemis {

  v8::Persistent<v8::Function> KeyPair::constructor;

  KeyPair::KeyPair(const std::vector<uint8_t>& private_key, const std::vector<uint8_t>& public_key) :
    private_key_(private_key),
    public_key_(public_key){}
  
  KeyPair::~KeyPair() {}

  void KeyPair::Init(v8::Handle<v8::Object> exports) {
    // Prepare constructor template
    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(New);
    tpl->SetClassName(v8::String::NewSymbol("KeyPair"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    // Prototype
    tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("private"), v8::FunctionTemplate::New(private_key)->GetFunction());
    tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("public"), v8::FunctionTemplate::New(public_key)->GetFunction());
    constructor = v8::Persistent<v8::Function>::New(tpl->GetFunction());
    exports->Set(v8::String::NewSymbol("KeyPair"), constructor);
  }

  v8::Handle<v8::Value> KeyPair::New(const v8::Arguments& args) {
    v8::HandleScope scope;

    if (args.IsConstructCall()) {
      if(args.Length()==2){
	std::vector<uint8_t> private_key((uint8_t*)(node::Buffer::Data(args[0])), (uint8_t*)(node::Buffer::Data(args[0])+node::Buffer::Length(args[0])));
	std::vector<uint8_t> public_key((uint8_t*)(node::Buffer::Data(args[1])), (uint8_t*)(node::Buffer::Data(args[1])+node::Buffer::Length(args[1])));
	KeyPair* obj = new KeyPair(private_key, public_key);
	obj->Wrap(args.This());
	return args.This();
      }else if(args.Length()==0){
	size_t private_key_length, public_key_length;
	if(themis_gen_ec_key_pair(NULL, &private_key_length, NULL, &public_key_length)!=THEMIS_BUFFER_TOO_SMALL){
	  ThrowException(v8::Exception::Error(v8::String::New("KeyPair generation (length determination) error")));
	  return scope.Close(v8::Undefined());
	}
	std::vector<uint8_t> prk(private_key_length);
	std::vector<uint8_t> puk(public_key_length);
	if(themis_gen_ec_key_pair(&prk[0], &private_key_length, &puk[0], &public_key_length)!=THEMIS_SUCCESS){
	  ThrowException(v8::Exception::Error(v8::String::New("KeyPair generation error")));
	  return scope.Close(v8::Undefined());
	}
	KeyPair* obj = new KeyPair(prk, puk);
	obj->Wrap(args.This());
	return args.This();
      } else {
	ThrowException(v8::Exception::Error(v8::String::New("KeyPair object initialisation error")));
	return scope.Close(v8::Undefined());
      }
    } else {
      const int argc = 2;
      v8::Local<v8::Value> argv[argc] = { args[0], args[1] };
      return scope.Close(constructor->NewInstance(argc, argv));
    }
  }

  v8::Handle<v8::Value> KeyPair::private_key(const v8::Arguments& args) {
    v8::HandleScope scope;
    KeyPair* obj = node::ObjectWrap::Unwrap<KeyPair>(args.This());
    node::Buffer *buffer = node::Buffer::New((const char*)(&(obj->private_key_)[0]), obj->private_key_.size());
    //    RETURN_BUFFER(encrypted_buffer, obj->private_key_.size());
    return scope.Close(buffer->handle_);
  }

  v8::Handle<v8::Value> KeyPair::public_key(const v8::Arguments& args){
    v8::HandleScope scope;
    KeyPair* obj = node::ObjectWrap::Unwrap<KeyPair>(args.This());
    node::Buffer *buffer = node::Buffer::New((const char*)(&(obj->public_key_)[0]), obj->public_key_.size());
    //    RETURN_BUFFER(encrypted_buffer, obj->public_key_.size());
    return scope.Close(buffer->handle_);
  }
  
} //end jsthemis
