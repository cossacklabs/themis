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

#include <themis/secure_comparator.h>
#include <themis/secure_comparator_t.h>
#include <stdio.h>
#include <string.h>
#include "themis_test.h"

/* Fuzz parameters */
#define MAX_SECRET_SIZE 2048

/* return codes for scheduler */
#define TEST_CONTINUE 1
#define TEST_STOP_SUCCESS 0
#define TEST_STOP_ERROR -1

static uint8_t secret[MAX_SECRET_SIZE];
static size_t secret_length = sizeof(secret);

static secure_comparator_t *alice = NULL;
static secure_comparator_t *bob = NULL;

/* Peers will communicate using shared memory */
static uint8_t shared_mem[512];
static size_t current_length = 0;

static int alice_function(void)
{
	size_t input_length = current_length;
	themis_status_t themis_status;

	current_length = sizeof(shared_mem);

	themis_status = secure_comparator_proceed_compare(alice, shared_mem, input_length, shared_mem, &current_length);

	switch (themis_status)
	{
	case THEMIS_SCOMPARE_SEND_OUTPUT_TO_PEER:
		if (0 == current_length)
		{
			testsuite_fail_if(true, "secure_comparator_proceed_compare: invalid output length");
			return TEST_STOP_ERROR;
		}
		return TEST_CONTINUE;
	case THEMIS_SUCCESS:
		if (current_length > 0)
		{
			testsuite_fail_if(true, "secure_comparator_proceed_compare: invalid output length");
			return TEST_STOP_ERROR;
		}
		return TEST_STOP_SUCCESS;
	default:
		return TEST_STOP_ERROR;
	}
}

static int bob_function(void)
{
	size_t input_length = current_length;
	themis_status_t themis_status;

	current_length = sizeof(shared_mem);

	themis_status = secure_comparator_proceed_compare(bob, shared_mem, input_length, shared_mem, &current_length);

	switch (themis_status)
	{
	case THEMIS_SCOMPARE_SEND_OUTPUT_TO_PEER:
		if (0 == current_length)
		{
			testsuite_fail_if(true, "secure_comparator_proceed_compare: invalid output length");
			return TEST_STOP_ERROR;
		}
		return TEST_CONTINUE;
	/* bob (as a responder) should not return THEMIS_SUCCESS */
	default:
		return TEST_STOP_ERROR;
	}
}

static void schedule(void)
{
	int (*peer_run)(void) = bob_function;
	int res = TEST_CONTINUE;
	themis_status_t themis_status;

	/* alice starts */
	testsuite_fail_unless(THEMIS_INVALID_PARAMETER == secure_comparator_begin_compare(NULL, shared_mem, &current_length), "secure_comparator_begin_compare: invalid context");
	testsuite_fail_unless(THEMIS_BUFFER_TOO_SMALL == secure_comparator_begin_compare(alice, NULL, &current_length), "secure_comparator_begin_compare: get output size (NULL out buffer)");
	current_length--;
	testsuite_fail_unless(THEMIS_BUFFER_TOO_SMALL == secure_comparator_begin_compare(alice, shared_mem, &current_length), "secure_comparator_begin_compare: get output size (small out buffer)");

	themis_status = secure_comparator_begin_compare(alice, shared_mem, &current_length);
	if (THEMIS_SCOMPARE_SEND_OUTPUT_TO_PEER != themis_status)
	{
		testsuite_fail_if(true, "secure_comparator_begin_compare: failed");
		return;
	}

	while (TEST_CONTINUE == res)
	{
		res = peer_run();
		peer_run = (peer_run == alice_function) ? bob_function : alice_function;
	}

	testsuite_fail_if(res, "secure comparator: protocol exchange");
}

static void secure_comparator_api_test(void)
{
	/* setup */
	themis_status_t themis_status;
	secret_length = rand_int(MAX_SECRET_SIZE);
	if (SOTER_SUCCESS != soter_rand(secret, secret_length))
	{
		testsuite_fail_if(true, "soter_rand failed");
		return;
	}

	/* match */
	alice = secure_comparator_create();
	if (!alice)
	{
		testsuite_fail_if(true, "secure_comparator_create failed");
		return;
	}

	bob = secure_comparator_create();
	if (!bob)
	{
		testsuite_fail_if(true, "secure_comparator_create failed");
		secure_comparator_destroy(alice);
		alice = NULL;
		return;
	}

	testsuite_fail_unless(THEMIS_INVALID_PARAMETER == secure_comparator_append_secret(NULL, secret, secret_length), "secure_comparator_append_secret: invalid context");
	testsuite_fail_unless(THEMIS_INVALID_PARAMETER == secure_comparator_append_secret(alice, NULL, secret_length), "secure_comparator_append_secret: invalid input buffer");
	testsuite_fail_unless(THEMIS_INVALID_PARAMETER == secure_comparator_append_secret(alice, secret, 0), "secure_comparator_append_secret: invalid input length");

	themis_status = secure_comparator_append_secret(alice, secret, secret_length);
	if (THEMIS_SUCCESS != themis_status)
	{
		testsuite_fail_if(true, "secure_comparator_append_secret failed");
		secure_comparator_destroy(bob);
		bob = NULL;
		secure_comparator_destroy(alice);
		alice = NULL;
		return;
	}

	themis_status = secure_comparator_append_secret(bob, secret, secret_length);
	if (THEMIS_SUCCESS != themis_status)
	{
		testsuite_fail_if(true, "secure_comparator_append_secret failed");
		secure_comparator_destroy(bob);
		bob = NULL;
		secure_comparator_destroy(alice);
		alice = NULL;
		return;
	}

	schedule();

	testsuite_fail_unless((THEMIS_SCOMPARE_MATCH == secure_comparator_get_result(alice)) && (THEMIS_SCOMPARE_MATCH == secure_comparator_get_result(bob)), "compare result match");

	testsuite_fail_unless(THEMIS_INVALID_PARAMETER == secure_comparator_destroy(NULL), "secure_comparator_destroy: invalid context");
	testsuite_fail_unless(THEMIS_SUCCESS == secure_comparator_destroy(bob), "secure_comparator_destroy: destroy bob");
	testsuite_fail_unless(THEMIS_SUCCESS == secure_comparator_destroy(alice), "secure_comparator_destroy: destroy alice");

	bob = NULL;
	alice = NULL;

	alice = secure_comparator_create();
	if (!alice)
	{
		testsuite_fail_if(true, "secure_comparator_create failed");
		return;
	}

	bob = secure_comparator_create();
	if (!bob)
	{
		testsuite_fail_if(true, "secure_comparator_create failed");
		secure_comparator_destroy(alice);
		alice = NULL;
		return;
	}

	themis_status = secure_comparator_append_secret(alice, secret, secret_length);
	if (THEMIS_SUCCESS != themis_status)
	{
		testsuite_fail_if(true, "secure_comparator_append_secret failed");
		secure_comparator_destroy(bob);
		bob = NULL;
		secure_comparator_destroy(alice);
		alice = NULL;
		return;
	}

	themis_status = secure_comparator_append_secret(bob, secret, secret_length - 1);
	if (THEMIS_SUCCESS != themis_status)
	{
		testsuite_fail_if(true, "secure_comparator_append_secret failed");
		secure_comparator_destroy(bob);
		bob = NULL;
		secure_comparator_destroy(alice);
		alice = NULL;
		return;
	}

	schedule();

	testsuite_fail_unless((THEMIS_SCOMPARE_NO_MATCH == secure_comparator_get_result(alice)) && (THEMIS_SCOMPARE_NO_MATCH == secure_comparator_get_result(bob)), "compare result no match");

	testsuite_fail_unless(THEMIS_SUCCESS == secure_comparator_destroy(bob), "secure_comparator_destroy: destroy bob");
	testsuite_fail_unless(THEMIS_SUCCESS == secure_comparator_destroy(alice), "secure_comparator_destroy: destroy alice");

	bob = NULL;
	alice = NULL;
}

#if 0
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
	ge_p3_tobytes(((unsigned char *)output) + ED25519_GE_LENGTH, &g3a);
}

static void corrupt_bob_step2(secure_comparator_t *bob, const void *input, size_t input_length, void *output, size_t *output_length)
{
	/* Let's assume bob is malicious and uses zeroes instead of random numbers */

	ge_p3 g2a;
	ge_p3 g3a;

	ge_p3 g2b;
	ge_p3 g3b;

	ge_frombytes_vartime(&g2a, (const unsigned char *)input);
	ge_frombytes_vartime(&g3a, ((const unsigned char *)input) + ED25519_GE_LENGTH);

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
	ge_p3_tobytes(((unsigned char *)output) + ED25519_GE_LENGTH, &g3b);
	ge_p3_tobytes(((unsigned char *)output) + ED25519_GE_LENGTH + ED25519_GE_LENGTH, &(bob->Q));
}

static void secure_comparator_security_test(void)
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
#endif

void run_secure_comparator_test(void)
{
	testsuite_enter_suite("secure comparator: api test");
	testsuite_run_test(secure_comparator_api_test);

	/* testsuite_enter_suite("secure comparator: security test");
	testsuite_run_test(secure_comparator_security_test); */
}

#endif
