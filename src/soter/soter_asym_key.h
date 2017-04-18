/*
 * Copyright (c) 2017 Cossack Labs Limited
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

#ifndef SOTER_SOTER_ASYM_KEY_H_
#define SOTER_SOTER_ASYM_KEY_H_

#include <soter/error.h>
#include <stdbool.h>

soter_status_t soter_key_pair_gen(int32_t alg_id, uint8_t* private_key, size_t* private_key_length, uint8_t* public_key, size_t* public_key_length);

bool soter_key_is_private(const uint8_t* key, const size_t key_length);
int32_t soter_key_get_alg_id(const uint8_t* key, const size_t key_length);
int32_t soter_key_get_length_id(const uint8_t* key, const size_t key_length);
#endif /* SOTER_SOTER_ASYM_KEY_H_ */
