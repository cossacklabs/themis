/**
 * @file
 * @brief tests for Soter
 *
 * (c) CossackLabs
 */

#include "soter_test.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    testsuite_start_testing();

//	run_soter_hash_tests();
//	run_soter_hmac_tests();
	run_soter_asym_cipher_tests();
//	run_soter_asym_ka_tests();
//	run_soter_sym_test();
//	run_soter_sign_test();
//	run_soter_rand_tests();

    testsuite_finish_testing();

    return testsuite_get_return_value();
}
