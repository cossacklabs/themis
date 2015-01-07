/**
 * @file
 * @brief tests for Soter hash
 *
 * (c) CossackLabs
 */

#include "soter_test.h"
#include <string.h>

#define TEST_DATA_SIZE 7000

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
	soter_asym_cipher_t *import_ctx;
	soter_status_t res = soter_rand(test_data, sizeof(test_data));

	if (HERMES_SUCCESS != res)
	{
		testsuite_fail_unless(HERMES_SUCCESS == res, "generate test data");
		return;
	}

	ctx = soter_asym_cipher_create(SOTER_ASYM_CIPHER_OAEP);
	if (NULL == ctx)
	{
		testsuite_fail_if(NULL == ctx, "asym_cipher_ctx != NULL");
		return;
	}

	import_ctx = soter_asym_cipher_create(SOTER_ASYM_CIPHER_OAEP);
	if (NULL == import_ctx)
	{
		testsuite_fail_if(NULL == import_ctx, "asym_cipher_ctx != NULL");
		return;
	}

	res = soter_asym_cipher_gen_key(ctx);
	if (HERMES_SUCCESS != res)
	{
		testsuite_fail_unless(HERMES_SUCCESS == res, "soter_asym_cipher_gen_key fail");
		soter_asym_cipher_destroy(ctx);
		soter_asym_cipher_destroy(import_ctx);
		return;
	}

	res = soter_asym_cipher_encrypt(ctx, test_data, sizeof(test_data), encrypted_data, &encrypted_data_length);
	if (HERMES_SUCCESS != res)
	{
		testsuite_fail_unless(HERMES_SUCCESS == res, "soter_asym_cipher_encrypt fail");
		soter_asym_cipher_destroy(ctx);
		soter_asym_cipher_destroy(import_ctx);
		return;
	}

	/* Encrypted ciphertext for 2048 RSA key should be 256 bytes */
	testsuite_fail_unless(256 == encrypted_data_length, "RSA OAEP encryption");

	/* Export public key */
	res = soter_asym_cipher_export_key(ctx, key_data, &key_data_length, false);
	if (HERMES_SUCCESS != res)
	{
		testsuite_fail_unless(HERMES_SUCCESS == res, "soter_asym_cipher_export_key fail");
		soter_asym_cipher_destroy(ctx);
		soter_asym_cipher_destroy(import_ctx);
		return;
	}

	/* Import public key */
	res = soter_asym_cipher_import_key(import_ctx, key_data, key_data_length);
	if (HERMES_SUCCESS != res)
	{
		testsuite_fail_unless(HERMES_SUCCESS == res, "soter_asym_cipher_import_key fail");
		soter_asym_cipher_destroy(ctx);
		soter_asym_cipher_destroy(import_ctx);
		return;
	}

	res = soter_asym_cipher_encrypt(import_ctx, test_data, sizeof(test_data), encrypted_data_import_ctx, &encrypted_data_length_import_ctx);
	if (HERMES_SUCCESS != res)
	{
		testsuite_fail_unless(HERMES_SUCCESS == res, "soter_asym_cipher_encrypt fail");
		soter_asym_cipher_destroy(ctx);
		soter_asym_cipher_destroy(import_ctx);
		return;
	}

	testsuite_fail_unless(encrypted_data_length_import_ctx == encrypted_data_length, "RSA OAEP encryption with imported key");

	res = soter_asym_cipher_decrypt(ctx, encrypted_data, encrypted_data_length, decrypted_data, &decrypted_data_length);
	if (HERMES_SUCCESS != res)
	{
		testsuite_fail_unless(HERMES_SUCCESS == res, "soter_asym_cipher_decrypt fail");
		soter_asym_cipher_destroy(ctx);
		soter_asym_cipher_destroy(import_ctx);
		return;
	}

	testsuite_fail_unless((sizeof(test_data) == decrypted_data_length) && !(memcmp(test_data, decrypted_data, sizeof(test_data))), "RSA OAEP decryption");

	key_data_length = sizeof(key_data);

	/* Export private key */
	res = soter_asym_cipher_export_key(ctx, key_data, &key_data_length, true);
	if (HERMES_SUCCESS != res)
	{
		testsuite_fail_unless(HERMES_SUCCESS == res, "soter_asym_cipher_export_key fail");
		soter_asym_cipher_destroy(ctx);
		soter_asym_cipher_destroy(import_ctx);
		return;
	}

	/* Import private key */
	res = soter_asym_cipher_import_key(import_ctx, key_data, key_data_length);
	if (HERMES_SUCCESS != res)
	{
		testsuite_fail_unless(HERMES_SUCCESS == res, "soter_asym_cipher_import_key fail");
		soter_asym_cipher_destroy(ctx);
		soter_asym_cipher_destroy(import_ctx);
		return;
	}

	res = soter_asym_cipher_decrypt(import_ctx, encrypted_data_import_ctx, encrypted_data_length_import_ctx, decrypted_data_import_ctx, &decrypted_data_length_import_ctx);
	if (HERMES_SUCCESS != res)
	{
		testsuite_fail_unless(HERMES_SUCCESS == res, "soter_asym_cipher_decrypt fail");
		soter_asym_cipher_destroy(ctx);
		soter_asym_cipher_destroy(import_ctx);
		return;
	}

	testsuite_fail_unless((sizeof(test_data) == decrypted_data_length_import_ctx) && !(memcmp(test_data, decrypted_data_import_ctx, sizeof(test_data))), "RSA OAEP decryption with imported key");

	res = soter_asym_cipher_destroy(ctx);
	if (HERMES_SUCCESS != res)
	{
		testsuite_fail_unless(HERMES_SUCCESS == res, "soter_asym_cipher_destroy fail");
		return;
	}

	res = soter_asym_cipher_destroy(import_ctx);
	if (HERMES_SUCCESS != res)
	{
		testsuite_fail_unless(HERMES_SUCCESS == res, "soter_asym_cipher_destroy fail");
		return;
	}

}

void test_api(void)
{
	soter_status_t res;
	soter_asym_cipher_t ctx;

	uint8_t test_data[TEST_DATA_SIZE];
	uint8_t encrypted_data[4096];
	uint8_t decrypted_data[sizeof(encrypted_data)];
	size_t encrypted_data_length = sizeof(encrypted_data);
	size_t decrypted_data_length = sizeof(decrypted_data);

	uint8_t key_data[8192];
	size_t key_data_length = sizeof(key_data);

	memset(&ctx, 0, sizeof(soter_asym_cipher_t));

	testsuite_fail_unless(HERMES_INVALID_PARAMETER == soter_asym_cipher_init(NULL, SOTER_ASYM_CIPHER_OAEP), "soter_asym_cipher_init: invalid context");
	testsuite_fail_unless(HERMES_INVALID_PARAMETER == soter_asym_cipher_init(&ctx, (soter_asym_cipher_padding_t)0xffffffff), "soter_asym_cipher_init: invalid algorithm type");
	testsuite_fail_unless(NULL == soter_asym_cipher_create((soter_asym_cipher_padding_t)0xffffffff), "soter_asym_cipher_create: invalid algorithm type");

	res = soter_asym_cipher_init(&ctx, SOTER_ASYM_CIPHER_OAEP);
	if (HERMES_SUCCESS != res)
	{
		testsuite_fail_unless(HERMES_SUCCESS == res, "soter_asym_cipher_init fail");
		return;
	}

	testsuite_fail_unless(HERMES_INVALID_PARAMETER == soter_asym_cipher_gen_key(NULL), "soter_asym_cipher_gen_key: invalid context");

	res = soter_asym_cipher_gen_key(&ctx);
	if (HERMES_SUCCESS != res)
	{
		testsuite_fail_unless(HERMES_SUCCESS == res, "soter_asym_cipher_gen_key fail");
		return;
	}

	res = soter_rand(test_data, sizeof(test_data));
	if (HERMES_SUCCESS != res)
	{
		testsuite_fail_unless(HERMES_SUCCESS == res, "generate test data");
		return;
	}

	testsuite_fail_unless(HERMES_INVALID_PARAMETER == soter_asym_cipher_encrypt(NULL, test_data, sizeof(test_data), encrypted_data, &encrypted_data_length), "soter_asym_cipher_encrypt: invalid context");
	testsuite_fail_unless(HERMES_INVALID_PARAMETER == soter_asym_cipher_encrypt(&ctx, NULL, sizeof(test_data), encrypted_data, &encrypted_data_length), "soter_asym_cipher_encrypt: invalid input data");
	testsuite_fail_unless(HERMES_INVALID_PARAMETER == soter_asym_cipher_encrypt(&ctx, test_data, 0, encrypted_data, &encrypted_data_length), "soter_asym_cipher_encrypt: invalid input data length");
	testsuite_fail_unless(HERMES_INVALID_PARAMETER == soter_asym_cipher_encrypt(&ctx, test_data, sizeof(test_data), encrypted_data, NULL), "soter_asym_cipher_encrypt: invalid output data length");

	encrypted_data_length = 0;
	res = soter_asym_cipher_encrypt(&ctx, test_data, sizeof(test_data), NULL, &encrypted_data_length);
	testsuite_fail_unless((HERMES_BUFFER_TOO_SMALL == res) && (encrypted_data_length > 0), "soter_asym_cipher_encrypt: get output size (NULL out buffer)");

	encrypted_data_length--;
	res = soter_asym_cipher_encrypt(&ctx, test_data, sizeof(test_data), encrypted_data, &encrypted_data_length);
	testsuite_fail_unless((HERMES_BUFFER_TOO_SMALL == res) && (encrypted_data_length > 0), "soter_asym_cipher_encrypt: get output size (small out buffer)");

	res = soter_asym_cipher_encrypt(&ctx, test_data, sizeof(test_data), encrypted_data, &encrypted_data_length);
	if (HERMES_SUCCESS != res)
	{
		testsuite_fail_unless(HERMES_SUCCESS == res, "soter_asym_cipher_encrypt fail");
		return;
	}

	testsuite_fail_unless(HERMES_INVALID_PARAMETER == soter_asym_cipher_decrypt(NULL, encrypted_data, encrypted_data_length, decrypted_data, &decrypted_data_length), "soter_asym_cipher_decrypt: invalid context");
	testsuite_fail_unless(HERMES_INVALID_PARAMETER == soter_asym_cipher_decrypt(&ctx, NULL, encrypted_data_length, decrypted_data, &decrypted_data_length), "soter_asym_cipher_decrypt: invalid input data");
	testsuite_fail_unless(HERMES_INVALID_PARAMETER == soter_asym_cipher_decrypt(&ctx, encrypted_data, 0, decrypted_data, &decrypted_data_length), "soter_asym_cipher_decrypt: invalid input data length");
	testsuite_fail_unless(HERMES_INVALID_PARAMETER == soter_asym_cipher_decrypt(&ctx, encrypted_data, encrypted_data_length, decrypted_data, NULL), "soter_asym_cipher_decrypt: invalid output data length");

	decrypted_data_length = 0;
	res = soter_asym_cipher_decrypt(&ctx, encrypted_data, encrypted_data_length, NULL, &decrypted_data_length);
	testsuite_fail_unless((HERMES_BUFFER_TOO_SMALL == res) && (decrypted_data_length > 0), "soter_asym_cipher_decrypt: get output size (NULL out buffer)");

	decrypted_data_length = 0;
	res = soter_asym_cipher_encrypt(&ctx, encrypted_data, encrypted_data_length, decrypted_data, &decrypted_data_length);
	testsuite_fail_unless((HERMES_BUFFER_TOO_SMALL == res) && (decrypted_data_length > 0), "soter_asym_cipher_decrypt: get output size (small out buffer)");

	res = soter_asym_cipher_decrypt(&ctx, encrypted_data, encrypted_data_length, decrypted_data, &decrypted_data_length);
	if (HERMES_SUCCESS != res)
	{
		testsuite_fail_unless(HERMES_SUCCESS == res, "soter_asym_cipher_decrypt fail");
		return;
	}

	testsuite_fail_if((sizeof(test_data) != decrypted_data_length) || (memcmp(test_data, decrypted_data, sizeof(test_data))), "soter_asym_cipher: normal value");

	testsuite_fail_unless(HERMES_INVALID_PARAMETER == soter_asym_cipher_export_key(NULL, key_data, &key_data_length, false), "soter_asym_cipher_export_key: invalid context");
	testsuite_fail_unless(HERMES_INVALID_PARAMETER == soter_asym_cipher_export_key(&ctx, key_data, NULL, false), "soter_asym_cipher_export_key: invalid output data length");

	key_data_length = 0;
	res = soter_asym_cipher_export_key(&ctx, NULL, &key_data_length, false);
	testsuite_fail_unless((HERMES_BUFFER_TOO_SMALL == res) && (key_data_length > 0), "soter_asym_cipher_export_key: get output size (NULL out buffer)");

	key_data_length--;
	res = soter_asym_cipher_export_key(&ctx, key_data, &key_data_length, false);
	testsuite_fail_unless((HERMES_BUFFER_TOO_SMALL == res) && (key_data_length > 0), "soter_asym_cipher_export_key: get output size (small out buffer)");

	res = soter_asym_cipher_export_key(&ctx, key_data, &key_data_length, false);
	if (HERMES_SUCCESS != res)
	{
		testsuite_fail_unless(HERMES_SUCCESS == res, "soter_asym_cipher_export_key fail");
		return;
	}

	testsuite_fail_unless(HERMES_INVALID_PARAMETER == soter_asym_cipher_import_key(NULL, key_data, key_data_length), "soter_asym_cipher_import_key: invalid context");
	testsuite_fail_unless(HERMES_INVALID_PARAMETER == soter_asym_cipher_import_key(&ctx, NULL, key_data_length), "soter_asym_cipher_import_key: invalid input data");
	testsuite_fail_unless(HERMES_INVALID_PARAMETER == soter_asym_cipher_import_key(&ctx, key_data, 0), "soter_asym_cipher_import_key: invalid input data length");

	res = soter_asym_cipher_import_key(&ctx, key_data, key_data_length);
	if (HERMES_SUCCESS != res)
	{
		testsuite_fail_unless(HERMES_SUCCESS == res, "soter_asym_cipher_import_key fail");
		return;
	}

	testsuite_fail_unless(HERMES_INVALID_PARAMETER == soter_asym_cipher_cleanup(NULL), "soter_asym_cipher_cleanup: invalid context");

	res = soter_asym_cipher_cleanup(&ctx);
	if (HERMES_SUCCESS != res)
	{
		testsuite_fail_unless(HERMES_SUCCESS == res, "soter_asym_cipher_cleanup fail");
		return;
	}

	testsuite_fail_unless(HERMES_INVALID_PARAMETER == soter_asym_cipher_destroy(NULL), "soter_asym_cipher_destroy: invalid context");
}

void run_soter_asym_cipher_tests(void)
{
	testsuite_enter_suite("soter asym cipher: basic flow");
	testsuite_run_test(test_basic_encryption_flow);

	testsuite_enter_suite("soter asym cipher: api");
	testsuite_run_test(test_api);
}
