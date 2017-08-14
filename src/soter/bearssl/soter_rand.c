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

#include "soter/error.h"
#include "soter/soter.h"
#include <bearssl.h>

#include <time.h>
soter_status_t soter_rand(uint8_t* buffer, size_t length)
{
	if ((!buffer) || (!length))
	{
		return SOTER_INVALID_PARAMETER;
	}
	clock_t seed=clock();

	br_hmac_drbg_context rng;
	br_hmac_drbg_init(&rng, &br_sha256_vtable, (void*)(&seed), sizeof(clock_t));
	
	br_hmac_drbg_vtable.generate(&(rng.vtable), buffer, length);
	return SOTER_SUCCESS;
}
