/**
 * @file
 *
 * (c) CossackLabs
 */

#include "common/error.h"
#include "soter/soter.h"
#include "soter_openssl.h"

const EVP_CIPHER* algId_to_evp(const soter_sym_alg_t algId)
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
    };
  return NULL;
}


soter_status_t soter_pbkdf2(const void* password, const size_t password_length, const void* salt, const size_t salt_length, void* key)
{
   if(key=NULL)
     return HERMES_INVALID_PARAMETER;
   if(!PKCS5_PBKDF2_HMAC(password, password_length, salt, salt_length, 0, EVP_sha256(), SOTER_AES_KEY_LENGTH, key))
     return HERMES_FAIL;
   return HERMES_SUCCESS;
   
}

soter_status_t soter_evp_init_ex(soter_sym_ctx_t *sym_ctx, const soter_sym_alg_t alg, unsigned char *key, unsigned char *iv)
{
  switch(alg)
    {
    case SOTER_AES_ECB_PKCS7_PBKDF2_ENCRYPT:
    case SOTER_AES_CTR_PBKDF2_ENCRYPT:
    case SOTER_AES_GCM_PBKDF2_ENCRYPT:
    case SOTER_AES_XTS_PBKDF2_ENCRYPT:
      if(!EVP_EncryptInit_ex(&(sym_ctx->evp_sym_ctx), algId_to_evp(alg), NULL, key, NULL))
	return HERMES_FAIL;
      return HERMES_SUCCESS;
    case SOTER_AES_ECB_PKCS7_PBKDF2_DECRYPT:
    case SOTER_AES_CTR_PBKDF2_DECRYPT:
    case SOTER_AES_GCM_PBKDF2_DECRYPT:
    case SOTER_AES_XTS_PBKDF2_DECRYPT:
      if(!EVP_DecryptInit_ex(&(sym_ctx->evp_sym_ctx), algId_to_evp(alg), NULL, key, NULL))
	return HERMES_FAIL;
      return HERMES_SUCCESS;
    };
  return HERMES_FAIL;
}

soter_status_t soter_evp_sym_update(soter_sym_ctx_t *sym_ctx, const unsigned char *in, const size_t in_length, unsigned char *out, size_t *out_length)
{
  switch(sym_ctx->algId)
    {
    case SOTER_AES_ECB_PKCS7_PBKDF2_ENCRYPT:
    case SOTER_AES_CTR_PBKDF2_ENCRYPT:
    case SOTER_AES_GCM_PBKDF2_ENCRYPT:
    case SOTER_AES_XTS_PBKDF2_ENCRYPT:
      if(!EVP_EncryptUpdate(&(sym_ctx->evp_sym_ctx), out, (int*)out_length, in, in_length))
	return HERMES_FAIL;
      return HERMES_SUCCESS;
    case SOTER_AES_ECB_PKCS7_PBKDF2_DECRYPT:
    case SOTER_AES_CTR_PBKDF2_DECRYPT:
    case SOTER_AES_GCM_PBKDF2_DECRYPT:
    case SOTER_AES_XTS_PBKDF2_DECRYPT:
      if(!EVP_DecryptUpdate(&(sym_ctx->evp_sym_ctx), out, (int*)out_length, in, in_length))
	return HERMES_FAIL;
      return HERMES_SUCCESS;
    };
  return HERMES_FAIL;
}

soter_status_t soter_evp_sym_final(soter_sym_ctx_t *sym_ctx, unsigned char *out, size_t *out_length)
{
  switch(sym_ctx->algId)
    {
    case SOTER_AES_ECB_PKCS7_PBKDF2_ENCRYPT:
    case SOTER_AES_CTR_PBKDF2_ENCRYPT:
    case SOTER_AES_GCM_PBKDF2_ENCRYPT:
    case SOTER_AES_XTS_PBKDF2_ENCRYPT:
      if(!EVP_EncryptFinal(&(sym_ctx->evp_sym_ctx), out, (int*)out_length))
	return HERMES_FAIL;
      return HERMES_SUCCESS;
    case SOTER_AES_ECB_PKCS7_PBKDF2_DECRYPT:
    case SOTER_AES_CTR_PBKDF2_DECRYPT:
    case SOTER_AES_GCM_PBKDF2_DECRYPT:
    case SOTER_AES_XTS_PBKDF2_DECRYPT:
      if(!EVP_DecryptFinal(&(sym_ctx->evp_sym_ctx), out, (int*)out_length))
	return HERMES_FAIL;
      return HERMES_SUCCESS;
    };
  return HERMES_FAIL;
}

soter_status_t soter_sym_create(soter_sym_ctx_t *sym_ctx, size_t* sym_ctx_length, const soter_sym_alg_t algId, const void* key, const size_t key_length, const void* salt, const size_t salt_length)
{
  if(sym_ctx==NULL || key==NULL || key_length==0)
    {
      return HERMES_INVALID_PARAMETER;
    }
  if((*sym_ctx_length)<sizeof(soter_sym_ctx_t))
    {
      (*sym_ctx_length)=sizeof(soter_sym_ctx_t);
      return HERMES_NO_MEMORY;
    }
  uint8_t key_[SOTER_AES_KEY_LENGTH];
  if(soter_pbkdf2(key, key_length, salt, salt_length, key_)!=HERMES_SUCCESS)
    return HERMES_FAIL;
  sym_ctx->algId=algId;
  return soter_evp_init_ex(sym_ctx, algId, key_, NULL);
}

soter_status_t soter_sym_update(soter_sym_ctx_t *sym_ctx, const void* plain_data, const size_t data_length, void* cipher_data, size_t* cipher_data_length)
{
  return soter_evp_sym_update(sym_ctx, plain_data, data_length, cipher_data, cipher_data_length);
}

soter_status_t soter_sym_final(soter_sym_ctx_t *sym_ctx, void* cipher_data, size_t* cipher_data_length)
{
  return soter_evp_sym_final(sym_ctx, cipher_data, cipher_data_length);
}

soter_status_t soter_sym_destroy(soter_sym_ctx_t *ctx)
{
  if(!EVP_CIPHER_CTX_cleanup(&(ctx->evp_sym_ctx)))
    return HERMES_FAIL;
  return HERMES_SUCCESS;
}
