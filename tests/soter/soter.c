/**
 * @file
 * @brief tests for Soter
 *
 * (c) CossackLabs
 */

#include "common/error.h"
#include "soter/soter.h"
#include <stdio.h>

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
	uint8_t *bytes = NULL;
	size_t length = 4096;

	if (HERMES_SUCCESS == soter_rand(&bytes, length))
	{
		print_bytes(bytes, length);
		free(bytes);
	}
}

static void test_hash(void)
{
  uint8_t *bytes = "Test message";
  size_t length=13;
  uint8_t *hash=NULL;
  size_t hash_length;
  soter_hash_ctx_t* hash_ctx;
  hash_ctx=soter_hash_create(SOTER_HASH_SHA256);
  if(!hash_ctx)
    {
      HERMES_ERROR_OUT("can`t create hash_context");
      return;
    }
  if(soter_hash_update(hash_ctx, bytes, length)!=HERMES_SUCCESS)
    {
      HERMES_ERROR_OUT("can`t update hash_context");
      return;
    }
  if(soter_hash_final(hash_ctx, &hash, &hash_length))
    {
      HERMES_ERROR_OUT("can`t final hash_context");
      return;
    }
  if(soter_hash_destroy(hash_ctx)!=HERMES_SUCCESS)
    {
      HERMES_ERROR_OUT("can`t destroy hash_context");
      return;
    }
  fprintf(stdout,"soter hash_context success:\n");
  print_bytes(hash,hash_length);
  free(hash);
}

static void test_sym(void)
{
  char key[]="password";
  char salt[]="salt";
  const unsigned stack_block_size=1024;
  char stack_block[stack_block_size];
  if(soter_sym_create((soter_sym_ctx_t*)stack_block, &stack_block_size, SOTER_AES_CTR_PBKDF2_ENCRYPT, key, strlen(key), salt, strlen(salt))!=HERMES_SUCCESS)
    return HERMES_FAIL;
  
}

int main()
{
	test_rand();
	test_hash();
    return 0;
}
