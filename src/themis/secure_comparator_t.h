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

#ifndef THEMIS_SECURE_COMPARATOR_T_H
#define THEMIS_SECURE_COMPARATOR_T_H

#ifdef SECURE_COMPARATOR_ENABLED
#include "secure_comparator.h"
#include <soter/ed25519/ge_utils.h>
#include <soter/ed25519/sc.h>
#include <soter/soter_t.h>

typedef themis_status_t (*secure_compare_handler)(secure_comparator_t *comp_ctx, const void *input, size_t input_length, void *output, size_t *output_length);

struct secure_comparator_type
{
	unsigned char secret[ED25519_GE_LENGTH];
	unsigned char rand2[ED25519_GE_LENGTH];
	unsigned char rand3[ED25519_GE_LENGTH];

	unsigned char rand[ED25519_GE_LENGTH];

	themis_status_t result;

	secure_compare_handler state_handler;

	ge_p3 g2;
	ge_p3 g3;

	ge_p3 P;
	ge_p3 Q;

	ge_p3 Pp;
	ge_p3 Qa_Qb;
	ge_p3 g3p;

	soter_hash_ctx_t hash_ctx;
};

themis_status_t secure_comparator_init(secure_comparator_t *comp_ctx);
themis_status_t secure_comparator_cleanup(secure_comparator_t *comp_ctx);

#endif
#endif /* THEMIS_SECURE_COMPARATOR_T_H */
