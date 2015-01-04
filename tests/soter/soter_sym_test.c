/**
 * @file
 *
 * (c) CossackLabs
 */

#include <string.h>
#include <stdio.h>
#include "soter_test.h"

struct test_init_vector_type{
  char* key;
  uint8_t* salt;
  size_t salt_length;
};

typedef struct test_init_vector_type test_init_vector_t;

static test_init_vector_t test_init_vectors[]={
  {"test_password1234567890123456789", (uint8_t*)"1234567890123456",16}
};

struct test_data_vector_type{
  uint8_t* data;
  size_t data_length;
};

typedef struct test_data_vector_type test_data_vector_t;

static test_data_vector_t test_data_vectors[]={
  {(uint8_t*)"test message test",12}
};

size_t solo_test(const soter_sym_alg_t alg, const test_init_vector_t init_data, const uint8_t *data, const size_t data_length, uint8_t** res_data)
{
  soter_sym_ctx_t* ctx=NULL;
  test_utils_status_t res;
  uint8_t* result_data=NULL;
  size_t result_data_length=0;
  uint8_t* revers_result_data=NULL;
  size_t revers_result_length=0;
  size_t final_data_length=256;
  
  ctx=soter_sym_create(alg, init_data.key,strlen(init_data.key), init_data.salt, init_data.salt_length);
  if(!ctx){
    testsuite_fail_if(NULL==ctx, "sym_context == NULL");
    return 0;
  }
  res=soter_sym_update(ctx, data, data_length, NULL, &result_data_length);
  if((res!=HERMES_BUFFER_TOO_SMALL) || (result_data_length==0)){
    testsuite_fail_if((res!=HERMES_BUFFER_TOO_SMALL) || (result_data_length == 0), "soter sym result buffer size determination failed");
    soter_sym_destroy(ctx);
    return 0;
  }
  result_data=malloc(result_data_length+final_data_length);
  if (result_data == NULL){
    testsuite_fail_if(result_data == NULL, "malloc failed");
    soter_sym_destroy(ctx);
    return 0;
  }
  //  result_data_length+=final_data_length;
  res=soter_sym_update(ctx, data, data_length, result_data, &result_data_length);
    if(res!=HERMES_SUCCESS){
    testsuite_fail_if(res!=HERMES_SUCCESS, "soter sym update failed");
    free(result_data);
    soter_sym_destroy(ctx);
    return 0;
  }
  res=soter_sym_final(ctx, result_data+result_data_length, &final_data_length);
  if(res!=HERMES_SUCCESS){
    testsuite_fail_if(res!=HERMES_SUCCESS, "soter sym final failed");
    free(result_data);
    soter_sym_destroy(ctx);
    return 0;
  }
  uint8_t auth_tag[16];
  size_t auth_tag_length=16;
  res=soter_sym_get_auth_tag(ctx, auth_tag, &auth_tag_length);
  if(res!=HERMES_SUCCESS){
    testsuite_fail_if(res!=HERMES_SUCCESS, "soter sym get auth tag failed");
    free(result_data);
    soter_sym_destroy(ctx);
    return 0;
  }
  res=soter_sym_destroy(ctx);
  if(res!=HERMES_SUCCESS){
    testsuite_fail_if(res!=HERMES_SUCCESS, "soter sym destroy failed");
    free(result_data);
    soter_sym_destroy(ctx);
    return 0;
  }
  (*res_data)=result_data;
  return result_data_length+final_data_length;
}

static int alg_test(soter_sym_alg_t encrypt_alg, soter_sym_alg_t decrypt_alg)
{
  size_t i,j;
  uint8_t* encrypted_data;
  size_t encrypted_data_length;
  uint8_t* decrypted_data;
  size_t decrypted_data_length;
  for(i=0; i< (sizeof(test_init_vectors)/sizeof(test_init_vector_t)); ++i){
    for(j=0;j<(sizeof(test_data_vectors)/sizeof(test_data_vector_t)); ++j){
      encrypted_data_length=solo_test(encrypt_alg, test_init_vectors[i], test_data_vectors[j].data, test_data_vectors[j].data_length, &encrypted_data);
      if(encrypted_data==NULL || encrypted_data_length == 0){
	testsuite_fail_if(encrypted_data==NULL || encrypted_data_length == 0, "solo_test encryption failed");
	return -1;
      }
      decrypted_data_length=solo_test(decrypt_alg, test_init_vectors[i], encrypted_data, encrypted_data_length, &decrypted_data);
      if(decrypted_data==NULL || decrypted_data_length == 0){
	testsuite_fail_if(decrypted_data==NULL || decrypted_data_length == 0, "solo_test decryption failed");
	free(encrypted_data);
	return -1;
      }
      if(test_data_vectors[j].data_length!=decrypted_data_length){
	testsuite_fail_if(test_data_vectors[j].data_length!=decrypted_data_length, "data length for encryption and after decription non equal");
	free(encrypted_data);
	free(decrypted_data);
	return -1;
      }
      if(memcmp(test_data_vectors[j].data, decrypted_data, decrypted_data_length)){
	testsuite_fail_if(memcmp(test_data_vectors[j].data, decrypted_data, decrypted_data_length), "data for encryption and after encription is equal");
	free(encrypted_data);
	free(decrypted_data);
	return -1;
      }
    }
  }
  return 0;
}

#define SOTER_SYM_TESTS SOTER_SYM_ALGS


#define SOTER_SYM_ALG(alg, mode, padding, kdf)	\
  char message##alg##mode##padding##kdf[256];\
  sprintf(message##alg##mode##padding##kdf, "soter sym  %s:%s:%s:%s", #alg, #mode, #padding, #kdf);				\
  testsuite_fail_if(alg_test( SOTER_##alg##_##mode##_##padding##_##kdf##_Encrypt,  SOTER_##alg##_##mode##_##padding##_##kdf##_Decrypt),message##alg##mode##padding##kdf);
  

static void soter_sym_test()
{
    SOTER_SYM_TESTS
      //SOTER_SYM_ALG(aes,ecb,pkcs7,PBKDF2)
}

#undef SOTER_SYM_ALG

struct test_vector_type
{
	soter_sym_alg_t alg;
	char *key;
	char *iv;
	char *plaintext;
	char *ciphertext;
};

typedef struct test_vector_type test_vector_t;

struct test_vector_aead_type
{
	soter_sym_alg_t alg;
	char *key;
	char *iv;
	char *aad;
	char *plaintext;
	char *ciphertext;
	char *authtag;
};

typedef struct test_vector_aead_type test_vector_aead_t;

/* taken from NIST SP800-38A */
static test_vector_t vectors[] = {
	{
		SOTER_aes_ecb_pkcs7_nonkdf_Encrypt,
		"2b7e151628aed2a6abf7158809cf4f3c",
		"",
		"6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411e5fbc1191a0a52eff69f2445df4f9b17ad2b417be66c3710",
		"3ad77bb40d7a3660a89ecaf32466ef97f5d3d58503b9699de785895a96fdbaaf43b1cd7f598ece23881b00e3ed0306887b0c785e27e8ad3f8223207104725dd4"
	},
	{
		SOTER_aes_ecb_pkcs7_nonkdf_Encrypt,
		"603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4",
		"",
		"6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411e5fbc1191a0a52eff69f2445df4f9b17ad2b417be66c3710",
		"f3eed1bdb5d2a03c064b5a7e3db181f8591ccb10d410ed26dc5ba74a31362870b6ed21b99ca6f4f9f153e7b1beafed1d23304b7a39f9f3ff067d8d8f9e24ecc7"
	},
	{
		SOTER_aes_ctr_none_nonkdf_Encrypt,
		"2b7e151628aed2a6abf7158809cf4f3c",
		"f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff",
		"6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411e5fbc1191a0a52eff69f2445df4f9b17ad2b417be66c3710",
		"874d6191b620e3261bef6864990db6ce9806f66b7970fdff8617187bb9fffdff5ae4df3edbd5d35e5b4f09020db03eab1e031dda2fbe03d1792170a0f3009cee"
	},
	{
		SOTER_aes_ctr_none_nonkdf_Encrypt,
		"603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4",
		"f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff",
		"6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411e5fbc1191a0a52eff69f2445df4f9b17ad2b417be66c3710",
		"601ec313775789a5b7a7f504bbf3d228f443e3ca4d62b59aca84e990cacaf5c52b0930daa23de94ce87017ba2d84988ddfc9c58db67aada613c2dd08457941a6"
	}
};

/* GCM - taken from The Galois/Counter Mode of Operation (GCM) by David A. McGrew and John Viega */
static test_vector_aead_t vectors_aead[] =
{
	{
		SOTER_aes_gcm_none_nonkdf_Encrypt,
		"feffe9928665731c6d6a8f9467308308",
		"cafebabefacedbaddecaf888",
		"",
		"d9313225f88406e5a55909c5aff5269a86a7a9531534f7da2e4c303d8a318a721c3c0c95956809532fcf0e2449a6b525b16aedf5aa0de657ba637b391aafd255",
		"42831ec2217774244b7221b784d0d49ce3aa212f2c02a4e035c17e2329aca12e21d514b25466931c7d8f6a5aac84aa051ba30b396a0aac973d58e091473f5985",
		"4d5c2af327cd64a62cf35abd2ba6fab4"
	},
	{
		SOTER_aes_gcm_none_nonkdf_Encrypt,
		"feffe9928665731c6d6a8f9467308308",
		"cafebabefacedbaddecaf888",
		"feedfacedeadbeeffeedfacedeadbeefabaddad2",
		"d9313225f88406e5a55909c5aff5269a86a7a9531534f7da2e4c303d8a318a721c3c0c95956809532fcf0e2449a6b525b16aedf5aa0de657ba637b39",
		"42831ec2217774244b7221b784d0d49ce3aa212f2c02a4e035c17e2329aca12e21d514b25466931c7d8f6a5aac84aa051ba30b396a0aac973d58e091",
		"5bc94fbc3221a5db94fae95ae7121a47"
	},
	{
		SOTER_aes_gcm_none_nonkdf_Encrypt,
		"feffe9928665731c6d6a8f9467308308feffe9928665731c6d6a8f9467308308",
		"cafebabefacedbaddecaf888",
		"",
		"d9313225f88406e5a55909c5aff5269a86a7a9531534f7da2e4c303d8a318a721c3c0c95956809532fcf0e2449a6b525b16aedf5aa0de657ba637b391aafd255",
		"522dc1f099567d07f47f37a32a84427d643a8cdcbfe5c0c97598a2bd2555d1aa8cb08e48590dbb3da7b08b1056828838c5f61e6393ba7a0abcc9f662898015ad",
		"b094dac5d93471bdec1a502270e3cc6c"
	},
	{
		SOTER_aes_gcm_none_nonkdf_Encrypt,
		"feffe9928665731c6d6a8f9467308308feffe9928665731c6d6a8f9467308308",
		"cafebabefacedbaddecaf888",
		"feedfacedeadbeeffeedfacedeadbeefabaddad2",
		"d9313225f88406e5a55909c5aff5269a86a7a9531534f7da2e4c303d8a318a721c3c0c95956809532fcf0e2449a6b525b16aedf5aa0de657ba637b39",
		"522dc1f099567d07f47f37a32a84427d643a8cdcbfe5c0c97598a2bd2555d1aa8cb08e48590dbb3da7b08b1056828838c5f61e6393ba7a0abcc9f662",
		"76fc6ece0f4e1768cddf8853bb2d551b"
	}
};

#define MAX_KEY_LENGTH 32
#define MAX_DATA_LENGTH 1024
#define MAX_IV_LENGTH 32

static void test_known_values(void)
{
	soter_sym_ctx_t *ctx;
	int i;

	uint8_t key[MAX_KEY_LENGTH];
	uint8_t iv[MAX_IV_LENGTH];
	uint8_t plaintext[MAX_DATA_LENGTH];
	uint8_t ciphertext[MAX_DATA_LENGTH];

	uint8_t computed[MAX_DATA_LENGTH];
	size_t computed_length;

	uint8_t pad[MAX_IV_LENGTH];
	size_t pad_length = sizeof(pad);

	soter_status_t res;

	for (i = 0; i < (sizeof(vectors) / sizeof(test_vector_t)); i++)
	{
		res = string_to_bytes(vectors[i].key, key, sizeof(key));
		if (HERMES_SUCCESS != res)
		{
			testsuite_fail_if(res, "string_to_bytes");
			continue;
		}

		res = string_to_bytes(vectors[i].iv, iv, sizeof(iv));
		if (HERMES_SUCCESS != res)
		{
			testsuite_fail_if(res, "string_to_bytes");
			continue;
		}

		res = string_to_bytes(vectors[i].plaintext, plaintext, sizeof(plaintext));
		if (HERMES_SUCCESS != res)
		{
			testsuite_fail_if(res, "string_to_bytes");
			continue;
		}

		res = string_to_bytes(vectors[i].ciphertext, ciphertext, sizeof(ciphertext));
		if (HERMES_SUCCESS != res)
		{
			testsuite_fail_if(res, "string_to_bytes");
			continue;
		}

		/* Encryption */
		ctx = soter_sym_create(vectors[i].alg, key, strlen(vectors[i].key) / 2, iv, strlen(vectors[i].iv) / 2);
		if (NULL == ctx)
		{
			testsuite_fail_unless(ctx, "soter_sym_create");
			continue;
		}

		computed_length = sizeof(computed);

		res = soter_sym_update(ctx, plaintext, strlen(vectors[i].plaintext) / 2, computed, &computed_length);
		if (HERMES_SUCCESS != res)
		{
			testsuite_fail_if(res, "soter_sym_update");
			soter_sym_destroy(ctx);
			continue;
		}

		res = soter_sym_final(ctx, pad, &pad_length);
		if (HERMES_SUCCESS != res)
		{
			testsuite_fail_if(res, "soter_sym_final");
			soter_sym_destroy(ctx);
			continue;
		}

		res = soter_sym_destroy(ctx);
		if (HERMES_SUCCESS != res)
		{
			testsuite_fail_if(res, "soter_sym_destroy");
		}

		testsuite_fail_if(memcmp(computed, ciphertext, computed_length), "known encryption");

		/* Decryption */
		ctx = soter_sym_create(vectors[i].alg + 1, key, strlen(vectors[i].key) / 2, iv, strlen(vectors[i].iv) / 2);
		if (NULL == ctx)
		{
			testsuite_fail_unless(ctx, "soter_sym_create");
			continue;
		}

		res = soter_sym_update(ctx, computed, computed_length, computed, &computed_length);
		if (HERMES_SUCCESS != res)
		{
			testsuite_fail_if(res, "soter_sym_update");
			soter_sym_destroy(ctx);
			continue;
		}

		res = soter_sym_final(ctx, pad, &pad_length);
		if (HERMES_SUCCESS != res)
		{
			testsuite_fail_if(res, "soter_sym_final");
			soter_sym_destroy(ctx);
			continue;
		}

		res = soter_sym_destroy(ctx);
		if (HERMES_SUCCESS != res)
		{
			testsuite_fail_if(res, "soter_sym_destroy");
			soter_sym_destroy(ctx);
			continue;
		}

		testsuite_fail_if(memcmp(computed, plaintext, computed_length), "known decryption");
	}
}

void run_soter_sym_test()
{
  testsuite_enter_suite("soter sym");
  testsuite_run_test(soter_sym_test);
  testsuite_run_test(test_known_values);
  //  soter_sym_test();
}
