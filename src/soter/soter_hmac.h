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

#ifndef SOTER_HMAC_H
#define SOTER_HMAC_H

#include <soter/soter_hash.h>

typedef struct soter_hmac_ctx_type soter_hmac_ctx_t;

soter_hmac_ctx_t* soter_hmac_create(soter_hash_algo_t algo, const uint8_t* key, size_t key_length);
soter_status_t soter_hmac_destroy(soter_hmac_ctx_t *hmac_ctx);
soter_status_t soter_hmac_update(soter_hmac_ctx_t *hmac_ctx, const void *data, size_t length);
soter_status_t soter_hmac_final(soter_hmac_ctx_t *hmac_ctx, uint8_t* hmac_value, size_t* hmac_length);

#endif /* SOTER_HMAC_H */
