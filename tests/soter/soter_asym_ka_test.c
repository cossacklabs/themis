/**
 * @file
 * @brief tests for Soter key agreement
 *
 * (c) CossackLabs
 */

#include "soter_test.h"

#define KEY_BUFFER_SIZE 2048
#define SHARED_SECRET_BUFFER_SIZE 128

static void test_basic_ka_flow(void)
{
	uint8_t key_buffer[KEY_BUFFER_SIZE];
	size_t key_buffer_length = sizeof(key_buffer);

	uint8_t peer1_shared_secret[SHARED_SECRET_BUFFER_SIZE];
	size_t peer1_shared_secret_length = sizeof(peer1_shared_secret);

	uint8_t peer2_shared_secret[SHARED_SECRET_BUFFER_SIZE];
	size_t peer2_shared_secret_length = sizeof(peer2_shared_secret);

	soter_status_t res;
	soter_asym_ka_t *peer1 = soter_asym_ka_create(SOTER_ASYM_KA_EC_P256);
	soter_asym_ka_t *peer2 = soter_asym_ka_create(SOTER_ASYM_KA_EC_P256);

	if ((!peer1) || (!peer2))
	{
		goto err;
	}

	res = soter_asym_ka_gen_key(peer1);
	if (HERMES_SUCCESS != res)
	{
		testsuite_fail_unless(HERMES_SUCCESS == res, "soter_asym_ka_gen_key fail");
		goto err;
	}

	res = soter_asym_ka_gen_key(peer2);
	if (HERMES_SUCCESS != res)
	{
		testsuite_fail_unless(HERMES_SUCCESS == res, "soter_asym_ka_gen_key fail");
		goto err;
	}

	res = soter_asym_ka_export_key(peer2, key_buffer, &key_buffer_length, false);
	if (HERMES_SUCCESS != res)
	{
		testsuite_fail_unless(HERMES_SUCCESS == res, "soter_asym_ka_export_key fail");
		goto err;
	}

	res = soter_asym_ka_derive(peer1, key_buffer, key_buffer_length, peer1_shared_secret, &peer1_shared_secret_length);
	if (HERMES_SUCCESS != res)
	{
		testsuite_fail_unless(HERMES_SUCCESS == res, "soter_asym_ka_derive fail");
		goto err;
	}

	key_buffer_length = sizeof(key_buffer);

	res = soter_asym_ka_export_key(peer1, key_buffer, &key_buffer_length, false);
	if (HERMES_SUCCESS != res)
	{
		testsuite_fail_unless(HERMES_SUCCESS == res, "soter_asym_ka_export_key fail");
		goto err;
	}

	res = soter_asym_ka_derive(peer2, key_buffer, key_buffer_length, peer2_shared_secret, &peer2_shared_secret_length);
	if (HERMES_SUCCESS != res)
	{
		testsuite_fail_unless(HERMES_SUCCESS == res, "soter_asym_ka_derive fail");
		goto err;
	}

	testsuite_fail_unless((peer1_shared_secret_length == peer2_shared_secret_length) && !memcmp(peer1_shared_secret, peer2_shared_secret), "Basic ECDH");

err:
	if (peer1)
	{
		res = soter_asym_ka_destroy(peer1);
		if (HERMES_SUCCESS != res)
		{
			testsuite_fail_unless(HERMES_SUCCESS == res, "soter_asym_ka_destroy fail");
		}
	}

	if (peer2)
	{
		res = soter_asym_ka_destroy(peer2);
		if (HERMES_SUCCESS != res)
		{
			testsuite_fail_unless(HERMES_SUCCESS == res, "soter_asym_ka_destroy fail");
		}
	}
}

void run_soter_asym_ka_tests(void)
{
	testsuite_enter_suite("soter asym ka: basic flow");

	testsuite_run_test(test_basic_ka_flow);
}
