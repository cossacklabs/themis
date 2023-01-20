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

#ifndef SOTER_BORINGSSL_ECDSA_COMMON_H
#define SOTER_BORINGSSL_ECDSA_COMMON_H

#include "soter/boringssl/soter_engine.h"
#include "soter/soter_ec_key.h"
#include "soter/soter_error.h"

soter_status_t soter_ec_gen_key(EVP_PKEY_CTX* pkey_ctx);
soter_status_t soter_ec_import_key(EVP_PKEY* pkey, const void* key, size_t key_length);
soter_status_t soter_ec_export_private_key(const EVP_PKEY* pkey, void* key, size_t* key_length);
soter_status_t soter_ec_export_public_key(const EVP_PKEY* pkey, bool compressed, void* key, size_t* key_length);

#endif /* SOTER_BORINGSSL_ECDSA_COMMON_H */
