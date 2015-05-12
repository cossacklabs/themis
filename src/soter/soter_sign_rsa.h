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

#ifndef SOTER_SIGN_RSA_H
#define SOTER_SIGN_RSA_H

#include "soter/soter.h"

soter_status_t soter_sign_init_rsa_pss_pkcs8(soter_sign_ctx_t* ctx, const void* private_key, const size_t private_key_length, const void* public_key, const size_t public_key_length);
soter_status_t soter_sign_update_rsa_pss_pkcs8(soter_sign_ctx_t* ctx, const void* data, const size_t data_length);
soter_status_t soter_sign_final_rsa_pss_pkcs8(soter_sign_ctx_t* ctx, void* signature, size_t* signature_length);
soter_status_t soter_sign_export_key_rsa_pss_pkcs8(soter_sign_ctx_t* ctx, void* key, size_t* key_length, bool isprivate);

soter_status_t soter_verify_init_rsa_pss_pkcs8(soter_sign_ctx_t* ctx, const void* private_key, const size_t private_key_length, const void* public_key, const size_t public_key_length);
soter_status_t soter_verify_update_rsa_pss_pkcs8(soter_sign_ctx_t* ctx, const void* data, const size_t data_length);
soter_status_t soter_verify_final_rsa_pss_pkcs8(soter_sign_ctx_t* ctx, const void* signature, const size_t signature_length);

#endif /*SOTER_SIGN_RSA_H*/
