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

#include "ge_utils.h"
#include <string.h>
#include <stdint.h>

int ge_cmp(const ge_p3 *a, const ge_p3 *b)
{
	unsigned char a_comp[ED25519_GE_LENGTH];
	unsigned char b_comp[ED25519_GE_LENGTH];

	ge_p3_tobytes(a_comp, a);
	ge_p3_tobytes(b_comp, b);

	return crypto_verify_32(a_comp, b_comp);
}
