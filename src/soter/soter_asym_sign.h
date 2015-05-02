/*
* Copyright (C) 2015 CossackLabs
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

#ifndef SOTER_ASYM_SIGN_H
#define SOTER_ASYM_SIGN_H

#include <soter/soter.h>

enum soter_sign_alg_type
{
  SOTER_SIGN_undefined,
  SOTER_SIGN_rsa_pss_pkcs8,
  SOTER_SIGN_ecdsa_none_pkcs8
};

typedef enum soter_sign_alg_type soter_sign_alg_t;

typedef struct soter_sign_ctx_type soter_sign_ctx_t;

soter_sign_ctx_t* soter_sign_create(soter_sign_alg_t alg, const void* private_key, const size_t private_key_length, const void* public_key, const size_t public_key_length);
soter_status_t soter_sign_update(soter_sign_ctx_t* ctx, const void* data, const size_t data_length);
soter_status_t soter_sign_final(soter_sign_ctx_t* ctx, void* signature, size_t* signature_length);
soter_status_t soter_sign_export_key(soter_sign_ctx_t* ctx, void* key, size_t* key_length, bool isprivate);
soter_status_t soter_sign_destroy(soter_sign_ctx_t* ctx);

soter_sign_alg_t soter_sign_get_alg_id(soter_sign_ctx_t* ctx);

typedef struct soter_sign_ctx_type soter_verify_ctx_t;

soter_verify_ctx_t* soter_verify_create(soter_sign_alg_t alg, const void* private_key, const size_t private_key_length, const void* public_key, const size_t public_key_length);
soter_status_t soter_verify_update(soter_verify_ctx_t* ctx, const void* data, const size_t data_length);
soter_status_t soter_verify_final(soter_verify_ctx_t* ctx, const void* signature, const size_t signature_length);
soter_status_t soter_verify_destroy(soter_verify_ctx_t* ctx);

soter_sign_alg_t soter_verify_get_alg_id(soter_verify_ctx_t* ctx);

#endif /* SOTER_ASYM_SIGN_H */
