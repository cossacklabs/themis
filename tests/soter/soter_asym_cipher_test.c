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

	uint8_t key_data[8192];
	size_t key_data_length = sizeof(key_data);

	soter_asym_cipher_t *ctx;
	soter_asym_cipher_t *decrypt_ctx;
	soter_status_t res = soter_rand(test_data, sizeof(test_data));

	if (SOTER_SUCCESS != res)
	{
		testsuite_fail_unless(SOTER_SUCCESS == res, "generate test data");
		return;
	}

	soter_rsa_key_pair_gen_t* key_pair_ctx=soter_rsa_key_pair_gen_create(RSA_KEY_LENGTH_1024);
	if(!key_pair_ctx){
		testsuite_fail_if(!key_pair_ctx, "generate key test data");
		return;
	}
	
	res = soter_rsa_key_pair_gen_export_key(key_pair_ctx, key_data, &key_data_length, false);
	if(res!=SOTER_SUCCESS){
		testsuite_fail_if(res!=SOTER_SUCCESS, "export generated public key");
		soter_rsa_key_pair_gen_destroy(key_pair_ctx);
		soter_status_t soter_asym_cipher_cleanup(soter_asym_cipher_t* asym_cipher);
		return;
	}

	ctx = soter_asym_cipher_create(key_data, key_data_length, SOTER_ASYM_CIPHER_OAEP);
	if (NULL == ctx)
	{
		testsuite_fail_if(NULL == ctx, "asym_cipher_ctx != NULL");
		soter_rsa_key_pair_gen_destroy(key_pair_ctx);
		return;
	}

	res = soter_asym_cipher_encrypt(ctx, test_data, sizeof(test_data), encrypted_data, &encrypted_data_length);
	if (SOTER_SUCCESS != res)
	{
		testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_cipher_encrypt fail");
		soter_asym_cipher_destroy(ctx);
		soter_asym_cipher_destroy(decrypt_ctx);
		return;
	}

	/* Encrypted ciphertext for 2048 RSA key should be 256 bytes */
	testsuite_fail_unless(128 == encrypted_data_length, "RSA OAEP encryption");
	key_data_length = sizeof(key_data);
	res = soter_rsa_key_pair_gen_export_key(key_pair_ctx, key_data, &key_data_length, true);
	printf("%i\n", res);
	if(res!=SOTER_SUCCESS){
		testsuite_fail_if(res!=SOTER_SUCCESS, "export generated private key");
		soter_status_t soter_asym_cipher_cleanup(soter_asym_cipher_t* asym_cipher);
		soter_rsa_key_pair_gen_destroy(key_pair_ctx);
		return;
	}

	decrypt_ctx = soter_asym_cipher_create(key_data, key_data_length, SOTER_ASYM_CIPHER_OAEP);
	if (NULL == decrypt_ctx)
	{
		testsuite_fail_if(NULL == decrypt_ctx, "asym_cipher_ctx != NULL");
		soter_asym_cipher_destroy(ctx);
		soter_rsa_key_pair_gen_destroy(key_pair_ctx);
		return;
	}

	res = soter_asym_cipher_decrypt(decrypt_ctx, encrypted_data, encrypted_data_length, decrypted_data, &decrypted_data_length);
	if (SOTER_SUCCESS != res)
	{
		testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_cipher_decrypt fail");
		soter_asym_cipher_destroy(ctx);
		soter_asym_cipher_destroy(decrypt_ctx);
		soter_rsa_key_pair_gen_destroy(key_pair_ctx);
		return;
	}

	testsuite_fail_unless((sizeof(test_data) == decrypted_data_length) && !(memcmp(test_data, decrypted_data, sizeof(test_data))), "RSA OAEP decryption");

	key_data_length = sizeof(key_data);

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
	res = soter_rsa_key_pair_gen_destroy(key_pair_ctx);
	if (SOTER_SUCCESS != res)
	{
		testsuite_fail_unless(SOTER_SUCCESS == res, "soter_rsa_key_pair_gen_destroy fail");
		return;
	}
}

void test_api(int key_length)
{
	soter_status_t res;
	soter_asym_cipher_t ctx, decrypt_ctx;

	uint8_t test_data[TEST_DATA_SIZE];
	uint8_t encrypted_data[4096];
	uint8_t decrypted_data[sizeof(encrypted_data)];
	size_t encrypted_data_length = sizeof(encrypted_data);
	size_t decrypted_data_length = sizeof(decrypted_data);

	uint8_t key_data[8192];
	size_t key_data_length = sizeof(key_data);

	memset(&ctx, 0, sizeof(soter_asym_cipher_t));

	soter_rsa_key_pair_gen_t* key_pair_ctx=soter_rsa_key_pair_gen_create(key_length);
	if(!key_pair_ctx){
		testsuite_fail_if(!key_pair_ctx, "generate key test data");
		return;
	}
	

	res = soter_rand(test_data, sizeof(test_data));
	if (SOTER_SUCCESS != res)
	{
		testsuite_fail_unless(SOTER_SUCCESS == res, "generate test data");
		return;
	}

	testsuite_fail_unless(SOTER_SUCCESS==soter_rsa_key_pair_gen_export_key(key_pair_ctx, key_data, &key_data_length, false), "export private_key failed");	
	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_asym_cipher_init(NULL, key_data, key_data_length, SOTER_ASYM_CIPHER_OAEP), "soter_asym_cipher_init: invalid context");
	testsuite_fail_unless(SOTER_FAIL == soter_asym_cipher_init(&ctx, NULL, key_data_length, SOTER_ASYM_CIPHER_OAEP), "soter_asym_cipher_init: invalid key");
	testsuite_fail_unless(SOTER_FAIL == soter_asym_cipher_init(&ctx, key_data, 0, SOTER_ASYM_CIPHER_OAEP), "soter_asym_cipher_init: invalid key length");
	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_asym_cipher_init(&ctx, key_data, key_data_length, (soter_asym_cipher_padding_t)0xffffffff), "soter_asym_cipher_init: invalid algorithm type");
	testsuite_fail_unless(NULL == soter_asym_cipher_create(key_data, key_data_length, (soter_asym_cipher_padding_t)0xffffffff), "soter_asym_cipher_create: invalid algorithm type");

	res = soter_asym_cipher_init(&ctx, key_data, key_data_length, SOTER_ASYM_CIPHER_OAEP);
	if (SOTER_SUCCESS != res)
	{
		testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_cipher_init fail");
		return;
	}
	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_asym_cipher_encrypt(NULL, test_data, sizeof(test_data), encrypted_data, &encrypted_data_length), "soter_asym_cipher_encrypt: invalid context");
	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_asym_cipher_encrypt(&ctx, NULL, sizeof(test_data), encrypted_data, &encrypted_data_length), "soter_asym_cipher_encrypt: invalid input data");
	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_asym_cipher_encrypt(&ctx, test_data, 0, encrypted_data, &encrypted_data_length), "soter_asym_cipher_encrypt: invalid input data length");
	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_asym_cipher_encrypt(&ctx, test_data, sizeof(test_data), encrypted_data, NULL), "soter_asym_cipher_encrypt: invalid output data length");

	encrypted_data_length = 0;
	res = soter_asym_cipher_encrypt(&ctx, test_data, sizeof(test_data), NULL, &encrypted_data_length);
	testsuite_fail_unless((SOTER_BUFFER_TOO_SMALL == res) && (encrypted_data_length > 0), "soter_asym_cipher_encrypt: get output size (NULL out buffer)");

	encrypted_data_length--;
	res = soter_asym_cipher_encrypt(&ctx, test_data, sizeof(test_data), encrypted_data, &encrypted_data_length);
	testsuite_fail_unless((SOTER_BUFFER_TOO_SMALL == res) && (encrypted_data_length > 0), "soter_asym_cipher_encrypt: get output size (small out buffer)");

	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_asym_cipher_encrypt(&ctx, test_data, 2048, encrypted_data, &encrypted_data_length), "soter_asym_cipher_encrypt: plaintext too large");

	res = soter_asym_cipher_encrypt(&ctx, test_data, sizeof(test_data), encrypted_data, &encrypted_data_length);
	if (SOTER_SUCCESS != res)
	{
		testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_cipher_encrypt fail");
		return;
	}


	key_data_length = sizeof(key_data);
	testsuite_fail_unless(SOTER_SUCCESS==soter_rsa_key_pair_gen_export_key(key_pair_ctx, key_data, &key_data_length, true), "export private_key failed");
	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_asym_cipher_init(NULL, key_data, key_data_length, SOTER_ASYM_CIPHER_OAEP), "soter_asym_cipher_init: invalid context");
	testsuite_fail_unless(SOTER_FAIL == soter_asym_cipher_init(&decrypt_ctx, NULL, key_data_length, SOTER_ASYM_CIPHER_OAEP), "soter_asym_cipher_init: invalid key");
	testsuite_fail_unless(SOTER_FAIL == soter_asym_cipher_init(&decrypt_ctx, key_data, 0, SOTER_ASYM_CIPHER_OAEP), "soter_asym_cipher_init: invalid key length");
	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_asym_cipher_init(&decrypt_ctx, key_data, key_data_length, (soter_asym_cipher_padding_t)0xffffffff), "soter_asym_cipher_init: invalid algorithm type");
	testsuite_fail_unless(NULL == soter_asym_cipher_create(key_data, key_data_length, (soter_asym_cipher_padding_t)0xffffffff), "soter_asym_cipher_create: invalid algorithm type");

	res = soter_asym_cipher_init(&decrypt_ctx, key_data, key_data_length, SOTER_ASYM_CIPHER_OAEP);
	if (SOTER_SUCCESS != res)
	{
		testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_cipher_init fail");
		return;
	}
	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_asym_cipher_decrypt(NULL, encrypted_data, encrypted_data_length, decrypted_data, &decrypted_data_length), "soter_asym_cipher_decrypt: invalid context");
	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_asym_cipher_decrypt(&decrypt_ctx, NULL, encrypted_data_length, decrypted_data, &decrypted_data_length), "soter_asym_cipher_decrypt: invalid input data");
	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_asym_cipher_decrypt(&decrypt_ctx, encrypted_data, 0, decrypted_data, &decrypted_data_length), "soter_asym_cipher_decrypt: invalid input data length");
	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_asym_cipher_decrypt(&decrypt_ctx, encrypted_data, encrypted_data_length, decrypted_data, NULL), "soter_asym_cipher_decrypt: invalid output data length");

	decrypted_data_length = 0;
	res = soter_asym_cipher_decrypt(&decrypt_ctx, encrypted_data, encrypted_data_length, NULL, &decrypted_data_length);
	testsuite_fail_unless((SOTER_BUFFER_TOO_SMALL == res) && (decrypted_data_length > 0), "soter_asym_cipher_decrypt: get output size (NULL out buffer)");

	decrypted_data_length = 0;
	res = soter_asym_cipher_decrypt(&decrypt_ctx, encrypted_data, encrypted_data_length, decrypted_data, &decrypted_data_length);
	testsuite_fail_unless((SOTER_BUFFER_TOO_SMALL == res) && (decrypted_data_length > 0), "soter_asym_cipher_decrypt: get output size (small out buffer)");

	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_asym_cipher_decrypt(&decrypt_ctx, encrypted_data, encrypted_data_length - 1, decrypted_data, &decrypted_data_length), "soter_asym_cipher_decrypt: ciphertext too small");

	res = soter_asym_cipher_decrypt(&decrypt_ctx, encrypted_data, encrypted_data_length, decrypted_data, &decrypted_data_length);
	if (SOTER_SUCCESS != res)
	{
		testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_cipher_decrypt fail");
		return;
	}

	testsuite_fail_if((sizeof(test_data) != decrypted_data_length) || (memcmp(test_data, decrypted_data, sizeof(test_data))), "soter_asym_cipher: normal value");


	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_asym_cipher_cleanup(NULL), "soter_asym_cipher_cleanup: invalid context");

	res = soter_asym_cipher_cleanup(&ctx);
	if (SOTER_SUCCESS != res)
	{
		testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_cipher_cleanup fail");
		return;
	}

	res = soter_asym_cipher_cleanup(&decrypt_ctx);
	if (SOTER_SUCCESS != res)
	{
		testsuite_fail_unless(SOTER_SUCCESS == res, "soter_asym_cipher_cleanup fail");
		return;
	}
	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_asym_cipher_destroy(NULL), "soter_asym_cipher_destroy: invalid context");
	soter_rsa_key_pair_gen_destroy(key_pair_ctx);
}

void test_api_all()
{
    test_api(RSA_KEY_LENGTH_1024);
    test_api(RSA_KEY_LENGTH_2048);
    test_api(RSA_KEY_LENGTH_4096);
//    test_api(RSA_KEY_LENGTH_8192);
}
void run_soter_asym_cipher_tests(void)
{
	testsuite_enter_suite("soter asym cipher: basic flow");
	testsuite_run_test(test_basic_encryption_flow);

	testsuite_enter_suite("soter asym cipher: api");
	testsuite_run_test(test_api_all);
}
