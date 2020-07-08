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

#include "secure_comparator.hpp"

#include <cstring>

#include <node_buffer.h>

#include <themis/themis.h>

#include "errors.hpp"

namespace jsthemis
{

Nan::Persistent<v8::Function> SecureComparator::constructor;

SecureComparator::SecureComparator(const std::vector<uint8_t>& secret)
    : comparator_(NULL)
{
    comparator_ = secure_comparator_create();
    if (!comparator_) {
        ThrowError("Secure Comparator constructor", THEMIS_FAIL);
        return;
    }
    themis_status_t status = secure_comparator_append_secret(comparator_, &secret[0], secret.size());
    if (THEMIS_SUCCESS != status) {
        ThrowError("Secure Comparator failed to append secret", status);
        return;
    }
}

SecureComparator::~SecureComparator()
{
    if (comparator_) {
        secure_comparator_destroy(comparator_);
        comparator_ = NULL;
    }
}

void SecureComparator::Init(v8::Local<v8::Object> exports)
{
    v8::Local<v8::String> className = Nan::New("SecureComparator").ToLocalChecked();
    // Prepare constructor template
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(className);
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    // Prototype
    Nan::SetPrototypeMethod(tpl, "beginCompare", beginCompare);
    Nan::SetPrototypeMethod(tpl, "proceedCompare", proceedCompare);
    Nan::SetPrototypeMethod(tpl, "isMatch", isMatch);
    Nan::SetPrototypeMethod(tpl, "isCompareComplete", isCompareComplete);
    // Export constructor
    v8::Local<v8::Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(exports, className, function);
}

void SecureComparator::New(const Nan::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.IsConstructCall()) {
        if (args.Length() < 1) {
            ThrowParameterError("Secure Comparator constructor",
                                "not enough arguments, expected secret to compare");
            return;
        }
        if (!args[0]->IsUint8Array()) {
            ThrowTypeError("SecureComparator",
                           "secret is not a byte buffer, use ByteBuffer or Uint8Array");
            return;
        }
        if (node::Buffer::Length(args[0]) == 0) {
            ThrowParameterError("Secure Comparator constructor", "secret is empty");
            return;
        }
        std::vector<uint8_t> secret((uint8_t*)(node::Buffer::Data(args[0])),
                                    (uint8_t*)(node::Buffer::Data(args[0])
                                               + node::Buffer::Length(args[0])));
        SecureComparator* obj = new SecureComparator(secret);
        obj->Wrap(args.This());
        args.GetReturnValue().Set(args.This());
    } else {
        const int argc = 3;
        v8::Local<v8::Value> argv[argc] = {args[0], args[1], args[2]};
        v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
        args.GetReturnValue().Set(Nan::NewInstance(cons, argc, argv).ToLocalChecked());
    }
}

void SecureComparator::beginCompare(const Nan::FunctionCallbackInfo<v8::Value>& args)
{
    themis_status_t status = THEMIS_FAIL;
    SecureComparator* obj = Nan::ObjectWrap::Unwrap<SecureComparator>(args.This());
    size_t length = 0;
    status = secure_comparator_begin_compare(obj->comparator_, NULL, &length);
    if (THEMIS_BUFFER_TOO_SMALL != status) {
        ThrowSecureComparatorError("Secure Comparator failed to begin comparison", status);
        args.GetReturnValue().SetUndefined();
        return;
    }
    uint8_t* data = (uint8_t*)(malloc(length));
    status = secure_comparator_begin_compare(obj->comparator_, data, &length);
    if (THEMIS_SCOMPARE_SEND_OUTPUT_TO_PEER != status) {
        ThrowSecureComparatorError("Secure Comparator failed to begin comparison", status);
        free(data);
        args.GetReturnValue().SetUndefined();
        return;
    }
    args.GetReturnValue().Set(Nan::NewBuffer((char*)(data), length).ToLocalChecked());
}

void SecureComparator::proceedCompare(const Nan::FunctionCallbackInfo<v8::Value>& args)
{
    themis_status_t status = THEMIS_FAIL;
    SecureComparator* obj = Nan::ObjectWrap::Unwrap<SecureComparator>(args.This());
    if (args.Length() < 1) {
        ThrowParameterError("Secure Comparator failed to proceed comparison",
                            "not enough arguments, expected message");
        return;
    }
    if (!args[0]->IsUint8Array()) {
        ThrowTypeError("SecureComparator", "message is not a byte buffer, use ByteBuffer or Uint8Array");
        return;
    }
    if (node::Buffer::Length(args[0]) == 0) {
        ThrowParameterError("Secure Comparator failed to proceed comparison", "message is empty");
        return;
    }
    size_t length = 0;
    status = secure_comparator_proceed_compare(obj->comparator_,
                                               (const uint8_t*)(node::Buffer::Data(args[0])),
                                               node::Buffer::Length(args[0]),
                                               NULL,
                                               &length);
    if (THEMIS_BUFFER_TOO_SMALL != status) {
        ThrowSecureComparatorError("Secure Comparator failed to proceed comparison", status);
        args.GetReturnValue().SetUndefined();
        return;
    }
    uint8_t* data = (uint8_t*)(malloc(length));
    status = secure_comparator_proceed_compare(obj->comparator_,
                                               (const uint8_t*)(node::Buffer::Data(args[0])),
                                               node::Buffer::Length(args[0]),
                                               data,
                                               &length);
    if (THEMIS_SCOMPARE_SEND_OUTPUT_TO_PEER != status) {
        if (THEMIS_SUCCESS != status) {
            ThrowSecureComparatorError("Secure Comparator failed to proceed comparison", status);
        }
        free(data);
        args.GetReturnValue().SetUndefined();
        return;
    }
    args.GetReturnValue().Set(Nan::NewBuffer((char*)(data), length).ToLocalChecked());
}

void SecureComparator::isMatch(const Nan::FunctionCallbackInfo<v8::Value>& args)
{
    SecureComparator* obj = Nan::ObjectWrap::Unwrap<SecureComparator>(args.This());
    themis_status_t res = secure_comparator_get_result(obj->comparator_);
    if (THEMIS_SCOMPARE_NOT_READY == res) {
        ThrowSecureComparatorError("Secure Comparator not ready", THEMIS_SCOMPARE_NOT_READY);
        args.GetReturnValue().SetUndefined();
        return;
    }
    args.GetReturnValue().Set(Nan::New<v8::Boolean>(THEMIS_SCOMPARE_NO_MATCH != res));
}

void SecureComparator::isCompareComplete(const Nan::FunctionCallbackInfo<v8::Value>& args)
{
    SecureComparator* obj = Nan::ObjectWrap::Unwrap<SecureComparator>(args.This());
    args.GetReturnValue().Set(Nan::New<v8::Integer>(
        THEMIS_SCOMPARE_NOT_READY != secure_comparator_get_result(obj->comparator_)));
}

} // namespace jsthemis
