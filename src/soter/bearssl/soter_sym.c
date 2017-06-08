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
#include "soter_engine.h"
#include <bearssl.h>

#include "soter_engine_consts.h"

#include <assert.h>

static inline size_t soter_sym_get_key_length(alg){
  switch(alg){
  case  SOTER_SYM_AEAD_SYM_AES_GCM_256:
    return SOTER_SYM_AEAD_DEFAULT_ALG_KEY_LENGTH;
  case  SOTER_SYM_SYM_AES_GCM_256:
    return SOTER_SYM_DEFAULT_ALG_KEY_LENGTH;
  default:
    return 0;
  }
}

static inline size_t soter_sym_get_salt_length(alg){
  switch(alg){
  case  SOTER_SYM_AEAD_SYM_AES_GCM_256:
    return SOTER_SYM_AEAD_DEFAULT_ALG_IV_SIZE;
  case  SOTER_SYM_SYM_AES_GCM_256:
    return SOTER_SYM_AEAD_DEFAULT_ALG_IV_SIZE;
  default:
    return 0;
  }
}

soter_status_t soter_sym_ctx_init(soter_sym_ctx_t* ctx,
                                  const uint32_t alg,
                                  const void* key,
                                  const size_t key_length,
                                  const void* salt,
                                  const size_t salt_length,
                                  bool encrypt){
  SOTER_CHECK_IN_PARAM(ctx);
  SOTER_CHECK_IN_BUF_PARAM(key, key_length, soter_sym_get_key_length(alg));
  SOTER_CHECK_IN_BUF_PARAM(salt, salt_length, soter_sym_get_salt_length(alg));
  ctx->alg=alg;
  switch(alg){
  case SOTER_SYM_SYM_AES_GCM_256:
      br_aes_big_ctr_init(&(ctx->impl.aes_ctr_256), key, key_len);
      memcpy(ctx->nonce.aes_ctr_256, salt, salt_length);
      break;
  default:
    return SOTER_INALID_PARAMETER;
  }  
  return SOTER_SUCCESS;
}


soter_status_t soter_sym_aead_ctx_init(soter_sym_aead_ctx_t* ctx,
                                       const uint32_t alg,
                                       const void* key,
                                       const size_t key_length,
                                       const void* salt,
                                       const size_t salt_length,
                                       bool encrypt){
  SOTER_CHECK_IN_PARAM(ctx);
  SOTER_CHECK_IN_BUF_PARAM(key, key_length, soter_sym_get_key_length(alg));
  SOTER_CHECK_IN_BUF_PARAM(salt, salt_length, soter_sym_get_salt_length(alg));
  soter_status_t res;
  switch(alg){
  case SOTER_SYM_AEAD_SYM_AES_GCM_256:{
    res=soter_sym_ctx_init(&(ctx->cypher), SOTER_SYM_SYM_AES_CTR_256, key, key_length, salt, salt_length);
    if(SOTER_SUCCESS != res){
      return res;
    }
    memset(ctx->tag.aes_gcm_256, 0, sizeof(ctx->tag.aes_gcm_256));
    size_t tag_length=SOTER_SYM_AEAD_SYM_AES_GCM_256_AUTH_TAG_SIZE;
    br_aes_big_ctr_run(&(ctx->impl.aes_ctr_256), ctx->nonce.aes_ctr_256, 0, ctx->tag.aes_gcm_256, SOTER_SYM_AEAD_SYM_AES_GCM_256_AUTH_TAG_SIZE);
    ctx->state=true;
    if(SOTER_SUCCESS != res){
      return res;
    }
  }
  default:
    return SOTER_INALID_PARAMETER;
  }
  return SOTER_FAIL;
}

soter_status_t soter_sym_ctx_update(soter_sym_ctx_t *ctx,
                                    const void* in_data,
                                    const size_t in_data_length,
                                    void* out_data,
                                    size_t* out_data_length,
                                    bool encrypt){
  SOTER_CHECK_IN_PARAM(ctx);
  SOTER_CHECK_IN_BUF_PARAM_NON_EMPTY(in_data, in_data_length);
  SOTER_CHECK_OUT_BUF_PARAM(out_data, out_data_length, in_data_length);

  switch(ctx->alg){
  case SOTER_SYM_SYM_AES_CTR_256:
    *out_data_length=in_data_length;
    if(in_data != out_data){
      memcpy(out_data, in_data, in_data_length);
    }
    br_aes_big_ctr_run(&(ctx->impl.aes_ctr_2556), ctx->nonce.aes_ctr_256, 0, out_data, *out_data_len);    
    break;
  default:
    return SOTER_INALID_PARAMETER;
  }  
  return SOTER_SUCCESS;
}

soter_status_t soter_sym_aead_ctx_encrypt_update(soter_sym_aead_ctx_t *ctx,
                                                 const void* in_data,
                                                 const size_t in_data_length,
                                                 void* out_data,
                                                 size_t* out_data_length){
  SOTER_CHECK_IN_PARAM(ctx);
  SOTER_CHECK_IN_PARAM(ctx->state);
  SOTER_CHECK_IN_BUF_PARAM_NON_EMPTY(in_data, in_data_length);
  SOTER_CHECK_OUT_BUF_PARAM(out_data, out_data_length, in_data_length);

  soter_status_t res;
  if(SOTER_SUCCESS!=(res=soter_sym_ctx_update(&(ctx->ctx), in_data, in_data_length, out_data, out_data_length, true))){
    return res;
  }
  switch(ctx->alg){
  case SOTER_SYM_AEAD_SYM_AES_GCM_256:{
    uint8_t h[16]={0};
    br_ghash_ctmul(ctx->tag.aes_gcm_256, h, out_data,  out_data_length);
  }
  default:
    free(tmp);
    return SOTER_INALID_PARAMETER;
  }
  return SOTER_SUCCESS;
}

soter_status_t soter_sym_aead_ctx_aad_update(soter_sym_aead_ctx_t *ctx,
                                	     const void* in_data,
                                    	     const size_t in_data_length){
  SOTER_CHECK_IN_PARAM(ctx);
  SOTER_CHECK_IN_PARAM(ctx->state);
  SOTER_CHECK_IN_BUF_PARAM_NON_EMPTY(in_data, in_data_length);

  soter_status_t res;
  void *tmp=malloc(in_data_length);
  size_t tmp_length=in_data_length;
  assert(tmp);
  res = soter_sym_aead_ctx_encrypt_update(ctx, in_data, in_data_length, tmp, tmp_length);
  free(tmp);
  return res;
}


soter_status_t soter_sym_aead_ctx_decrypt_update(soter_sym_aead_ctx_t *ctx,
                                         const void* in_data,
                                         const size_t in_data_length,
                                         void* out_data,
                                         size_t* out_data_length){
  SOTER_CHECK_IN_PARAM(ctx);
  SOTER_CHECK_IN_PARAM(ctx->state);
  SOTER_CHECK_IN_BUF_PARAM_NON_EMPTY(in_data, in_data_length);
  SOTER_CHECK_OUT_BUF_PARAM(out_data, out_data_length, in_data_length);

  soter_status_t res;
  switch(ctx->alg){
  case SOTER_SYM_AEAD_SYM_AES_GCM_256:{
    uint8_t h[16]={0};
    br_ghash_ctmul(ctx->tag.aes_gcm_256, h, in_data,  in_data_length);
  }
  default:
    free(tmp);
    return SOTER_INALID_PARAMETER;
  }
  if(SOTER_SUCCESS!=(res=soter_sym_ctx_update(&(ctx->ctx), in_data, in_data_length, out_data, out_data_length, false))){
    return res;
  }
  return SOTER_SUCCESS;
}

soter_status_t soter_sym_aead_ctx_update(soter_sym_aead_ctx_t *ctx,
                                         const void* in_data,
                                         const size_t in_data_length,
                                         void* out_data,
                                         size_t* out_data_length,
                                         bool encrypt){
  SOTER_CHECK_IN_PARAM(ctx);
  SOTER_CHECK_IN_BUF_PARAM_NON_EMPTY(in_data, in_data_length);
  SOTER_CHECK_OUT_BUF_PARAM(out_data, out_data_length, in_data_length);

  if(encrypt){
    return soter_sym_aead_ctx_encrypt_update(ctx, in_data, in_data_length, out_data, out_data_length);
  }
  return soter_sym_aead_ctx_decrypt_update(ctx, in_data, in_data_length, out_data, out_data_length);
}

soter_status_t soter_sym_aead_ctx_final(soter_sym_aead_ctx_t *ctx,
				   void* out_data,
				   size_t* out_data_length,
				   bool encrypt){

  SOTER_CHECK_IN_PARAM(ctx);
  SOTER_CHECK_OUT_BUF_PARAM(out_data, out_data_length, SOTER_SYM_AEAD_SYM_AES_GCM_256_AUTH_TAG_SIZE);

  soter_status_t res;
  switch(ctx->alg){
  case SOTER_SYM_AEAD_SYM_AES_GCM_256:{
    if(encrypt){
      memcpy(out_data, ctx->tag.aes_gcm_256, SOTER_SYM_AEAD_SYM_AES_GCM_256_AUTH_TAG_SIZE);
      memset(ctx->tag.aes_gcm_256, 0, SOTER_SYM_AEAD_SYM_AES_GCM_256_AUTH_TAG_SIZE);
      return SOTER_SUCCESS;
    } else {
      if(0==memcmp(out_data, ctx->tag.aes_gcm_256, SOTER_SYM_AEAD_SYM_AES_GCM_256_AUTH_TAG_SIZE)){
        return SOTER_SUCCESS;
      }
    }
  }
  default:
    free(tmp);
    return SOTER_INALID_PARAMETER;
  }
  return SOTER_FAIL;
}

soter_status_t soter_sym_ctx_destroy(soter_sym_ctx_t *ctx){
  free(ctx);
  return SOTER_SUCCESS;
}

soter_status_t soter_sym_aead_ctx_destroy(soter_sym_aead_ctx_t *ctx){
  free(ctx);
  return SOTER_SUCCESS;
}

soter_sym_ctx_t* soter_sym_encrypt_create(const uint32_t alg, const void* key, const size_t key_length, const void* salt, const size_t salt_length, const void* iv, const size_t iv_length){
  soter_sym_ctx_t* ctx=malloc(sizeof(soter_sym_ctx_t));
  assert(ctx);
  if(SOTER_SUCCESS != soter_sym_ctx_init(ctx, alg, key, key_length ,salt?salt:iv, salt?salt_length:iv_length, true)){
    free(ctx);
    return NULL;
  }
  return ctx;
}

soter_status_t soter_sym_encrypt_update(soter_sym_ctx_t *ctx, const void* plain_data,  const size_t plain_data_length, void* cipher_data, size_t* cipher_data_length){
  return soter_sym_ctx_update(ctx, plain_data, plain_data_length, cipher_data, cipher_data_length, true);
}

soter_status_t soter_sym_encrypt_destroy(soter_sym_ctx_t *ctx){
  SOTER_CHECK_IN_PARAM(ctx);
  free(ctx);
  return SOTER_SUCCESS;
}

soter_sym_ctx_t* soter_sym_decrypt_create(const uint32_t alg, const void* key, const size_t key_length, const void* salt, const size_t salt_length, const void* iv, const size_t iv_length){
  soter_sym_ctx_t* ctx=malloc(sizeof(soter_sym_ctx_t));
  assert(ctx);
  if(SOTER_SUCCESS != soter_sym_ctx_init(ctx, alg, key, key_length ,salt?salt:iv, salt?salt_length:iv_length, false)){
    free(ctx);
    return NULL;
  }
  return ctx;
}

soter_status_t soter_sym_decrypt_update(soter_sym_ctx_t *ctx, const void* cipher_data,  const size_t cipher_data_length, void* plain_data, size_t* plain_data_length){
  return soter_sym_ctx_update(ctx, plain_data, plain_data_length, cipher_data, cipher_data_length, false);
}

soter_status_t soter_sym_decrypt_destroy(soter_sym_ctx_t *ctx){
  SOTER_CHECK_IN_PARAM(ctx);
  free(ctx);
  return SOTER_SUCCESS;
}

soter_sym_aead_ctx_t* soter_sym_aead_encrypt_create(const uint32_t alg, const void* key, const size_t key_length, const void* salt, const size_t salt_length, const void* iv, const size_t iv_length){
  soter_sym_aead_ctx_t* ctx = malloc(sizeof(soter_sym_aead_ctx_t));
  assert(ctx);
  if(SOTER_SUCCESS!=soter_sym_aead_ctx_init(ctx, alg, key, key_length, salt?salt:iv, salt?salt_length:iv_length, true)){
    free(ctx);
    return NULL; 
  }
  return ctx;
}

soter_status_t soter_sym_aead_encrypt_update(soter_sym_aead_ctx_t *ctx, const void* plain_data,  const size_t plain_data_length, void* cipher_data, size_t* cipher_data_length){
  return soter_sym_aead_ctx_update(ctx, plain_data, plain_data_length, cipher_data, cipher_data_length, true);
}

soter_status_t soter_sym_aead_encrypt_aad(soter_sym_aead_ctx_t *ctx, const void* plain_data,  const size_t plain_data_length){
  return soter_sym_aead_ctx_aad_update(ctx, plain_data, plain_data_length);
}

soter_status_t soter_sym_aead_encrypt_final(soter_sym_aead_ctx_t *ctx, void* auth_tag, size_t* auth_tag_length){
  return soter_sym_aead_ctx_final(ctx, auth_tag, auth_tag_length, true);
}

soter_status_t soter_sym_aead_encrypt_destroy(soter_sym_aead_ctx_t *ctx){
  return soter_sym_aead_ctx_destroy(ctx);
}

soter_sym_aead_ctx_t* soter_sym_aead_decrypt_create(const uint32_t alg, const void* key, const size_t key_length, const void* salt, const size_t salt_length, const void* iv, const size_t iv_length){
  soter_sym_aead_ctx_t* ctx = malloc(sizeof(soter_sym_aead_ctx_t));
  assert(ctx);
  if(SOTER_SUCCESS!=soter_sym_aead_ctx_init(ctx, alg, key,key_length, salt?salt:iv, salt?salt_length:iv_length, false)){
    free(ctx);
    return NULL;
  }
  return ctx;
}

soter_status_t soter_sym_aead_decrypt_update(soter_sym_aead_ctx_t *ctx, const void* cipher_data,  const size_t cipher_data_length, void* plain_data, size_t* plain_data_length){
  return soter_sym_aead_ctx_update(ctx, cipher_data,  cipher_data_length, plain_data, plain_data_length, false);
}

soter_status_t soter_sym_aead_decrypt_aad(soter_sym_aead_ctx_t *ctx, const void* plain_data,  const size_t plain_data_length){
  size_t tmp=0;
  return soter_sym_aead_ctx_aad_update(ctx, plain_data, plain_data_length);
}

soter_status_t soter_sym_aead_decrypt_final(soter_sym_aead_ctx_t *ctx, const void* auth_tag, const size_t auth_tag_length){
  return  soter_sym_aead_ctx_final(ctx, (void*)auth_tag, (size_t*)&auth_tag_length, false);
}

soter_status_t soter_sym_aead_decrypt_destroy(soter_sym_aead_ctx_t *ctx){
  return soter_sym_aead_ctx_destroy(ctx);
}
