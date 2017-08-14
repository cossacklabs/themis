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
#include <string.h>

#define KEY_BUFFER_SIZE 2048
#define SHARED_SECRET_BUFFER_SIZE 1024

static void test_basic_ka_flow(void)
{

  uint8_t peer1_private_key_data[8192];
  size_t peer1_private_key_data_length = sizeof(peer1_private_key_data);
  uint8_t peer1_public_key_data[8192];
  size_t peer1_public_key_data_length = sizeof(peer1_public_key_data);

  uint8_t peer2_private_key_data[8192];
  size_t peer2_private_key_data_length = sizeof(peer2_private_key_data);
  uint8_t peer2_public_key_data[8192];
  size_t peer2_public_key_data_length = sizeof(peer2_public_key_data);

  soter_status_t res;

  res = soter_key_pair_gen(SOTER_ASYM_KA_DEFAULT_ALG, peer1_private_key_data, &peer1_private_key_data_length, peer1_public_key_data, &peer1_public_key_data_length);
  if(res!=SOTER_SUCCESS){
    testsuite_fail_if(res!=SOTER_SUCCESS, " generation peer1 key pair");
    return;
  }

  res = soter_key_pair_gen(SOTER_ASYM_KA_DEFAULT_ALG, peer2_private_key_data, &peer2_private_key_data_length, peer2_public_key_data, &peer2_public_key_data_length);
  if(res!=SOTER_SUCCESS){
    testsuite_fail_if(res!=SOTER_SUCCESS, " generation peer2 key pair");
    return;
  }

  uint8_t peer1_shared_secret[SHARED_SECRET_BUFFER_SIZE];
  size_t peer1_shared_secret_length = sizeof(peer1_shared_secret);
  
  uint8_t peer2_shared_secret[SHARED_SECRET_BUFFER_SIZE];
  size_t peer2_shared_secret_length = sizeof(peer2_shared_secret);
  
  soter_asym_ka_t *peer1 = soter_asym_ka_create(peer1_private_key_data, peer1_private_key_data_length);
  soter_asym_ka_t *peer2 = soter_asym_ka_create(peer2_private_key_data, peer2_private_key_data_length);
  if((!peer1) || (!peer2)){
    testsuite_fail_unless(false, "soter_asym_ka_object creation fail");
    goto err;
  }

  res = soter_asym_ka_derive(peer1, peer2_public_key_data, peer2_public_key_data_length, peer1_shared_secret, &peer1_shared_secret_length);
  if (SOTER_SUCCESS != res){
    testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_ka_derive fail");
    goto err;
  }

  res = soter_asym_ka_derive(peer2, peer1_public_key_data, peer1_public_key_data_length , peer2_shared_secret, &peer2_shared_secret_length);
  if (SOTER_SUCCESS != res){
    testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_ka_derive fail");
    goto err;
  }

  testsuite_fail_unless((peer1_shared_secret_length == peer2_shared_secret_length) && !memcmp(peer1_shared_secret, peer2_shared_secret, peer1_shared_secret_length), "Basic ECDH");

err:
  if (peer1){
    res = soter_asym_ka_destroy(peer1);
    if (SOTER_SUCCESS != res){
        testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_ka_destroy fail");
      }
  }
  
  if (peer2){
    res = soter_asym_ka_destroy(peer2);
    if (SOTER_SUCCESS != res){
        testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_ka_destroy fail");
      }
  }
}

static void test_ka_api(alg)
{
  soter_status_t res;
  
  uint8_t peer1_private_key_data[8192];
  size_t peer1_private_key_data_length = sizeof(peer1_private_key_data);
  uint8_t peer1_public_key_data[8192];
  size_t peer1_public_key_data_length = sizeof(peer1_public_key_data);
  
  uint8_t peer2_private_key_data[8192];
  size_t peer2_private_key_data_length = sizeof(peer2_private_key_data);
  uint8_t peer2_public_key_data[8192];
  size_t peer2_public_key_data_length = sizeof(peer2_public_key_data);
  
  res = soter_key_pair_gen(alg, peer1_private_key_data, &peer1_private_key_data_length, peer1_public_key_data, &peer1_public_key_data_length);
  if(res!=SOTER_SUCCESS){
    testsuite_fail_if(res!=SOTER_SUCCESS, " generation peer1 key pair");
    return;
  }
  
  res = soter_key_pair_gen(alg, peer2_private_key_data, &peer2_private_key_data_length, peer2_public_key_data, &peer2_public_key_data_length);
  if(res!=SOTER_SUCCESS){
    testsuite_fail_if(res!=SOTER_SUCCESS, " generation peer2 key pair");
    return;
  }
  
  soter_asym_ka_t *ctx=NULL;
  
  uint8_t shared_secret[SHARED_SECRET_BUFFER_SIZE];
  size_t shared_secret_length = sizeof(shared_secret);
  
  testsuite_fail_unless(NULL == soter_asym_ka_create(NULL, peer1_private_key_data_length), "soter_asym_ka_create: invalid key");
  testsuite_fail_unless(NULL == soter_asym_ka_create(peer1_private_key_data, 0), "soter_asym_ka_create: invalid key length");

  ctx = soter_asym_ka_create(peer1_private_key_data, peer1_private_key_data_length);
  if (!ctx){
    testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_ka_init fail");
    return;
  }

  testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_asym_ka_derive(NULL, peer1_public_key_data, peer1_public_key_data_length, shared_secret, &shared_secret_length), "soter_asym_ka_derive: invalid context");
  testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_asym_ka_derive(ctx, NULL, peer1_public_key_data_length , shared_secret, &shared_secret_length), "soter_asym_ka_derive: invalid input data");
  testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_asym_ka_derive(ctx, peer1_public_key_data, 0, shared_secret, &shared_secret_length), "soter_asym_ka_derive: invalid input data length");
  testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_asym_ka_derive(ctx, peer1_public_key_data, peer1_public_key_data_length, shared_secret, NULL), "soter_asym_ka_derive: invalid output data length");

  shared_secret_length = 0;
  res = soter_asym_ka_derive(ctx, peer1_public_key_data, peer1_public_key_data_length, NULL, &shared_secret_length);
  testsuite_fail_unless((SOTER_BUFFER_TOO_SMALL == res) && (shared_secret_length > 0), "soter_asym_ka_derive: get output size (NULL out buffer)");

  shared_secret_length--;
  res = soter_asym_ka_derive(ctx, peer1_public_key_data, peer1_public_key_data_length, shared_secret, &shared_secret_length);
  testsuite_fail_unless((SOTER_BUFFER_TOO_SMALL == res) && (shared_secret_length > 0), "soter_asym_ka_derive: get output size (small out buffer)");
  
  res = soter_asym_ka_derive(ctx, peer1_public_key_data, peer1_public_key_data_length, shared_secret, &shared_secret_length);
  if (SOTER_SUCCESS != res){
    testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_ka_derive fail");
    return;
  }

  testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_asym_ka_destroy(NULL), "soter_asym_ka_cleanup: invalid context");

  res = soter_asym_ka_destroy(ctx);
  if (SOTER_SUCCESS != res){
    testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_ka_cleanup fail");
    return;
  }
}

void test_ka_api_all()
{
  test_ka_api(SOTER_ASYM_KA_DEFAULT_ALG);  
#if  defined (OPENSSL) || defined (LIBRESSL)
  test_ka_api(SOTER_ASYM_EC|SOTER_ASYM_EC_LENGTH_256);
  test_ka_api(SOTER_ASYM_EC|SOTER_ASYM_EC_LENGTH_384);
  test_ka_api(SOTER_ASYM_EC|SOTER_ASYM_EC_LENGTH_521);
#endif
}

void run_soter_asym_ka_tests(void)
{
	testsuite_enter_suite("soter asym ka: basic flow");
	testsuite_run_test(test_basic_ka_flow);

	testsuite_enter_suite("soter asym ka: api");
	testsuite_run_test(test_ka_api_all);
}
