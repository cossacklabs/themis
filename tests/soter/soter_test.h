/**
 * @file
 * @brief tests for Soter
 *
 * (c) CossackLabs
 */

#ifndef SOTER_TEST_H
#define SOTER_TEST_H

#include <common/error.h>
#include <common/test_utils.h>
#include <soter/soter_t.h>

void run_soter_hash_tests(void);
void run_soter_asym_cipher_tests(void);
void run_soter_asym_ka_tests(void);
void run_soter_sym_test(void);
void run_soter_sign_test(void);
void run_soter_rand_tests(void);
void run_soter_hmac_tests(void);

#endif /* SOTER_TEST_H */
