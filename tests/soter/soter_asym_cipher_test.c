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

#include <soter/soter.h>

#define TEST_DATA_SIZE 70

static void test_basic_encryption_flow(void)
{
	uint8_t test_data[TEST_DATA_SIZE];
	uint8_t encrypted_data[4096];
	uint8_t decrypted_data[sizeof(encrypted_data)];
	size_t encrypted_data_length = sizeof(encrypted_data);
	size_t decrypted_data_length = sizeof(decrypted_data);

	uint8_t encrypted_data_import_ctx[sizeof(encrypted_data)];
	uint8_t decrypted_data_import_ctx[sizeof(encrypted_data_import_ctx)];
	size_t encrypted_data_length_import_ctx = sizeof(encrypted_data_import_ctx);
	size_t decrypted_data_length_import_ctx = sizeof(decrypted_data_import_ctx);

	uint8_t private_key_data[8192];
	size_t private_key_data_length = sizeof(private_key_data);

        uint8_t public_key_data[8192];
	size_t public_key_data_length = sizeof(public_key_data);

	soter_asym_cipher_t *ctx;
	soter_asym_cipher_t *decrypt_ctx;
	soter_status_t res = soter_rand(test_data, sizeof(test_data));

	if (SOTER_SUCCESS != res)
	{
		testsuite_fail_unless(SOTER_SUCCESS == res, "generate test data");
		return;
	}
	
        res = soter_key_pair_gen(SOTER_ASYM_CIPHER_DEFAULT_ALG, private_key_data, &private_key_data_length, public_key_data, &public_key_data_length);
	if(res!=SOTER_SUCCESS){
		testsuite_fail_if(res!=SOTER_SUCCESS, "export generated public key");
		return;
	}

	ctx = soter_asym_cipher_create(public_key_data, public_key_data_length);
	if (NULL == ctx)
	{
		testsuite_fail_if(NULL == ctx, "asym_cipher_ctx != NULL");
		return;
	}

	res = soter_asym_cipher_encrypt(ctx, test_data, sizeof(test_data), encrypted_data, &encrypted_data_length);
	if (SOTER_SUCCESS != res)
	{
		testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_cipher_encrypt fail");
		soter_asym_cipher_destroy(ctx);
		return;
	}

	/* Encrypted ciphertext for 2048 RSA key should be 256 bytes */
	testsuite_fail_unless(256 == encrypted_data_length, "RSA OAEP encryption");
        
	decrypt_ctx = soter_asym_cipher_create(private_key_data, private_key_data_length);
	if (NULL == decrypt_ctx)
	{
		testsuite_fail_if(NULL == decrypt_ctx, "asym_cipher_ctx != NULL");
		soter_asym_cipher_destroy(ctx);
		return;
	}

	res = soter_asym_cipher_decrypt(decrypt_ctx, encrypted_data, encrypted_data_length, decrypted_data, &decrypted_data_length);
	if (SOTER_SUCCESS != res)
	{
		testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_cipher_decrypt fail");
		soter_asym_cipher_destroy(ctx);
		soter_asym_cipher_destroy(decrypt_ctx);
		return;
	}

	testsuite_fail_unless((sizeof(test_data) == decrypted_data_length) && !(memcmp(test_data, decrypted_data, sizeof(test_data))), "RSA OAEP decryption");

	res = soter_asym_cipher_destroy(ctx);
	if (SOTER_SUCCESS != res)
	{
		testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_cipher_destroy fail");
		return;
	}

	res = soter_asym_cipher_destroy(decrypt_ctx);
	if (SOTER_SUCCESS != res)
	{
		testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_cipher_destroy fail");
		return;
	}
}

void test_api(int alg)
{
	soter_status_t res;
	soter_asym_cipher_t *ctx, *decrypt_ctx;

	uint8_t test_data[TEST_DATA_SIZE];
	uint8_t encrypted_data[4096];
	uint8_t decrypted_data[sizeof(encrypted_data)];
	size_t encrypted_data_length = sizeof(encrypted_data);
	size_t decrypted_data_length = sizeof(decrypted_data);

	uint8_t private_key_data[8192];
	size_t private_key_data_length = sizeof(private_key_data);
   
	uint8_t public_key_data[8192];
	size_t public_key_data_length = sizeof(public_key_data);
     

	res=soter_key_pair_gen(alg, private_key_data, &private_key_data_length, public_key_data, &public_key_data_length);
	if(SOTER_SUCCESS != res){
          testsuite_fail_if(SOTER_SUCCESS != res, "generate key test data");
          return;
	}
	res = soter_rand(test_data, sizeof(test_data));
	if (SOTER_SUCCESS != res)
	{
		testsuite_fail_unless(SOTER_SUCCESS == res, "generate test data");
		return;
	}

	testsuite_fail_unless(NULL == soter_asym_cipher_create(NULL, public_key_data_length), "soter_asym_cipher_init: invalid key");
	testsuite_fail_unless(NULL == soter_asym_cipher_create(public_key_data, 0), "soter_asym_cipher_init: invalid key length");

	ctx = soter_asym_cipher_create(public_key_data, public_key_data_length);
	if (!ctx){
		testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_cipher_create fail");
		return;
	}
	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_asym_cipher_encrypt(NULL, test_data, sizeof(test_data), encrypted_data, &encrypted_data_length), "soter_asym_cipher_encrypt: invalid context");
	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_asym_cipher_encrypt(ctx, NULL, sizeof(test_data), encrypted_data, &encrypted_data_length), "soter_asym_cipher_encrypt: invalid input data");
	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_asym_cipher_encrypt(ctx, test_data, 0, encrypted_data, &encrypted_data_length), "soter_asym_cipher_encrypt: invalid input data length");
	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_asym_cipher_encrypt(ctx, test_data, sizeof(test_data), encrypted_data, NULL), "soter_asym_cipher_encrypt: invalid output data length");

	encrypted_data_length = 0;
	res = soter_asym_cipher_encrypt(ctx, test_data, sizeof(test_data), NULL, &encrypted_data_length);
	testsuite_fail_unless((SOTER_BUFFER_TOO_SMALL == res) && (encrypted_data_length > 0), "soter_asym_cipher_encrypt: get output size (NULL out buffer)");

	encrypted_data_length--;
	res = soter_asym_cipher_encrypt(ctx, test_data, sizeof(test_data), encrypted_data, &encrypted_data_length);
	testsuite_fail_unless((SOTER_BUFFER_TOO_SMALL == res) && (encrypted_data_length > 0), "soter_asym_cipher_encrypt: get output size (small out buffer)");

	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_asym_cipher_encrypt(ctx, test_data, 2048, encrypted_data, &encrypted_data_length), "soter_asym_cipher_encrypt: plaintext too large");

	res = soter_asym_cipher_encrypt(ctx, test_data, sizeof(test_data), encrypted_data, &encrypted_data_length);
	if (SOTER_SUCCESS != res)
	{
		testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_cipher_encrypt fail");
		return;
	}

	testsuite_fail_unless(NULL == soter_asym_cipher_create(NULL, private_key_data_length), "soter_asym_cipher_init: invalid key");
	testsuite_fail_unless(NULL == soter_asym_cipher_create(private_key_data, 0), "soter_asym_cipher_init: invalid key length");

	decrypt_ctx = soter_asym_cipher_create(private_key_data, private_key_data_length);
	if (!decrypt_ctx){
		testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_cipher_create fail");
		return;
	}
	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_asym_cipher_decrypt(NULL, encrypted_data, encrypted_data_length, decrypted_data, &decrypted_data_length), "soter_asym_cipher_decrypt: invalid context");
	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_asym_cipher_decrypt(decrypt_ctx, NULL, encrypted_data_length, decrypted_data, &decrypted_data_length), "soter_asym_cipher_decrypt: invalid input data");
	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_asym_cipher_decrypt(decrypt_ctx, encrypted_data, 0, decrypted_data, &decrypted_data_length), "soter_asym_cipher_decrypt: invalid input data length");
	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_asym_cipher_decrypt(decrypt_ctx, encrypted_data, encrypted_data_length, decrypted_data, NULL), "soter_asym_cipher_decrypt: invalid output data length");

	decrypted_data_length = 0;
	res = soter_asym_cipher_decrypt(decrypt_ctx, encrypted_data, encrypted_data_length, NULL, &decrypted_data_length);
	testsuite_fail_unless((SOTER_BUFFER_TOO_SMALL == res) && (decrypted_data_length > 0), "soter_asym_cipher_decrypt: get output size (NULL out buffer)");

	decrypted_data_length = 0;
	res = soter_asym_cipher_decrypt(decrypt_ctx, encrypted_data, encrypted_data_length, decrypted_data, &decrypted_data_length);
	testsuite_fail_unless((SOTER_BUFFER_TOO_SMALL == res) && (decrypted_data_length > 0), "soter_asym_cipher_decrypt: get output size (small out buffer)");

	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_asym_cipher_decrypt(decrypt_ctx, encrypted_data, encrypted_data_length - 1, decrypted_data, &decrypted_data_length), "soter_asym_cipher_decrypt: ciphertext too small");

	res = soter_asym_cipher_decrypt(decrypt_ctx, encrypted_data, encrypted_data_length, decrypted_data, &decrypted_data_length);
	if (SOTER_SUCCESS != res)
	{
		testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_cipher_decrypt fail");
		return;
	}

	testsuite_fail_if((sizeof(test_data) != decrypted_data_length) || (memcmp(test_data, decrypted_data, sizeof(test_data))), "soter_asym_cipher: normal value");


	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_asym_cipher_destroy(NULL), "soter_asym_cipher_cleanup: invalid context");

	res = soter_asym_cipher_destroy(ctx);
	if (SOTER_SUCCESS != res)
	{
		testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_cipher_cleanup fail");
		return;
	}

	res = soter_asym_cipher_destroy(decrypt_ctx);
	if (SOTER_SUCCESS != res)
	{
		testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_cipher_cleanup fail");
		return;
	}
}

void test_api_all()
{
#if  defined (OPENSSL) || defined (LIBRESSL) || defined (BORINGSSL)
  test_api(SOTER_ASYM_CIPHER_DEFAULT_ALG);  
  test_api(SOTER_ASYM_RSA|SOTER_ASYM_RSA_LENGTH_1024);
  test_api(SOTER_ASYM_RSA|SOTER_ASYM_RSA_LENGTH_2048);
  test_api(SOTER_ASYM_RSA|SOTER_ASYM_RSA_LENGTH_4096);
#endif
}
void run_soter_asym_cipher_tests(void)
{
#if  defined (OPENSSL) || defined (LIBRESSL) || defined (BORINGSSL)
	testsuite_enter_suite("soter asym cipher: basic flow");
	testsuite_run_test(test_basic_encryption_flow);

	testsuite_enter_suite("soter asym cipher: api");
	testsuite_run_test(test_api_all);
#endif
}
