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

#ifndef SOTER_KEY_STORE_H
#define SOTER_KEY_STORE_H

#include <soter/soter_asym_ka.h>
#include "db.h"

#include <stdint.h>
#include <stdlib.h>

typedef struct soter_crypter_type soter_crypter_t;

soter_crypter_t* soter_crypter_create(soter_keys_db_t* db);
soter_status_t soter_crypter_destroy(soter_crypter_t** ks);
soter_status_t soter_crypter_main(soter_crypter_t* ks);


#endif //KEY_STORE_H
