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

#ifdef SECURE_COMPARATOR_ENABLED

#include "secure_comparator_t.h"
#include <string.h>

static themis_status_t secure_comparator_alice_step1(secure_comparator_t *comp_ctx, void *output, size_t *output_length);
static themis_status_t secure_comparator_bob_step2(secure_comparator_t *comp_ctx, const void *input, size_t input_length, void *output, size_t *output_length);
static themis_status_t secure_comparator_alice_step3(secure_comparator_t *comp_ctx, const void *input, size_t input_length, void *output, size_t *output_length);
static themis_status_t secure_comparator_bob_step4(secure_comparator_t *comp_ctx, const void *input, size_t input_length, void *output, size_t *output_length);
static themis_status_t secure_comparator_alice_step5(secure_comparator_t *comp_ctx, const void *input, size_t input_length, void *output, size_t *output_length);

static bool ge_is_zero(const ge_p3 *ge)
{
	int i;
	bool res = true;

	for (i = 0; i < (sizeof(fe) / sizeof(crypto_int32)); i++)
	{
		if (ge->X[i])
		{
			res = false;
		}

		if (ge->Y[i] != ge->Z[i])
		{
			res = false;
		}
	}

	return res;
}

/* TODO: Check for errors */
static void ed_sign(uint8_t pos, const uint8_t *scalar, uint8_t *signature)
{
	uint8_t r[ED25519_GE_LENGTH];
	ge_p3 R;
	uint8_t k[64];

	soter_hash_ctx_t hash_ctx;
	size_t hash_length = 64;

	generate_random_32(r);
	ge_scalarmult_base(&R, r);
	ge_p3_tobytes(k, &R);

	soter_hash_init(&hash_ctx, SOTER_HASH_SHA512);
	soter_hash_update(&hash_ctx, k, ED25519_GE_LENGTH);
	soter_hash_update(&hash_ctx, &pos, sizeof(pos));
	soter_hash_final(&hash_ctx, k, &hash_length);

	sc_reduce(k);

	memcpy(signature, k, ED25519_GE_LENGTH);
	sc_muladd(signature + ED25519_GE_LENGTH, k, scalar, r);
}

static bool ed_verify(uint8_t pos, const ge_p3 *point, const uint8_t *signature)
{
	ge_p3 A;
	ge_p2 R;
	uint8_t k[64];

	soter_hash_ctx_t hash_ctx;
	size_t hash_length = 64;

	if (signature[63] & 224)
	{
		return false;
	}

	memcpy(&A, point, sizeof(ge_p3));
	fe_neg(A.X, A.X);
	fe_neg(A.T, A.T);

	ge_double_scalarmult_vartime(&R, signature, &A, signature + ED25519_GE_LENGTH);
	ge_tobytes(k, &R);

	soter_hash_init(&hash_ctx, SOTER_HASH_SHA512);
	soter_hash_update(&hash_ctx, k, ED25519_GE_LENGTH);
	soter_hash_update(&hash_ctx, &pos, sizeof(pos));
	soter_hash_final(&hash_ctx, k, &hash_length);

	sc_reduce(k);

	return !memcmp(k, signature, ED25519_GE_LENGTH);
}

static void ed_dbl_base_sign(uint8_t pos, const uint8_t *scalar1, const uint8_t *scalar2, const ge_p3 *base1, const ge_p3 *base2, uint8_t *signature)
{
	uint8_t r1[ED25519_GE_LENGTH];
	uint8_t r2[ED25519_GE_LENGTH];
	ge_p3 R1;
	ge_p2 R2;
	uint8_t k[64];

	soter_hash_ctx_t hash_ctx;
	size_t hash_length = 64;

	generate_random_32(r1);
	generate_random_32(r2);
	ge_scalarmult_blinded(&R1, r1, base2);
	ge_double_scalarmult_vartime(&R2, r2, base1, r1);

	soter_hash_init(&hash_ctx, SOTER_HASH_SHA512);

	ge_p3_tobytes(k, &R1);
	soter_hash_update(&hash_ctx, k, ED25519_GE_LENGTH);

	ge_tobytes(k, &R2);
	soter_hash_update(&hash_ctx, k, ED25519_GE_LENGTH);

	soter_hash_update(&hash_ctx, &pos, sizeof(pos));
	soter_hash_final(&hash_ctx, k, &hash_length);

	sc_reduce(k);
	memcpy(signature, k, ED25519_GE_LENGTH);
	sc_muladd(signature + ED25519_GE_LENGTH, k, scalar1, r1);
	sc_muladd(signature + (2 * ED25519_GE_LENGTH), k, scalar2, r2);
}

static bool ed_dbl_base_verify(uint8_t pos, const ge_p3 *base1, const ge_p3 *base2, const ge_p3 *point1, const ge_p3 *point2, const uint8_t *signature)
{
	ge_p3 R1;
	ge_p3 R2;

	uint8_t k[64];

	soter_hash_ctx_t hash_ctx;
	size_t hash_length = 64;

	if ((signature[63] & 224) || (signature[63 + ED25519_GE_LENGTH] & 224))
	{
		return false;
	}

	ge_scalarmult_blinded(&R1, signature + ED25519_GE_LENGTH, base2);
	ge_scalarmult_blinded(&R2, signature, point1);
	ge_p3_sub(&R1, &R1, &R2);
	ge_p3_tobytes(k, &R1);

	soter_hash_init(&hash_ctx, SOTER_HASH_SHA512);
	soter_hash_update(&hash_ctx, k, ED25519_GE_LENGTH);

	ge_double_scalarmult_vartime((ge_p2 *)&R1, signature + (2 *ED25519_GE_LENGTH), base1, signature + ED25519_GE_LENGTH);
	ge_p2_to_p3(&R1, (const ge_p2 *)&R1);
	ge_scalarmult_blinded(&R2, signature, point2);
	ge_p3_sub(&R1, &R1, &R2);
	ge_p3_tobytes(k, &R1);

	soter_hash_update(&hash_ctx, k, ED25519_GE_LENGTH);
	soter_hash_update(&hash_ctx, &pos, sizeof(pos));
	soter_hash_final(&hash_ctx, k, &hash_length);

	sc_reduce(k);

	/* TODO: const time? */
	return !memcmp(k, signature, ED25519_GE_LENGTH);
}

static void ed_point_sign(uint8_t pos, const uint8_t *scalar, const ge_p3 *point, uint8_t *signature)
{
	uint8_t r[ED25519_GE_LENGTH];
	ge_p3 R;
	uint8_t k[64];

	soter_hash_ctx_t hash_ctx;
	size_t hash_length = 64;

	generate_random_32(r);
	ge_scalarmult_base(&R, r);
	ge_p3_tobytes(k, &R);

	soter_hash_init(&hash_ctx, SOTER_HASH_SHA512);
	soter_hash_update(&hash_ctx, k, ED25519_GE_LENGTH);

	ge_scalarmult_blinded(&R, r, point);
	ge_p3_tobytes(k, &R);
	soter_hash_update(&hash_ctx, k, ED25519_GE_LENGTH);
	soter_hash_update(&hash_ctx, &pos, sizeof(pos));
	soter_hash_final(&hash_ctx, k, &hash_length);

	sc_reduce(k);
	memcpy(signature, k, ED25519_GE_LENGTH);
	sc_muladd(signature + ED25519_GE_LENGTH, k, scalar, r);
}

static bool ed_point_verify(uint8_t pos, const ge_p3 *base2, const ge_p3 *point1, const ge_p3 *point2, const uint8_t *signature)
{
	ge_p3 p_neg;
	ge_p3 R1;
	ge_p3 R2;

	uint8_t k[64];

	soter_hash_ctx_t hash_ctx;
	size_t hash_length = 64;

	if (signature[63] & 224)
	{
		return false;
	}

	memcpy(&p_neg, base2, sizeof(ge_p3));
	fe_neg(p_neg.X, p_neg.X);
	fe_neg(p_neg.T, p_neg.T);

	ge_double_scalarmult_vartime((ge_p2 *)&R1, signature, &p_neg, signature + ED25519_GE_LENGTH);
	ge_tobytes(k, (const ge_p2 *)&R1);

	soter_hash_init(&hash_ctx, SOTER_HASH_SHA512);
	soter_hash_update(&hash_ctx, k, ED25519_GE_LENGTH);

	ge_scalarmult_blinded(&R1, signature + ED25519_GE_LENGTH, point1);
	ge_scalarmult_blinded(&R2, signature, point2);
	ge_p3_sub(&R1, &R1, &R2);
	ge_p3_tobytes(k, &R1);

	soter_hash_update(&hash_ctx, k, ED25519_GE_LENGTH);
	soter_hash_update(&hash_ctx, &pos, sizeof(pos));
	soter_hash_final(&hash_ctx, k, &hash_length);

	sc_reduce(k);

	/* TODO: const time? */
	return !memcmp(k, signature, ED25519_GE_LENGTH);
}

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

	/* Output will contain 2 group elements and 2 * 2 ZK-proofs */
	if ((!output) || (*output_length < (6 * ED25519_GE_LENGTH)))
	{
		*output_length = 6 * ED25519_GE_LENGTH;
		return THEMIS_BUFFER_TOO_SMALL;
	}

	*output_length = 6 * ED25519_GE_LENGTH;

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
	ed_sign(1, comp_ctx->rand2, ((unsigned char *)output) + ED25519_GE_LENGTH);

	ge_p3_tobytes(((unsigned char *)output) + (3 * ED25519_GE_LENGTH), &g3a);
	ed_sign(1, comp_ctx->rand3, ((unsigned char *)output) + (4 * ED25519_GE_LENGTH));

	comp_ctx->state_handler = secure_comparator_alice_step3;

	return THEMIS_SCOMPARE_SEND_OUTPUT_TO_PEER;
}

static themis_status_t secure_comparator_bob_step2(secure_comparator_t *comp_ctx, const void *input, size_t input_length, void *output, size_t *output_length)
{
	ge_p3 g2a;

	ge_p3 g2b;
	ge_p3 g3b;

	soter_status_t soter_status;
	size_t secret_length = sizeof(comp_ctx->secret);

	if (input_length < (6 * ED25519_GE_LENGTH))
	{
		return THEMIS_INVALID_PARAMETER;
	}

	if (ge_frombytes_vartime(&g2a, (const unsigned char *)input))
	{
		return THEMIS_INVALID_PARAMETER;
	}
	if (ge_frombytes_vartime(&(comp_ctx->g3p), ((const unsigned char *)input) + (3 * ED25519_GE_LENGTH)))
	{
		return THEMIS_INVALID_PARAMETER;
	}

	/* Output will contain 4 group elements and 2 * 2 + 3 ZK-proofs */
	if ((!output) || (*output_length < (11 * ED25519_GE_LENGTH)))
	{
		*output_length = 11 * ED25519_GE_LENGTH;
		return THEMIS_BUFFER_TOO_SMALL;
	}

	if (!ed_verify(1, &g2a, (const unsigned char *)input + ED25519_GE_LENGTH))
	{
		return THEMIS_INVALID_PARAMETER;
	}
	if (!ed_verify(1, &(comp_ctx->g3p), (const unsigned char *)input + (4 * ED25519_GE_LENGTH)))
	{
		return THEMIS_INVALID_PARAMETER;
	}

	*output_length = 11 * ED25519_GE_LENGTH;

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
	ge_scalarmult_blinded(&(comp_ctx->g3), comp_ctx->rand3, &(comp_ctx->g3p));

	if (ge_is_zero(&(comp_ctx->g2)) || ge_is_zero(&(comp_ctx->g3)))
	{
		comp_ctx->result = THEMIS_SCOMPARE_NO_MATCH;
	}

	generate_random_32(comp_ctx->rand);

	ge_scalarmult_blinded(&(comp_ctx->P), comp_ctx->rand, &(comp_ctx->g3));
	ge_double_scalarmult_vartime((ge_p2 *)&(comp_ctx->Q), comp_ctx->secret, &(comp_ctx->g2), comp_ctx->rand);
	ge_p2_to_p3(&(comp_ctx->Q), (const ge_p2 *)&(comp_ctx->Q));

	ge_p3_tobytes((unsigned char *)output, &g2b);
	ed_sign(2, comp_ctx->rand2, ((unsigned char *)output) + ED25519_GE_LENGTH);

	ge_p3_tobytes(((unsigned char *)output) + (3 * ED25519_GE_LENGTH), &g3b);
	ed_sign(2, comp_ctx->rand3, ((unsigned char *)output) + (4 * ED25519_GE_LENGTH));

	ge_p3_tobytes(((unsigned char *)output) + (6 * ED25519_GE_LENGTH), &(comp_ctx->P));
	ge_p3_tobytes(((unsigned char *)output) + (7 * ED25519_GE_LENGTH), &(comp_ctx->Q));
	ed_dbl_base_sign(2, comp_ctx->rand, comp_ctx->secret, &(comp_ctx->g2), &(comp_ctx->g3), ((unsigned char *)output) + (8 * ED25519_GE_LENGTH));

	comp_ctx->state_handler = secure_comparator_bob_step4;

	return THEMIS_SCOMPARE_SEND_OUTPUT_TO_PEER;
}

static themis_status_t secure_comparator_alice_step3(secure_comparator_t *comp_ctx, const void *input, size_t input_length, void *output, size_t *output_length)
{
	ge_p3 g2b;
	ge_p3 Qb;

	ge_p3 R;

	if (input_length < (11 * ED25519_GE_LENGTH))
	{
		return THEMIS_INVALID_PARAMETER;
	}

	if (ge_frombytes_vartime(&g2b, (const unsigned char *)input))
	{
		return THEMIS_INVALID_PARAMETER;
	}
	if (ge_frombytes_vartime(&(comp_ctx->g3p), ((const unsigned char *)input) + (3 * ED25519_GE_LENGTH)))
	{
		return THEMIS_INVALID_PARAMETER;
	}
	if (ge_frombytes_vartime(&(comp_ctx->Pp), ((const unsigned char *)input) + (6 * ED25519_GE_LENGTH)))
	{
		return THEMIS_INVALID_PARAMETER;
	}
	if (ge_frombytes_vartime(&Qb, ((const unsigned char *)input) + (7 * ED25519_GE_LENGTH)))
	{
		return THEMIS_INVALID_PARAMETER;
	}

	/* Output will contain 3 group elements and 5 ZK-proofs */
	if ((!output) || (*output_length < (8 * ED25519_GE_LENGTH)))
	{
		*output_length = 8 * ED25519_GE_LENGTH;
		return THEMIS_BUFFER_TOO_SMALL;
	}

	if (!ed_verify(2, &g2b, (const unsigned char *)input + ED25519_GE_LENGTH))
	{
		return THEMIS_INVALID_PARAMETER;
	}
	if (!ed_verify(2, &(comp_ctx->g3p), (const unsigned char *)input + (4 * ED25519_GE_LENGTH)))
	{
		return THEMIS_INVALID_PARAMETER;
	}

	*output_length = 8 * ED25519_GE_LENGTH;

	ge_scalarmult_blinded(&(comp_ctx->g2), comp_ctx->rand2, &g2b);
	ge_scalarmult_blinded(&(comp_ctx->g3), comp_ctx->rand3, &(comp_ctx->g3p));

	if (ge_is_zero(&(comp_ctx->g2)) || ge_is_zero(&(comp_ctx->g3)))
	{
		comp_ctx->result = THEMIS_SCOMPARE_NO_MATCH;
	}

	if (!ed_dbl_base_verify(2, &(comp_ctx->g2), &(comp_ctx->g3), &(comp_ctx->Pp), &Qb, ((unsigned char *)output) + (8 * ED25519_GE_LENGTH)))
	{
		return THEMIS_INVALID_PARAMETER;
	}

	generate_random_32(comp_ctx->rand);

	ge_scalarmult_blinded(&(comp_ctx->P), comp_ctx->rand, &(comp_ctx->g3));
	ge_double_scalarmult_vartime((ge_p2 *)&(comp_ctx->Q), comp_ctx->secret, &(comp_ctx->g2), comp_ctx->rand);
	ge_p2_to_p3(&(comp_ctx->Q), (const ge_p2 *)&(comp_ctx->Q));

	if (!ge_cmp(&Qb, &(comp_ctx->Q)))
	{
		comp_ctx->result = THEMIS_SCOMPARE_NO_MATCH;
	}

	ge_p3_sub(&(comp_ctx->Qa_Qb), &(comp_ctx->Q), &Qb);
	ge_scalarmult_blinded(&R, comp_ctx->rand3, &(comp_ctx->Qa_Qb));

	/* send to bob */
	ge_p3_tobytes((unsigned char *)output, &(comp_ctx->P));
	ge_p3_tobytes(((unsigned char *)output) + ED25519_GE_LENGTH, &(comp_ctx->Q));
	ed_dbl_base_sign(3, comp_ctx->rand, comp_ctx->secret, &(comp_ctx->g2), &(comp_ctx->g3), ((unsigned char *)output) + (2 * ED25519_GE_LENGTH));
	ge_p3_tobytes(((unsigned char *)output) + (5 * ED25519_GE_LENGTH), &R);
	ed_point_sign(3, comp_ctx->rand3, &(comp_ctx->Qa_Qb), ((unsigned char *)output) + (6 * ED25519_GE_LENGTH));

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

	if (input_length < (8 * ED25519_GE_LENGTH))
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
	if (ge_frombytes_vartime(&Ra, ((const unsigned char *)input) + (5 * ED25519_GE_LENGTH)))
	{
		return THEMIS_INVALID_PARAMETER;
	}

	if (!ge_cmp(&Qa, &(comp_ctx->Q)))
	{
		comp_ctx->result = THEMIS_SCOMPARE_NO_MATCH;
	}
	if (!ge_cmp(&Pa, &(comp_ctx->P)))
	{
		comp_ctx->result = THEMIS_SCOMPARE_NO_MATCH;
	}

	/* Output will contain 1 group element and 2 ZK-proofs */
	if ((!output) || (*output_length < (3 * ED25519_GE_LENGTH)))
	{
		*output_length = 3 * ED25519_GE_LENGTH;
		return THEMIS_BUFFER_TOO_SMALL;
	}

	*output_length = 3 * ED25519_GE_LENGTH;

	if (!ed_dbl_base_verify(3, &(comp_ctx->g2), &(comp_ctx->g3), &Pa, &Qa, ((unsigned char *)output) + (2 * ED25519_GE_LENGTH)))
	{
		return THEMIS_INVALID_PARAMETER;
	}

	ge_p3_sub(&Qa, &Qa, &(comp_ctx->Q));
	if (!ed_point_verify(3, &(comp_ctx->g3p), &Qa, &Ra, ((unsigned char *)output) + (6 * ED25519_GE_LENGTH)))
	{
		return THEMIS_INVALID_PARAMETER;
	}

	ge_scalarmult_blinded(&R, comp_ctx->rand3, &Qa);

	ge_scalarmult_blinded(&Rab, comp_ctx->rand3, &Ra);
	ge_p3_sub(&Pa_Pb, &Pa, &(comp_ctx->P));

	if (THEMIS_SCOMPARE_NOT_READY == comp_ctx->result)
	{
		comp_ctx->result = ge_cmp(&Rab, &Pa_Pb) ? THEMIS_SCOMPARE_NO_MATCH : THEMIS_SCOMPARE_MATCH;
	}

	ge_p3_tobytes((unsigned char *)output, &R);
	ed_point_sign(4, comp_ctx->rand3, &Qa, ((unsigned char *)output) + ED25519_GE_LENGTH);

	memset(comp_ctx->secret, 0, sizeof(comp_ctx->secret));
	comp_ctx->state_handler = NULL;

	return THEMIS_SCOMPARE_SEND_OUTPUT_TO_PEER;
}

static themis_status_t secure_comparator_alice_step5(secure_comparator_t *comp_ctx, const void *input, size_t input_length, void *output, size_t *output_length)
{
	ge_p3 Rb;

	ge_p3 Rab;
	ge_p3 Pa_Pb;

	if (input_length < (3 * ED25519_GE_LENGTH))
	{
		return THEMIS_INVALID_PARAMETER;
	}

	if (ge_frombytes_vartime(&Rb, (const unsigned char *)input))
	{
		return THEMIS_INVALID_PARAMETER;
	}

	if (!ge_cmp(&(comp_ctx->Pp), &(comp_ctx->P)))
	{
		comp_ctx->result = THEMIS_SCOMPARE_NO_MATCH;
	}

	/* No output */
	if (!output)
	{
		*output_length = 0;
		return THEMIS_BUFFER_TOO_SMALL;
	}

	*output_length = 0;

	if (!ed_point_verify(4, &(comp_ctx->g3p), &(comp_ctx->Qa_Qb), &Rb, ((unsigned char *)output) + ED25519_GE_LENGTH))
	{
		return THEMIS_INVALID_PARAMETER;
	}

	ge_scalarmult_blinded(&Rab, comp_ctx->rand3, &Rb);
	ge_p3_sub(&Pa_Pb, &(comp_ctx->P), &(comp_ctx->Pp));

	if (THEMIS_SCOMPARE_NOT_READY == comp_ctx->result)
	{
		comp_ctx->result = ge_cmp(&Rab, &Pa_Pb) ? THEMIS_SCOMPARE_NO_MATCH : THEMIS_SCOMPARE_MATCH;
	}

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

#endif
