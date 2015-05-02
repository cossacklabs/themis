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

#ifndef SOTER_ASYM_KA_H
#define SOTER_ASYM_KA_H

#include <soter/soter.h>

enum soter_asym_ka_alg_type
{
	SOTER_ASYM_KA_EC_P256
};

typedef enum soter_asym_ka_alg_type soter_asym_ka_alg_t;

typedef struct soter_asym_ka_type soter_asym_ka_t;

soter_asym_ka_t* soter_asym_ka_create(soter_asym_ka_alg_t alg);
soter_status_t soter_asym_ka_gen_key(soter_asym_ka_t* asym_ka_ctx);
soter_status_t soter_asym_ka_export_key(soter_asym_ka_t* asym_ka_ctx, void* key, size_t* key_length, bool isprivate);
soter_status_t soter_asym_ka_import_key(soter_asym_ka_t* asym_ka_ctx, const void* key, size_t key_length);
soter_status_t soter_asym_ka_derive(soter_asym_ka_t* asym_ka_ctx, const void* peer_key, size_t peer_key_length, void *shared_secret, size_t* shared_secret_length);
soter_status_t soter_asym_ka_destroy(soter_asym_ka_t* asym_ka_ctx);

#endif /* SOTER_ASYM_KA_H */
