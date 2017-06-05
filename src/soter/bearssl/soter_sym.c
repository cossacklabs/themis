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

size_t soter_sym_get_key_length(alg){
  switch(alg){
  case  SOTER_SYM_AEAD_SYM_AES_GCM_256:
    return SOTER_SYM_AEAD_DEFAULT_ALG_KEY_LENGTH;
  case  SOTER_SYM_SYM_AES_GCM_256:
    return SOTER_SYM_DEFAULT_ALG_KEY_LENGTH;
  default:
    return 0;
  }
}

size_t soter_sym_get_salt_length(alg){
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
  if(!ctx || !key || !key_length || key_length!=soter_sym_get_key_length(alg) || !salt || !salt_length || salt_length!=soter_sym_get_salt_length()){
    return SOTER_INVALID_PARAMETER;
  }
  ctx->alg=alg;  
  memcpy(ctx->nonce, salt, salt_length);
  return SOTER_SUCCESS;
}


soter_status_t soter_sym_aead_ctx_init(soter_sym_aead_ctx_t* ctx,
                                       const uint32_t alg,
                                       const void* key,
                                       const size_t key_length,
                                       const void* salt,
                                       const size_t salt_length,
                                       bool encrypt){
  if(!ctx || !key || !key_length || key_length!=soter_sym_get_key_length(alg) || !salt || !salt_length || salt_length!=soter_sym_get_salt_length()){
    return SOTER_INVALID_PARAMETER;
  }
  ctx->alg=alg;
  switch(alg){
  case SOTER_SYM_AEAD_SYM_AES_GCM_256:
    if(encrypt){
      br_sslrec_out_gcm_vtable.init(&(ctx.impl.gcm.out), &br_aes_big_ctr_vtable, key, key_len, br_ghash_ctmul, salt);
    }else{
      br_sslrec_in_gcm_vtable.init(&(ctx.impl.gcm.in), &br_aes_big_ctr_vtable, key, key_len, br_ghash_ctmul, salt);
    }
  default:
    return SOTER_INALID_PARAMETER;
  }
  return SOTER_SUCCESS;
}

soter_status_t soter_sym_ctx_update(soter_sym_ctx_t *ctx,
                                    const void* in_data,
                                    const size_t in_data_length,
                                    void* out_data,
                                    size_t* out_data_length,
                                    bool encrypt){
  if(!ctx || !in_data || !in_data_length){
    return SOTER_INVALID_PARAMETER;
  }
  if(!out_data || *out_data_length<in_data_length){
    *out_data_length=in_data_length;
    return SOTER_BUFFER_TOO_SMALL;
  }
  if(0!=crypto_stream_chacha20_xor_ic(out_data, in_data, in_data_length, ctx->nonce, ctx->count, ctx->key)){
    return SOTER_FAIL;
  }
  *out_data_length=in_data_length;
//  ++(*((uint64_t*)(ctx->nonce)));
//  ++(ctx->count);
  return SOTER_SUCCESS;
}



soter_status_t soter_sym_aead_ctx_aad_update(soter_sym_aead_ctx_t *ctx,
                                	     const void* in_data,
                                    	     const size_t in_data_length){
  soter_status_t res;
  void *tmp=malloc(in_data_length);
  size_t tmp_length=in_data_length;
  assert(tmp);
  if(SOTER_SUCCESS!=(res=soter_sym_ctx_update(&(ctx->ctx), in_data, in_data_length, tmp, &tmp_length, true))){
    free(tmp);
    return res;
  }
  if(0!=crypto_onetimeauth_poly1305_update(&(ctx->state), tmp, tmp_length)){
    free(tmp);
    return SOTER_FAIL;
  }
  free(tmp);
  return SOTER_SUCCESS;
}

soter_status_t soter_sym_aead_ctx_encrypt_update(soter_sym_aead_ctx_t *ctx,
                                         const void* in_data,
                                         const size_t in_data_length,
                                         void* out_data,
                                         size_t* out_data_length){
  soter_status_t res;
  if(SOTER_SUCCESS!=(res=soter_sym_ctx_update(&(ctx->ctx), in_data, in_data_length, out_data, out_data_length, true))){
    return res;
  }
  if(0!=crypto_onetimeauth_poly1305_update(&(ctx->state), out_data, *out_data_length)){
    return SOTER_FAIL;
  }
  *out_data_length=in_data_length;
  return SOTER_SUCCESS;
}

soter_status_t soter_sym_aead_ctx_decrypt_update(soter_sym_aead_ctx_t *ctx,
                                         const void* in_data,
                                         const size_t in_data_length,
                                         void* out_data,
                                         size_t* out_data_length){
  soter_status_t res;
  if(SOTER_SUCCESS!=(res=soter_sym_ctx_update(&(ctx->ctx), in_data, in_data_length, out_data, out_data_length, false))){
    return res;
  }
  if(0!=crypto_onetimeauth_poly1305_update(&(ctx->state), in_data, in_data_length)){
    return SOTER_FAIL;
  }
  *out_data_length=in_data_length;
  return SOTER_SUCCESS;
}



soter_status_t soter_sym_aead_ctx_update(soter_sym_aead_ctx_t *ctx,
                                         const void* in_data,
                                         const size_t in_data_length,
                                         void* out_data,
                                         size_t* out_data_length,
                                         bool encrypt){
  if(encrypt){
    return soter_sym_aead_ctx_encrypt_update(ctx, in_data, in_data_length, out_data, out_data_length);
  }
  return soter_sym_aead_ctx_decrypt_update(ctx, in_data, in_data_length, out_data, out_data_length);
}

soter_status_t soter_sym_aead_ctx_final(soter_sym_aead_ctx_t *ctx,
				   void* out_data,
				   size_t* out_data_length,
				   bool encrypt){
  if(encrypt){
    if(!out_data || !(*out_data_length) || (*out_data_length)<crypto_aead_chacha20poly1305_ABYTES){
	*out_data_length=crypto_aead_chacha20poly1305_ABYTES;
	return SOTER_BUFFER_TOO_SMALL;
    }
    crypto_onetimeauth_poly1305_final(&(ctx->state), out_data);
    sodium_memzero(&(ctx->state), sizeof(ctx->state));
  } else {
    if(!out_data || (*out_data_length)!=crypto_aead_chacha20poly1305_ABYTES){
	return SOTER_FAIL;
    }
    unsigned char computed_mac[crypto_aead_chacha20poly1305_ABYTES];
    crypto_onetimeauth_poly1305_final(&(ctx->state), computed_mac);
    if(0!=crypto_verify_16(computed_mac, out_data)){
	return SOTER_FAIL;
    }
  }    
  return SOTER_SUCCESS;
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
  return NULL;
}

soter_status_t soter_sym_encrypt_update(soter_sym_ctx_t *ctx, const void* plain_data,  const size_t plain_data_length, void* cipher_data, size_t* cipher_data_length){
  return SOTER_NOT_SUPPORTED;
}

soter_status_t soter_sym_encrypt_final(soter_sym_ctx_t *ctx, void* cipher_data, size_t* cipher_data_length){
  return SOTER_NOT_SUPPORTED;
}

soter_status_t soter_sym_encrypt_destroy(soter_sym_ctx_t *ctx){
  return SOTER_NOT_SUPPORTED;
}

soter_sym_ctx_t* soter_sym_decrypt_create(const uint32_t alg, const void* key, const size_t key_length, const void* salt, const size_t salt_length, const void* iv, const size_t iv_length){
  return NULL;
}

soter_status_t soter_sym_decrypt_update(soter_sym_ctx_t *ctx, const void* cipher_data,  const size_t cipher_data_length, void* plain_data, size_t* plain_data_length){
  return SOTER_NOT_SUPPORTED;
}

soter_status_t soter_sym_decrypt_final(soter_sym_ctx_t *ctx, void* plain_data, size_t* plain_data_length){
  return SOTER_NOT_SUPPORTED;
}

soter_status_t soter_sym_decrypt_destroy(soter_sym_ctx_t *ctx){
  return SOTER_NOT_SUPPORTED;
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
  if(!auth_tag || (*auth_tag_length)<crypto_aead_chacha20poly1305_ABYTES){
    (*auth_tag_length)=crypto_aead_chacha20poly1305_ABYTES;
    return SOTER_BUFFER_TOO_SMALL;
  }
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
  if(plain_data==NULL || (*plain_data_length)<cipher_data_length){
    (*plain_data_length)=cipher_data_length;
    return SOTER_BUFFER_TOO_SMALL;
  }
  return soter_sym_aead_ctx_update(ctx, cipher_data,  cipher_data_length, plain_data, plain_data_length, false);
}

soter_status_t soter_sym_aead_decrypt_aad(soter_sym_aead_ctx_t *ctx, const void* plain_data,  const size_t plain_data_length){
  size_t tmp=0;
  return soter_sym_aead_ctx_aad_update(ctx, plain_data, plain_data_length);
}

soter_status_t soter_sym_aead_decrypt_final(soter_sym_aead_ctx_t *ctx, const void* auth_tag, const size_t auth_tag_length)
{
  if(!ctx || !auth_tag || auth_tag_length<crypto_aead_chacha20poly1305_ABYTES){
    return SOTER_INVALID_PARAMETER;
  }
  return  soter_sym_aead_ctx_final(ctx, (void*)auth_tag, (size_t*)&auth_tag_length, false);
}

soter_status_t soter_sym_aead_decrypt_destroy(soter_sym_aead_ctx_t *ctx){
  return soter_sym_aead_ctx_destroy(ctx);
}
