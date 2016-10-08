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
#include "secure_cell_token_protect.hpp"

namespace jsthemis {

  Nan::Persistent<v8::Function> SecureCellTokenProtect::constructor;

  SecureCellTokenProtect::SecureCellTokenProtect(const std::vector<uint8_t>& key) :
    key_(key){}

  SecureCellTokenProtect::~SecureCellTokenProtect() {}

  void SecureCellTokenProtect::Init(v8::Handle<v8::Object> exports) {
    // Prepare constructor template
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(SecureCellTokenProtect::New);
    tpl->SetClassName(Nan::New("SecureCellTokenProtect").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    // Prototype
    Nan::SetPrototypeMethod(tpl, "encrypt", encrypt);
    Nan::SetPrototypeMethod(tpl, "decrypt", decrypt);
    constructor.Reset(tpl->GetFunction());
    exports->Set(Nan::New("SecureCellTokenProtect").ToLocalChecked(), tpl->GetFunction());
  }

  void SecureCellTokenProtect::New(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    if (args.IsConstructCall()) {
      std::vector<uint8_t> key((uint8_t*)(node::Buffer::Data(args[0])), (uint8_t*)(node::Buffer::Data(args[0])+node::Buffer::Length(args[0])));
      SecureCellTokenProtect* obj = new SecureCellTokenProtect(key);
      obj->Wrap(args.This());
      args.GetReturnValue().Set(args.This());
    } else {
      const int argc = 1;
      v8::Local<v8::Value> argv[argc] = { args[0]};
      v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
      args.GetReturnValue().Set(cons->NewInstance(argc, argv));
    }
  }

  void SecureCellTokenProtect::encrypt(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    SecureCellTokenProtect* obj = Nan::ObjectWrap::Unwrap<SecureCellTokenProtect>(args.This());
    size_t length=0;
    size_t token_length=0;
    const uint8_t* context=NULL;
    size_t context_length=0;
    if(args.Length()==2){
      context = (const uint8_t*)(node::Buffer::Data(args[1]));
      context_length = node::Buffer::Length(args[1]);
    }
    if(themis_secure_cell_encrypt_token_protect(&(obj->key_)[0], obj->key_.size(), context, context_length, (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), NULL, &token_length, NULL, &length)!=THEMIS_BUFFER_TOO_SMALL){
      Nan::ThrowError("Secure Cell (Token Protect) failed encrypting");
      args.GetReturnValue().SetUndefined();
      return;
    }
    uint8_t* data=(uint8_t*)(malloc(length));
    uint8_t* token=(uint8_t*)(malloc(token_length));
    if(themis_secure_cell_encrypt_token_protect(&(obj->key_)[0], obj->key_.size(), context, context_length, (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), token, &token_length, data, &length)!=THEMIS_SUCCESS){
      Nan::ThrowError("Secure Cell (Token Protect) failed encrypting");
      free(data);
      free(token);
      args.GetReturnValue().SetUndefined();
      return;
    }
    v8::Local<v8::Object> retobj = Nan::New<v8::Object>();
    Nan::Set(retobj, Nan::New("data").ToLocalChecked(), Nan::NewBuffer((char*)(data), length).ToLocalChecked());
    Nan::Set(retobj, Nan::New("token").ToLocalChecked(), Nan::NewBuffer((char*)(token), token_length).ToLocalChecked());
    args.GetReturnValue().Set(retobj);
  }

  void SecureCellTokenProtect::decrypt(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    SecureCellTokenProtect* obj = Nan::ObjectWrap::Unwrap<SecureCellTokenProtect>(args.This());
    size_t length=0;
    const uint8_t* context=NULL;
    size_t context_length=0;
    if(args.Length()==3){
      context = (const uint8_t*)(node::Buffer::Data(args[2]));
      context_length = node::Buffer::Length(args[2]);
    }
    if(themis_secure_cell_decrypt_token_protect(&(obj->key_)[0], obj->key_.size(), context, context_length, (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), (const uint8_t*)(node::Buffer::Data(args[1])), node::Buffer::Length(args[1]), NULL, &length)!=THEMIS_BUFFER_TOO_SMALL){
      Nan::ThrowError("Secure Cell (Token Protect) failed decrypting");
      args.GetReturnValue().SetUndefined();
      return;
    }
    uint8_t* data=(uint8_t*)(malloc(length));
    themis_status_t res=themis_secure_cell_decrypt_token_protect(&(obj->key_)[0], obj->key_.size(), context, context_length, (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), (const uint8_t*)(node::Buffer::Data(args[1])), node::Buffer::Length(args[1]), data, &length);
    if(res!=THEMIS_SUCCESS){
      Nan::ThrowError("Secure Cell (Token Protect) failed decrypting");
      free(data);
      args.GetReturnValue().SetUndefined();
      return;
    }
    args.GetReturnValue().Set(Nan::NewBuffer((char*)(data), length).ToLocalChecked());
  }
} //end jsthemis
