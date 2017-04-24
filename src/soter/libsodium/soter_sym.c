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
#include <sodium.h>


#include "soter_engine_consts.h"

//soter_status_t soter_pbkdf2(const uint8_t* password, const size_t password_length, const uint8_t* salt, const size_t salt_length, uint8_t* key, size_t* key_length){
//  if(!PKCS5_PBKDF2_HMAC((const char*)password, (const int)password_length, salt, (const int)salt_length, 0, EVP_sha256(), (const int)(*key_length), key)){
//      return SOTER_FAIL;
//    }
//  return SOTER_SUCCESS;
//}

soter_status_t soter_sym_ctx_init(soter_sym_ctx_t* ctx,
                                  const uint32_t alg,
                                  const void* key,
                                  const size_t key_length,
                                  const void* salt,
                                  const size_t salt_length,
                                  bool encrypt){
  if(!ctx || !key || key_length!=crypto_stream_chacha20_KEYBYTES || !salt || salt_length!=crypto_stream_chacha20_NONCEBYTES){
    return SOTER_INVALID_PARAMETER;
  }
  ctx->alg=alg;
  memcpy(ctx->key, key, key_length);
  memcpy(ctx->nonce, salt, salt_length);
  ctx->count=0;
  return SOTER_SUCCESS;
}


soter_status_t soter_sym_aead_ctx_init(soter_sym_aead_ctx_t* ctx,
                                       const uint32_t alg,
                                       const void* key,
                                       const size_t key_length,
                                       const void* salt,
                                       const size_t salt_length,
                                       bool encrypt){
  if(!key || key_length!=crypto_stream_chacha20_KEYBYTES || !salt || salt_length!=crypto_stream_chacha20_NONCEBYTES){
    return SOTER_INVALID_PARAMETER;
  }
  soter_status_t res;
  if(SOTER_SUCCESS!=(res=soter_sym_ctx_init(&(ctx->ctx), alg, key, key_length, salt, salt_length, encrypt))){
    return res;   
  }
  unsigned char block0[64U];
  unsigned char slen[8U];
  crypto_stream_chacha20(block0, sizeof(block0), salt, key);
  crypto_onetimeauth_poly1305_init(&(ctx->state), block0);
  sodium_memzero(block0, sizeof(block0));
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
  ++(*((uint64_t*)(ctx->nonce)));
  ++(ctx->count);
  return SOTER_SUCCESS;
}

soter_status_t soter_sym_aead_ctx_update(soter_sym_aead_ctx_t *ctx,
                                         const void* in_data,
                                         const size_t in_data_length,
                                         void* out_data,
                                         size_t* out_data_length,
                                         bool encrypt){
  soter_status_t res;
  void *tmp=NULL;
  if(out_data){
    tmp=out_data;
  }else{
    tmp=malloc(in_data_length);
    assert(tmp);
    *out_data_length=in_data_length;
  }
  if(SOTER_SUCCESS!=(res=soter_sym_ctx_update(&(ctx->ctx), in_data, in_data_length, tmp, out_data_length, encrypt))){
    if(!out_data){
      free(tmp);
    }
    return res;
  }
  if(0!=crypto_onetimeauth_poly1305_update(&(ctx->state), tmp, in_data_length) || 0!=crypto_onetimeauth_poly1305_update(&(ctx->state), (void*)(*out_data_length), sizeof(uint32_t))){
    if(!out_data){
      free(tmp);
    }
    return SOTER_FAIL;
  }
  if(!out_data){
    free(tmp);
  }
  *out_data_length=in_data_length;
  return SOTER_SUCCESS;
}

soter_status_t soter_sym_aead_ctx_final(soter_sym_aead_ctx_t *ctx,
				   void* out_data,
				   size_t* out_data_length,
				   bool encrypt){
  if(encrypt){
    if(!out_data || !(*out_data_length)){
	*out_data_length=crypto_aead_chacha20poly1305_ABYTES;
	return SOTER_BUFFER_TOO_SMALL;
    }
    crypto_onetimeauth_poly1305_final(&(ctx->state), out_data);
    sodium_memzero(&(ctx->state), sizeof(ctx->state));
  } else {
    if(!out_data || (*out_data_length)<crypto_aead_chacha20poly1305_ABYTES){
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
  if(THEMIS_SUCCESS!=soter_sym_aead_ctx_init(ctx, alg, key, key_length, salt, salt_length, true)){
    free(ctx);
    return NULL;
  }
  return ctx;
}

soter_status_t soter_sym_aead_encrypt_update(soter_sym_ctx_t *ctx, const void* plain_data,  const size_t plain_data_length, void* cipher_data, size_t* cipher_data_length){
  return soter_sym_aead_ctx_update(ctx, plain_data, plain_data_length, cipher_data, cipher_data_length, true);
}

soter_status_t soter_sym_aead_encrypt_aad(soter_sym_ctx_t *ctx, const void* plain_data,  const size_t plain_data_length){
  size_t tmp=0;
  return soter_sym_aead_ctx_update(ctx, plain_data, plain_data_length, NULL, &tmp, true);
}

soter_status_t soter_sym_aead_encrypt_final(soter_sym_ctx_t *ctx, void* auth_tag, size_t* auth_tag_length){
  SOTER_CHECK_PARAM(auth_tag!=NULL);
  if((*auth_tag_length)<crypto_aead_chacha20poly1305_ABYTES){
    (*auth_tag_length)=crypto_aead_chacha20poly1305_ABYTES;
    return SOTER_BUFFER_TOO_SMALL;
  }
  SOTER_CHECK(soter_sym_aead_ctx_final(ctx, auth_tag, auth_tag_size, true)==SOTER_SUCCESS);
  return SOTER_SUCCESS;
}

soter_status_t soter_sym_aead_encrypt_destroy(soter_sym_ctx_t *ctx){
  return soter_sym_ctx_destroy(ctx);
}

soter_sym_ctx_t* soter_sym_aead_decrypt_create(const uint32_t alg, const void* key, const size_t key_length, const void* salt, const size_t salt_length, const void* iv, const size_t iv_length){
  return soter_sym_aead_ctx_init(alg, key,key_length, salt, salt_length, false);
}

soter_status_t soter_sym_aead_decrypt_update(soter_sym_ctx_t *ctx, const void* cipher_data,  const size_t cipher_data_length, void* plain_data, size_t* plain_data_length){
  if(plain_data==NULL || (*plain_data_length)<cipher_data_length){
    (*plain_data_length)=cipher_data_length;
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
  if(!ctx || !auth_tag || auth_tag_length<crypto_aead_chacha20poly1305_ABYTES){
    return SOTER_INALID_PARAMETER;
  }
  return  soter_sym_aead_ctx_final(ctx, auth_tag, &auth_tag_length, false);
}
soter_status_t soter_sym_aead_decrypt_destroy(soter_sym_ctx_t *ctx){
  return soter_sym_ctx_destroy(ctx);
}
