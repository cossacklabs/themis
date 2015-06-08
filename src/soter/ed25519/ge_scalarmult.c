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

void ge_scalarmult_blinded(ge_p3 *r, const unsigned char *a, const ge_p3 *A)
{
	unsigned char rnd[32];
	ge_p3 sub;

	generate_random_32(rnd);

	ge_double_scalarmult_vartime((ge_p2 *)r, a, A, rnd);
	ge_p2_to_p3(r, (ge_p2 *)r);

	ge_scalarmult_base(&sub, rnd);
	ge_p3_sub(r, r, &sub);
}
