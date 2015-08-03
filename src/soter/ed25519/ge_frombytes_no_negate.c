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

int ge_frombytes_vartime(ge_p3 *h, const unsigned char *s)
{
	int res = ge_frombytes_negate_vartime(h, s);

	if (0 == res)
	{
		/* Undo negate */
		fe_neg(h->X, h->X);
		fe_neg(h->T, h->T);
	}

	return res;
}
