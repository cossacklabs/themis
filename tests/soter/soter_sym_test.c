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

size_t solo_aes_ecb_test(const test_init_vector_t init_data, const uint8_t *data, const size_t data_length)
{
  soter_sym_ctx_t* ctx=NULL;
  test_utils_status_t res;
  uint8_t* result_data=NULL;
  size_t result_data_length=0;
  uint8_t* revers_data=NULL;
  size_t revers_data_length=0;
  size_t final_data_length=16;
  
  ctx=soter_aes_ecb_encrypt_create(PBKDF2, init_data.key,strlen(init_data.key), init_data.salt, init_data.salt_length);
  if(!ctx){
    testsuite_fail_if(NULL==ctx, "sym_context == NULL");
    return 0;
  }
  res=soter_aes_ecb_encrypt_update(ctx, data, data_length, NULL, &result_data_length);
  if((res!=HERMES_BUFFER_TOO_SMALL) || (result_data_length==0)){
    testsuite_fail_if((res!=HERMES_BUFFER_TOO_SMALL) || (result_data_length == 0), "soter aes_ecb sym result buffer size determination failed");
    soter_aes_ecb_encrypt_destroy(ctx);
    return 0;
  }
  result_data=malloc(result_data_length+final_data_length);
  if (result_data == NULL){
    testsuite_fail_if(result_data == NULL, "malloc failed");
    soter_aes_ecb_encrypt_destroy(ctx);
    return 0;
  }
  res=soter_aes_ecb_encrypt_update(ctx, data, data_length, result_data, &result_data_length);
    if(res!=HERMES_SUCCESS){
    testsuite_fail_if(res!=HERMES_SUCCESS, "soter aes_ecb sym update failed");
    free(result_data);
    soter_aes_ecb_encrypt_destroy(ctx);
    return 0;
  }
  res=soter_aes_ecb_encrypt_final(ctx, result_data+result_data_length, &final_data_length);
  if(res!=HERMES_SUCCESS){
    testsuite_fail_if(res!=HERMES_SUCCESS, "soter aes_ecb sym final failed");
    free(result_data);
    soter_aes_ecb_encrypt_destroy(ctx);
    return 0;
  }
  result_data_length+=final_data_length;
  res=soter_aes_ecb_encrypt_destroy(ctx);
  if(res!=HERMES_SUCCESS){
    testsuite_fail_if(res!=HERMES_SUCCESS, "soter aes_ecb sym destroy failed");
    free(result_data);
    soter_aes_ecb_encrypt_destroy(ctx);
    return 0;
  }

  ctx=soter_aes_ecb_decrypt_create(PBKDF2, init_data.key,strlen(init_data.key), init_data.salt, init_data.salt_length);
  if(!ctx){
    testsuite_fail_if(NULL==ctx, "sym_context == NULL");
    return 0;
  }

  res=soter_aes_ecb_decrypt_update(ctx, result_data, result_data_length, NULL, &revers_data_length);
  if((res!=HERMES_BUFFER_TOO_SMALL) || (revers_data_length==0)){
    testsuite_fail_if((res!=HERMES_BUFFER_TOO_SMALL) || (result_data_length == 0), "soter aes_ecb decrypt sym result buffer size determination failed");
    soter_aes_ecb_decrypt_destroy(ctx);
    return 0;
  }
  revers_data=malloc(revers_data_length+final_data_length);
  if (revers_data == NULL){
    testsuite_fail_if(revers_data == NULL, "malloc failed");
    soter_aes_ecb_decrypt_destroy(ctx);
    return 0;
  }
  res=soter_aes_ecb_decrypt_update(ctx, result_data, result_data_length, revers_data, &revers_data_length);
    if(res!=HERMES_SUCCESS){
    testsuite_fail_if(res!=HERMES_SUCCESS, "soter aes_ecb decrypt sym update failed");
    free(result_data);
    soter_aes_ecb_decrypt_destroy(ctx);
    return 0;
  }
  res=soter_aes_ecb_decrypt_final(ctx, revers_data+revers_data_length, &final_data_length);
  if(res!=HERMES_SUCCESS){
    testsuite_fail_if(res!=HERMES_SUCCESS, "soter aes_ecb sym decrypt final failed");
    free(result_data);
    soter_aes_ecb_decrypt_destroy(ctx);
    return 0;
  }
  revers_data_length+=final_data_length;
  res=soter_aes_ecb_decrypt_destroy(ctx);
  if(res!=HERMES_SUCCESS){
    testsuite_fail_if(res!=HERMES_SUCCESS, "soter aes_ecb sym decrypt destroy failed");
    free(result_data);
    soter_aes_ecb_decrypt_destroy(ctx);
    return 0;
  }

  if(memcmp(data, revers_data, data_length)!=0){
    testsuite_fail_if(res!=HERMES_SUCCESS, "soter aes_ecb plain and decrypted_data not equal");
    free(result_data);
    return 0;
  }
  return 1;
}

static int aes_ecb_test()
{
  size_t i,j;
  uint8_t* encrypted_data;
  size_t encrypted_data_length;
  uint8_t* decrypted_data;
  size_t decrypted_data_length;
  for(i=0; i< (sizeof(test_init_vectors)/sizeof(test_init_vector_t)); ++i){
    for(j=0;j<(sizeof(test_data_vectors)/sizeof(test_data_vector_t)); ++j){
      encrypted_data_length=solo_aes_ecb_test(test_init_vectors[i], test_data_vectors[j].data, test_data_vectors[j].data_length);
      fprintf(stderr, "2\n");
      if(encrypted_data_length!=1){
	testsuite_fail_if(encrypted_data_length!=1, "solo_test encryption failed");
	return -1;
      }
    }
  }
  return 0;
}


static void soter_sym_test()
{
  testsuite_fail_if(aes_ecb_test()!=0, "SOTER sym aes ecb test");
}


struct test_vector_type
{
  soter_sym_ctx_t* (*encrypt_create)(const soter_sym_kdf_t, const void* key, const size_t key_length, const void* salt, const size_t salt_length);
  soter_status_t (*encrypt_update)(soter_sym_ctx_t *ctx, const void* plain_data,  const size_t data_length, void* chiper_data, size_t* chipher_data_length);
  soter_status_t (*encrypt_final)(soter_sym_ctx_t *ctx, void* chipher_data, size_t* chipher_data_length);
  soter_status_t (*encrypt_destroy)(soter_sym_ctx_t *ctx);
  soter_sym_ctx_t* (*decrypt_create)(const soter_sym_kdf_t, const void* key, const size_t key_length, const void* salt, const size_t salt_length);
  soter_status_t (*decrypt_update)(soter_sym_ctx_t *ctx, const void* plain_data,  const size_t data_length, void* chiper_data, size_t* chipher_data_length);
  soter_status_t (*decrypt_final)(soter_sym_ctx_t *ctx, void* chipher_data, size_t* chipher_data_length);
  soter_status_t (*decrypt_destroy)(soter_sym_ctx_t *ctx);
  char *key;
  char *iv;
  char *plaintext;
  char *ciphertext;
};

typedef struct test_vector_type test_vector_t;

struct test_vector_aead_type
{
  soter_sym_ctx_t* (*encrypt_create)(const soter_sym_kdf_t, const void* key, const size_t key_length, const void* salt, const size_t salt_length);
  soter_status_t (*encrypt_update)(soter_sym_ctx_t *ctx, const void* plain_data,  const size_t data_length, void* chiper_data, size_t* chipher_data_length);
  soter_status_t (*encrypt_final)(soter_sym_ctx_t *ctx, void* auth_tag, size_t* auth_tag_length,  void* chipher_data, size_t* chipher_data_length);
  soter_status_t (*encrypt_destroy)(soter_sym_ctx_t *ctx);
  soter_sym_ctx_t* (*decrypt_create)(const soter_sym_kdf_t, const void* key, const size_t key_length, const void* salt, const size_t salt_length, const void* auth_tag, const size_t auth_tag_length);
  soter_status_t (*decrypt_update)(soter_sym_ctx_t *ctx, const void* plain_data,  const size_t data_length, void* chiper_data, size_t* chipher_data_length);
  soter_status_t (*decrypt_final)(soter_sym_ctx_t *ctx, void* chipher_data, size_t* chipher_data_length);
  soter_status_t (*decrypt_destroy)(soter_sym_ctx_t *ctx);
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
    soter_aes_ecb_encrypt_create,
    soter_aes_ecb_encrypt_update,
    soter_aes_ecb_encrypt_final,
    soter_aes_ecb_encrypt_destroy,
    soter_aes_ecb_decrypt_create,
    soter_aes_ecb_decrypt_update,
    soter_aes_ecb_decrypt_final,
    soter_aes_ecb_decrypt_destroy,
    "2b7e151628aed2a6abf7158809cf4f3c",
    "",
    "6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411e5fbc1191a0a52eff69f2445df4f9b17ad2b417be66c3710",
    "3ad77bb40d7a3660a89ecaf32466ef97f5d3d58503b9699de785895a96fdbaaf43b1cd7f598ece23881b00e3ed0306887b0c785e27e8ad3f8223207104725dd4"
  },
  {
    soter_aes_ecb_encrypt_create,
    soter_aes_ecb_encrypt_update,
    soter_aes_ecb_encrypt_final,
    soter_aes_ecb_encrypt_destroy,
    soter_aes_ecb_decrypt_create,
    soter_aes_ecb_decrypt_update,
    soter_aes_ecb_decrypt_final,
    soter_aes_ecb_decrypt_destroy,
    "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4",
    "",
    "6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411e5fbc1191a0a52eff69f2445df4f9b17ad2b417be66c3710",
    "f3eed1bdb5d2a03c064b5a7e3db181f8591ccb10d410ed26dc5ba74a31362870b6ed21b99ca6f4f9f153e7b1beafed1d23304b7a39f9f3ff067d8d8f9e24ecc7"
  },
  {
    soter_aes_ctr_encrypt_create,
    soter_aes_ctr_encrypt_update,
    soter_aes_ctr_encrypt_final,
    soter_aes_ctr_encrypt_destroy,
    soter_aes_ctr_decrypt_create,
    soter_aes_ctr_decrypt_update,
    soter_aes_ctr_decrypt_final,
    soter_aes_ctr_decrypt_destroy,
    "2b7e151628aed2a6abf7158809cf4f3c",
    "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff",
    "6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411e5fbc1191a0a52eff69f2445df4f9b17ad2b417be66c3710",
    "874d6191b620e3261bef6864990db6ce9806f66b7970fdff8617187bb9fffdff5ae4df3edbd5d35e5b4f09020db03eab1e031dda2fbe03d1792170a0f3009cee"
  },
  {
    soter_aes_ctr_encrypt_create,
    soter_aes_ctr_encrypt_update,
    soter_aes_ctr_encrypt_final,
    soter_aes_ctr_encrypt_destroy,
    soter_aes_ctr_decrypt_create,
    soter_aes_ctr_decrypt_update,
    soter_aes_ctr_decrypt_final,
    soter_aes_ctr_decrypt_destroy,
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
      soter_aes_gcm_encrypt_create,
      soter_aes_gcm_encrypt_update,
      soter_aes_gcm_encrypt_final,
      soter_aes_gcm_encrypt_destroy,
      soter_aes_gcm_decrypt_create,
      soter_aes_gcm_decrypt_update,
      soter_aes_gcm_decrypt_final,
      soter_aes_gcm_decrypt_destroy,
      "feffe9928665731c6d6a8f9467308308",
      "cafebabefacedbaddecaf888",
      "",
      "d9313225f88406e5a55909c5aff5269a86a7a9531534f7da2e4c303d8a318a721c3c0c95956809532fcf0e2449a6b525b16aedf5aa0de657ba637b391aafd255",
      "42831ec2217774244b7221b784d0d49ce3aa212f2c02a4e035c17e2329aca12e21d514b25466931c7d8f6a5aac84aa051ba30b396a0aac973d58e091473f5985",
      "4d5c2af327cd64a62cf35abd2ba6fab4"
    },
    {
      soter_aes_gcm_encrypt_create,
      soter_aes_gcm_encrypt_update,
      soter_aes_gcm_encrypt_final,
      soter_aes_gcm_encrypt_destroy,
      soter_aes_gcm_decrypt_create,
      soter_aes_gcm_decrypt_update,
      soter_aes_gcm_decrypt_final,
      soter_aes_gcm_decrypt_destroy,
      "feffe9928665731c6d6a8f9467308308",
      "cafebabefacedbaddecaf888",
      "feedfacedeadbeeffeedfacedeadbeefabaddad2",
      "d9313225f88406e5a55909c5aff5269a86a7a9531534f7da2e4c303d8a318a721c3c0c95956809532fcf0e2449a6b525b16aedf5aa0de657ba637b39",
      "42831ec2217774244b7221b784d0d49ce3aa212f2c02a4e035c17e2329aca12e21d514b25466931c7d8f6a5aac84aa051ba30b396a0aac973d58e091",
      "5bc94fbc3221a5db94fae95ae7121a47"
    },
    {
      soter_aes_gcm_encrypt_create,
      soter_aes_gcm_encrypt_update,
      soter_aes_gcm_encrypt_final,
      soter_aes_gcm_encrypt_destroy,
      soter_aes_gcm_decrypt_create,
      soter_aes_gcm_decrypt_update,
      soter_aes_gcm_decrypt_final,
      soter_aes_gcm_decrypt_destroy,
      "feffe9928665731c6d6a8f9467308308feffe9928665731c6d6a8f9467308308",
      "cafebabefacedbaddecaf888",
      "",
      "d9313225f88406e5a55909c5aff5269a86a7a9531534f7da2e4c303d8a318a721c3c0c95956809532fcf0e2449a6b525b16aedf5aa0de657ba637b391aafd255",
      "522dc1f099567d07f47f37a32a84427d643a8cdcbfe5c0c97598a2bd2555d1aa8cb08e48590dbb3da7b08b1056828838c5f61e6393ba7a0abcc9f662898015ad",
      "b094dac5d93471bdec1a502270e3cc6c"
    },
    {
      soter_aes_gcm_encrypt_create,
      soter_aes_gcm_encrypt_update,
      soter_aes_gcm_encrypt_final,
      soter_aes_gcm_encrypt_destroy,
      soter_aes_gcm_decrypt_create,
      soter_aes_gcm_decrypt_update,
      soter_aes_gcm_decrypt_final,
      soter_aes_gcm_decrypt_destroy,
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
  uint8_t computed2[MAX_DATA_LENGTH];  
  size_t computed_length2=MAX_DATA_LENGTH;
  
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
      ctx = vectors[i].encrypt_create(NOKDF, key, strlen(vectors[i].key) / 2, iv, strlen(vectors[i].iv) / 2);
      if (NULL == ctx)
	{
	  testsuite_fail_unless(ctx, "soter_sym_create");
	  continue;
	}
      
      computed_length = sizeof(computed);
      
      res = vectors[i].encrypt_update(ctx, plaintext, strlen(vectors[i].plaintext) / 2, computed, &computed_length);
      if (HERMES_SUCCESS != res)
	{
	  testsuite_fail_if(res, "soter_sym_update");
	  vectors[i].encrypt_destroy(ctx);
	  continue;
	}

      pad_length=sizeof(pad);
      res = vectors[i].encrypt_final(ctx, computed+computed_length, &pad_length);
      if (HERMES_SUCCESS != res)
	{
	  testsuite_fail_if(res, "soter_sym_final");
	  vectors[i].encrypt_destroy(ctx);
 	  continue;
	}
      computed_length+=pad_length;
      res = vectors[i].encrypt_destroy(ctx);
      if (HERMES_SUCCESS != res)
	{
	  testsuite_fail_if(res, "soter_sym_destroy");
	}

      testsuite_fail_if(memcmp(computed, ciphertext, computed_length), "known encryption");

      /* Decryption */
      ctx = vectors[i].decrypt_create(NOKDF, key, strlen(vectors[i].key) / 2, iv, strlen(vectors[i].iv) / 2);
      if (NULL == ctx)
	{
	  testsuite_fail_unless(ctx, "soter_sym_create");
	  continue;
	}
      computed_length2=MAX_DATA_LENGTH;
      res = vectors[i].decrypt_update(ctx, computed, computed_length, computed2, &computed_length2);
      if (HERMES_SUCCESS != res)
	{
	  testsuite_fail_if(res, "soter_sym_update");
	  vectors[i].decrypt_destroy(ctx);
	  continue;
	}
      pad_length=sizeof(pad);
      res = vectors[i].decrypt_final(ctx, pad, &pad_length);
      if (HERMES_SUCCESS != res)
	{
	  testsuite_fail_if(res, "soter_sym_final");
	  vectors[i].decrypt_destroy(ctx);
	  continue;
	}

      res = vectors[i].decrypt_destroy(ctx);
      if (HERMES_SUCCESS != res)
	{
	  testsuite_fail_if(res, "soter_sym_destroy");
	  vectors[i].decrypt_destroy(ctx);
	  continue;
	}

      testsuite_fail_if(memcmp(computed2, plaintext, computed_length), "known decryption");
    }
}

#define MAX_AAD_LENGTH 1024
#define MAX_AUTH_TAG_LENGTH 16

static void test_known_values_gcm(void)
{
  soter_sym_ctx_t *ctx;
  int i;
  
  uint8_t key[MAX_KEY_LENGTH];
  uint8_t iv[MAX_IV_LENGTH];
  uint8_t plaintext[MAX_DATA_LENGTH];
  uint8_t ciphertext[MAX_DATA_LENGTH];
  
  uint8_t computed[MAX_DATA_LENGTH];
  size_t computed_length;

  uint8_t computed2[MAX_DATA_LENGTH];
  size_t computed_length2;

  uint8_t pad[MAX_IV_LENGTH];
  size_t pad_length = sizeof(pad);

  uint8_t aad[MAX_AAD_LENGTH];
  size_t aad_length=sizeof(aad);

  uint8_t auth_tag[MAX_AUTH_TAG_LENGTH];
  size_t auth_tag_length=sizeof(auth_tag);
  soter_status_t res;
  
  for (i = 0; i < (sizeof(vectors_aead) / sizeof(test_vector_aead_t)); i++)
    {
      res = string_to_bytes(vectors_aead[i].key, key, sizeof(key));
      if (HERMES_SUCCESS != res)
	{
	  testsuite_fail_if(res, "string_to_bytes");
	  continue;
	}
      
      res = string_to_bytes(vectors_aead[i].iv, iv, sizeof(iv));
      if (HERMES_SUCCESS != res)
	{
	  testsuite_fail_if(res, "string_to_bytes");
	  continue;
	}
      
      res = string_to_bytes(vectors_aead[i].plaintext, plaintext, sizeof(plaintext));
      if (HERMES_SUCCESS != res)
	{
	  testsuite_fail_if(res, "string_to_bytes");
	  continue;
	}

      res = string_to_bytes(vectors_aead[i].ciphertext, ciphertext, sizeof(ciphertext));
      if (HERMES_SUCCESS != res)
	{
	  testsuite_fail_if(res, "string_to_bytes");
	  continue;
	}

      res = string_to_bytes(vectors_aead[i].aad, aad, sizeof(aad));
      if (HERMES_SUCCESS != res)
	{
	  testsuite_fail_if(res, "string_to_bytes");
	  continue;
	}

      /* Encryption */
      ctx = vectors_aead[i].encrypt_create(NOKDF, key, strlen(vectors_aead[i].key) / 2, iv, strlen(vectors_aead[i].iv) / 2);
      if (NULL == ctx)
	{
	  testsuite_fail_unless(ctx, "soter_sym_create");
	  continue;
	}
      computed_length = sizeof(computed);     
      res = vectors_aead[i].encrypt_update(ctx, aad, strlen(vectors_aead[i].aad) / 2, NULL, &computed_length);
      if (HERMES_SUCCESS != res)
	{
	  testsuite_fail_if(res, "soter_sym_update aad");
	  vectors_aead[i].encrypt_destroy(ctx);
	  continue;
	}

      computed_length = sizeof(computed);
      
      res = vectors_aead[i].encrypt_update(ctx, plaintext, strlen(vectors_aead[i].plaintext) / 2, computed, &computed_length);
      if (HERMES_SUCCESS != res)
	{
	  testsuite_fail_if(res, "soter_sym_update");
	  vectors_aead[i].encrypt_destroy(ctx);
	  continue;
	}

      res = vectors_aead[i].encrypt_final(ctx, auth_tag, &auth_tag_length, computed+computed_length, &pad_length);
      if (HERMES_SUCCESS != res)
	{
	  testsuite_fail_if(res, "soter_sym_final");
	  vectors_aead[i].encrypt_destroy(ctx);
 	  continue;
	}
      computed_length+=pad_length;
      res = vectors_aead[i].encrypt_destroy(ctx);
      if (HERMES_SUCCESS != res)
	{
	  testsuite_fail_if(res, "soter_sym_destroy");
	}

      //testsuite_fail_if(memcmp(computed, ciphertext, computed_length), "known encryption");
      /* Decryption */
      ctx = vectors_aead[i].decrypt_create(NOKDF, key, strlen(vectors_aead[i].key) / 2, iv, strlen(vectors_aead[i].iv) / 2, auth_tag, auth_tag_length);
      if (NULL == ctx)
	{
	  testsuite_fail_unless(ctx, "soter_sym_create");
	  continue;
	}

      computed_length2 = sizeof(computed2);     
      res = vectors_aead[i].decrypt_update(ctx, aad, strlen(vectors_aead[i].aad) / 2, NULL, &computed_length2);
      if (HERMES_SUCCESS != res)
	{
	  testsuite_fail_if(res, "soter_sym_update aad");
	  vectors_aead[i].encrypt_destroy(ctx);
	  continue;
	}

      computed_length2= sizeof(computed2);     
      res = vectors_aead[i].decrypt_update(ctx, computed, computed_length, computed2, &computed_length2);
      if (HERMES_SUCCESS != res)
	{
	  testsuite_fail_if(res, "soter_sym_decrypt_update");
	  vectors_aead[i].decrypt_destroy(ctx);
	  continue;
	}

      res = vectors_aead[i].decrypt_final(ctx, computed2+computed_length2, &pad_length);
      if (HERMES_SUCCESS != res)
	{
	  testsuite_fail_if(res, "soter_sym_final");
	  vectors_aead[i].decrypt_destroy(ctx);
	  continue;
	}
      computed_length2+=pad_length;
      res = vectors_aead[i].decrypt_destroy(ctx);
      if (HERMES_SUCCESS != res)
	{
	  testsuite_fail_if(res, "soter_sym_destroy");
	  vectors_aead[i].decrypt_destroy(ctx);
	  continue;
	}

      testsuite_fail_if(memcmp(computed2, plaintext, computed_length2), "known decryption");
    }
}

void run_soter_sym_test()
{
  testsuite_enter_suite("soter sym");
  //  testsuite_run_test(soter_sym_test);
  //  testsuite_run_test(test_known_values);
  testsuite_run_test(test_known_values_gcm);
  //  soter_sym_test();
}
