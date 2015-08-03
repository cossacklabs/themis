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

#include "secure_comparator_t.h"
#include <string.h>

static themis_status_t secure_comparator_alice_step1(secure_comparator_t *comp_ctx, void *output, size_t *output_length);
static themis_status_t secure_comparator_bob_step2(secure_comparator_t *comp_ctx, const void *input, size_t input_length, void *output, size_t *output_length);
static themis_status_t secure_comparator_alice_step3(secure_comparator_t *comp_ctx, const void *input, size_t input_length, void *output, size_t *output_length);
static themis_status_t secure_comparator_bob_step4(secure_comparator_t *comp_ctx, const void *input, size_t input_length, void *output, size_t *output_length);
static themis_status_t secure_comparator_alice_step5(secure_comparator_t *comp_ctx, const void *input, size_t input_length, void *output, size_t *output_length);

themis_status_t secure_comparator_init(secure_comparator_t *comp_ctx)
{
	soter_status_t soter_status;

	if (!comp_ctx)
	{
		return THEMIS_INVALID_PARAMETER;
	}

	memset(comp_ctx, 0, sizeof(secure_comparator_t));

	soter_status = soter_hash_init(&(comp_ctx->hash_ctx), SOTER_HASH_SHA256);
	if (SOTER_SUCCESS != soter_status)
	{
		return (themis_status_t)soter_status;
	}

	return THEMIS_SUCCESS;
}

themis_status_t secure_comparator_cleanup(secure_comparator_t *comp_ctx)
{
	if (!comp_ctx)
	{
		return THEMIS_INVALID_PARAMETER;
	}

	soter_hash_init(&(comp_ctx->hash_ctx), SOTER_HASH_SHA256);
	memset(comp_ctx, 0, sizeof(secure_comparator_t));

	return THEMIS_SUCCESS;
}

secure_comparator_t* secure_comparator_create(void)
{
	themis_status_t res;
	secure_comparator_t* ctx = malloc(sizeof(secure_comparator_t));
	if (!ctx)
	{
		return NULL;
	}

	res = secure_comparator_init(ctx);
	if (THEMIS_SUCCESS == res)
	{
		return ctx;
	}
	else
	{
		free(ctx);
		return NULL;
	}
}

themis_status_t secure_comparator_destroy(secure_comparator_t *comp_ctx)
{
	themis_status_t res = secure_comparator_cleanup(comp_ctx);
	if (THEMIS_SUCCESS == res)
	{
		free(comp_ctx);
	}

	return res;
}

themis_status_t secure_comparator_append_secret(secure_comparator_t *comp_ctx, const void *secret_data, size_t secret_data_length)
{
	soter_status_t soter_status;

	if ((!comp_ctx) || (!secret_data) || (0 == secret_data_length))
	{
		return THEMIS_INVALID_PARAMETER;
	}

	soter_status = soter_hash_update(&(comp_ctx->hash_ctx), secret_data, secret_data_length);
	return (themis_status_t)soter_status;
}

static themis_status_t secure_comparator_alice_step1(secure_comparator_t *comp_ctx, void *output, size_t *output_length)
{
	soter_status_t soter_status;
	size_t secret_length = sizeof(comp_ctx->secret);

	ge_p3 g2a;
	ge_p3 g3a;

	/* Output will contain 2 group elements */
	if ((!output) || (*output_length < (2 * ED25519_GE_LENGTH)))
	{
		*output_length = 2 * ED25519_GE_LENGTH;
		return THEMIS_BUFFER_TOO_SMALL;
	}

	*output_length = 2 * ED25519_GE_LENGTH;

	soter_status = soter_hash_final(&(comp_ctx->hash_ctx), comp_ctx->secret, &secret_length);
	if (SOTER_SUCCESS != soter_status)
	{
		return (themis_status_t)soter_status;
	}

	if (sizeof(comp_ctx->secret) != secret_length)
	{
		return THEMIS_FAIL;
	}

	clip_random_32(comp_ctx->secret);

	generate_random_32(comp_ctx->rand2);
	generate_random_32(comp_ctx->rand3);

	ge_scalarmult_base(&g2a, comp_ctx->rand2);
	ge_scalarmult_base(&g3a, comp_ctx->rand3);

	ge_p3_tobytes((unsigned char *)output, &g2a);
	ge_p3_tobytes(((unsigned char *)output) + ED25519_GE_LENGTH, &g3a);

	comp_ctx->state_handler = secure_comparator_alice_step3;

	return THEMIS_SCOMPARE_SEND_OUTPUT_TO_PEER;
}

static themis_status_t secure_comparator_bob_step2(secure_comparator_t *comp_ctx, const void *input, size_t input_length, void *output, size_t *output_length)
{
	ge_p3 g2a;
	ge_p3 g3a;

	ge_p3 g2b;
	ge_p3 g3b;

	soter_status_t soter_status;
	size_t secret_length = sizeof(comp_ctx->secret);

	if (input_length < (2 * ED25519_GE_LENGTH))
	{
		return THEMIS_INVALID_PARAMETER;
	}

	if (ge_frombytes_vartime(&g2a, (const unsigned char *)input))
	{
		return THEMIS_INVALID_PARAMETER;
	}
	if (ge_frombytes_vartime(&g3a, ((const unsigned char *)input) + ED25519_GE_LENGTH))
	{
		return THEMIS_INVALID_PARAMETER;
	}

	/* Output will contain 3 group elements */
	if ((!output) || (*output_length < (3 * ED25519_GE_LENGTH)))
	{
		*output_length = 3 * ED25519_GE_LENGTH;
		return THEMIS_BUFFER_TOO_SMALL;
	}

	*output_length = 3 * ED25519_GE_LENGTH;

	soter_status = soter_hash_final(&(comp_ctx->hash_ctx), comp_ctx->secret, &secret_length);
	if (SOTER_SUCCESS != soter_status)
	{
		return (themis_status_t)soter_status;
	}

	if (sizeof(comp_ctx->secret) != secret_length)
	{
		return THEMIS_FAIL;
	}

	clip_random_32(comp_ctx->secret);

	generate_random_32(comp_ctx->rand2);
	generate_random_32(comp_ctx->rand3);

	ge_scalarmult_base(&g2b, comp_ctx->rand2);
	ge_scalarmult_base(&g3b, comp_ctx->rand3);

	ge_scalarmult_blinded(&(comp_ctx->g2), comp_ctx->rand2, &g2a);
	ge_scalarmult_blinded(&(comp_ctx->g3), comp_ctx->rand3, &g3a);

	generate_random_32(comp_ctx->rand);

	ge_scalarmult_blinded(&(comp_ctx->P), comp_ctx->rand, &(comp_ctx->g3));
	ge_double_scalarmult_vartime((ge_p2 *)&(comp_ctx->Q), comp_ctx->secret, &(comp_ctx->g2), comp_ctx->rand);
	ge_p2_to_p3(&(comp_ctx->Q), (const ge_p2 *)&(comp_ctx->Q));

	ge_p3_tobytes((unsigned char *)output, &g2b);
	ge_p3_tobytes(((unsigned char *)output) + ED25519_GE_LENGTH, &g3b);
	ge_p3_tobytes(((unsigned char *)output) + ED25519_GE_LENGTH + ED25519_GE_LENGTH, &(comp_ctx->Q));

	comp_ctx->state_handler = secure_comparator_bob_step4;

	return THEMIS_SCOMPARE_SEND_OUTPUT_TO_PEER;
}

static themis_status_t secure_comparator_alice_step3(secure_comparator_t *comp_ctx, const void *input, size_t input_length, void *output, size_t *output_length)
{
	ge_p3 g2b;
	ge_p3 g3b;
	ge_p3 Qb;

	ge_p3 R;

	if (input_length < (3 * ED25519_GE_LENGTH))
	{
		return THEMIS_INVALID_PARAMETER;
	}

	if (ge_frombytes_vartime(&g2b, (const unsigned char *)input))
	{
		return THEMIS_INVALID_PARAMETER;
	}
	if (ge_frombytes_vartime(&g3b, ((const unsigned char *)input) + ED25519_GE_LENGTH))
	{
		return THEMIS_INVALID_PARAMETER;
	}
	if (ge_frombytes_vartime(&Qb, ((const unsigned char *)input) + ED25519_GE_LENGTH + ED25519_GE_LENGTH))
	{
		return THEMIS_INVALID_PARAMETER;
	}

	/* Output will contain 3 group elements */
	if ((!output) || (*output_length < (3 * ED25519_GE_LENGTH)))
	{
		*output_length = 3 * ED25519_GE_LENGTH;
		return THEMIS_BUFFER_TOO_SMALL;
	}

	*output_length = 3 * ED25519_GE_LENGTH;

	ge_scalarmult_blinded(&(comp_ctx->g2), comp_ctx->rand2, &g2b);
	ge_scalarmult_blinded(&(comp_ctx->g3), comp_ctx->rand3, &g3b);

	generate_random_32(comp_ctx->rand);

	ge_scalarmult_blinded(&(comp_ctx->P), comp_ctx->rand, &(comp_ctx->g3));
	ge_double_scalarmult_vartime((ge_p2 *)&(comp_ctx->Q), comp_ctx->secret, &(comp_ctx->g2), comp_ctx->rand);
	ge_p2_to_p3(&(comp_ctx->Q), (const ge_p2 *)&(comp_ctx->Q));

	ge_p3_sub(&R, &(comp_ctx->Q), &Qb);
	ge_scalarmult_blinded(&R, comp_ctx->rand3, &R);

	/* send to bob */
	ge_p3_tobytes((unsigned char *)output, &(comp_ctx->P));
	ge_p3_tobytes(((unsigned char *)output) + ED25519_GE_LENGTH, &(comp_ctx->Q));
	ge_p3_tobytes(((unsigned char *)output) + ED25519_GE_LENGTH + ED25519_GE_LENGTH, &R);

	comp_ctx->state_handler = secure_comparator_alice_step5;

	return THEMIS_SCOMPARE_SEND_OUTPUT_TO_PEER;
}

static themis_status_t secure_comparator_bob_step4(secure_comparator_t *comp_ctx, const void *input, size_t input_length, void *output, size_t *output_length)
{
	ge_p3 Pa;
	ge_p3 Qa;
	ge_p3 Ra;

	ge_p3 R;
	ge_p3 Rab;
	ge_p3 Pa_Pb;

	if (input_length < (3 * ED25519_GE_LENGTH))
	{
		return THEMIS_INVALID_PARAMETER;
	}

	if (ge_frombytes_vartime(&Pa, (const unsigned char *)input))
	{
		return THEMIS_INVALID_PARAMETER;
	}
	if (ge_frombytes_vartime(&Qa, ((const unsigned char *)input) + ED25519_GE_LENGTH))
	{
		return THEMIS_INVALID_PARAMETER;
	}
	if (ge_frombytes_vartime(&Ra, ((const unsigned char *)input) + ED25519_GE_LENGTH + ED25519_GE_LENGTH))
	{
		return THEMIS_INVALID_PARAMETER;
	}

	/* Output will contain 2 group elements */
	if ((!output) || (*output_length < (2 * ED25519_GE_LENGTH)))
	{
		*output_length = 2 * ED25519_GE_LENGTH;
		return THEMIS_BUFFER_TOO_SMALL;
	}

	*output_length = 2 * ED25519_GE_LENGTH;

	ge_p3_sub(&R, &Qa, &(comp_ctx->Q));
	ge_scalarmult_blinded(&R, comp_ctx->rand3, &R);

	ge_scalarmult_blinded(&Rab, comp_ctx->rand3, &Ra);
	ge_p3_sub(&Pa_Pb, &Pa, &(comp_ctx->P));

	comp_ctx->result = ge_cmp(&Rab, &Pa_Pb) ? THEMIS_SCOMPARE_NO_MATCH : THEMIS_SCOMPARE_MATCH;

	ge_p3_tobytes((unsigned char *)output, &(comp_ctx->P));
	ge_p3_tobytes(((unsigned char *)output) + ED25519_GE_LENGTH, &R);

	memset(comp_ctx->secret, 0, sizeof(comp_ctx->secret));
	comp_ctx->state_handler = NULL;

	return THEMIS_SCOMPARE_SEND_OUTPUT_TO_PEER;
}

static themis_status_t secure_comparator_alice_step5(secure_comparator_t *comp_ctx, const void *input, size_t input_length, void *output, size_t *output_length)
{
	ge_p3 Pb;
	ge_p3 Rb;

	ge_p3 Rab;
	ge_p3 Pa_Pb;

	if (input_length < (2 * ED25519_GE_LENGTH))
	{
		return THEMIS_INVALID_PARAMETER;
	}

	if (ge_frombytes_vartime(&Pb, (const unsigned char *)input))
	{
		return THEMIS_INVALID_PARAMETER;
	}
	if (ge_frombytes_vartime(&Rb, ((const unsigned char *)input) + ED25519_GE_LENGTH))
	{
		return THEMIS_INVALID_PARAMETER;
	}

	/* No output */
	if (!output)
	{
		*output_length = 0;
		return THEMIS_BUFFER_TOO_SMALL;
	}

	*output_length = 0;

	ge_scalarmult_blinded(&Rab, comp_ctx->rand3, &Rb);
	ge_p3_sub(&Pa_Pb, &(comp_ctx->P), &Pb);

	comp_ctx->result = ge_cmp(&Rab, &Pa_Pb) ? THEMIS_SCOMPARE_NO_MATCH : THEMIS_SCOMPARE_MATCH;

	memset(comp_ctx->secret, 0, sizeof(comp_ctx->secret));
	comp_ctx->state_handler = NULL;

	return THEMIS_SUCCESS;
}

themis_status_t secure_comparator_begin_compare(secure_comparator_t *comp_ctx, void *compare_data, size_t *compare_data_length)
{
	if ((!comp_ctx) || (!compare_data_length))
	{
		return THEMIS_INVALID_PARAMETER;
	}

	if (comp_ctx->state_handler)
	{
		/* We are already comparing. Wrong call. */
		return THEMIS_INVALID_PARAMETER;
	}

	return secure_comparator_alice_step1(comp_ctx, compare_data, compare_data_length);
}

themis_status_t secure_comparator_proceed_compare(secure_comparator_t *comp_ctx, const void *peer_compare_data, size_t peer_compare_data_length, void *compare_data, size_t *compare_data_length)
{
	if ((!comp_ctx) || (!peer_compare_data) || (0 == peer_compare_data_length) || (!compare_data_length))
	{
		return THEMIS_INVALID_PARAMETER;
	}

	if (comp_ctx->state_handler)
	{
		/* We are already comparing. Pass the data to the handler */
		return comp_ctx->state_handler(comp_ctx, peer_compare_data, peer_compare_data_length, compare_data, compare_data_length);
	}

	/* This is initial proceed call. "Accept" the protocol. */
	return secure_comparator_bob_step2(comp_ctx, peer_compare_data, peer_compare_data_length, compare_data, compare_data_length);
}

themis_status_t secure_comparator_get_result(const secure_comparator_t *comp_ctx)
{
	if (!comp_ctx)
	{
		return THEMIS_INVALID_PARAMETER;
	}

	return comp_ctx->result;
}
