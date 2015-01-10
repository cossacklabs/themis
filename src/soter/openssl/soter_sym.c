/**
 * @file
 *
 * (c) CossackLabs
 */

#include <string.h>
#include "common/error.h"
#include "soter/soter.h"
#include "soter_openssl.h"
#include <openssl/err.h>

#define SOTER_AES_BLOCK_LENGTH 16
#define SOTER_AES_KEY_LENGTH 32
#define SOTER_AES_GCM_AUTH_TAG_LENGTH 16

soter_status_t soter_pbkdf2(const uint8_t* password, const size_t password_length, const uint8_t* salt, const size_t salt_length, uint8_t* key, size_t* key_length){
  if(!PKCS5_PBKDF2_HMAC((const char*)password, password_length, salt, salt_length, 0, EVP_sha256(), (*key_length), key)){
      return HERMES_FAIL;
    }
  return HERMES_SUCCESS;			
}

soter_status_t soter_nokdf(const uint8_t* password, const size_t password_length, const uint8_t* salt, const size_t salt_length, uint8_t* key, size_t* key_length){
  if(password_length<SOTER_AES_KEY_LENGTH){
    return HERMES_FAIL;
  }
  memcpy(key, password, SOTER_AES_KEY_LENGTH);
  return HERMES_SUCCESS;
}

soter_sym_ctx_t* soter_sym_ctx_init(const soter_sym_kdf_t kdf,
				    const void* key,
				    const size_t key_length,
				    const void* salt,
				    const size_t salt_length,
				    int (*evp_init)(EVP_CIPHER_CTX *ctx, const EVP_CIPHER *type, ENGINE *impl, const unsigned char *key, const unsigned char *iv),
				    const EVP_CIPHER* evp_cipher){
  HERMES_CHECK_PARAM_(key!=NULL);
  HERMES_CHECK_PARAM_(key_length!=0);
  soter_sym_ctx_t* ctx=NULL;
  ctx=malloc(sizeof(soter_sym_ctx_t));
  HERMES_CHECK_MALLOC_(ctx);
  uint8_t key_[SOTER_AES_KEY_LENGTH];
  size_t key_length_=SOTER_AES_KEY_LENGTH;
  EVP_CIPHER_CTX_init(&(ctx->evp_sym_ctx));
  const uint8_t default_iv[SOTER_AES_BLOCK_LENGTH]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  switch(kdf){
    case PBKDF2:
      HERMES_IF_FAIL_(soter_pbkdf2(key, key_length, salt, salt_length, key_, &key_length_)==HERMES_SUCCESS, soter_aes_ecb_encrypt_destroy(ctx));
      HERMES_IF_FAIL_(evp_init(&(ctx->evp_sym_ctx), evp_cipher, NULL, key_, (uint8_t*)default_iv), soter_aes_ecb_encrypt_destroy(ctx));
      break;
    case NOKDF:
      HERMES_IF_FAIL_(soter_nokdf(key, key_length, salt, salt_length, key_, &key_length_)==HERMES_SUCCESS, soter_aes_ecb_encrypt_destroy(ctx));
      HERMES_IF_FAIL_(evp_init(&(ctx->evp_sym_ctx), evp_cipher, NULL, key_, (salt_length<SOTER_AES_BLOCK_LENGTH)?(uint8_t*)default_iv:(uint8_t*)salt), soter_aes_ecb_encrypt_destroy(ctx));
      break;
    default:
      HERMES_IF_FAIL_(true, soter_aes_ecb_encrypt_destroy(ctx));
  }
  return ctx;
}

soter_status_t soter_sym_ctx_update(soter_sym_ctx_t *ctx,
				    const void* in_data,
				    const size_t in_data_length,
				    void* out_data,
				    size_t* out_data_length,
				    int (*evp_update)(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl, const unsigned char *in, int inl)){
  HERMES_CHECK(evp_update(&(ctx->evp_sym_ctx), out_data, (int*)out_data_length, (void*)in_data, in_data_length)==1);
  return HERMES_SUCCESS;
}

soter_status_t soter_sym_ctx_final(soter_sym_ctx_t *ctx,
				   void* out_data,
				   size_t* out_data_length,
				   int (*evp_final)(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl)){
   HERMES_CHECK(evp_final(&(ctx->evp_sym_ctx), out_data, (int*)out_data_length)!=0);
   return HERMES_SUCCESS;
}

soter_status_t soter_sym_ctx_destroy(soter_sym_ctx_t *ctx){
  EVP_CIPHER_CTX_cleanup(&(ctx->evp_sym_ctx));
  free(ctx);
  return HERMES_SUCCESS;    
}

soter_sym_ctx_t* soter_aes_ecb_encrypt_create(const soter_sym_kdf_t kdf, const void* key, const size_t key_length, const void* salt, const size_t salt_length){
  return soter_sym_ctx_init(kdf, key,key_length, salt, salt_length, EVP_EncryptInit_ex, EVP_aes_256_ecb());
}

soter_status_t soter_aes_ecb_encrypt_update(soter_sym_ctx_t *ctx, const void* plain_data,  const size_t plain_data_length, void* cipher_data, size_t* cipher_data_length){
   if((*cipher_data_length)<(plain_data_length+SOTER_AES_BLOCK_LENGTH-1)){
     (*cipher_data_length)=plain_data_length+SOTER_AES_BLOCK_LENGTH-1;
      return HERMES_BUFFER_TOO_SMALL;
   }
   return soter_sym_ctx_update(ctx, plain_data,  plain_data_length, cipher_data, cipher_data_length, EVP_EncryptUpdate);
}

soter_status_t soter_aes_ecb_encrypt_final(soter_sym_ctx_t *ctx, void* cipher_data, size_t* cipher_data_length){
   if((*cipher_data_length)<SOTER_AES_BLOCK_LENGTH){
     (*cipher_data_length)=SOTER_AES_BLOCK_LENGTH;
     return HERMES_BUFFER_TOO_SMALL;
   }
   return  soter_sym_ctx_final(ctx, cipher_data, cipher_data_length, EVP_EncryptFinal);
}

soter_status_t soter_aes_ecb_encrypt_destroy(soter_sym_ctx_t *ctx){
  return soter_sym_ctx_destroy(ctx);
}

soter_sym_ctx_t* soter_aes_ecb_decrypt_create(const soter_sym_kdf_t kdf, const void* key, const size_t key_length, const void* salt, const size_t salt_length){
  return soter_sym_ctx_init(kdf, key,key_length, salt, salt_length, EVP_DecryptInit_ex, EVP_aes_256_ecb());
}

soter_status_t soter_aes_ecb_decrypt_update(soter_sym_ctx_t *ctx, const void* cipher_data,  const size_t cipher_data_length, void* plain_data, size_t* plain_data_length){
   if((*plain_data_length)<(cipher_data_length+SOTER_AES_BLOCK_LENGTH)){
     (*plain_data_length)=cipher_data_length+SOTER_AES_BLOCK_LENGTH;
      return HERMES_BUFFER_TOO_SMALL;
   }
   return soter_sym_ctx_update(ctx, cipher_data,  cipher_data_length, plain_data, plain_data_length, EVP_DecryptUpdate);
}

soter_status_t soter_aes_ecb_decrypt_final(soter_sym_ctx_t *ctx, void* plain_data, size_t* plain_data_length){
   return  soter_sym_ctx_final(ctx, plain_data, plain_data_length, EVP_DecryptFinal);
}

soter_status_t soter_aes_ecb_decrypt_destroy(soter_sym_ctx_t *ctx){
  return soter_sym_ctx_destroy(ctx);
}

soter_sym_ctx_t* soter_aes_ctr_encrypt_create(const soter_sym_kdf_t kdf, const void* key, const size_t key_length, const void* salt, const size_t salt_length){
  return soter_sym_ctx_init(kdf, key,key_length, salt, salt_length, EVP_EncryptInit_ex, EVP_aes_256_ctr());
}

soter_status_t soter_aes_ctr_encrypt_update(soter_sym_ctx_t *ctx, const void* plain_data,  const size_t plain_data_length, void* cipher_data, size_t* cipher_data_length){
   if((*cipher_data_length)<(plain_data_length+SOTER_AES_BLOCK_LENGTH-1)){
     (*cipher_data_length)=plain_data_length+SOTER_AES_BLOCK_LENGTH-1;
      return HERMES_BUFFER_TOO_SMALL;
   }
   return soter_sym_ctx_update(ctx, plain_data,  plain_data_length, cipher_data, cipher_data_length, EVP_EncryptUpdate);
}

soter_status_t soter_aes_ctr_encrypt_final(soter_sym_ctx_t *ctx, void* cipher_data, size_t* cipher_data_length){
   if((*cipher_data_length)<SOTER_AES_BLOCK_LENGTH){
     (*cipher_data_length)=SOTER_AES_BLOCK_LENGTH;
     return HERMES_BUFFER_TOO_SMALL;
   }
   return  soter_sym_ctx_final(ctx, cipher_data, cipher_data_length, EVP_EncryptFinal);
}

soter_status_t soter_aes_ctr_encrypt_destroy(soter_sym_ctx_t *ctx){
  return soter_sym_ctx_destroy(ctx);
}

soter_sym_ctx_t* soter_aes_ctr_decrypt_create(const soter_sym_kdf_t kdf, const void* key, const size_t key_length, const void* salt, const size_t salt_length){
  return soter_sym_ctx_init(kdf, key,key_length, salt, salt_length, EVP_DecryptInit_ex, EVP_aes_256_ctr());
}

soter_status_t soter_aes_ctr_decrypt_update(soter_sym_ctx_t *ctx, const void* cipher_data,  const size_t cipher_data_length, void* plain_data, size_t* plain_data_length){
   if((*plain_data_length)<(cipher_data_length+SOTER_AES_BLOCK_LENGTH)){
     (*plain_data_length)=cipher_data_length+SOTER_AES_BLOCK_LENGTH;
     return HERMES_BUFFER_TOO_SMALL;
   }
   return soter_sym_ctx_update(ctx, cipher_data,  cipher_data_length, plain_data, plain_data_length, EVP_DecryptUpdate);
}

soter_status_t soter_aes_ctr_decrypt_final(soter_sym_ctx_t *ctx, void* plain_data, size_t* plain_data_length){
   return  soter_sym_ctx_final(ctx, plain_data, plain_data_length, EVP_DecryptFinal);
}

soter_status_t soter_aes_ctr_decrypt_destroy(soter_sym_ctx_t *ctx){
  return soter_sym_ctx_destroy(ctx);
}

soter_sym_ctx_t* soter_aes_gcm_encrypt_create(const soter_sym_kdf_t kdf, const void* key, const size_t key_length, const void* salt, const size_t salt_length){
  return soter_sym_ctx_init(kdf, key,key_length, salt, salt_length, EVP_EncryptInit_ex, EVP_aes_256_gcm());
}

soter_status_t soter_aes_gcm_encrypt_update(soter_sym_ctx_t *ctx, const void* plain_data,  const size_t plain_data_length, void* cipher_data, size_t* cipher_data_length){
   if((*cipher_data_length)<(plain_data_length+SOTER_AES_BLOCK_LENGTH-1)){
     (*cipher_data_length)=plain_data_length+SOTER_AES_BLOCK_LENGTH-1;
      return HERMES_BUFFER_TOO_SMALL;
   }
   return soter_sym_ctx_update(ctx, plain_data, plain_data_length, cipher_data, cipher_data_length, EVP_EncryptUpdate);
}

soter_status_t soter_aes_gcm_encrypt_final(soter_sym_ctx_t *ctx, void* auth_tag, size_t* auth_tag_length,  void* cipher_data, size_t* cipher_data_length){
  HERMES_CHECK_PARAM(auth_tag!=NULL);
  if((*auth_tag_length)<SOTER_AES_GCM_AUTH_TAG_LENGTH){
    (*auth_tag_length)=SOTER_AES_GCM_AUTH_TAG_LENGTH;
    return HERMES_BUFFER_TOO_SMALL;
  }
  HERMES_CHECK(soter_sym_ctx_final(ctx, cipher_data, cipher_data_length, EVP_EncryptFinal)==HERMES_SUCCESS);
  HERMES_CHECK(EVP_CIPHER_CTX_ctrl(&(ctx->evp_sym_ctx), EVP_CTRL_GCM_GET_TAG, SOTER_AES_GCM_AUTH_TAG_LENGTH, auth_tag));
  return HERMES_SUCCESS;
}

soter_status_t soter_aes_gcm_encrypt_destroy(soter_sym_ctx_t *ctx){
  return soter_sym_ctx_destroy(ctx);
}

soter_sym_ctx_t* soter_aes_gcm_decrypt_create(const soter_sym_kdf_t kdf, const void* key, const size_t key_length, const void* salt, const size_t salt_length, const void* auth_tag, const size_t auth_tag_length){
  HERMES_CHECK_PARAM_(auth_tag!=NULL);
  HERMES_CHECK_PARAM_(auth_tag_length>=SOTER_AES_GCM_AUTH_TAG_LENGTH);
  soter_sym_ctx_t* ctx=soter_sym_ctx_init(kdf, key,key_length, salt, salt_length, EVP_DecryptInit_ex, EVP_aes_256_gcm());
  HERMES_CHECK_(ctx!=NULL);
  HERMES_IF_FAIL_(EVP_CIPHER_CTX_ctrl(&(ctx->evp_sym_ctx), EVP_CTRL_GCM_SET_TAG, SOTER_AES_GCM_AUTH_TAG_LENGTH, (void*)auth_tag), soter_aes_gcm_decrypt_destroy(ctx));
  return ctx;
}

soter_status_t soter_aes_gcm_decrypt_update(soter_sym_ctx_t *ctx, const void* cipher_data,  const size_t cipher_data_length, void* plain_data, size_t* plain_data_length){
   if((*plain_data_length)<(cipher_data_length+SOTER_AES_BLOCK_LENGTH)){
     (*plain_data_length)=cipher_data_length+SOTER_AES_BLOCK_LENGTH;
      return HERMES_BUFFER_TOO_SMALL;
   }
   return soter_sym_ctx_update(ctx, cipher_data,  cipher_data_length, plain_data, plain_data_length, EVP_DecryptUpdate);
}

soter_status_t soter_aes_gcm_decrypt_final(soter_sym_ctx_t *ctx, void* plain_data, size_t* plain_data_length)
{
  return  soter_sym_ctx_final(ctx, plain_data, plain_data_length, EVP_DecryptFinal);
}
soter_status_t soter_aes_gcm_decrypt_destroy(soter_sym_ctx_t *ctx){
  return soter_sym_ctx_destroy(ctx);
}


