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

#ifndef SOTER_H
#define SOTER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/**
 * @brief Function return codes
 *
 * TODO: Should define detailed error codes. What codes will we use for errors? It would be good to make them consistent with errno.h where applicable.
 */

typedef int soter_status_t;

#include <soter/soter_rand.h>
#include <soter/soter_hash.h>
#include <soter/soter_hmac.h>
#include <soter/soter_sym.h>
#include <soter/soter_asym_cipher.h>
#include <soter/soter_asym_ka.h>
#include <soter/soter_asym_sign.h>
#include <soter/soter_kdf.h>

const char* soter_version();
#endif /* SOTER_H */
