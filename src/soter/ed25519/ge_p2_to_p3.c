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

void ge_p2_to_p3(ge_p3 *r, const ge_p2 *p)
{
	if (p != (const ge_p2 *)r)
	{
		fe_copy(r->X, p->X);
		fe_copy(r->Y, p->Y);
		fe_copy(r->Z, p->Z);
	}

	fe_invert(r->T, p->Z);
	fe_mul(r->T, r->T, p->X);
	fe_mul(r->T, r->T, p->Y);
}
