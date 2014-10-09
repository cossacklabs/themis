/**
 * @file
 * @brief tests for Soter hash
 *
 * (c) CossackLabs
 */

#include "soter_test.h"
#include "common/test_utils.h"

#define TEST_DATA_SIZE 70

static void test_basic_encryption_flow(void)
{
	uint8_t test_data[TEST_DATA_SIZE];
	uint8_t encrypted_data[4096];
	uint8_t decrypted_data[sizeof(encrypted_data)];
	size_t encrypted_data_length = sizeof(encrypted_data);
	size_t decrypted_data_length = sizeof(decrypted_data);

	soter_asym_cipher_t *ctx;
	soter_status_t res = soter_rand(test_data, sizeof(test_data));

	if (HERMES_SUCCESS != res)
	{
		sput_fail_unless(HERMES_SUCCESS == res, "generate test data");
		return;
	}

	ctx = soter_asym_cipher_create(SOTER_ASYM_CIPHER_OAEP);
	if (NULL == ctx)
	{
		sput_fail_if(NULL == ctx, "asym_cipher_ctx != NULL");
		return;
	}

	res = soter_asym_cipher_gen_key(ctx);
	if (HERMES_SUCCESS != res)
	{
		sput_fail_unless(HERMES_SUCCESS == res, "soter_asym_cipher_gen_key fail");
		soter_asym_cipher_destroy(ctx);
		return;
	}

	res = soter_asym_cipher_encrypt(ctx, test_data, sizeof(test_data), encrypted_data, &encrypted_data_length);
	if (HERMES_SUCCESS != res)
	{
		sput_fail_unless(HERMES_SUCCESS == res, "soter_asym_cipher_encrypt fail");
		soter_asym_cipher_destroy(ctx);
		return;
	}

	/* Encrypted ciphertext for 2048 RSA key should be 256 bytes */
	sput_fail_unless(256 == encrypted_data_length, "RSA OAEP encryption");

	res = soter_asym_cipher_decrypt(ctx, encrypted_data, encrypted_data_length, decrypted_data, &decrypted_data_length);
	if (HERMES_SUCCESS != res)
	{
		sput_fail_unless(HERMES_SUCCESS == res, "soter_asym_cipher_decrypt fail");
		soter_asym_cipher_destroy(ctx);
		return;
	}

	sput_fail_unless((sizeof(test_data) == decrypted_data_length) && !(memcmp(test_data, decrypted_data, sizeof(test_data))), "RSA OAEP decryption");

	res = soter_asym_cipher_destroy(ctx);
	if (HERMES_SUCCESS != res)
	{
		sput_fail_unless(HERMES_SUCCESS == res, "soter_asym_cipher_destroy fail");
		return;
	}

}

void run_soter_asym_cipher_tests(void)
{
	sput_start_testing();

	sput_enter_suite("soter asym cipher: basic flow");
	sput_run_test(test_basic_encryption_flow);

	sput_finish_testing();
}
