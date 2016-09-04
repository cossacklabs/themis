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

#include "soter/error.h"
#include "soter/soter.h"
#include <openssl/rand.h>

soter_status_t soter_rand(uint8_t* buffer, size_t length)
{
	if ((!buffer) || (!length))
	{
		return SOTER_INVALID_PARAMETER;
	}

	if (RAND_bytes(buffer, (int)length))
	{
		return SOTER_SUCCESS;
	}
	else
	{
		/* For some reason OpenSSL generator failed */
		return SOTER_FAIL;
	}
}
