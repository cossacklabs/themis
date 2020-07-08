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

static const uint8_t zrtp_key[] = "\xAD\xEA\xDB\xEE\x01\x02\x03\x04";
static const size_t zrtp_key_length = 8;
static const char* zrtp_label = "ZRTP KDF test vectors";
static const uint8_t zrtp_context[] = "\x43\x83\xF3\x27\x0E\x11";
static const size_t zrtp_context_length = 6;

struct zrtp_kdf_test_vector {
    size_t output_length;
    const void* output;
};

/* Generated by test_vectors/soter_kdf.go */
static const struct zrtp_kdf_test_vector zrtp_kdf_test_vectors[] = {
    {1, "\x93"},
    {2, "\x93\xF1"},
    {4, "\x93\xF1\x61\xF7"},
    {8, "\x93\xF1\x61\xF7\xD0\x86\xBA\x6D"},
    {16, "\x93\xF1\x61\xF7\xD0\x86\xBA\x6D\x02\xFB\x0A\x24\xBF\x5E\x7C\x45"},
    {32, "\x93\xF1\x61\xF7\xD0\x86\xBA\x6D\x02\xFB\x0A\x24\xBF\x5E\x7C\x45\x32\x27\x7E\xF9\x1C\x4F\xB4\xF2\x90\x68\x0E\x3F\x4D\xF5\xE5\x92"},
};

static void test_soter_kdf(void)
{
    soter_status_t res;
    uint8_t key_buffer[32];
    char name_buffer[256];
    soter_kdf_context_buf_t context = {zrtp_context, zrtp_context_length};
    size_t i;

    for (i = 0; i < ARRAY_SIZE(zrtp_kdf_test_vectors); i++) {
        const struct zrtp_kdf_test_vector* v = &zrtp_kdf_test_vectors[i];

        snprintf(name_buffer, sizeof(name_buffer), "test vector #%zu", i + 1);

        res = soter_kdf(zrtp_key, zrtp_key_length, zrtp_label, &context, 1, key_buffer, v->output_length);

        testsuite_fail_unless(res == SOTER_SUCCESS
                                  && memcmp(key_buffer, v->output, v->output_length) == 0,
                              name_buffer);
    }
}

static void test_soter_kdf_api(void)
{
    soter_status_t res;
    uint8_t key_buffer[32];
    uint8_t big_key_buffer[64];
    soter_kdf_context_buf_t context = {zrtp_context, zrtp_context_length};

    res = soter_kdf(NULL, 0, zrtp_label, &context, 1, key_buffer, sizeof(key_buffer));
    testsuite_fail_unless(res == SOTER_SUCCESS, "null key, zero length");

    res = soter_kdf(NULL, zrtp_key_length, zrtp_label, &context, 1, key_buffer, sizeof(key_buffer));
    testsuite_fail_unless(res == SOTER_INVALID_PARAMETER, "null key, non-zero length");

    res = soter_kdf(zrtp_key, 0, zrtp_label, &context, 1, key_buffer, sizeof(key_buffer));
    testsuite_fail_unless(res == SOTER_INVALID_PARAMETER, "non-null key, zero length");

    res = soter_kdf(zrtp_key, zrtp_key_length, NULL, &context, 1, key_buffer, sizeof(key_buffer));
    testsuite_fail_unless(res == SOTER_INVALID_PARAMETER, "null key label");

    res = soter_kdf(zrtp_key, zrtp_key_length, "", &context, 1, key_buffer, sizeof(key_buffer));
    testsuite_fail_unless(res == SOTER_SUCCESS, "empty key label");

    res = soter_kdf(zrtp_key, zrtp_key_length, zrtp_label, NULL, 0, key_buffer, sizeof(key_buffer));
    testsuite_fail_unless(res == SOTER_SUCCESS, "null context, zero length");

    res = soter_kdf(zrtp_key, zrtp_key_length, zrtp_label, &context, 0, key_buffer, sizeof(key_buffer));
    testsuite_fail_unless(res == SOTER_SUCCESS, "non-null context, zero length");

    res = soter_kdf(zrtp_key, zrtp_key_length, zrtp_label, NULL, 1, key_buffer, sizeof(key_buffer));
    testsuite_fail_unless(res == SOTER_INVALID_PARAMETER, "null context, non-zero length");

    res = soter_kdf(zrtp_key, zrtp_key_length, zrtp_label, &context, 1, NULL, 0);
    testsuite_fail_unless(res == SOTER_INVALID_PARAMETER, "null buffer, zero length");

    res = soter_kdf(zrtp_key, zrtp_key_length, zrtp_label, &context, 1, NULL, sizeof(key_buffer));
    testsuite_fail_unless(res == SOTER_INVALID_PARAMETER, "null buffer, non-zero length");

    res = soter_kdf(zrtp_key, zrtp_key_length, zrtp_label, &context, 1, key_buffer, 0);
    testsuite_fail_unless(res == SOTER_INVALID_PARAMETER, "non-null buffer, zero length");

    res = soter_kdf(zrtp_key, zrtp_key_length, zrtp_label, &context, 1, big_key_buffer, sizeof(big_key_buffer));
    testsuite_fail_unless(res == SOTER_INVALID_PARAMETER, "too big buffer");
}

static void test_soter_kdf_skip_key(void)
{
    soter_status_t res;
    /*
     * Soter KDF allows a key to be omitted, in which case an implicit key
     * derived from label and context is used.
     *
     * Implicit key is XOR sum of label and each individual context buffer,
     * truncated or zero-extended to 32 bytes.
     */
    uint8_t expected_implicit_key[32] =
        "\x19\xD1\xA7\x77\x2E\x5A\x44\x46\x20\x74\x65\x73\x74\x20\x76\x65\x63\x74\x6F\x72\x73";
    uint8_t key_buffer_implicit[32] = {0};
    uint8_t key_buffer_explicit[32] = {0};
    soter_kdf_context_buf_t context = {zrtp_context, zrtp_context_length};

    res = soter_kdf(NULL, 0, zrtp_label, &context, 1, key_buffer_implicit, sizeof(key_buffer_implicit));
    testsuite_fail_unless(res == SOTER_SUCCESS, "implicit key");

    res = soter_kdf(expected_implicit_key,
                    sizeof(expected_implicit_key),
                    zrtp_label,
                    &context,
                    1,
                    key_buffer_explicit,
                    sizeof(key_buffer_explicit));
    testsuite_fail_unless(res == SOTER_SUCCESS, "explicit key");

    testsuite_fail_if(memcmp(key_buffer_implicit, key_buffer_explicit, sizeof(key_buffer_implicit)),
                      "implicit key == explicit key");
}

static void test_soter_kdf_skip_context(void)
{
    soter_status_t res;
    /*
     * Context buffers are concatenated and NULL buffers are skipped.
     * All of these should result in the same derived key.
     */
    soter_kdf_context_buf_t single_context[] = {{zrtp_context, zrtp_context_length}};
    soter_kdf_context_buf_t padded_context[] = {{zrtp_context, zrtp_context_length}, {NULL, 0}};
    soter_kdf_context_buf_t split_context[] = {{zrtp_context, 2},
                                               {NULL, 0},
                                               {&zrtp_context[2], zrtp_context_length - 2}};
    uint8_t key_buffer_single[32];
    uint8_t key_buffer_padded[32];
    uint8_t key_buffer_split[32];
    const struct zrtp_kdf_test_vector* test = &zrtp_kdf_test_vectors[ARRAY_SIZE(zrtp_kdf_test_vectors) - 1];

    res = soter_kdf(zrtp_key,
                    zrtp_key_length,
                    zrtp_label,
                    single_context,
                    ARRAY_SIZE(single_context),
                    key_buffer_single,
                    test->output_length);
    testsuite_fail_unless(res == SOTER_SUCCESS, "single buffer");
    testsuite_fail_if(memcmp(key_buffer_single, test->output, test->output_length),
                      "single buffer: data");

    res = soter_kdf(zrtp_key,
                    zrtp_key_length,
                    zrtp_label,
                    padded_context,
                    ARRAY_SIZE(padded_context),
                    key_buffer_padded,
                    test->output_length);
    testsuite_fail_unless(res == SOTER_SUCCESS, "NULL-padded buffer");
    testsuite_fail_if(memcmp(key_buffer_padded, test->output, test->output_length),
                      "NULL-padded buffer: data");

    res = soter_kdf(zrtp_key,
                    zrtp_key_length,
                    zrtp_label,
                    split_context,
                    ARRAY_SIZE(split_context),
                    key_buffer_split,
                    test->output_length);
    testsuite_fail_unless(res == SOTER_SUCCESS, "split multiple buffers");
    testsuite_fail_if(memcmp(key_buffer_split, test->output, test->output_length),
                      "split multiple buffers: data");
}

void run_soter_kdf_tests(void)
{
    testsuite_enter_suite("Soter KDF: PBKDF2 HMAC-SHA-256");
    testsuite_run_test(test_pbkdf2_sha256);
    testsuite_run_test(test_pbkdf2_sha256_api);
    testsuite_enter_suite("Soter KDF: NIST SP 800-108 KDF");
    testsuite_run_test(test_soter_kdf);
    testsuite_run_test(test_soter_kdf_api);
    testsuite_run_test(test_soter_kdf_skip_key);
    testsuite_run_test(test_soter_kdf_skip_context);
}
