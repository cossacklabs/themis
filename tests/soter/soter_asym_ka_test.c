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

#define KEY_BUFFER_SIZE 2048
#define SHARED_SECRET_BUFFER_SIZE 128

static void test_basic_ka_flow(void)
{
    uint8_t key_buffer[KEY_BUFFER_SIZE];
    size_t key_buffer_length = sizeof(key_buffer);

    uint8_t peer1_shared_secret[SHARED_SECRET_BUFFER_SIZE];
    size_t peer1_shared_secret_length = sizeof(peer1_shared_secret);

    uint8_t peer2_shared_secret[SHARED_SECRET_BUFFER_SIZE];
    size_t peer2_shared_secret_length = sizeof(peer2_shared_secret);

    uint8_t peer2_private[SHARED_SECRET_BUFFER_SIZE];
    size_t peer2_private_length = sizeof(peer2_private);

    soter_status_t res;
    soter_asym_ka_t* peer1 = soter_asym_ka_create(SOTER_ASYM_KA_EC_P256);
    soter_asym_ka_t* peer2 = soter_asym_ka_create(SOTER_ASYM_KA_EC_P256);

    if ((!peer1) || (!peer2)) {
        goto err;
    }

    res = soter_asym_ka_gen_key(peer1);
    if (SOTER_SUCCESS != res) {
        testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_ka_gen_key fail");
        goto err;
    }

    res = soter_asym_ka_gen_key(peer2);
    if (SOTER_SUCCESS != res) {
        testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_ka_gen_key fail");
        goto err;
    }

    res = soter_asym_ka_export_key(peer2, key_buffer, &key_buffer_length, false);
    if (SOTER_SUCCESS != res) {
        testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_ka_export_key fail");
        goto err;
    }

    res = soter_asym_ka_derive(peer1,
                               key_buffer,
                               key_buffer_length,
                               peer1_shared_secret,
                               &peer1_shared_secret_length);
    if (SOTER_SUCCESS != res) {
        testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_ka_derive fail");
        goto err;
    }

    key_buffer_length = sizeof(key_buffer);

    res = soter_asym_ka_export_key(peer1, key_buffer, &key_buffer_length, false);
    if (SOTER_SUCCESS != res) {
        testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_ka_export_key fail");
        goto err;
    }

    res = soter_asym_ka_derive(peer2,
                               key_buffer,
                               key_buffer_length,
                               peer2_shared_secret,
                               &peer2_shared_secret_length);
    if (SOTER_SUCCESS != res) {
        testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_ka_derive fail");
        goto err;
    }

    testsuite_fail_unless((peer1_shared_secret_length == peer2_shared_secret_length)
                              && !memcmp(peer1_shared_secret, peer2_shared_secret, peer1_shared_secret_length),
                          "Basic ECDH");

    res = soter_asym_ka_export_key(peer2, peer2_private, &peer2_private_length, true);
    if (SOTER_SUCCESS != res) {
        testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_ka_export_key fail");
        goto err;
    }

    res = soter_asym_ka_destroy(peer2);
    if (SOTER_SUCCESS != res) {
        testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_ka_destroy fail");
        goto err;
    }

    peer2 = soter_asym_ka_create(SOTER_ASYM_KA_EC_P256);
    if (!peer2) {
        testsuite_fail_if(NULL == peer2, "soter_asym_ka_create fail");
        goto err;
    }

    res = soter_asym_ka_import_key(peer2, peer2_private, peer2_private_length);
    if (SOTER_SUCCESS != res) {
        testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_ka_import_key fail");
        goto err;
    }

    peer2_shared_secret_length = sizeof(peer2_shared_secret);

    res = soter_asym_ka_derive(peer2,
                               key_buffer,
                               key_buffer_length,
                               peer2_shared_secret,
                               &peer2_shared_secret_length);
    if (SOTER_SUCCESS != res) {
        testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_ka_derive fail");
        goto err;
    }

    testsuite_fail_unless((peer1_shared_secret_length == peer2_shared_secret_length)
                              && !memcmp(peer1_shared_secret, peer2_shared_secret, peer1_shared_secret_length),
                          "Basic ECDH with imported key");

err:
    if (peer1) {
        res = soter_asym_ka_destroy(peer1);
        if (SOTER_SUCCESS != res) {
            testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_ka_destroy fail");
        }
    }

    if (peer2) {
        res = soter_asym_ka_destroy(peer2);
        if (SOTER_SUCCESS != res) {
            testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_ka_destroy fail");
        }
    }
}

static void test_api(void)
{
    soter_status_t res;
    soter_asym_ka_t ctx;

    uint8_t key_buffer[KEY_BUFFER_SIZE];
    size_t key_buffer_length = sizeof(key_buffer);

    uint8_t shared_secret[SHARED_SECRET_BUFFER_SIZE];
    size_t shared_secret_length = sizeof(shared_secret);

    memset(&ctx, 0, sizeof(soter_asym_ka_t));

    testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_asym_ka_init(NULL, SOTER_ASYM_KA_EC_P256),
                          "soter_asym_ka_init: invalid context");
    testsuite_fail_unless(SOTER_INVALID_PARAMETER
                              == soter_asym_ka_init(&ctx, (soter_asym_ka_alg_t)0xffffffff),
                          "soter_asym_ka_init: invalid algorithm type");
    testsuite_fail_unless(NULL == soter_asym_ka_create((soter_asym_ka_alg_t)0xffffffff),
                          "soter_asym_ka_create: invalid algorithm type");

    res = soter_asym_ka_init(&ctx, SOTER_ASYM_KA_EC_P256);
    if (SOTER_SUCCESS != res) {
        testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_ka_init fail");
        return;
    }

    testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_asym_ka_gen_key(NULL),
                          "soter_asym_ka_gen_key: invalid context");

    res = soter_asym_ka_gen_key(&ctx);
    if (SOTER_SUCCESS != res) {
        testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_ka_gen_key fail");
        return;
    }

    testsuite_fail_unless(SOTER_INVALID_PARAMETER
                              == soter_asym_ka_export_key(NULL, key_buffer, &key_buffer_length, true),
                          "soter_asym_ka_export_key: invalid context");
    testsuite_fail_unless(SOTER_INVALID_PARAMETER
                              == soter_asym_ka_export_key(&ctx, key_buffer, NULL, false),
                          "soter_asym_ka_export_key: invalid output data length");

    key_buffer_length = 0;
    res = soter_asym_ka_export_key(&ctx, NULL, &key_buffer_length, false);
    testsuite_fail_unless((SOTER_BUFFER_TOO_SMALL == res) && (key_buffer_length > 0),
                          "soter_asym_ka_export_key: get output size (NULL out buffer)");

    key_buffer_length--;
    res = soter_asym_ka_export_key(&ctx, key_buffer, &key_buffer_length, false);
    testsuite_fail_unless((SOTER_BUFFER_TOO_SMALL == res) && (key_buffer_length > 0),
                          "soter_asym_ka_export_key: get output size (small out buffer)");

    res = soter_asym_ka_export_key(&ctx, key_buffer, &key_buffer_length, false);
    if (SOTER_SUCCESS != res) {
        testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_ka_export_key fail");
        return;
    }

    testsuite_fail_unless(SOTER_INVALID_PARAMETER
                              == soter_asym_ka_import_key(NULL, key_buffer, key_buffer_length),
                          "soter_asym_ka_import_key: invalid context");
    testsuite_fail_unless(SOTER_INVALID_PARAMETER
                              == soter_asym_ka_import_key(&ctx, NULL, key_buffer_length),
                          "soter_asym_ka_import_key: invalid input data");
    testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_asym_ka_import_key(NULL, key_buffer, 0),
                          "soter_asym_ka_import_key: invalid input data length");

    testsuite_fail_unless(SOTER_INVALID_PARAMETER
                              == soter_asym_ka_derive(NULL,
                                                      key_buffer,
                                                      key_buffer_length,
                                                      shared_secret,
                                                      &shared_secret_length),
                          "soter_asym_ka_derive: invalid context");
    testsuite_fail_unless(SOTER_INVALID_PARAMETER
                              == soter_asym_ka_derive(&ctx,
                                                      NULL,
                                                      key_buffer_length,
                                                      shared_secret,
                                                      &shared_secret_length),
                          "soter_asym_ka_derive: invalid input data");
    testsuite_fail_unless(SOTER_INVALID_PARAMETER
                              == soter_asym_ka_derive(&ctx, key_buffer, 0, shared_secret, &shared_secret_length),
                          "soter_asym_ka_derive: invalid input data length");
    testsuite_fail_unless(SOTER_INVALID_PARAMETER
                              == soter_asym_ka_derive(&ctx, key_buffer, key_buffer_length, shared_secret, NULL),
                          "soter_asym_ka_derive: invalid output data length");

    shared_secret_length = 0;
    res = soter_asym_ka_derive(&ctx, key_buffer, key_buffer_length, NULL, &shared_secret_length);
    testsuite_fail_unless((SOTER_BUFFER_TOO_SMALL == res) && (shared_secret_length > 0),
                          "soter_asym_ka_derive: get output size (NULL out buffer)");

    shared_secret_length--;
    res = soter_asym_ka_derive(&ctx, key_buffer, key_buffer_length, shared_secret, &shared_secret_length);
    testsuite_fail_unless((SOTER_BUFFER_TOO_SMALL == res) && (shared_secret_length > 0),
                          "soter_asym_ka_derive: get output size (small out buffer)");

    res = soter_asym_ka_derive(&ctx, key_buffer, key_buffer_length, shared_secret, &shared_secret_length);
    if (SOTER_SUCCESS != res) {
        testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_ka_derive fail");
        return;
    }

    testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_asym_ka_cleanup(NULL),
                          "soter_asym_ka_cleanup: invalid context");

    res = soter_asym_ka_cleanup(&ctx);
    if (SOTER_SUCCESS != res) {
        testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_ka_cleanup fail");
        return;
    }

    testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_asym_ka_destroy(NULL),
                          "soter_asym_ka_destroy: invalid context");
}

static void test_ec_key_size_quirk(void)
{
    soter_status_t res;
    soter_asym_ka_t ctx;

    uint8_t public_key_buffer[KEY_BUFFER_SIZE];
    uint8_t private_key_buffer[KEY_BUFFER_SIZE];
    size_t public_key_buffer_length = sizeof(public_key_buffer);
    size_t private_key_buffer_length = sizeof(private_key_buffer);

    memset(&ctx, 0, sizeof(soter_asym_ka_t));
    memset(&public_key_buffer, 0xFF, sizeof(public_key_buffer));
    memset(&private_key_buffer, 0xFF, sizeof(private_key_buffer));

    res = soter_asym_ka_init(&ctx, SOTER_ASYM_KA_EC_P256);
    if (res != SOTER_SUCCESS) {
        testsuite_fail_unless(false, "soter_asym_ka_init failed");
        return;
    }

    res = soter_asym_ka_gen_key(&ctx);
    if (res != SOTER_SUCCESS) {
        testsuite_fail_unless(false, "soter_asym_ka_gen_key failed");
        return;
    }

    res = soter_asym_ka_export_key(&ctx, public_key_buffer, &public_key_buffer_length, false);
    if (res != SOTER_SUCCESS) {
        testsuite_fail_unless(false, "soter_asym_ka_export_key failed");
        return;
    }

    res = soter_asym_ka_export_key(&ctx, private_key_buffer, &private_key_buffer_length, true);
    if (res != SOTER_SUCCESS) {
        testsuite_fail_unless(false, "soter_asym_ka_export_key failed");
        return;
    }

    res = soter_asym_ka_cleanup(&ctx);
    if (res != SOTER_SUCCESS) {
        testsuite_fail_unless(false, "soter_asym_ka_cleanup failed");
        return;
    }

    /*
     * Due to historical reasons, private keys have the same length as public
     * ones, with the most significant byte (the first one in network order)
     * always being set to zero. We have to maintain this format to preserve
     * backwards compatibility.
     */
    testsuite_fail_unless(private_key_buffer_length == public_key_buffer_length,
                          "private key has the same length as public key");

    testsuite_fail_unless(soter_container_const_data((const void*)private_key_buffer)[0] == 0,
                          "first byte of private key is zero");
}

void run_soter_asym_ka_tests(void)
{
    testsuite_enter_suite("soter asym ka: basic flow");
    testsuite_run_test(test_basic_ka_flow);

    testsuite_enter_suite("soter asym ka: api");
    testsuite_run_test(test_api);

    testsuite_enter_suite("soter asym ka: EC key size quirks");
    testsuite_run_test(test_ec_key_size_quirk);
}
