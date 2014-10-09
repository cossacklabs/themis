/**
 * @file
 * @brief tests for Soter
 *
 * (c) CossackLabs
 */

#include "soter_test.h"

static void print_bytes(const uint8_t *bytes, size_t length)
{
	size_t i;

	for (i = 0; i < length; i++)
		printf("%02x", bytes[i]);
	puts("");
}

/* For now, just generate some random bits and print them */
static void test_rand(void)
{
	uint8_t bytes[4096];
	size_t length = sizeof(bytes);

	if (HERMES_SUCCESS == soter_rand(bytes, length))
		print_bytes(bytes, length);
}

/*static void test_sym(void)
{
  char key[]="password";
  char salt[]="salt";
  const unsigned stack_block_size=1024;
  char stack_block[stack_block_size];

  char message[]="test message";
  unsigned encrypted_message_length=1024;
  char encrypted_message[1024];

  if(soter_sym_create((soter_sym_ctx_t*)stack_block, &stack_block_size, SOTER_AES_CTR_PBKDF2_ENCRYPT, key, strlen(key), salt, strlen(salt))!=HERMES_SUCCESS)
    {
      HERMES_ERROR_OUT("SOTER SYM OBJECT creation error\n");
      return;
    }
  if(soter_sym_update((soter_sym_ctx_t*)stack_block, message, strlen(message), encrypted_message, &encrypted_message_length)!=HERMES_SUCCESS)
    {
      HERMES_ERROR_OUT("SOTER SYM OBJECT update error\n");
      return;
    }
  if(soter_sym_final((soter_sym_ctx_t*)stack_block, encrypted_message+, &encrypted_message_length)!=HERMES_SUCCESS)
    {
      HERMES_ERROR_OUT("SOTER SYM OBJECT final error\n");
      return;
    }
  
  fprintf(stdout, "soter sym encrypt ");
}*/

int main()
{
	test_rand();

	run_soter_hash_tests();
	run_soter_rsa_tests();
	run_soter_asym_cipher_tests();

    return sput_get_return_value();
}
