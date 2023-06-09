/*
 * Copyright (c) 2023 Cossack Labs Limited
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

#include <openssl/opensslv.h>

#if OPENSSL_VERSION_NUMBER >= 0x30000000

#include "soter/openssl/soter_engine.h"
#include "soter/soter_test.h"

#include <string.h>

void test_openssl_constants(void)
{
    testsuite_fail_if(MAX_CURVE_NAME_LEN < strlen(SN_X9_62_prime256v1) + 1,
                      "MAX_CURVE_NAME_LEN could fit SN_X9_62_prime256v1");
    testsuite_fail_if(MAX_CURVE_NAME_LEN < strlen(SN_secp384r1) + 1,
                      "MAX_CURVE_NAME_LEN could fit SN_secp384r1");
    testsuite_fail_if(MAX_CURVE_NAME_LEN < strlen(SN_secp521r1) + 1,
                      "MAX_CURVE_NAME_LEN could fit SN_secp521r1");
}

void run_openssl_tests(void)
{
    testsuite_enter_suite("soter openssl backend: constants");
    testsuite_run_test(test_openssl_constants);
}

#else

void run_openssl_tests(void)
{
}

#endif
