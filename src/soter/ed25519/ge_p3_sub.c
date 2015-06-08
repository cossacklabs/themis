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

void ge_p3_sub(ge_p3 *r, const ge_p3 *p, const ge_p3 *q)
{
	ge_cached q_cached;
	ge_p1p1 r_p1p1;

	ge_p3_to_cached(&q_cached, q);
	ge_sub(&r_p1p1, p, &q_cached);

	ge_p1p1_to_p3(r, &r_p1p1);
}
