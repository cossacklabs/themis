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

#include <assert.h>

#include <soter/error.h>
#include <soter/soter.h>
#include "soter_engine.h"
#include <sodium.h>

soter_status_t soter_asym_cipher_encrypt(soter_asym_cipher_t* asym_cipher, const void* plain_data, size_t plain_data_length, void* cipher_data, size_t* cipher_data_length){
    return SOTER_NOT_SUPPORTED;
}

soter_status_t soter_asym_cipher_decrypt(soter_asym_cipher_t* asym_cipher, const void* cipher_data, size_t cipher_data_length, void* plain_data, size_t* plain_data_length){
    return SOTER_NOT_SUPPORTED;
}

soter_asym_cipher_t* soter_asym_cipher_create(const void* key, const size_t key_length){
    return NULL;
}

soter_status_t soter_asym_cipher_destroy(soter_asym_cipher_t* asym_cipher){
    return SOTER_NOT_SUPPORTED;
}
