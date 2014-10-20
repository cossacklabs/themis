/**
 * @file
 * @brief tests for Soter
 *
 * (c) CossackLabs
 */

#include "soter_test.h"
#include <stdio.h>

static void print_bytes(const uint8_t *bytes, size_t length)
{
	size_t i;

	for (i = 0; i < length; i++)
		printf("%02x", bytes[i]);
	puts("");
}

/* For now, just generate some random bits and print them */
static void test_rand(void)
{
	uint8_t bytes[4096];
	size_t length = sizeof(bytes);

	if (HERMES_SUCCESS == soter_rand(bytes, length))
		print_bytes(bytes, length);
}

int main()
{
//	test_rand();

    testsuite_start_testing();

	run_soter_hash_tests();
	run_soter_asym_cipher_tests();
	run_soter_sym_test();

    testsuite_finish_testing();

    return testsuite_get_return_value();
}
