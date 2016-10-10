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

#include <node_buffer.h>
#include <themis/themis.h>
#include <vector>
#include "secure_cell_seal.hpp"

namespace jsthemis {

  Nan::Persistent<v8::Function> SecureCellSeal::constructor;

  SecureCellSeal::SecureCellSeal(const std::vector<uint8_t>& key) :
    key_(key){}

  SecureCellSeal::~SecureCellSeal() {}

  void SecureCellSeal::Init(v8::Handle<v8::Object> exports) {
    // Prepare constructor template
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("SecureCellSeal").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    // Prototype
    Nan::SetPrototypeMethod(tpl, "encrypt", encrypt);
    Nan::SetPrototypeMethod(tpl, "decrypt", decrypt);
    constructor.Reset(tpl->GetFunction());
    exports->Set(Nan::New("SecureCellSeal").ToLocalChecked(), tpl->GetFunction());
  }

  void SecureCellSeal::New(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    if (args.IsConstructCall()) {
      std::vector<uint8_t> key((uint8_t*)(node::Buffer::Data(args[0])), (uint8_t*)(node::Buffer::Data(args[0])+node::Buffer::Length(args[0])));
      SecureCellSeal* obj = new SecureCellSeal(key);
      obj->Wrap(args.This());
      args.GetReturnValue().Set(args.This());
    } else {
      const int argc = 1;
      v8::Local<v8::Value> argv[argc] = { args[0]};
      v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
      args.GetReturnValue().Set(cons->NewInstance(argc, argv));
    }
  }

  void SecureCellSeal::encrypt(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    SecureCellSeal* obj = Nan::ObjectWrap::Unwrap<SecureCellSeal>(args.This());
    size_t length=0;
    const uint8_t* context=NULL;
    size_t context_length=0;
    if(args.Length()==2){
      context = (const uint8_t*)(node::Buffer::Data(args[1]));
      context_length = node::Buffer::Length(args[1]);
    }
    if(themis_secure_cell_encrypt_seal(&(obj->key_)[0], obj->key_.size(), context, context_length, (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), NULL, &length)!=THEMIS_BUFFER_TOO_SMALL){
      Nan::ThrowError("Secure Cell (seal) failed encrypting");
      args.GetReturnValue().SetUndefined();
      return;
    }
    uint8_t* data=(uint8_t*)(malloc(length));
    if(themis_secure_cell_encrypt_seal(&(obj->key_)[0], obj->key_.size(), context, context_length, (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), data, &length)!=THEMIS_SUCCESS){
      Nan::ThrowError("Secure Cell (seal) failed encrypting");
      free(data);
      args.GetReturnValue().SetUndefined();
      return;
    }
    args.GetReturnValue().Set(Nan::NewBuffer((char*)(data), length).ToLocalChecked());
  }

  void SecureCellSeal::decrypt(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    SecureCellSeal* obj = Nan::ObjectWrap::Unwrap<SecureCellSeal>(args.This());
    size_t length=0;
    const uint8_t* context=NULL;
    size_t context_length=0;
    if(args.Length()==2){
      context = (const uint8_t*)(node::Buffer::Data(args[1]));
      context_length = node::Buffer::Length(args[1]);
    }
    if(themis_secure_cell_decrypt_seal(&(obj->key_)[0], obj->key_.size(), context, context_length, (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), NULL, &length)!=THEMIS_BUFFER_TOO_SMALL){
      Nan::ThrowError("Secure Cell (seal) failed decrypting");
      args.GetReturnValue().SetUndefined();
      return;
    }
    uint8_t* data=(uint8_t*)malloc(length);
    if(themis_secure_cell_decrypt_seal(&(obj->key_)[0], obj->key_.size(), context, context_length, (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), data, &length)!=THEMIS_SUCCESS){
      Nan::ThrowError("Secure Cell (seal) failed decrypting");
      free(data);
      args.GetReturnValue().SetUndefined();
      return;
    }
    args.GetReturnValue().Set(Nan::NewBuffer((char*)(data), length).ToLocalChecked());
  }
} //end jsthemis
