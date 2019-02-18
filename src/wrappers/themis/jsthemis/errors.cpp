/*
 * Copyright (c) 2019 Cossack Labs Limited
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

#include "errors.hpp"

#include <string>

#include <nan.h>

namespace jsthemis {

  static const char* ErrorDescription(themis_status_t status) {
    switch (status) {
    case THEMIS_SUCCESS:
      return "success";
    case THEMIS_FAIL:
      return "failure";
    case THEMIS_INVALID_PARAMETER:
      return "invalid parameter";
    case THEMIS_NO_MEMORY:
      return "out of memory";
    case THEMIS_BUFFER_TOO_SMALL:
      return "buffer too small";
    case THEMIS_DATA_CORRUPT:
      return "corrupted data";
    case THEMIS_INVALID_SIGNATURE:
      return "invalid signature";
    case THEMIS_NOT_SUPPORTED:
      return "operation not supported";
    default:
      return "unknown error";
    }
  }

  static const char* ErrorDescriptionSecureSession(themis_status_t status) {
    switch (status) {
    case THEMIS_SSESSION_SEND_OUTPUT_TO_PEER:
      return "send key agreement data to peer";
    case THEMIS_SSESSION_KA_NOT_FINISHED:
      return "key agreement not finished";
    case THEMIS_SSESSION_TRANSPORT_ERROR:
      return "transport layer error";
    case THEMIS_SSESSION_GET_PUB_FOR_ID_CALLBACK_ERROR:
      return "failed to get public key for ID";
    default:
      return ErrorDescription(status);
    }
  }

  static const char* ErrorDescriptionSecureComparator(themis_status_t status) {
    switch (status) {
    case THEMIS_SCOMPARE_SEND_OUTPUT_TO_PEER:
      return "send comparison data to peer";
    case THEMIS_SCOMPARE_NOT_READY:
      return "comparator not ready";
    case THEMIS_SCOMPARE_MATCH:
      return "data matches";
    case THEMIS_SCOMPARE_NO_MATCH:
      return "data does not match";
    default:
      return ErrorDescription(status);
    }
  }

  void ThrowError(const char* domain, themis_status_t status) {
    std::string message;
    message += domain;
    message += ": ";
    message += ErrorDescription(status);
    Nan::ThrowError(message.c_str());
  }

  void ThrowError(const char* domain, themis_status_t status, const char* description) {
    std::string message;
    message += domain;
    message += ": ";
    message += ErrorDescription(status);
    message += ": ";
    message += description;
    Nan::ThrowError(message.c_str());
  }

  void ThrowSecureSessionError(const char* domain, themis_status_t status) {
    std::string message;
    message += domain;
    message += ": ";
    message += ErrorDescriptionSecureSession(status);
    Nan::ThrowError(message.c_str());
  }

  void ThrowSecureSessionError(const char* domain, themis_status_t status, const char* description) {
    std::string message;
    message += domain;
    message += ": ";
    message += ErrorDescriptionSecureSession(status);
    message += ": ";
    message += description;
    Nan::ThrowError(message.c_str());
  }

  void ThrowSecureComparatorError(const char* domain, themis_status_t status) {
    std::string message;
    message += domain;
    message += ": ";
    message += ErrorDescriptionSecureComparator(status);
    Nan::ThrowError(message.c_str());
  }

  void ThrowSecureComparatorError(const char* domain, themis_status_t status, const char* description) {
    std::string message;
    message += domain;
    message += ": ";
    message += ErrorDescriptionSecureComparator(status);
    message += ": ";
    message += description;
    Nan::ThrowError(message.c_str());
  }

} // namespace jsthemis
