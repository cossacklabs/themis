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
    char* key;
    char* data;
    char* hmac_sha256;
    char* hmac_sha512;
};

/* Taken from RFC 4231 */
static test_vector_t vectors[] =
    {{"0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b",
      "4869205468657265",
      "b0344c61d8db38535ca8afceaf0bf12b881dc200c9833da726e9376c2e32cff7",
      "87aa7cdea5ef619d4ff0b4241a1d6cb02379f4e2ce4ec2787ad0b30545e17cdedaa833b7d6b8a702038b274eaea3f4e4be9d914eeb61f1702e696c203a126854"},
     {"4a656665",
      "7768617420646f2079612077616e7420666f72206e6f7468696e673f",
      "5bdcc146bf60754e6a042426089575c75a003f089d2739839dec58b964ec3843",
      "164b7a7bfcf819e2e395fbe73b56e0a387bd64222e831fd610270cd7ea2505549758bf75c05a994a6d034f65f8f0e6fdcaeab1a34d4a6b4b636e070a38bce737"},
     {"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
      "dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd",
      "773ea91e36800e46854db8ebd09181a72959098b3ef8c122d9635514ced565fe",
      "fa73b0089d56a284efb0f0756c890be9b1b5dbdd8ee81a3655f83e33b2279d39bf3e848279a722c806b485a47e67c807b946a337bee8942674278859e13292fb"},
     {"0102030405060708090a0b0c0d0e0f10111213141516171819",
      "cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd",
      "82558a389a443c0ea4cc819899f2083a85f0faa3e578f8077a2e3ff46729665b",
      "b0ba465637458c6990e5a8c5f61d4af7e576d97ff94b872de76f8050361ee3dba91ca5c11aa25eb4d679275cc5788063a5f19741120c4f2de2adebeb10a298dd"},
     {"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
      "54657374205573696e67204c6172676572205468616e20426c6f636b2d53697a65204b6579202d2048617368204b6579204669727374",
      "60e431591ee0b67f0d8a26aacbf5b77f8e0bc6213728c5140546040f0ee37f54",
      "80b24263c7c1a3ebb71493c1dd7be8b49b46d1f41b4aeec1121b013783f8f3526b56d037e05f2598bd0fd2215d6a1e5295e64f73f63f0aec8b915a985d786598"},
     {"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
      "5468697320697320612074657374207573696e672061206c6172676572207468616e20626c6f636b2d73697a65206b657920616e642061206c6172676572207468616e20626c6f636b2d73697a6520646174612e20546865206b6579206e6565647320746f20626520686173686564206265666f7265206265696e6720757365642062792074686520484d414320616c676f726974686d2e",
      "9b09ffa71b942fcb27635fbcd5b0e944bfdc63644f0713938a7f51535c3a35e2",
      "e37b6a775dc87dbaa4dfa9f96e5e3ffddebd71f8867289865df5a32d20cdc944b6022cac3c4982b10d5eeb55c3e4de15134676fb6de0446065c97440fa8c6a58"}};

#define MAX_TEST_DATA 2048
#define MAX_TEST_KEY 256
#define HMAC_SIZE 64

static void test_known_values(void)
{
    soter_hmac_ctx_t* ctx;
    uint8_t data[MAX_TEST_DATA];
    uint8_t key[MAX_TEST_KEY];
    uint8_t hmac[HMAC_SIZE], result[HMAC_SIZE];
    size_t i, data_len, key_len, hmac_len = sizeof(hmac);
    test_utils_status_t res;

    for (i = 0; i < (sizeof(vectors) / sizeof(test_vector_t)); i++) {
        data_len = strlen(vectors[i].data) / 2;
        if (data_len > MAX_TEST_DATA) {
            testsuite_fail_if(data_len > MAX_TEST_DATA, "data_len > MAX_TEST_DATA");
            continue;
        }

        key_len = strlen(vectors[i].key) / 2;
        if (key_len > MAX_TEST_KEY) {
            testsuite_fail_if(key_len > MAX_TEST_KEY, "key_len > MAX_TEST_KEY");
            continue;
        }

        res = string_to_bytes(vectors[i].data, data, sizeof(data));
        if (res) {
            testsuite_fail_if(res, "data read fail");
            continue;
        }

        res = string_to_bytes(vectors[i].key, key, sizeof(key));
        if (res) {
            testsuite_fail_if(res, "key read fail");
            continue;
        }

        res = string_to_bytes(vectors[i].hmac_sha256, result, sizeof(result));
        if (res) {
            testsuite_fail_if(res, "result read fail");
            continue;
        }

        ctx = soter_hmac_create(SOTER_HASH_SHA256, key, key_len);
        if (!ctx) {
            testsuite_fail_if(NULL == ctx, "hash_ctx != NULL");
            continue;
        }

        res = soter_hmac_update(ctx, data, data_len);
        if (res) {
            testsuite_fail_if(res, "soter_hmac_update fail");
            soter_hmac_destroy(ctx);
            continue;
        }

        res = soter_hmac_final(ctx, hmac, &hmac_len);
        if (res) {
            testsuite_fail_if(res, "soter_hmac_final fail");
            soter_hmac_destroy(ctx);
            continue;
        }

        soter_hmac_destroy(ctx);

        testsuite_fail_if(memcmp(hmac, result, hmac_len), "hmac == know value");

        hmac_len = sizeof(hmac);

        res = string_to_bytes(vectors[i].hmac_sha512, result, sizeof(result));
        if (res) {
            testsuite_fail_if(res, "result read fail");
            continue;
        }

        ctx = soter_hmac_create(SOTER_HASH_SHA512, key, key_len);
        if (!ctx) {
            testsuite_fail_if(NULL == ctx, "hash_ctx != NULL");
            continue;
        }

        res = soter_hmac_update(ctx, data, data_len);
        if (res) {
            testsuite_fail_if(res, "soter_hmac_update fail");
            soter_hmac_destroy(ctx);
            continue;
        }

        res = soter_hmac_final(ctx, hmac, &hmac_len);
        if (res) {
            testsuite_fail_if(res, "soter_hmac_final fail");
            soter_hmac_destroy(ctx);
            continue;
        }

        soter_hmac_destroy(ctx);

        testsuite_fail_if(memcmp(hmac, result, hmac_len), "hmac == know value");
    }
}

static void test_api_(soter_hmac_ctx_t* ctx, uint8_t* data, uint8_t* key)
{
    soter_status_t res;
    uint8_t hmac[HMAC_SIZE], result[HMAC_SIZE];
    size_t data_len, key_len, hmac_len = sizeof(hmac);
    test_utils_status_t util_res;

    data_len = strlen(vectors[4].data) / 2;
    if (data_len > MAX_TEST_DATA) {
        testsuite_fail_if(data_len > MAX_TEST_DATA, "data_len > MAX_TEST_DATA");
        return;
    }

    util_res = string_to_bytes(vectors[4].data, data, MAX_TEST_DATA);
    if (util_res) {
        testsuite_fail_if(util_res, "data read fail");
        return;
    }

    key_len = strlen(vectors[4].key) / 2;
    if (key_len > MAX_TEST_KEY) {
        testsuite_fail_if(key_len > MAX_TEST_KEY, "key_len > MAX_TEST_KEY");
        return;
    }

    testsuite_fail_unless(SOTER_INVALID_PARAMETER
                              == soter_hmac_init(NULL, SOTER_HASH_SHA256, key, key_len),
                          "soter_hmac_init: invalid context");
    testsuite_fail_unless(SOTER_INVALID_PARAMETER
                              == soter_hmac_init(ctx, (soter_hash_algo_t)0xffffffff, key, key_len),
                          "soter_hmac_init: invalid algorithm type");
    testsuite_fail_unless(SOTER_INVALID_PARAMETER
                              == soter_hmac_init(ctx, SOTER_HASH_SHA256, NULL, key_len),
                          "soter_hmac_init: invalid key");
    testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_hmac_init(ctx, SOTER_HASH_SHA256, key, 0),
                          "soter_hmac_init: invalid key length");
    testsuite_fail_unless(NULL == soter_hmac_create((soter_hash_algo_t)0xffffffff, key, key_len),
                          "soter_hmac_create: invalid algorithm type");
    testsuite_fail_unless(NULL == soter_hmac_create(SOTER_HASH_SHA256, NULL, key_len),
                          "soter_hmac_create: invalid key");
    testsuite_fail_unless(NULL == soter_hmac_create(SOTER_HASH_SHA256, key, 0),
                          "soter_hmac_create: invalid key length");

    res = soter_hmac_init(ctx, SOTER_HASH_SHA256, key, key_len);
    if (SOTER_SUCCESS != res) {
        testsuite_fail_if(SOTER_SUCCESS != res, "soter_hmac_init failed");
        return;
    }

    testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_hmac_update(NULL, data, data_len),
                          "soter_hmac_update: invalid context");
    testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_hmac_update(ctx, NULL, data_len),
                          "soter_hmac_update: invalid data");

    res = soter_hmac_update(ctx, data, data_len);
    if (SOTER_SUCCESS != res) {
        testsuite_fail_if(SOTER_SUCCESS != res, "soter_hmac_update failed");
        return;
    }

    testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_hmac_final(NULL, hmac, &hmac_len),
                          "soter_hmac_final: invalid context");
    testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_hmac_final(ctx, hmac, NULL),
                          "soter_hmac_final: invalid size pointer");

    res = soter_hmac_final(ctx, NULL, &hmac_len);
    testsuite_fail_unless((SOTER_BUFFER_TOO_SMALL == res) && (32 == hmac_len),
                          "soter_hmac_final: get output size (NULL out buffer)");

    hmac_len--;
    res = soter_hmac_final(ctx, hmac, &hmac_len);
    testsuite_fail_unless((SOTER_BUFFER_TOO_SMALL == res) && (32 == hmac_len),
                          "soter_hmac_final: get output size (small out buffer)");

    util_res = string_to_bytes(vectors[4].hmac_sha256, result, sizeof(result));
    if (util_res) {
        testsuite_fail_if(util_res, "result read fail");
        return;
    }

    res = soter_hmac_final(ctx, hmac, &hmac_len);
    testsuite_fail_unless((SOTER_SUCCESS == res) && (32 == hmac_len) && !memcmp(hmac, result, hmac_len),
                          "soter_hmac_final: normal value");

    testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_hmac_cleanup(NULL),
                          "soter_hmac_cleanup: invalid context");
    testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_hmac_destroy(NULL),
                          "soter_hash_destroy: invalid context");
    testsuite_fail_unless(SOTER_SUCCESS == soter_hmac_cleanup(ctx), "soter_hmac_cleanup: can't cleanup");
}

static void test_api(void)
{
    uint8_t data[MAX_TEST_DATA];
    uint8_t key[MAX_TEST_KEY];

    test_utils_status_t util_res = string_to_bytes(vectors[4].key, key, sizeof(key));
    if (util_res) {
        testsuite_fail_if(util_res, "key read fail");
        return;
    }

    soter_hmac_ctx_t* ctx = soter_hmac_create(SOTER_HASH_SHA256, key, HMAC_SIZE);
    if (!ctx) {
        testsuite_fail_if(!ctx, "soter_hmac_create: can't create");
        return;
    }
    soter_status_t res = soter_hmac_cleanup(ctx);
    if (SOTER_SUCCESS != res) {
        testsuite_fail_if(true, "soter_hmac_cleanup: error");
        return;
    }
    test_api_(ctx, &data[0], &key[0]);
    testsuite_fail_unless(SOTER_SUCCESS == soter_hmac_destroy(ctx), "soter_hmac_destroy: can't destroy");
}

static void test_api_stack_struct(void)
{
    uint8_t data[MAX_TEST_DATA];
    uint8_t key[MAX_TEST_KEY];

    test_utils_status_t util_res = string_to_bytes(vectors[4].key, key, sizeof(key));
    if (util_res) {
        testsuite_fail_if(util_res, "key read fail");
        return;
    }

    soter_hmac_ctx_t ctx;
    test_api_(&ctx, &data[0], &key[0]);
}

void run_soter_hmac_tests(void)
{
    testsuite_enter_suite("soter hmac: known values");
    testsuite_run_test(test_known_values);

    testsuite_enter_suite("soter hmac: api");
    testsuite_run_test(test_api);

    testsuite_enter_suite("soter hmac: api with stack initialization");
    testsuite_run_test(test_api_stack_struct);
}
