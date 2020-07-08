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

#include "soter/soter_test.h"

#include <string.h>

typedef struct test_vector_type test_vector_t;

struct test_vector_type {
    char* input;
    char* result;
};

/* Taken from NIST test suite */
static test_vector_t vectors[] =
    {{"", "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"},
     {"74cb9381d89f5aa73368", "73d6fad1caaa75b43b21733561fd3958bdc555194a037c2addec19dc2d7a52bd"},
     {"09fc1accc230a205e4a208e64a8f204291f581a12756392da4b8c0cf5ef02b95",
      "4f44c1c7fbebb6f9601829f3897bfd650c56fa07844be76489076356ac1886a4"},
     {"5a86b737eaea8ee976a0a24da63e7ed7eefad18a101c1211e2b3650c5187c2a8a650547208251f6d4237e661c7bf4c77f335390394c37fa1a9f9be836ac28509",
      "42e61e174fbb3897d6dd6cef3dd2802fe67b331953b06114a65c772859dfc1aa"},
     {"451101250ec6f26652249d59dc974b7361d571a8101cdfd36aba3b5854d3ae086b5fdd4597721b66e3c0dc5d8c606d9657d0e323283a5217d1f53f2f284f57b85c8a61ac8924711f895c5ed90ef17745ed2d728abd22a5f7a13479a462d71b56c19a74a40b655c58edfe0a188ad2cf46cbf30524f65d423c837dd1ff2bf462ac4198007345bb44dbb7b1c861298cdf61982a833afc728fae1eda2f87aa2c9480858bec",
      "3c593aa539fdcdae516cdf2f15000f6634185c88f505b39775fb9ab137a10aa2"}};

#define MAX_TEST_INPUT 2048

static void test_known_values(void)
{
    soter_hash_ctx_t* ctx;
    uint8_t input[MAX_TEST_INPUT];
    uint8_t hash[32], result[32]; /* TODO: define this and also consider whole SHA-2, not only SHA-256 */
    size_t i, input_len, hash_len = sizeof(hash);
    test_utils_status_t res;

    for (i = 0; i < (sizeof(vectors) / sizeof(test_vector_t)); i++) {
        input_len = strlen(vectors[i].input) / 2;
        if (input_len > MAX_TEST_INPUT) {
            testsuite_fail_if(input_len > MAX_TEST_INPUT, "input_len > MAX_TEST_INPUT");
            continue;
        }

        res = string_to_bytes(vectors[i].input, input, sizeof(input));
        if (res) {
            testsuite_fail_if(res, "input read fail");
            continue;
        }

        res = string_to_bytes(vectors[i].result, result, sizeof(result));
        if (res) {
            testsuite_fail_if(res, "result read fail");
            continue;
        }

        ctx = soter_hash_create(SOTER_HASH_SHA256);
        if (!ctx) {
            testsuite_fail_if(NULL == ctx, "hash_ctx != NULL");
            continue;
        }

        res = soter_hash_update(ctx, input, input_len);
        if (res) {
            testsuite_fail_if(res, "soter_hash_update fail");
            soter_hash_destroy(ctx);
            continue;
        }

        res = soter_hash_final(ctx, hash, &hash_len);
        if (res) {
            testsuite_fail_if(res, "soter_hash_final fail");
            soter_hash_destroy(ctx);
            continue;
        }

        soter_hash_destroy(ctx);

        testsuite_fail_if((hash_len != sizeof(result)) || (memcmp(hash, result, hash_len) != 0),
                          "hash == know value");
    }
}

static void test_api_(soter_hash_ctx_t* ctx)
{
    soter_status_t res;
    uint8_t input[MAX_TEST_INPUT];
    uint8_t hash[32], result[32]; /* TODO: define this and also consider whole SHA-2, not only SHA-256 */
    size_t input_len, hash_len = sizeof(hash);
    test_utils_status_t util_res;

    testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_hash_init(NULL, SOTER_HASH_SHA256),
                          "soter_hash_init: invalid context");
    testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_hash_init(ctx, (soter_hash_algo_t)0xffffffff),
                          "soter_hash_init: invalid algorithm type");
    testsuite_fail_unless(NULL == soter_hash_create((soter_hash_algo_t)0xffffffff),
                          "soter_hash_create: invalid algorithm type");

    input_len = strlen(vectors[4].input) / 2;
    if (input_len > MAX_TEST_INPUT) {
        testsuite_fail_if(input_len > MAX_TEST_INPUT, "input_len > MAX_TEST_INPUT");
        return;
    }

    util_res = string_to_bytes(vectors[4].input, input, sizeof(input));
    if (util_res) {
        testsuite_fail_if(util_res, "input read fail");
        return;
    }

    res = soter_hash_init(ctx, SOTER_HASH_SHA256);
    if (SOTER_SUCCESS != res) {
        testsuite_fail_if(SOTER_SUCCESS != res, "soter_hash_init failed");
        return;
    }

    testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_hash_update(NULL, input, input_len),
                          "soter_hash_update: invalid context");
    testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_hash_update(ctx, NULL, input_len),
                          "soter_hash_update: invalid data");

    res = soter_hash_update(ctx, input, input_len);
    if (SOTER_SUCCESS != res) {
        testsuite_fail_if(SOTER_SUCCESS != res, "soter_hash_update failed");
        return;
    }

    testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_hash_final(NULL, hash, &hash_len),
                          "soter_hash_final: invalid context");
    testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_hash_final(ctx, hash, NULL),
                          "soter_hash_final: invalid size pointer");

    res = soter_hash_final(ctx, NULL, &hash_len);
    testsuite_fail_unless((SOTER_BUFFER_TOO_SMALL == res) && (32 == hash_len),
                          "soter_hash_final: get output size (NULL out buffer)");

    hash_len--;
    res = soter_hash_final(ctx, hash, &hash_len);
    testsuite_fail_unless((SOTER_BUFFER_TOO_SMALL == res) && (32 == hash_len),
                          "soter_hash_final: get output size (small out buffer)");

    util_res = string_to_bytes(vectors[4].result, result, sizeof(result));
    if (util_res) {
        testsuite_fail_if(util_res, "result read fail");
        return;
    }

    res = soter_hash_final(ctx, hash, &hash_len);
    testsuite_fail_unless((SOTER_SUCCESS == res) && (32 == hash_len) && !memcmp(hash, result, hash_len),
                          "soter_hash_final: normal value");
    testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_hash_update(ctx, input, input_len),
                          "soter_hash_update: use after final");
    testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_hash_destroy(NULL),
                          "soter_hash_destroy: invalid context");
    testsuite_fail_unless(SOTER_SUCCESS == soter_hash_cleanup(ctx), "soter_hash_cleanup: can't cleanup");
}

static void test_api(void)
{
    soter_hash_ctx_t* ctx = soter_hash_create(SOTER_HASH_SHA256);
    if (!ctx) {
        testsuite_fail_if(true, "soter_hash_create failed");
        return;
    }
    soter_status_t res = soter_hash_cleanup(ctx);
    if (SOTER_SUCCESS != res) {
        testsuite_fail_if(SOTER_SUCCESS != res, "soter_hash_cleanup failed");
        return;
    }
    test_api_(ctx);
    testsuite_fail_unless(SOTER_SUCCESS == soter_hash_destroy(ctx), "soter_hash_destroy: can't destroy");
}

static void test_api_stack_struct(void)
{
    soter_hash_ctx_t ctx;
    test_api_(&ctx);
}

void run_soter_hash_tests(void)
{
    testsuite_enter_suite("soter hash: known values");
    testsuite_run_test(test_known_values);

    testsuite_enter_suite("soter hash: api");
    testsuite_run_test(test_api);

    testsuite_enter_suite("soter hash: stack struct");
    testsuite_run_test(test_api_stack_struct);
}
