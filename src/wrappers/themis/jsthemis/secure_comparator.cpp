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
#include "secure_comparator.hpp"

namespace jsthemis {

  Nan::Persistent<v8::Function> SecureComparator::constructor;

  SecureComparator::SecureComparator(const std::vector<uint8_t>& secret):
    comparator_(NULL){
    comparator_=secure_comparator_create();
    if(!comparator_){
      Nan::ThrowError("Secure Comparator creation failed");
      return;
    }
    if(THEMIS_SUCCESS!=secure_comparator_append_secret(comparator_, &secret[0], secret.size())){
      Nan::ThrowError("Secure Comparator creation failed (appending secret)");
      return;
    }
  }

  SecureComparator::~SecureComparator() {
    if(comparator_){
      secure_comparator_destroy(comparator_);
      comparator_=NULL;
    }
  }

  void SecureComparator::Init(v8::Handle<v8::Object> exports) {
    // Prepare constructor template
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("SecureComparator").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    // Prototype
    Nan::SetPrototypeMethod(tpl, "beginCompare", beginCompare);
    Nan::SetPrototypeMethod(tpl, "proceedCompare",proceedCompare);
    Nan::SetPrototypeMethod(tpl, "isMatch", isMatch);
    Nan::SetPrototypeMethod(tpl, "isCompareComplete", isCompareComplete);
    constructor.Reset(tpl->GetFunction());
    exports->Set(Nan::New("SecureComparator").ToLocalChecked(), tpl->GetFunction());
  }

  void SecureComparator::New(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    if (args.IsConstructCall()) {
      std::vector<uint8_t> secret((uint8_t*)(node::Buffer::Data(args[0])), (uint8_t*)(node::Buffer::Data(args[0])+node::Buffer::Length(args[0])));
      SecureComparator* obj = new SecureComparator(secret);
      obj->Wrap(args.This());
      args.GetReturnValue().Set(args.This());
    } else {
      const int argc = 3;
      v8::Local<v8::Value> argv[argc] = { args[0], args[1], args[2]};
      v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
      args.GetReturnValue().Set(cons->NewInstance(argc, argv));
    }
  }

  void SecureComparator::beginCompare(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    SecureComparator* obj = Nan::ObjectWrap::Unwrap<SecureComparator>(args.This());
    size_t length=0;
    if(THEMIS_BUFFER_TOO_SMALL != secure_comparator_begin_compare(obj->comparator_, NULL, &length)){
      Nan::ThrowError("Secure comparator failed generating initial data (length determination)");
      args.GetReturnValue().SetUndefined();
      return;
    }
    uint8_t* data=(uint8_t*)(malloc(length));
    if(THEMIS_SCOMPARE_SEND_OUTPUT_TO_PEER!=secure_comparator_begin_compare(obj->comparator_, data, &length)){
      Nan::ThrowError("Secure Comparator failed generating initial data");
      free(data);
      args.GetReturnValue().SetUndefined();
      return;
    }
    args.GetReturnValue().Set(Nan::NewBuffer((char*)(data), length).ToLocalChecked());
  }

  void SecureComparator::proceedCompare(const Nan::FunctionCallbackInfo<v8::Value>& args){
    SecureComparator* obj = Nan::ObjectWrap::Unwrap<SecureComparator>(args.This());
    size_t length=0;
    if(THEMIS_BUFFER_TOO_SMALL!=secure_comparator_proceed_compare(obj->comparator_, (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), NULL, &length)){
      Nan::ThrowError("Secure Comparator failed proceed comparation data (length determination)");
      args.GetReturnValue().SetUndefined();
      return;
    }
    uint8_t* data=(uint8_t*)(malloc(length));
    themis_status_t res = secure_comparator_proceed_compare(obj->comparator_, (const uint8_t*)(node::Buffer::Data(args[0])), node::Buffer::Length(args[0]), data, &length);
    if(THEMIS_SCOMPARE_SEND_OUTPUT_TO_PEER!=res){
      if(THEMIS_SUCCESS != res){
        Nan::ThrowError("Secure Comparator failed proceed comparation data");
      }
      free(data);
      args.GetReturnValue().SetUndefined();
      return;
    }
    args.GetReturnValue().Set(Nan::NewBuffer((char*)(data), length).ToLocalChecked());
  }

  void SecureComparator::isMatch(const Nan::FunctionCallbackInfo<v8::Value>& args){
    SecureComparator* obj = Nan::ObjectWrap::Unwrap<SecureComparator>(args.This());
    themis_status_t res = secure_comparator_get_result(obj->comparator_);
    if(THEMIS_SCOMPARE_NOT_READY == res){
        Nan::ThrowError("Secure Compare not ready");
	args.GetReturnValue().SetUndefined();
	return;
    }
    args.GetReturnValue().Set(Nan::New<v8::Boolean>((THEMIS_SCOMPARE_NO_MATCH==res)?false:true));
  }

  void SecureComparator::isCompareComplete(const Nan::FunctionCallbackInfo<v8::Value>& args){
    SecureComparator* obj = Nan::ObjectWrap::Unwrap<SecureComparator>(args.This());
    args.GetReturnValue().Set(Nan::New<v8::Integer>((THEMIS_SCOMPARE_NOT_READY==secure_comparator_get_result(obj->comparator_))?false:true));
  }
  
} //end jsthemis
