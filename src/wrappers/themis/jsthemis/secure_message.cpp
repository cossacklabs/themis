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
#include "secure_message.hpp"

namespace jsthemis {

  v8::Persistent<v8::Function> SecureMessage::constructor;

  SecureMessage::SecureMessage(const std::vector<uint8_t>& private_key, const std::vector<uint8_t>& peer_public_key) :
    private_key_(private_key),
    peer_public_key_(peer_public_key){}

  SecureMessage::~SecureMessage() {}

  void SecureMessage::Init(v8::Handle<v8::Object> exports) {
    // Prepare constructor template
    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(SecureMessage::New);
    tpl->SetClassName(v8::String::NewSymbol("SecureMessage"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    // Prototype
    tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("encrypt"), v8::FunctionTemplate::New(SecureMessage::encrypt)->GetFunction());
    tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("decrypt"), v8::FunctionTemplate::New(SecureMessage::decrypt)->GetFunction());
    tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("sign"), v8::FunctionTemplate::New(SecureMessage::sign)->GetFunction());
    tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("verify"), v8::FunctionTemplate::New(SecureMessage::verify)->GetFunction());
    constructor = v8::Persistent<v8::Function>::New(tpl->GetFunction());
    exports->Set(v8::String::NewSymbol("SecureMessage"), constructor);
  }

  v8::Handle<v8::Value> SecureMessage::New(const v8::Arguments& args) {
    v8::HandleScope scope;

    if (args.IsConstructCall()) {
      std::vector<uint8_t> private_key((uint8_t*)(node::Buffer::Data(args[0])), (uint8_t*)(node::Buffer::Data(args[0])+node::Buffer::Length(args[0])));
      std::vector<uint8_t> public_key((uint8_t*)(node::Buffer::Data(args[1])), (uint8_t*)(node::Buffer::Data(args[1])+node::Buffer::Length(args[1])));
      SecureMessage* obj = new SecureMessage(private_key, public_key);
      obj->Wrap(args.This());
      return args.This();
    } else {
      const int argc = 2;
      v8::Local<v8::Value> argv[argc] = { args[0], args[1] };
      return scope.Close(constructor->NewInstance(argc, argv));
    }
  }

  v8::Handle<v8::Value> SecureMessage::encrypt(const v8::Arguments& args) {
    v8::HandleScope scope;
    SecureMessage* obj = node::ObjectWrap::Unwrap<SecureMessage>(args.This());
    size_t encrypted_length=0;
    if(themis_secure_message_wrap(&(obj->private_key_)[0], obj->private_key_.size(), &(obj->peer_public_key_)[0], obj->peer_public_key_.size(), (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), NULL, &encrypted_length)!=THEMIS_BUFFER_TOO_SMALL){
      ThrowException(v8::Exception::Error(v8::String::New("Secure Message failed encrypting")));
      return scope.Close(v8::Undefined());
    }
    uint8_t* encrypted_data=new uint8_t[encrypted_length];
    if(themis_secure_message_wrap(&(obj->private_key_)[0], obj->private_key_.size(), &(obj->peer_public_key_)[0], obj->peer_public_key_.size(), (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), encrypted_data, &encrypted_length)!=THEMIS_SUCCESS){
      ThrowException(v8::Exception::Error(v8::String::New("Secure Message failed encrypting")));
      delete[] encrypted_data;
      return scope.Close(v8::Undefined());
    }
    node::Buffer *encrypted_buffer = node::Buffer::New((const char*)(encrypted_data), encrypted_length);
    delete[] encrypted_data;
    return scope.Close(encrypted_buffer->handle_);
  }

  v8::Handle<v8::Value> SecureMessage::decrypt(const v8::Arguments& args){
    v8::HandleScope scope;
    SecureMessage* obj = node::ObjectWrap::Unwrap<SecureMessage>(args.This());
    size_t decrypted_length=0;
    if(themis_secure_message_unwrap(&(obj->private_key_)[0], obj->private_key_.size(), &(obj->peer_public_key_)[0], obj->peer_public_key_.size(), (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), NULL, &decrypted_length)!=THEMIS_BUFFER_TOO_SMALL){
      ThrowException(v8::Exception::Error(v8::String::New("Secure Message failed decrypting")));
      return scope.Close(v8::Undefined());
    }
    uint8_t* decrypted_data=new uint8_t[decrypted_length];
    if(themis_secure_message_unwrap(&(obj->private_key_)[0], obj->private_key_.size(), &(obj->peer_public_key_)[0], obj->peer_public_key_.size(), (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), decrypted_data, &decrypted_length)!=THEMIS_SUCCESS){
      ThrowException(v8::Exception::Error(v8::String::New("Secure Message failed decrypting")));
      delete[] decrypted_data;
      return scope.Close(v8::Undefined());
    }
    node::Buffer *decrypted_buffer = node::Buffer::New((const char*)(decrypted_data), decrypted_length);
    delete[] decrypted_data;
    return scope.Close(decrypted_buffer->handle_);
  }
  
  v8::Handle<v8::Value> SecureMessage::sign(const v8::Arguments& args){
    v8::HandleScope scope;
    SecureMessage* obj = node::ObjectWrap::Unwrap<SecureMessage>(args.This());
    size_t encrypted_length=0;
    if(themis_secure_message_wrap(&(obj->private_key_)[0], obj->private_key_.size(), NULL, 0, (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), NULL, &encrypted_length)!=THEMIS_BUFFER_TOO_SMALL){
      ThrowException(v8::Exception::Error(v8::String::New("Secure Message failed singing")));
      return scope.Close(v8::Undefined());
    }
    uint8_t* encrypted_data=new uint8_t[encrypted_length];
    if(themis_secure_message_wrap(&(obj->private_key_)[0], obj->private_key_.size(), NULL, 0, (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), encrypted_data, &encrypted_length)!=THEMIS_SUCCESS){
      ThrowException(v8::Exception::Error(v8::String::New("Secure Message failed singing")));
      delete[] encrypted_data;
      return scope.Close(v8::Undefined());
    }
    node::Buffer *encrypted_buffer = node::Buffer::New((const char*)(encrypted_data), encrypted_length);
    delete[] encrypted_data;
    return scope.Close(encrypted_buffer->handle_);
  }

  v8::Handle<v8::Value> SecureMessage::verify(const v8::Arguments& args){
      v8::HandleScope scope;
    SecureMessage* obj = node::ObjectWrap::Unwrap<SecureMessage>(args.This());
    size_t decrypted_length=0;
    if(themis_secure_message_unwrap(NULL, 0, &(obj->peer_public_key_)[0], obj->peer_public_key_.size(), (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), NULL, &decrypted_length)!=THEMIS_BUFFER_TOO_SMALL){
      ThrowException(v8::Exception::Error(v8::String::New("Secure Message failed verifying")));
      return scope.Close(v8::Undefined());
    }
    uint8_t* decrypted_data=new uint8_t[decrypted_length];
    if(themis_secure_message_unwrap(NULL, 0, &(obj->peer_public_key_)[0], obj->peer_public_key_.size(), (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), decrypted_data, &decrypted_length)!=THEMIS_SUCCESS){
      ThrowException(v8::Exception::Error(v8::String::New("Secure Message failed verifying")));
      delete[] decrypted_data;
      return scope.Close(v8::Undefined());
    }
    node::Buffer *decrypted_buffer = node::Buffer::New((const char*)(decrypted_data), decrypted_length);
    delete[] decrypted_data;
    return scope.Close(decrypted_buffer->handle_);
}
  
} //end jsthemis
