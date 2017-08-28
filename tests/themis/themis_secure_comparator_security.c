/*
 * themis_secure_comparator_security.c
 *
 *  Created on: 14 Jan 2016
 *      Author: ignat
 */

#include <themis/secure_comparator.h>
#include <themis/secure_comparator_t.h>
#include <stdio.h>
#include <string.h>
#include "themis_test.h"

/* Peers will communicate using shared memory */
static uint8_t shared_mem[512];
static size_t current_length = 0;

/* below functions are copied from secure_comparator.c */
/* Having duplicate code in tests is better than making them public in real code */
static bool ge_is_zero(const ge_p3 *ge)
{
	uint8_t y[ED25519_GE_LENGTH];
	uint8_t z[ED25519_GE_LENGTH];

	fe_tobytes(y, ge->Y);
	fe_tobytes(z, ge->Z);

	return (!fe_isnonzero(ge->X)) && (!crypto_verify_32(y, z));
}

static themis_status_t ed_sign(uint8_t pos, const uint8_t *scalar, uint8_t *signature)
{
	uint8_t r[ED25519_GE_LENGTH];
	ge_p3 R;
	uint8_t k[64];

	soter_hash_ctx_t hash_ctx;
	size_t hash_length = 64;
	themis_status_t res;

	generate_random_32(r);
	ge_scalarmult_base(&R, r);
	ge_p3_tobytes(k, &R);

	res = soter_hash_init(&hash_ctx, SOTER_HASH_SHA512);
	if (THEMIS_SUCCESS != res)
	{
		return res;
	}

	res = soter_hash_update(&hash_ctx, k, ED25519_GE_LENGTH);
	if (THEMIS_SUCCESS != res)
	{
		soter_hash_final(&hash_ctx, k, &hash_length);
		return res;
	}

	res = soter_hash_update(&hash_ctx, &pos, sizeof(pos));
	if (THEMIS_SUCCESS != res)
	{
		soter_hash_final(&hash_ctx, k, &hash_length);
		return res;
	}

	res = soter_hash_final(&hash_ctx, k, &hash_length);

	if (THEMIS_SUCCESS == res)
	{
		sc_reduce(k);

		memcpy(signature, k, ED25519_GE_LENGTH);
		sc_muladd(signature + ED25519_GE_LENGTH, k, scalar, r);
	}

	return res;
}

static themis_status_t ed_dbl_base_sign(uint8_t pos, const uint8_t *scalar1, const uint8_t *scalar2, const ge_p3 *base1, const ge_p3 *base2, uint8_t *signature)
{
	uint8_t r1[ED25519_GE_LENGTH];
	uint8_t r2[ED25519_GE_LENGTH];
	ge_p3 R1;
	ge_p2 R2;
	uint8_t k[64];

	soter_hash_ctx_t hash_ctx;
	size_t hash_length = 64;

	themis_status_t res;

	generate_random_32(r1);
	generate_random_32(r2);
	ge_scalarmult_blinded(&R1, r1, base2);
	ge_double_scalarmult_vartime(&R2, r2, base1, r1);

	res = soter_hash_init(&hash_ctx, SOTER_HASH_SHA512);
	if (THEMIS_SUCCESS != res)
	{
		return res;
	}

	ge_p3_tobytes(k, &R1);
	res = soter_hash_update(&hash_ctx, k, ED25519_GE_LENGTH);
	if (THEMIS_SUCCESS != res)
	{
		soter_hash_final(&hash_ctx, k, &hash_length);
		return res;
	}

	ge_tobytes(k, &R2);
	res = soter_hash_update(&hash_ctx, k, ED25519_GE_LENGTH);
	if (THEMIS_SUCCESS != res)
	{
		soter_hash_final(&hash_ctx, k, &hash_length);
		return res;
	}

	res = soter_hash_update(&hash_ctx, &pos, sizeof(pos));
	if (THEMIS_SUCCESS != res)
	{
		soter_hash_final(&hash_ctx, k, &hash_length);
		return res;
	}

	res = soter_hash_final(&hash_ctx, k, &hash_length);

	if (THEMIS_SUCCESS == res)
	{
		sc_reduce(k);
		memcpy(signature, k, ED25519_GE_LENGTH);
		sc_muladd(signature + ED25519_GE_LENGTH, k, scalar1, r1);
		sc_muladd(signature + (2 * ED25519_GE_LENGTH), k, scalar2, r2);
	}

	return res;
}

static themis_status_t ed_point_sign(uint8_t pos, const uint8_t *scalar, const ge_p3 *point, uint8_t *signature)
{
	uint8_t r[ED25519_GE_LENGTH];
	ge_p3 R;
	uint8_t k[64];

	soter_hash_ctx_t hash_ctx;
	size_t hash_length = 64;

	themis_status_t res;

	generate_random_32(r);
	ge_scalarmult_base(&R, r);
	ge_p3_tobytes(k, &R);

	res = soter_hash_init(&hash_ctx, SOTER_HASH_SHA512);
	if (THEMIS_SUCCESS != res)
	{
		return res;
	}

	res = soter_hash_update(&hash_ctx, k, ED25519_GE_LENGTH);
	if (THEMIS_SUCCESS != res)
	{
		soter_hash_final(&hash_ctx, k, &hash_length);
		return res;
	}

	ge_scalarmult_blinded(&R, r, point);
	ge_p3_tobytes(k, &R);

	res = soter_hash_update(&hash_ctx, k, ED25519_GE_LENGTH);
	if (THEMIS_SUCCESS != res)
	{
		soter_hash_final(&hash_ctx, k, &hash_length);
		return res;
	}

	res = soter_hash_update(&hash_ctx, &pos, sizeof(pos));
	if (THEMIS_SUCCESS != res)
	{
		soter_hash_final(&hash_ctx, k, &hash_length);
		return res;
	}

	res = soter_hash_final(&hash_ctx, k, &hash_length);
	if (THEMIS_SUCCESS != res)
	{
		return res;
	}

	if (THEMIS_SUCCESS == res)
	{
		sc_reduce(k);
		memcpy(signature, k, ED25519_GE_LENGTH);
		sc_muladd(signature + ED25519_GE_LENGTH, k, scalar, r);
	}

	return res;
}

static void corrupt_alice_step1(secure_comparator_t *alice, void *output)
{
	/* Let's assume alice is malicious and uses zeroes instead of random numbers */

	ge_p3 g2a;
	ge_p3 g3a;

	memset(alice->rand2, 0, sizeof(alice->rand2));
	memset(alice->rand3, 0, sizeof(alice->rand3));

	ge_scalarmult_base(&g2a, alice->rand2);
	ge_scalarmult_base(&g3a, alice->rand3);

	ge_p3_tobytes((unsigned char *)output, &g2a);
	ed_sign(1, alice->rand2, ((unsigned char *)output) + ED25519_GE_LENGTH);

	ge_p3_tobytes(((unsigned char *)output) + (3 * ED25519_GE_LENGTH), &g3a);
	ed_sign(2, alice->rand3, ((unsigned char *)output) + (4 * ED25519_GE_LENGTH));
}

static void corrupt_bob_step2(secure_comparator_t *bob, const void *input, size_t input_length, void *output, size_t *output_length)
{
	/* Let's assume bob is malicious and uses zeroes instead of random numbers */

	ge_p3 g2a;
	ge_p3 g3a;

	ge_p3 g2b;
	ge_p3 g3b;

	ge_frombytes_vartime(&g2a, (const unsigned char *)input);
	ge_frombytes_vartime(&g3a, ((const unsigned char *)input) + (3 * ED25519_GE_LENGTH));

	if (THEMIS_SCOMPARE_SEND_OUTPUT_TO_PEER != secure_comparator_proceed_compare(bob, input, input_length, output, output_length))
	{
		testsuite_fail_if(true, "secure_comparator_proceed_compare failed");
		return;
	}

	memset(bob->rand2, 0, sizeof(bob->rand2));
	memset(bob->rand3, 0, sizeof(bob->rand3));

	ge_scalarmult_base(&g2b, bob->rand2);
	ge_scalarmult_base(&g3b, bob->rand3);

	ge_scalarmult_blinded(&(bob->g2), bob->rand2, &g2a);
	ge_scalarmult_blinded(&(bob->g3), bob->rand3, &g3a);

	memset(bob->rand, 0, sizeof(bob->rand));

	ge_scalarmult_blinded(&(bob->P), bob->rand, &(bob->g3));
	ge_double_scalarmult_vartime((ge_p2 *)&(bob->Q), bob->secret, &(bob->g2), bob->rand);
	ge_p2_to_p3(&(bob->Q), (const ge_p2 *)&(bob->Q));

	ge_p3_tobytes((unsigned char *)output, &g2b);
	ed_sign(3, bob->rand2, ((unsigned char *)output) + ED25519_GE_LENGTH);

	ge_p3_tobytes(((unsigned char *)output) + (3 * ED25519_GE_LENGTH), &g3b);
	ed_sign(4, bob->rand3, ((unsigned char *)output) + (4 * ED25519_GE_LENGTH));

	ge_p3_tobytes(((unsigned char *)output) + (6 * ED25519_GE_LENGTH), &(bob->P));
	ge_p3_tobytes(((unsigned char *)output) + (7 * ED25519_GE_LENGTH), &(bob->Q));
	ed_dbl_base_sign(5, bob->rand, bob->secret, &(bob->g2), &(bob->g3), ((unsigned char *)output) + (8 * ED25519_GE_LENGTH));
}

void secure_comparator_security_test(void)
{
	const char alice_secret[] = "alice secret";
	const char bob_secret[] = "bob secret";

	size_t output_length = sizeof(shared_mem);

	secure_comparator_t alice, bob;

	if (THEMIS_SUCCESS != secure_comparator_init(&alice))
	{
		testsuite_fail_if(true, "secure_comparator_init failed");
		return;
	}

	if (THEMIS_SUCCESS != secure_comparator_init(&bob))
	{
		testsuite_fail_if(true, "secure_comparator_init failed");
		return;
	}

	if (THEMIS_SUCCESS != secure_comparator_append_secret(&alice, alice_secret, sizeof(alice_secret)))
	{
		testsuite_fail_if(true, "secure_comparator_append_secret failed");
		return;
	}

	if (THEMIS_SUCCESS != secure_comparator_append_secret(&bob, bob_secret, sizeof(bob_secret)))
	{
		testsuite_fail_if(true, "secure_comparator_append_secret failed");
		return;
	}

	current_length = sizeof(shared_mem);

	if (THEMIS_SCOMPARE_SEND_OUTPUT_TO_PEER != secure_comparator_begin_compare(&alice, shared_mem, &current_length))
	{
		testsuite_fail_if(true, "secure_comparator_begin_compare failed");
		return;
	}

	corrupt_alice_step1(&alice, shared_mem);

	corrupt_bob_step2(&bob, shared_mem, current_length, shared_mem, &output_length);

	current_length = output_length;
	output_length = sizeof(shared_mem);

	if (THEMIS_SCOMPARE_SEND_OUTPUT_TO_PEER != secure_comparator_proceed_compare(&alice, shared_mem, current_length, shared_mem, &output_length))
	{
		testsuite_fail_if(true, "secure_comparator_proceed_compare failed");
		return;
	}

	current_length = output_length;
	output_length = sizeof(shared_mem);

	if (THEMIS_SCOMPARE_SEND_OUTPUT_TO_PEER != secure_comparator_proceed_compare(&bob, shared_mem, current_length, shared_mem, &output_length))
	{
		testsuite_fail_if(true, "secure_comparator_proceed_compare failed");
		return;
	}

	current_length = output_length;
	output_length = sizeof(shared_mem);

	if (THEMIS_SUCCESS != secure_comparator_proceed_compare(&alice, shared_mem, current_length, shared_mem, &output_length))
	{
		testsuite_fail_if(true, "secure_comparator_proceed_compare failed");
		return;
	}

	testsuite_fail_unless((THEMIS_SCOMPARE_NO_MATCH == secure_comparator_get_result(&alice)) && (THEMIS_SCOMPARE_NO_MATCH == secure_comparator_get_result(&bob)), "compare result no match");
}
