/*
* Copyright (c) 2015 Cossack Labs Limited
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include <string.h>
#include "soter/error.h"
#include "soter/soter.h"
#include "soter_openssl.h"
#include <openssl/err.h>

#define SOTER_SYM_MAX_KEY_LENGTH 128
#define SOTER_SYM_MAX_IV_LENGTH 16
#define SOTER_AES_GCM_AUTH_TAG_LENGTH 16

soter_status_t soter_pbkdf2(const uint8_t* password, const size_t password_length, const uint8_t* salt, const size_t salt_length, uint8_t* key, size_t* key_length){
  if(!PKCS5_PBKDF2_HMAC((const char*)password, (const int)password_length, salt, (const int)salt_length, 0, EVP_sha256(), (const int)(*key_length), key)){
      return SOTER_FAIL;
    }
  return SOTER_SUCCESS;
}

soter_status_t soter_nokdf(const uint8_t* password, const size_t password_length, uint8_t* key, size_t* key_length){
  if(password_length<(*key_length)){
    return SOTER_FAIL;
  }
  memcpy(key, password, (*key_length));
  return SOTER_SUCCESS;
}

soter_status_t soter_withkdf(uint32_t alg, const uint8_t* password, const size_t password_length, const uint8_t* salt, const size_t salt_length, uint8_t* key, size_t* key_length){
  switch(alg&SOTER_SYM_KDF_MASK){
  case SOTER_SYM_NOKDF:
    return soter_nokdf(password, password_length, key, key_length);
    break;
  case SOTER_SYM_PBKDF2:
    return soter_pbkdf2(password, password_length, salt, salt_length, key, key_length);
    break;    
  }
  return SOTER_INVALID_PARAMETER;
}

const EVP_CIPHER* algid_to_evp(uint32_t alg){
  switch(alg&(SOTER_SYM_ALG_MASK|SOTER_SYM_PADDING_MASK|SOTER_SYM_KEY_LENGTH_MASK)){
    case SOTER_SYM_AES_ECB_PKCS7|SOTER_SYM_256_KEY_LENGTH:
  return EVP_aes_256_ecb();
    case SOTER_SYM_AES_ECB_PKCS7|SOTER_SYM_192_KEY_LENGTH:
      return EVP_aes_192_ecb();
    case SOTER_SYM_AES_ECB_PKCS7|SOTER_SYM_128_KEY_LENGTH:
      return EVP_aes_128_ecb();
    case SOTER_SYM_AES_CTR|SOTER_SYM_256_KEY_LENGTH:
      return EVP_aes_256_ctr();
    case SOTER_SYM_AES_CTR|SOTER_SYM_192_KEY_LENGTH:
      return EVP_aes_192_ctr();
    case SOTER_SYM_AES_CTR|SOTER_SYM_128_KEY_LENGTH:
      return EVP_aes_128_ctr();
    case SOTER_SYM_AES_XTS|SOTER_SYM_256_KEY_LENGTH:
      return EVP_aes_256_xts();
  }
  return NULL;
}

const EVP_CIPHER* algid_to_evp_aead(uint32_t alg){
  switch(alg&(SOTER_SYM_ALG_MASK|SOTER_SYM_PADDING_MASK|SOTER_SYM_KEY_LENGTH_MASK)){
    case SOTER_SYM_AES_GCM|SOTER_SYM_256_KEY_LENGTH:
      return EVP_aes_256_gcm();
    case SOTER_SYM_AES_GCM|SOTER_SYM_192_KEY_LENGTH:
      return EVP_aes_192_gcm();
    case SOTER_SYM_AES_GCM|SOTER_SYM_128_KEY_LENGTH:
      return EVP_aes_128_gcm();
  }
  return NULL;
}


soter_sym_ctx_t* soter_sym_ctx_init(const uint32_t alg,
				    const void* key,
				    const size_t key_length,
				    const void* salt,
				    const size_t salt_length,
				    const void* iv,
				    const size_t iv_length,
				    bool encrypt){
  SOTER_CHECK_PARAM_(key!=NULL);
  SOTER_CHECK_PARAM_(key_length!=0);
  soter_sym_ctx_t* ctx=NULL;
  ctx=malloc(sizeof(soter_sym_ctx_t));
  SOTER_CHECK_MALLOC_(ctx);
  ctx->alg=alg;
  uint8_t key_[SOTER_SYM_MAX_KEY_LENGTH];
  size_t key_length_=(alg&SOTER_SYM_KEY_LENGTH_MASK)/8;
  EVP_CIPHER_CTX_init(&(ctx->evp_sym_ctx));
  //  if(iv!=NULL && (iv_length<SOTER_SYM_BLOCK_LENGTH(alg))){ // как проверить длину iv??
  //  return NULL;
  //}
  SOTER_IF_FAIL_(soter_withkdf(alg,key, key_length, salt, salt_length, key_, &key_length_)==SOTER_SUCCESS, soter_sym_encrypt_destroy(ctx));
  if(encrypt){
    SOTER_IF_FAIL_(EVP_EncryptInit_ex(&(ctx->evp_sym_ctx), algid_to_evp(alg), NULL, key_, iv), soter_sym_encrypt_destroy(ctx));
  } else {
    SOTER_IF_FAIL_(EVP_DecryptInit_ex(&(ctx->evp_sym_ctx), algid_to_evp(alg), NULL, key_, iv), soter_sym_encrypt_destroy(ctx));
  }
  return ctx;
}

soter_sym_ctx_t* soter_sym_aead_ctx_init(const uint32_t alg,
				    const void* key,
				    const size_t key_length,
				    const void* salt,
				    const size_t salt_length,
				    const void* iv,
				    const size_t iv_length,
				    bool encrypt){
  SOTER_CHECK_PARAM_(key!=NULL);
  SOTER_CHECK_PARAM_(key_length!=0);
  soter_sym_ctx_t* ctx=NULL;
  ctx=malloc(sizeof(soter_sym_ctx_t));
  SOTER_CHECK_MALLOC_(ctx);
  ctx->alg=alg;
  uint8_t key_[SOTER_SYM_MAX_KEY_LENGTH];
  size_t key_length_=(alg&SOTER_SYM_KEY_LENGTH_MASK)/8;
  EVP_CIPHER_CTX_init(&(ctx->evp_sym_ctx));
  SOTER_IF_FAIL_(soter_withkdf(alg,key, key_length, salt, salt_length, key_, &key_length_)==SOTER_SUCCESS, soter_sym_encrypt_destroy(ctx));
  if(encrypt){
    SOTER_IF_FAIL_(EVP_EncryptInit_ex(&(ctx->evp_sym_ctx), algid_to_evp_aead(alg), NULL, key_, iv), soter_sym_encrypt_destroy(ctx));
  } else {
    SOTER_IF_FAIL_(EVP_DecryptInit_ex(&(ctx->evp_sym_ctx), algid_to_evp_aead(alg), NULL, key_, iv), soter_sym_encrypt_destroy(ctx));
  }
  return ctx;
}



soter_status_t soter_sym_ctx_update(soter_sym_ctx_t *ctx,
				    const void* in_data,
				    const size_t in_data_length,
				    void* out_data,
				    size_t* out_data_length,
				    bool encrypt){
  if(encrypt){
    SOTER_CHECK(EVP_EncryptUpdate(&(ctx->evp_sym_ctx), out_data, (int*)out_data_length, (void*)in_data, (int)in_data_length)==1);
  } else {
    SOTER_CHECK(EVP_DecryptUpdate(&(ctx->evp_sym_ctx), out_data, (int*)out_data_length, (void*)in_data, (int)in_data_length)==1);
  }    
  return SOTER_SUCCESS;
}

soter_status_t soter_sym_ctx_final(soter_sym_ctx_t *ctx,
				   void* out_data,
				   size_t* out_data_length,
				   bool encrypt){
  if((ctx->alg&SOTER_SYM_PADDING_MASK)!=0){
    if((*out_data_length)<EVP_CIPHER_CTX_block_size(&(ctx->evp_sym_ctx))){
      (*out_data_length)=EVP_CIPHER_CTX_block_size(&(ctx->evp_sym_ctx));
      return SOTER_BUFFER_TOO_SMALL;
    }
  }
  if(encrypt){
    SOTER_CHECK(EVP_EncryptFinal(&(ctx->evp_sym_ctx), out_data, (int*)out_data_length)!=0);
  } else {
    SOTER_CHECK(EVP_DecryptFinal(&(ctx->evp_sym_ctx), out_data, (int*)out_data_length)!=0);
  }    
  return SOTER_SUCCESS;
}

soter_status_t soter_sym_aead_ctx_final(soter_sym_ctx_t *ctx,bool encrypt){
  uint8_t out_data[16];
  size_t out_data_length=0;
  if(encrypt){
    SOTER_CHECK(EVP_EncryptFinal(&(ctx->evp_sym_ctx), out_data, (int*)&out_data_length)!=0 && out_data_length==0);
  } else {
    SOTER_CHECK(EVP_DecryptFinal(&(ctx->evp_sym_ctx), out_data, (int*)&out_data_length)!=0 && out_data_length==0);
  }    
  return SOTER_SUCCESS;
}

soter_status_t soter_sym_ctx_destroy(soter_sym_ctx_t *ctx){
  EVP_CIPHER_CTX_cleanup(&(ctx->evp_sym_ctx));
  free(ctx);
  return SOTER_SUCCESS;    
}

soter_sym_ctx_t* soter_sym_encrypt_create(const uint32_t alg, const void* key, const size_t key_length, const void* salt, const size_t salt_length, const void* iv, const size_t iv_length){
  return soter_sym_ctx_init(alg, key,key_length, salt, salt_length, iv, iv_length, true);
}

soter_status_t soter_sym_encrypt_update(soter_sym_ctx_t *ctx, const void* plain_data,  const size_t plain_data_length, void* cipher_data, size_t* cipher_data_length){
  if(cipher_data==NULL || (*cipher_data_length)<(plain_data_length+EVP_CIPHER_CTX_block_size(&(ctx->evp_sym_ctx))-1)){
    (*cipher_data_length)=plain_data_length+EVP_CIPHER_CTX_block_size(&(ctx->evp_sym_ctx))-1;
    return SOTER_BUFFER_TOO_SMALL;
  }
  return soter_sym_ctx_update(ctx, plain_data,  plain_data_length, cipher_data, cipher_data_length, true);
}

soter_status_t soter_sym_encrypt_final(soter_sym_ctx_t *ctx, void* cipher_data, size_t* cipher_data_length){
   return  soter_sym_ctx_final(ctx, cipher_data, cipher_data_length, true);
}

soter_status_t soter_sym_encrypt_destroy(soter_sym_ctx_t *ctx){
  return soter_sym_ctx_destroy(ctx);
}

soter_sym_ctx_t* soter_sym_decrypt_create(const uint32_t alg, const void* key, const size_t key_length, const void* salt, const size_t salt_length, const void* iv, const size_t iv_length){
  return soter_sym_ctx_init(alg, key,key_length, salt, salt_length, iv, iv_length, false);
}

soter_status_t soter_sym_decrypt_update(soter_sym_ctx_t *ctx, const void* cipher_data,  const size_t cipher_data_length, void* plain_data, size_t* plain_data_length){
  if(plain_data==NULL || (*plain_data_length)<(cipher_data_length+EVP_CIPHER_CTX_block_size(&(ctx->evp_sym_ctx))-1)){
    (*plain_data_length)=cipher_data_length+EVP_CIPHER_CTX_block_size(&(ctx->evp_sym_ctx))-1;
    return SOTER_BUFFER_TOO_SMALL;
  }
  return soter_sym_ctx_update(ctx, cipher_data,  cipher_data_length, plain_data, plain_data_length, false);
}

soter_status_t soter_sym_decrypt_final(soter_sym_ctx_t *ctx, void* plain_data, size_t* plain_data_length){
   return  soter_sym_ctx_final(ctx, plain_data, plain_data_length, false);
}

soter_status_t soter_sym_decrypt_destroy(soter_sym_ctx_t *ctx){
  return soter_sym_ctx_destroy(ctx);
}

soter_sym_ctx_t* soter_sym_aead_encrypt_create(const uint32_t alg, const void* key, const size_t key_length, const void* salt, const size_t salt_length, const void* iv, const size_t iv_length){
  return soter_sym_aead_ctx_init(alg, key,key_length, salt, salt_length, iv, iv_length, true);
}

soter_status_t soter_sym_aead_encrypt_update(soter_sym_ctx_t *ctx, const void* plain_data,  const size_t plain_data_length, void* cipher_data, size_t* cipher_data_length){
  if(cipher_data==NULL || (*cipher_data_length)<(plain_data_length+EVP_CIPHER_CTX_block_size(&(ctx->evp_sym_ctx))-1)){
    (*cipher_data_length)=plain_data_length+EVP_CIPHER_CTX_block_size(&(ctx->evp_sym_ctx))-1;
    return SOTER_BUFFER_TOO_SMALL;
  }
  (*cipher_data_length)=plain_data_length+EVP_CIPHER_CTX_block_size(&(ctx->evp_sym_ctx))-1;
  return soter_sym_ctx_update(ctx, plain_data, plain_data_length, cipher_data, cipher_data_length, true);
}

soter_status_t soter_sym_aead_encrypt_aad(soter_sym_ctx_t *ctx, const void* plain_data,  const size_t plain_data_length){
  size_t tmp=0;
  return soter_sym_ctx_update(ctx, plain_data, plain_data_length, NULL, &tmp, true);
}

soter_status_t soter_sym_aead_encrypt_final(soter_sym_ctx_t *ctx, void* auth_tag, size_t* auth_tag_length){
  SOTER_CHECK_PARAM(auth_tag!=NULL);
  if((*auth_tag_length)<SOTER_AES_GCM_AUTH_TAG_LENGTH){
    (*auth_tag_length)=SOTER_AES_GCM_AUTH_TAG_LENGTH;
    return SOTER_BUFFER_TOO_SMALL;
  }
  SOTER_CHECK(soter_sym_aead_ctx_final(ctx, true)==SOTER_SUCCESS);
  SOTER_CHECK(EVP_CIPHER_CTX_ctrl(&(ctx->evp_sym_ctx), EVP_CTRL_GCM_GET_TAG, SOTER_AES_GCM_AUTH_TAG_LENGTH, auth_tag));
  (*auth_tag_length)=SOTER_AES_GCM_AUTH_TAG_LENGTH;
  return SOTER_SUCCESS;
}

soter_status_t soter_sym_aead_encrypt_destroy(soter_sym_ctx_t *ctx){
  return soter_sym_ctx_destroy(ctx);
}

soter_sym_ctx_t* soter_sym_aead_decrypt_create(const uint32_t alg, const void* key, const size_t key_length, const void* salt, const size_t salt_length, const void* iv, const size_t iv_length){
  return soter_sym_aead_ctx_init(alg, key,key_length, salt, salt_length, iv, iv_length, false);
}

soter_status_t soter_sym_aead_decrypt_update(soter_sym_ctx_t *ctx, const void* cipher_data,  const size_t cipher_data_length, void* plain_data, size_t* plain_data_length){
  if(plain_data==NULL || (*plain_data_length)<(cipher_data_length+EVP_CIPHER_CTX_block_size(&(ctx->evp_sym_ctx))-1)){
    (*plain_data_length)=cipher_data_length+EVP_CIPHER_CTX_block_size(&(ctx->evp_sym_ctx))-1;
    return SOTER_BUFFER_TOO_SMALL;
  }
  return soter_sym_ctx_update(ctx, cipher_data,  cipher_data_length, plain_data, plain_data_length, false);
}

soter_status_t soter_sym_aead_decrypt_aad(soter_sym_ctx_t *ctx, const void* plain_data,  const size_t plain_data_length){
  size_t tmp=0;
  return soter_sym_ctx_update(ctx, plain_data, plain_data_length, NULL, &tmp, false);
}

soter_status_t soter_sym_aead_decrypt_final(soter_sym_ctx_t *ctx, const void* auth_tag, const size_t auth_tag_length)
{
  SOTER_CHECK_PARAM(auth_tag!=NULL);
  SOTER_CHECK_PARAM(auth_tag_length>=SOTER_AES_GCM_AUTH_TAG_LENGTH);
  SOTER_CHECK(ctx!=NULL);
  SOTER_IF_FAIL(EVP_CIPHER_CTX_ctrl(&(ctx->evp_sym_ctx), EVP_CTRL_GCM_SET_TAG, SOTER_AES_GCM_AUTH_TAG_LENGTH, (void*)auth_tag), soter_sym_aead_decrypt_destroy(ctx));
  return  soter_sym_aead_ctx_final(ctx, false);
}
soter_status_t soter_sym_aead_decrypt_destroy(soter_sym_ctx_t *ctx){
  return soter_sym_ctx_destroy(ctx);
}
