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

#include <themis/secure_comparator.h>
#include <themis/secure_comparator_t.h>
#include <stdio.h>
#include <string.h>
#include "themis_test.h"

/* Implemented in themis_secure_comparator_security.c */
void secure_comparator_security_test(void);

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

void run_secure_comparator_test(void)
{
	testsuite_enter_suite("secure comparator: api test");
	testsuite_run_test(secure_comparator_api_test);

	testsuite_enter_suite("secure comparator: security test");
	testsuite_run_test(secure_comparator_security_test);
}
