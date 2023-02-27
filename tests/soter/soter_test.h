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

#ifndef SOTER_TEST_H
#define SOTER_TEST_H

#include <soter/soter.h>
#include <soter/soter_rsa_key.h>
#include <soter/soter_t.h>

#include <common/test_utils.h>

void run_soter_hash_tests(void);
void run_soter_asym_cipher_tests(void);
void run_soter_asym_ka_tests(void);
void run_soter_sym_test(void);
void run_soter_sign_test(void);
void run_soter_rand_tests(void);
void run_soter_hmac_tests(void);
void run_soter_kdf_tests(void);
void run_openssl_tests(void);

#endif /* SOTER_TEST_H */
