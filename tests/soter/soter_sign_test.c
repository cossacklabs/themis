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

#include <string.h>
#include "soter_test.h"

#define MAX_TEST_DATA 2048
#define MAX_TEST_KEY MAX_TEST_DATA

static int sign_test(soter_sign_alg_t alg)
{
  char test_data[]="test message";
  size_t test_data_length=strlen(test_data);
  
  soter_sign_ctx_t* ctx=NULL;

  uint8_t* signature=NULL;
  size_t signature_length=0;

  ctx=soter_sign_create(alg,NULL,0,NULL,0);
  if(!ctx){
    testsuite_fail_if(!ctx, "soter_sign_ctx_t == NULL");
    return -1;
  }

  uint8_t private_key[8192];
  size_t private_key_length=sizeof(private_key);

  uint8_t public_key[8192];
  size_t public_key_length=sizeof(public_key);

  soter_status_t res;
  res=soter_sign_export_key(ctx, private_key, &private_key_length, true);
  if(res!=SOTER_SUCCESS){
    testsuite_fail_if(res!=SOTER_SUCCESS,"soter_sign_export_key (private key) fail");
    soter_sign_destroy(ctx);
    return -6;
  }

  res=soter_sign_export_key(ctx, public_key, &public_key_length, false);
  if(res!=SOTER_SUCCESS){
    testsuite_fail_if(res!=SOTER_SUCCESS,"soter_sign_export_key (public key) fail");
    soter_sign_destroy(ctx);
    return -7;
  }

  res=soter_sign_destroy(ctx);
  if(res!=SOTER_SUCCESS){
    testsuite_fail_if(res!=SOTER_SUCCESS,"soter_sign_destroy fail");
    return -8;
  }
  
  soter_sign_ctx_t* sctx=NULL;

  sctx=soter_sign_create(alg,private_key,private_key_length,NULL,0);
  if(!sctx){
    testsuite_fail_if(!sctx, "soter_sign_ctx_t == NULL 2");
    return -1;
  }

  res=soter_sign_update(sctx, test_data, test_data_length);
  if(res!=SOTER_SUCCESS){
    testsuite_fail_if(res!=SOTER_SUCCESS, "soter_sign_update fail");
    soter_sign_destroy(sctx);
    return -2;
  }

  res=soter_sign_final(sctx, signature, &signature_length);
  if(res!=SOTER_BUFFER_TOO_SMALL){
    testsuite_fail_if(res!=SOTER_BUFFER_TOO_SMALL, "soter_sign_final (signature length determine) fail");
    soter_sign_destroy(sctx);
    return -3;
  }

  signature=malloc(signature_length);
  if(!signature){
    testsuite_fail_if(!signature, "out of memory");
    soter_sign_destroy(ctx);
    return -4;
  }

  res=soter_sign_final(sctx, signature, &signature_length);
  if(res!=SOTER_SUCCESS){
    testsuite_fail_if(res!=SOTER_SUCCESS, "soter_sign_final fail");
    soter_sign_destroy(sctx);
    return -5;
  }
  res=soter_sign_destroy(sctx);
  if(res!=SOTER_SUCCESS){
    testsuite_fail_if(res!=SOTER_SUCCESS,"soter_sign_destroy fail");
    return -8;
  }
  
  soter_verify_ctx_t* vctx=NULL;

  vctx=soter_verify_create(alg, NULL, 0, public_key, public_key_length);
  if(!vctx){
    testsuite_fail_if(!vctx, "soter_verify_ctx_t == NULL");
    return -9;
  }

  res=soter_verify_update(vctx, test_data, test_data_length);
  if(res!=SOTER_SUCCESS){
    testsuite_fail_if(res!=SOTER_SUCCESS, "soter_verify_update fail");
    soter_verify_destroy(vctx);
    return -10;
  }

  res=soter_verify_final(vctx, signature, signature_length);
  if(res!=SOTER_SUCCESS){
    testsuite_fail_if(res!=SOTER_SUCCESS, "soter_verify_final fail");
    soter_verify_destroy(vctx);
    return -11;
  }

  res=soter_verify_destroy(vctx);
  if(res!=SOTER_SUCCESS){
    testsuite_fail_if(res!=SOTER_SUCCESS,"soter_sign_destroy fail");
    return -12;
  }
  free(signature);
  return 0;
}

static void soter_sign_test()
{
  testsuite_fail_if(sign_test(SOTER_SIGN_rsa_pss_pkcs8),"soter sign SOTER_SIGN_rsa_pss_pkcs8");
  testsuite_fail_if(sign_test(SOTER_SIGN_ecdsa_none_pkcs8),"soter sign SOTER_SIGN_ecdsa_none_pkcs8");
}

static void soter_sign_api_test()
{
	uint8_t priv[MAX_TEST_KEY];
	size_t priv_length = sizeof(priv);

	uint8_t pub[MAX_TEST_KEY];
	size_t pub_length = sizeof(pub);

	uint8_t message[MAX_TEST_DATA];
	size_t message_length = rand_int(MAX_TEST_DATA);

	uint8_t signature[MAX_TEST_DATA];
	size_t signature_length = sizeof(signature);

	soter_status_t res;

	if (soter_rand(message, message_length))
	{
		testsuite_fail_if(true, "soter_rand failed");
		return;
	}

	soter_sign_ctx_t *sign_ctx = soter_sign_create(SOTER_SIGN_ecdsa_none_pkcs8, NULL, 0, NULL, 0);
	if (!sign_ctx)
	{
		testsuite_fail_if(true, "soter_sign_create failed");
		return;
	}

	res = soter_sign_export_key(sign_ctx, priv, &priv_length, true);
	if (SOTER_SUCCESS != res)
	{
		testsuite_fail_if(true, "soter_sign_export_key failed");
		soter_sign_destroy(sign_ctx);
		return;
	}

	res = soter_sign_export_key(sign_ctx, pub, &pub_length, false);
	if (SOTER_SUCCESS != res)
	{
		testsuite_fail_if(true, "soter_sign_export_key failed");
		soter_sign_destroy(sign_ctx);
		return;
	}

	soter_sign_destroy(sign_ctx);

	sign_ctx = soter_sign_create((soter_sign_alg_t)-1, priv, priv_length, NULL, 0);
	testsuite_fail_if(sign_ctx, "soter_sign_create: invalid algorithm");

	sign_ctx = soter_sign_create(SOTER_SIGN_ecdsa_none_pkcs8, priv, priv_length - 1, NULL, 0);
	testsuite_fail_if(sign_ctx, "soter_sign_create: invalid private key length");

	sign_ctx = soter_sign_create(SOTER_SIGN_ecdsa_none_pkcs8, priv, priv_length, NULL, 0);
	if (!sign_ctx)
	{
		testsuite_fail_if(true, "soter_sign_create failed");
		return;
	}
	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_sign_update(NULL, message, message_length), "soter_sign_update: invalid context");
	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_sign_update(sign_ctx, NULL, message_length), "soter_sign_update: invalid message");
	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_sign_update(sign_ctx, message, 0), "soter_sign_update: invalid message length");

	res = soter_sign_update(sign_ctx, message, message_length);
	if (SOTER_SUCCESS != res)
	{
		testsuite_fail_if(true, "soter_sign_update failed");
		soter_sign_destroy(sign_ctx);
		return;
	}

	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_sign_final(NULL, signature, &signature_length), "soter_sign_final: invalid context");
	testsuite_fail_unless(SOTER_BUFFER_TOO_SMALL == soter_sign_final(sign_ctx, NULL, &signature_length), "soter_sign_final: get output size (NULL out buffer)");
	signature_length--;
	testsuite_fail_unless(SOTER_BUFFER_TOO_SMALL == soter_sign_final(sign_ctx, signature, &signature_length), "soter_sign_final: get output size (small out buffer)");

	res = soter_sign_final(sign_ctx, signature, &signature_length);
	if (SOTER_SUCCESS != res)
	{
		testsuite_fail_if(true, "soter_sign_final failed");
		soter_sign_destroy(sign_ctx);
		return;
	}
	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_sign_destroy(NULL), "soter_sign_destroy: invalid context");
	res = soter_sign_destroy(sign_ctx);
	if (SOTER_SUCCESS != res){
		testsuite_fail_if(true, "soter_sign_destroy failed");
		return;
	}

	sign_ctx = soter_verify_create((soter_sign_alg_t)-1, NULL, 0, pub, pub_length);
	testsuite_fail_if(sign_ctx, "soter_verify_create: invalid algorithm");

	sign_ctx = soter_verify_create(SOTER_SIGN_ecdsa_none_pkcs8, NULL, 0, pub, pub_length - 1);
	testsuite_fail_if(sign_ctx, "soter_verify_create: invalid public key length");

	sign_ctx = soter_verify_create(SOTER_SIGN_ecdsa_none_pkcs8, NULL, 0, pub, pub_length);
	if (!sign_ctx)
	{
		testsuite_fail_if(true, "soter_verify_create failed");
		return;
	}

	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_verify_update(NULL, message, message_length), "soter_verify_update: invalid context");
	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_verify_update(sign_ctx, NULL, message_length), "soter_verify_update: invalid message");
	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_verify_update(sign_ctx, message, 0), "soter_verify_update: invalid message length");

	res = soter_verify_update(sign_ctx, message, message_length);
	if (SOTER_SUCCESS != res)
	{
		testsuite_fail_if(true, "soter_verify_update failed");
		soter_verify_destroy(sign_ctx);
		return;
	}

	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_verify_final(NULL, signature, signature_length), "soter_verify_final: invalid context");
	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_verify_final(sign_ctx, NULL, signature_length), "soter_verify_final: invalid signature buffer");
	testsuite_fail_unless(SOTER_INVALID_SIGNATURE == soter_verify_final(sign_ctx, signature, signature_length - 1), "soter_verify_final: invalid signature length");
	signature[signature_length / 2]++;
	testsuite_fail_unless(SOTER_INVALID_SIGNATURE == soter_verify_final(sign_ctx, signature, signature_length), "soter_verify_final: invalid signature value");
	signature[signature_length / 2]--;

	testsuite_fail_unless(SOTER_SUCCESS == soter_verify_final(sign_ctx, signature, signature_length), "soter_verify_final: normal flow");

	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_verify_destroy(NULL), "soter_verify_destroy: invalid context");
	res = soter_verify_destroy(sign_ctx);
	if (SOTER_SUCCESS != res)
	{
		testsuite_fail_if(true, "soter_verify_destroy failed");
		return;
	}

	sign_ctx = soter_verify_create(SOTER_SIGN_ecdsa_none_pkcs8, NULL, 0, pub, pub_length);
	if (!sign_ctx)
	{
		testsuite_fail_if(true, "soter_verify_create failed");
		return;
	}

	message[message_length / 2]++;

	res = soter_verify_update(sign_ctx, message, message_length);
	if (SOTER_SUCCESS != res)
	{
		testsuite_fail_if(true, "soter_verify_update failed");
		soter_verify_destroy(sign_ctx);
		return;
	}

	message[message_length / 2]--;

	testsuite_fail_unless(SOTER_INVALID_SIGNATURE == soter_verify_final(sign_ctx, signature, signature_length), "soter_verify_final: wrong signed message");

	soter_verify_destroy(sign_ctx);
}

void run_soter_sign_test(){
  testsuite_enter_suite("soter sign: basic flow");
  testsuite_run_test(soter_sign_test);

  testsuite_enter_suite("soter sign: api");
  testsuite_run_test(soter_sign_api_test);
}
