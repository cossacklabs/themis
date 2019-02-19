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
#include <cstring>
#include "errors.hpp"
#include "secure_session.hpp"

namespace jsthemis {

  Nan::Persistent<v8::Function> SecureSession::constructor;

  int get_public_key_for_id_callback(const void* id, size_t id_length, void* key_buffer, size_t key_buffer_length, void* user_data){
    if(!key_buffer || !key_buffer_length)
      return THEMIS_BUFFER_TOO_SMALL;
    v8::Local<v8::Value> argv[1] = { Nan::CopyBuffer((char*)id, id_length).ToLocalChecked() };
    v8::Local<v8::Value> a = Nan::Call(((SecureSession*)(user_data))->id_to_pub_key_callback_, 1, argv).ToLocalChecked();
    std::memcpy(key_buffer, (const uint8_t*)(node::Buffer::Data(a.As<v8::Object>())), node::Buffer::Length(a.As<v8::Object>()));
    return THEMIS_SUCCESS;
  }
  
  SecureSession::SecureSession(const std::vector<uint8_t>& id, const std::vector<uint8_t>& private_key, v8::Local<v8::Function> get_pub_by_id_callback):
    session_(NULL),
    id_to_pub_key_callback_(get_pub_by_id_callback){
    callback_.get_public_key_for_id=jsthemis::get_public_key_for_id_callback;
    callback_.send_data=NULL;
    callback_.receive_data=NULL;
    callback_.state_changed=NULL;
    callback_.user_data=this;
    session_=secure_session_create(&id[0], id.size(), &private_key[0], private_key.size(), &callback_);
  }

  SecureSession::~SecureSession() {
    if(!session_){
      secure_session_destroy(session_);
      session_=NULL;
    }
  }

  void SecureSession::Init(v8::Handle<v8::Object> exports) {
    // Prepare constructor template
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("SecureSession").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    // Prototype
    Nan::SetPrototypeMethod(tpl, "connectRequest", connectRequest);
    Nan::SetPrototypeMethod(tpl, "wrap",wrap);
    Nan::SetPrototypeMethod(tpl, "unwrap", unwrap);
    Nan::SetPrototypeMethod(tpl, "isEstablished", isEstablished);
    constructor.Reset(tpl->GetFunction());
    exports->Set(Nan::New("SecureSession").ToLocalChecked(), tpl->GetFunction());
  }

  void SecureSession::New(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    if (args.IsConstructCall()) {
      if(args.Length()<3){
        ThrowParameterError("Secure Session constructor", "not enough arguments, expected client ID, private key, public key callback");
        args.GetReturnValue().SetUndefined();
        return;
      }
      if(!args[0]->IsUint8Array()){
        ThrowParameterError("Secure Session constructor", "client ID is not a byte buffer, use ByteBuffer or Uint8Array");
        args.GetReturnValue().SetUndefined();
        return;
      }
      if(node::Buffer::Length(args[0])==0){
        ThrowParameterError("Secure Session constructor", "client ID is empty");
        args.GetReturnValue().SetUndefined();
        return;
      }
      if(!args[1]->IsUint8Array()){
        ThrowParameterError("Secure Session constructor", "private key is not a byte buffer, use ByteBuffer or Uint8Array");
        args.GetReturnValue().SetUndefined();
        return;
      }
      if(node::Buffer::Length(args[1])==0){
        ThrowParameterError("Secure Session constructor", "private key is empty");
        args.GetReturnValue().SetUndefined();
        return;
      }
      if(!args[2]->IsFunction()){
        ThrowParameterError("Secure Session constructor", "public key callback is not a function");
        args.GetReturnValue().SetUndefined();
        return;
      }
      std::vector<uint8_t> id((uint8_t*)(node::Buffer::Data(args[0])), (uint8_t*)(node::Buffer::Data(args[0])+node::Buffer::Length(args[0])));
      std::vector<uint8_t> private_key((uint8_t*)(node::Buffer::Data(args[1])), (uint8_t*)(node::Buffer::Data(args[1])+node::Buffer::Length(args[1])));
      SecureSession* obj = new SecureSession(id, private_key, v8::Local<v8::Function>::Cast(args[2]));
      obj->Wrap(args.This());
      args.GetReturnValue().Set(args.This());
    } else {
      const int argc = 3;
      v8::Local<v8::Value> argv[argc] = { args[0], args[1], args[2]};
      v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
      args.GetReturnValue().Set(Nan::NewInstance(cons, argc, argv).ToLocalChecked());
    }
  }

  void SecureSession::connectRequest(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    themis_status_t status = THEMIS_FAIL;
    SecureSession* obj = Nan::ObjectWrap::Unwrap<SecureSession>(args.This());
    size_t length=0;
    status=secure_session_generate_connect_request(obj->session_, NULL, &length);
    if(status!=THEMIS_BUFFER_TOO_SMALL){
      ThrowSecureSessionError("Secure Session failed to generate connect request", status);
      args.GetReturnValue().SetUndefined();
      return;
    }
    uint8_t* data=(uint8_t*)(malloc(length));
    status=secure_session_generate_connect_request(obj->session_, data, &length);
    if(status!=THEMIS_SUCCESS){
      ThrowSecureSessionError("Secure Session failed to generate connect request", status);
      free(data);
      args.GetReturnValue().SetUndefined();
      return;
    }
    args.GetReturnValue().Set(Nan::NewBuffer((char*)(data), length).ToLocalChecked());
  }

  void SecureSession::wrap(const Nan::FunctionCallbackInfo<v8::Value>& args){
    themis_status_t status = THEMIS_FAIL;
    SecureSession* obj = Nan::ObjectWrap::Unwrap<SecureSession>(args.This());
    if(args.Length()<1){
      ThrowParameterError("Secure Session failed to encrypt", "not enough arguments, expected message");
      args.GetReturnValue().SetUndefined();
      return;
    }
    if(!args[0]->IsUint8Array()){
      ThrowParameterError("Secure Session failed to encrypt", "message is not a byte buffer, use ByteBuffer or Uint8Array");
      args.GetReturnValue().SetUndefined();
      return;
    }
    if(node::Buffer::Length(args[0])==0){
      ThrowParameterError("Secure Session failed to encrypt", "message is empty");
      args.GetReturnValue().SetUndefined();
      return;
    }
    size_t length=0;
    status=secure_session_wrap(obj->session_, (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), NULL, &length);
    if(status!=THEMIS_BUFFER_TOO_SMALL){
      ThrowSecureSessionError("Secure Session failed to encrypt", status);
      args.GetReturnValue().SetUndefined();
      return;
    }
    uint8_t* data=(uint8_t*)(malloc(length));
    status=secure_session_wrap(obj->session_, (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), data, &length);
    if(status!=THEMIS_SUCCESS){
      ThrowSecureSessionError("Secure Session failed to encrypt", status);
      free(data);
      args.GetReturnValue().SetUndefined();
      return;
    }
    args.GetReturnValue().Set(Nan::NewBuffer((char*)(data), length).ToLocalChecked());
  }
  
  void SecureSession::unwrap(const Nan::FunctionCallbackInfo<v8::Value>& args){
    themis_status_t status = THEMIS_FAIL;
    SecureSession* obj = Nan::ObjectWrap::Unwrap<SecureSession>(args.This());
    if(args.Length()<1){
      ThrowParameterError("Secure Session failed to decrypt", "not enough arguments, expected message");
      args.GetReturnValue().SetUndefined();
      return;
    }
    if(!args[0]->IsUint8Array()){
      ThrowParameterError("Secure Session failed to decrypt", "message is not a byte buffer, use ByteBuffer or Uint8Array");
      args.GetReturnValue().SetUndefined();
      return;
    }
    if(node::Buffer::Length(args[0])==0){
      ThrowParameterError("Secure Session failed to decrypt", "message is empty");
      args.GetReturnValue().SetUndefined();
      return;
    }
    size_t length=0;
    status=secure_session_unwrap(obj->session_, (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), NULL, &length);
    if(status!=THEMIS_BUFFER_TOO_SMALL){
      if(status!=THEMIS_SUCCESS)
        ThrowSecureSessionError("Secure Session failed to decrypt", status);
      args.GetReturnValue().SetUndefined();
      return;
    }
    uint8_t* data=(uint8_t*)(malloc(length));
    status=secure_session_unwrap(obj->session_, (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), data, &length);
    if(status!=THEMIS_SUCCESS && status!=THEMIS_SSESSION_SEND_OUTPUT_TO_PEER){
      ThrowSecureSessionError("Secure Session failed to decrypt", status);
      free(data);
      args.GetReturnValue().SetUndefined();
      return;
    }
    args.GetReturnValue().Set(Nan::NewBuffer((char*)(data), length).ToLocalChecked());
  }
  
  void SecureSession::isEstablished(const Nan::FunctionCallbackInfo<v8::Value>& args){
    SecureSession* obj = Nan::ObjectWrap::Unwrap<SecureSession>(args.This());
    args.GetReturnValue().Set(Nan::New<v8::Boolean>(secure_session_is_established(obj->session_)));    
  }
  
} //end jsthemis
