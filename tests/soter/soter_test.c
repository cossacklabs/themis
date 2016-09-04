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

#include "soter_test.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    testsuite_start_testing();

	run_soter_hash_tests();
	run_soter_hmac_tests();
	run_soter_asym_cipher_tests();
	run_soter_asym_ka_tests();
	run_soter_sym_test();
	run_soter_sign_test();
	run_soter_rand_tests();

    testsuite_finish_testing();

    return testsuite_get_return_value();
}
