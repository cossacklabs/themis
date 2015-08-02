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

#ifndef SOTER_RSA_KEY_PAIR_GEN_H
#define SOTER_RSA_KEY_PAIR_GEN_H

#include <soter/soter_rsa_key.h>

typedef struct soter_rsa_key_pair_gen_type soter_rsa_key_pair_gen_t;

soter_rsa_key_pair_gen_t* soter_rsa_key_pair_gen_create(const unsigned key_length);
soter_status_t soter_rsa_key_pair_gen_init(soter_rsa_key_pair_gen_t* ctx, const unsigned key_length);
soter_status_t soter_rsa_key_pair_gen_destroy(soter_rsa_key_pair_gen_t* ctx);
soter_status_t soter_rsa_key_pair_gen_export_key(soter_rsa_key_pair_gen_t* ctx, void* key, size_t* key_length, bool isprivate);
#endif /* SOTER_RSA_KEY_PAIR_GEN_H */
