/*
 * Copyright (c) 2019 Cossack Labs Limited
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

#include "soter/soter_test.h"

#include <stdio.h>
#include <string.h>

struct kdf_test_vector {
    const char* passphrase;
    size_t passphrase_length;
    const char* salt;
    size_t salt_length;
    size_t iterations;
    const char* key;
    size_t key_length;
};

/*
 * Unfortunately, there is no RFC with test vectors for PBKDF2 with HMAC-SHA2
 * family, so here's the best substitute available.
 *
 * https://stackoverflow.com/questions/5130513/pbkdf2-hmac-sha2-test-vectors
 */
static const struct kdf_test_vector pbkdf2_sha256_test_vectors[] = {
    {"password",
     8,
     "salt",
     4,
     1,
     "\x12\x0f\xb6\xcf\xfc\xf8\xb3\x2c\x43\xe7\x22\x52\x56\xc4\xf8\x37\xa8\x65\x48\xc9\x2c\xcc\x35\x48\x08\x05\x98\x7c\xb7\x0b\xe1\x7b",
     32},
    {"password",
     8,
     "salt",
     4,
     2,
     "\xae\x4d\x0c\x95\xaf\x6b\x46\xd3\x2d\x0a\xdf\xf9\x28\xf0\x6d\xd0\x2a\x30\x3f\x8e\xf3\xc2\x51\xdf\xd6\xe2\xd8\x5a\x95\x47\x4c\x43",
     32},
    {"password",
     8,
     "salt",
     4,
     4096,
     "\xc5\xe4\x78\xd5\x92\x88\xc8\x41\xaa\x53\x0d\xb6\x84\x5c\x4c\x8d\x96\x28\x93\xa0\x01\xce\x4e\x11\xa4\x96\x38\x73\xaa\x98\x13\x4a",
     32},
    {"password",
     8,
     "salt",
     4,
     16777216,
     "\xcf\x81\xc6\x6f\xe8\xcf\xc0\x4d\x1f\x31\xec\xb6\x5d\xab\x40\x89\xf7\xf1\x79\xe8\x9b\x3b\x0b\xcb\x17\xad\x10\xe3\xac\x6e\xba\x46",
     32},
    {"passwordPASSWORDpassword",
     24,
     "saltSALTsaltSALTsaltSALTsaltSALTsalt",
     36,
     4096,
     "\x34\x8c\x89\xdb\xcb\xd3\x2b\x2f\x32\xd8\x14\xb8\x11\x6e\x84\xcf\x2b\x17\x34\x7e\xbc\x18\x00\x18\x1c\x4e\x2a\x1f\xb8\xdd\x53\xe1\xc6\x35\x51\x8c\x7d\xac\x47\xe9",
     40},
    {"pass\0word", 9, "sa\0lt", 5, 4096, "\x89\xb6\x9d\x05\x16\xf8\x29\x89\x3c\x69\x62\x26\x65\x0a\x86\x87", 16},
};

#if SOTER_KDF_RUN_LONG_TESTS
static const size_t pbkdf2_iteration_limit = SIZE_MAX;
#else
static const size_t pbkdf2_iteration_limit = 2000000;
#endif

static void test_pbkdf2_sha256(void)
{
    soter_status_t res;
    uint8_t key_buffer[256];
    char name_buffer[256];
    size_t i;

    for (i = 0; i < ARRAY_SIZE(pbkdf2_sha256_test_vectors); i++) {
        const struct kdf_test_vector* v = &pbkdf2_sha256_test_vectors[i];

        if (v->iterations >= pbkdf2_iteration_limit) {
            snprintf(name_buffer, sizeof(name_buffer), "test vector #%zu (skipped)", i + 1);
            testsuite_fail_unless(true, name_buffer);
            continue;
        }

        snprintf(name_buffer, sizeof(name_buffer), "test vector #%zu", i + 1);

        res = soter_pbkdf2_sha256((const uint8_t*)v->passphrase,
                                  v->passphrase_length,
                                  (const uint8_t*)v->salt,
                                  v->salt_length,
                                  v->iterations,
                                  key_buffer,
                                  v->key_length);

        testsuite_fail_unless(res == SOTER_SUCCESS && memcmp(key_buffer, v->key, v->key_length) == 0,
                              name_buffer);
    }
}

static void test_pbkdf2_sha256_api(void)
{
    soter_status_t res;
    uint8_t key_buffer[256];
    uint8_t passphrase[] = {'p', 'a', 's', 's', 'w', 'o', 'r', 'd'};
    uint8_t salt[] = {'s', 'a', 'l', 't'};

    res = soter_pbkdf2_sha256(NULL, 0, salt, sizeof(salt), 1, key_buffer, 1);
    testsuite_fail_unless(res == SOTER_INVALID_PARAMETER, "null passphrase, zero length");

    res = soter_pbkdf2_sha256(NULL, 1, salt, sizeof(salt), 1, key_buffer, 1);
    testsuite_fail_unless(res == SOTER_INVALID_PARAMETER, "null passphrase, non-zero length");

    res = soter_pbkdf2_sha256(passphrase, sizeof(passphrase), NULL, 0, 1, key_buffer, 1);
    testsuite_fail_unless(res == SOTER_SUCCESS, "null salt, zero length");

    res = soter_pbkdf2_sha256(passphrase, sizeof(passphrase), NULL, 1, 1, key_buffer, 1);
    testsuite_fail_unless(res == SOTER_INVALID_PARAMETER, "null salt, non-zero length");

    res = soter_pbkdf2_sha256(passphrase, sizeof(passphrase), salt, sizeof(salt), 0, key_buffer, 1);
    testsuite_fail_unless(res == SOTER_INVALID_PARAMETER, "zero iterations");

    res = soter_pbkdf2_sha256(passphrase, sizeof(passphrase), salt, sizeof(salt), 1, NULL, 0);
    testsuite_fail_unless(res == SOTER_INVALID_PARAMETER, "null key");

    res = soter_pbkdf2_sha256(passphrase, sizeof(passphrase), salt, sizeof(salt), 1, key_buffer, 0);
    testsuite_fail_unless(res == SOTER_INVALID_PARAMETER, "zero key length");
}

void run_soter_kdf_tests(void)
{
    testsuite_enter_suite("Soter KDF: PBKDF2 HMAC-SHA-256");
    testsuite_run_test(test_pbkdf2_sha256);
    testsuite_run_test(test_pbkdf2_sha256_api);
}
