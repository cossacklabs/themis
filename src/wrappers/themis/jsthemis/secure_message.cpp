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
#include "errors.hpp"
#include "secure_message.hpp"

namespace jsthemis {

  Nan::Persistent<v8::Function> SecureMessage::constructor;

  SecureMessage::SecureMessage(const std::vector<uint8_t>& private_key, const std::vector<uint8_t>& peer_public_key) :
    private_key_(private_key),
    peer_public_key_(peer_public_key){}

  SecureMessage::~SecureMessage() {}

  void SecureMessage::Init(v8::Handle<v8::Object> exports) {
    // Prepare constructor template
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(SecureMessage::New);
    tpl->SetClassName(Nan::New("SecureMessage").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    // Prototype
    Nan::SetPrototypeMethod(tpl, "encrypt", SecureMessage::encrypt);
    Nan::SetPrototypeMethod(tpl, "decrypt", SecureMessage::decrypt);
    Nan::SetPrototypeMethod(tpl, "sign", SecureMessage::sign);
    Nan::SetPrototypeMethod(tpl, "verify", SecureMessage::verify);
    constructor.Reset(tpl->GetFunction());
    exports->Set(Nan::New("SecureMessage").ToLocalChecked(), tpl->GetFunction());
  }

  void SecureMessage::New(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    if (args.IsConstructCall()) {
      if(args.Length()<2){
        ThrowError("Secure Message constructor", THEMIS_INVALID_PARAMETER, "missing private and public key");
        args.GetReturnValue().SetUndefined();
        return;
      }
      if(!args[0]->IsUint8Array()){
        ThrowError("Secure Message constructor", THEMIS_INVALID_PARAMETER, "private key is not byte buffer");
        args.GetReturnValue().SetUndefined();
        return;
      }
      if(!args[1]->IsUint8Array()){
        ThrowError("Secure Message constructor", THEMIS_INVALID_PARAMETER, "public key is not byte buffer");
        args.GetReturnValue().SetUndefined();
        return;
      }
      std::vector<uint8_t> private_key((uint8_t*)(node::Buffer::Data(args[0])), (uint8_t*)(node::Buffer::Data(args[0])+node::Buffer::Length(args[0])));
      std::vector<uint8_t> public_key((uint8_t*)(node::Buffer::Data(args[1])), (uint8_t*)(node::Buffer::Data(args[1])+node::Buffer::Length(args[1])));
      SecureMessage* obj = new SecureMessage(private_key, public_key);
      obj->Wrap(args.This());
      args.GetReturnValue().Set(args.This());
      return;
    } else {
      const int argc = 2;
      v8::Local<v8::Value> argv[argc] = { args[0], args[1] };
      v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
      args.GetReturnValue().Set(Nan::NewInstance(cons, argc, argv).ToLocalChecked());
    }
  }

  void SecureMessage::encrypt(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    themis_status_t status = THEMIS_FAIL;
    SecureMessage* obj = Nan::ObjectWrap::Unwrap<SecureMessage>(args.This());
    if(obj->private_key_.empty()){
      ThrowError("Secure Message failed to encrypt message", THEMIS_INVALID_PARAMETER, "private key is empty");
      args.GetReturnValue().SetUndefined();
      return;
    }
    if(obj->peer_public_key_.empty()){
      ThrowError("Secure Message failed to encrypt message", THEMIS_INVALID_PARAMETER, "public key is empty");
      args.GetReturnValue().SetUndefined();
      return;
    }
    if(args.Length()<1){
      ThrowError("Secure Message failed to encrypt message", THEMIS_INVALID_PARAMETER, "missing message");
      args.GetReturnValue().SetUndefined();
      return;
    }
    if(!args[0]->IsUint8Array()){
      ThrowError("Secure Message failed to encrypt message", THEMIS_INVALID_PARAMETER, "message is not byte buffer");
      args.GetReturnValue().SetUndefined();
      return;
    }
    if(node::Buffer::Length(args[0])==0){
      ThrowError("Secure Message failed to encrypt message", THEMIS_INVALID_PARAMETER, "message is empty");
      args.GetReturnValue().SetUndefined();
      return;
    }
    size_t encrypted_length=0;
    status=themis_secure_message_wrap(&(obj->private_key_)[0], obj->private_key_.size(), &(obj->peer_public_key_)[0], obj->peer_public_key_.size(), (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), NULL, &encrypted_length);
    if(status!=THEMIS_BUFFER_TOO_SMALL){
      ThrowError("Secure Message failed to encrypt message", status);
      args.GetReturnValue().SetUndefined();
      return;
    }
    uint8_t* encrypted_data=(uint8_t*)(malloc(encrypted_length));
    status=themis_secure_message_wrap(&(obj->private_key_)[0], obj->private_key_.size(), &(obj->peer_public_key_)[0], obj->peer_public_key_.size(), (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), encrypted_data, &encrypted_length);
    if(status!=THEMIS_SUCCESS){
      ThrowError("Secure Message failed to encrypt message", status);
      free(encrypted_data);
      args.GetReturnValue().SetUndefined();
      return;
    }
    args.GetReturnValue().Set(Nan::NewBuffer((char*)encrypted_data, encrypted_length).ToLocalChecked());
  }

  void SecureMessage::decrypt(const Nan::FunctionCallbackInfo<v8::Value>& args){
    themis_status_t status = THEMIS_FAIL;
    SecureMessage* obj = Nan::ObjectWrap::Unwrap<SecureMessage>(args.This());
    if(obj->private_key_.empty()){
      ThrowError("Secure Message failed to decrypt message", THEMIS_INVALID_PARAMETER, "private key is empty");
      args.GetReturnValue().SetUndefined();
      return;
    }
    if(obj->peer_public_key_.empty()){
      ThrowError("Secure Message failed to decrypt message", THEMIS_INVALID_PARAMETER, "public key is empty");
      args.GetReturnValue().SetUndefined();
      return;
    }
    if(args.Length()<1){
      ThrowError("Secure Message failed to decrypt message", THEMIS_INVALID_PARAMETER, "missing message");
      args.GetReturnValue().SetUndefined();
      return;
    }
    if(!args[0]->IsUint8Array()){
      ThrowError("Secure Message failed to decrypt message", THEMIS_INVALID_PARAMETER, "message is not byte buffer");
      args.GetReturnValue().SetUndefined();
      return;
    }
    if(node::Buffer::Length(args[0])==0){
      ThrowError("Secure Message failed to decrypt message", THEMIS_INVALID_PARAMETER, "message is empty");
      args.GetReturnValue().SetUndefined();
      return;
    }
    size_t decrypted_length=0;
    status=themis_secure_message_unwrap(&(obj->private_key_)[0], obj->private_key_.size(), &(obj->peer_public_key_)[0], obj->peer_public_key_.size(), (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), NULL, &decrypted_length);
    if(status!=THEMIS_BUFFER_TOO_SMALL){
      ThrowError("Secure Message failed to decrypt message", status);
      args.GetReturnValue().SetUndefined();
      return;
    }
    uint8_t* decrypted_data=(uint8_t*)(malloc(decrypted_length));
    status=themis_secure_message_unwrap(&(obj->private_key_)[0], obj->private_key_.size(), &(obj->peer_public_key_)[0], obj->peer_public_key_.size(), (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), decrypted_data, &decrypted_length);
    if(status!=THEMIS_SUCCESS){
      ThrowError("Secure Message failed to decrypt message", status);
      free(decrypted_data);
      args.GetReturnValue().SetUndefined();
      return;
    }
    args.GetReturnValue().Set(Nan::NewBuffer((char*)decrypted_data, decrypted_length).ToLocalChecked());
  }
  
  void SecureMessage::sign(const Nan::FunctionCallbackInfo<v8::Value>& args){
    themis_status_t status = THEMIS_FAIL;
    SecureMessage* obj = Nan::ObjectWrap::Unwrap<SecureMessage>(args.This());
    if(obj->peer_public_key_.empty()){
      ThrowError("Secure Message failed to sign message", THEMIS_INVALID_PARAMETER, "public key is empty");
      args.GetReturnValue().SetUndefined();
      return;
    }
    if(args.Length()<1){
      ThrowError("Secure Message failed to sign message", THEMIS_INVALID_PARAMETER, "missing message");
      args.GetReturnValue().SetUndefined();
      return;
    }
    if(!args[0]->IsUint8Array()){
      ThrowError("Secure Message failed to sign message", THEMIS_INVALID_PARAMETER, "message is not byte buffer");
      args.GetReturnValue().SetUndefined();
      return;
    }
    if(node::Buffer::Length(args[0])==0){
      ThrowError("Secure Message failed to sign message", THEMIS_INVALID_PARAMETER, "message is empty");
      args.GetReturnValue().SetUndefined();
      return;
    }
    size_t encrypted_length=0;
    status=themis_secure_message_wrap(&(obj->private_key_)[0], obj->private_key_.size(), NULL, 0, (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), NULL, &encrypted_length);
    if(status!=THEMIS_BUFFER_TOO_SMALL){
      ThrowError("Secure Message failed to sign message", status);
      args.GetReturnValue().SetUndefined();
      return;
    }
    uint8_t* encrypted_data=(uint8_t*)(malloc(encrypted_length));
    status=themis_secure_message_wrap(&(obj->private_key_)[0], obj->private_key_.size(), NULL, 0, (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), encrypted_data, &encrypted_length);
    if(status!=THEMIS_SUCCESS){
      ThrowError("Secure Message failed to sign message", status);
      free(encrypted_data);
      args.GetReturnValue().SetUndefined();
      return;
    }
    args.GetReturnValue().Set(Nan::NewBuffer((char*)encrypted_data, encrypted_length).ToLocalChecked());
  }

  void SecureMessage::verify(const Nan::FunctionCallbackInfo<v8::Value>& args){
    themis_status_t status = THEMIS_FAIL;
    SecureMessage* obj = Nan::ObjectWrap::Unwrap<SecureMessage>(args.This());
    if(obj->private_key_.empty()){
      ThrowError("Secure Message failed to verify signature", THEMIS_INVALID_PARAMETER, "private key is empty");
      args.GetReturnValue().SetUndefined();
      return;
    }
    if(args.Length()<1){
      ThrowError("Secure Message failed to verify signature", THEMIS_INVALID_PARAMETER, "missing message");
      args.GetReturnValue().SetUndefined();
      return;
    }
    if(!args[0]->IsUint8Array()){
      ThrowError("Secure Message failed to verify signature", THEMIS_INVALID_PARAMETER, "message is not byte buffer");
      args.GetReturnValue().SetUndefined();
      return;
    }
    if(node::Buffer::Length(args[0])==0){
      ThrowError("Secure Message failed to verify signature", THEMIS_INVALID_PARAMETER, "message is empty");
      args.GetReturnValue().SetUndefined();
      return;
    }
    size_t decrypted_length=0;
    status=themis_secure_message_unwrap(NULL, 0, &(obj->peer_public_key_)[0], obj->peer_public_key_.size(), (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), NULL, &decrypted_length);
    if(status!=THEMIS_BUFFER_TOO_SMALL){
      ThrowError("Secure Message failed to verify signature", status);
      args.GetReturnValue().SetUndefined();
      return;
    }
    uint8_t* decrypted_data=(uint8_t*)(malloc(decrypted_length));
    status=themis_secure_message_unwrap(NULL, 0, &(obj->peer_public_key_)[0], obj->peer_public_key_.size(), (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), decrypted_data, &decrypted_length);
    if(status!=THEMIS_SUCCESS){
      ThrowError("Secure Message failed to verify signature", status);
      free(decrypted_data);
      args.GetReturnValue().SetUndefined();
      return;
    }
    args.GetReturnValue().Set(Nan::NewBuffer((char*)decrypted_data, decrypted_length).ToLocalChecked());
  }
} //end jsthemis
