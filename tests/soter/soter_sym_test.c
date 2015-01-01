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

void run_soter_sym_test()
{
  testsuite_enter_suite("soter sym");
  testsuite_run_test(soter_sym_test);
  //  soter_sym_test();
}
