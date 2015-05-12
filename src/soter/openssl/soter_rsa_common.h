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

#ifndef SOTER_RSA_COMMON_H
#define SOTER_RSA_COMMON_H

#include <soter/soter.h>
#include <soter/soter_rsa_key.h>
#include "soter_openssl.h"

soter_status_t soter_rsa_gen_key(EVP_PKEY_CTX *pkey_ctx);
soter_status_t soter_rsa_import_key(EVP_PKEY *pkey, const void* key, const size_t key_length);
soter_status_t soter_rsa_export_key(soter_sign_ctx_t* ctx, void* key, size_t* key_length, bool isprivate);

#endif

