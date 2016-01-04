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

/* This file contains additional routines not present in the original ed25519 code. We keep them separate for easier updates of main ed25519 code */

#ifndef GE_UTILS_H
#define GE_UTILS_H

#include "ge.h"

#define ED25519_GE_LENGTH 32

/* Generates valid ed25519 private key. r should be 32 bytes */
extern void generate_random_32(unsigned char *r);
extern void clip_random_32(unsigned char *r);

extern int ge_frombytes_vartime(ge_p3 *h, const unsigned char *s);
extern void ge_p2_to_p3(ge_p3 *r, const ge_p2 *p);
extern void ge_p3_sub(ge_p3 *r, const ge_p3 *p, const ge_p3 *q);
extern void ge_scalarmult_blinded(ge_p3 *r, const unsigned char *a, const ge_p3 *A);
extern int ge_cmp(const ge_p3 *a, const ge_p3 *b);

int crypto_verify_32(const unsigned char *x,const unsigned char *y);

#endif /* GE_UTILS_H */
