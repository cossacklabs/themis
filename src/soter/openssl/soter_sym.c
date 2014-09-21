/**
 * @file
 *
 * (c) CossackLabs
 */

#include "common/error.h"
#include "soter/soter.h"

EVP_CIPHER* algId_to_evp(const soter_sym_alg_t algId)
{
  switch(algId)
    {
    case SOTER_AES_ECB_PKCS7_PBKDF2_ENCRYPT:
    case SOTER_AES_ECB_PKCS7_PBKDF2_DECRYPT:
      return EVP_aes_256_ecb();
    case SOTER_AES_CTR_PBKDF2_ENCRYPT:
    case SOTER_AES_CTR_PBKDF2_DECRYPT:
      return EVP_aes_256_ctr();
    case SOTER_AES_GCM_PBKDF2_ENCRYPT:
    case SOTER_AES_GCM_PBKDF2_DECRYPT:
      return EVP_aes_256_gcm();
    case SOTER_AES_XTS_PBKDF2_ENCRYPT:
    case SOTER_AES_XTS_PBKDF2_DECRYPT:
      return EVP_aes_256_xts();
    }
  return NULL;
}

soter_status_t soter_pbkdf2(const void* password, const size_t password_length, const void* salt, const size_t salt_length, void* key)
{
   unsigned int i;
   unsigned char digest[outputBytes];
   if(!PKCS5_PBKDF2_HMAC(password, password_length, salt, salt_length, 0, EVP_sha256(), SOTER_AES_KEY_LENGTH, key))
     return HERMES_FAIL;
   return HERMES_SUCCESS;
   
}

soter_status_t soter_sym_create(soter_sym_ctx_t *sym_ctx, size_t* sym_ctx_length, const soter_sym_alg_t, const void* key, const size_t key_length, const void* salt, const size_t salt_length)
{
  if(sym_ctx==NULL || key==NULL || key_length==0)
    {
      return HERMES_INVALID_PARAMETR;
    }
  if(sym_ctx_length<sizeof(soter_sym_ctx_t))
    {
      (*sym_ctx_length)=sizeof(soter_sym_ctx_t);
      return HERMES_NO_MEMORY;
    }
  return HERMES_SUCCESS;
}
soter_status_t sym_update(sym_ctx_t *sym_ctx, const void* plain_data,  const size_t data_length, const void* chiper_data, size_t* chipher_data_length);
soter_status_t sym_final(sym_ctx_t *sym_ctx, const void* chipher_data, size_t* chipher_data_length);
soter_status_t soter_sym_destroy(sym_ctx_t *ctx);
