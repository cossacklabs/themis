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


#include <soter/soter_mem_cmp.h>

// https://github.com/chmike/cst_time_memcmp#fastest-implementation-using-subscipt

int cst_time_memcmp(const void *m1, const void *m2, size_t n) {
    const unsigned char *pm1 = (unsigned char *)m1; 
    const unsigned char *pm2 = (unsigned char *)m2; 
    int res = 0, diff;
    if (n > 0) {
        do {
            --n;
            diff = pm1[n] - pm2[n];
            res = (res & -!diff) | diff;
        } while (n != 0);
    }
    return (res > 0) - (res < 0);
}