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


#include <themis/error.h>
#include <soter/soter.h>
#include <themis/secure_message_wrapper.h>
#include <themis/secure_message_wrapper_enc_dec_ka.h>
#include <themis/secure_message_wrapper_enc_dec_chipher.h>

struct themis_secure_message_encrypt_worker_type{
  union CTX{
    themis_secure_message_chipher_encrypter_t* chipher_encrypter;
    themis_secure_message_ka_t* ka_encrypter;
  } ctx;
  bool is_ka_type;
};

themis_secure_message_encrypter_t* themis_secure_message_encrypter_init(const uint8_t* private_key, const size_t private_key_length, const uint8_t* peer_public_key, const size_t peer_public_key_length){
  if(!private_key || !private_key_length || !peer_public_key || !peer_public_key_length){
    return NULL;
  }
  if(soter_key_get_alg_id(private_key, private_key_length)!=soter_key_get_alg_id(peer_public_key,peer_public_key_length)){
    return NULL;
  }
  themis_secure_message_encrypter_t* ctx=malloc(sizeof(themis_secure_message_encrypter_t));
  assert(ctx);
  ctx->ctx.ka_encrypter=themis_secure_message_ka_encrypter_init(private_key, private_key_length, peer_public_key, peer_public_key_length);
  ctx->is_ka_type=true;
  if(!(ctx->ctx.ka_encrypter)){
    ctx->ctx.chipher_encrypter=themis_secure_message_chipher_encrypter_init(peer_public_key, peer_public_key_length);
    ctx->is_ka_type=false;
    if(!(ctx->ctx.chipher_encrypter)){
      free(ctx);
      return NULL;
    }
  }
  return ctx;
}

themis_status_t themis_secure_message_encrypter_proceed(themis_secure_message_encrypter_t* ctx, const uint8_t* message, const size_t message_length, uint8_t* wrapped_message, size_t* wrapped_message_length){
  if(!ctx || !message || !message_length){
    return THEMIS_INVALID_PARAMETER;
  }
  if(ctx->is_ka_type){
    return themis_secure_message_ka_encrypter_proceed(ctx->ctx.ka_encrypter, message, message_length, wrapped_message, wrapped_message_length);
  }
  return themis_secure_message_chipher_encrypter_proceed(ctx->ctx.chipher_encrypter, message, message_length, wrapped_message, wrapped_message_length);
}

themis_status_t themis_secure_message_encrypter_destroy(themis_secure_message_encrypter_t* ctx){
  if(!ctx){
    return THEMIS_INVALID_PARAMETER;
  }
  if(ctx->is_ka_type){
    return themis_secure_message_ka_encrypter_destroy(ctx->ctx.ka_encrypter);
  }
  return themis_secure_message_chipher_encrypter_destroy(ctx->ctx.chipher_encrypter);
}


struct themis_secure_message_decrypt_worker_type{
  uint8_t* sk;
  size_t sk_length;
  uint8_t* pk;
  size_t pk_length;
};

themis_secure_message_decrypter_t* themis_secure_message_decrypter_init(const uint8_t* private_key, const size_t private_key_length, const uint8_t* peer_public_key, const size_t peer_public_key_length){
  if(!private_key || !private_key_length){
    return NULL;
  }
  if(soter_key_get_alg_id(private_key, private_key_length)!=soter_key_get_alg_id(peer_public_key,peer_public_key_length)){
    return NULL;
  }
  themis_secure_message_decrypter_t* ctx=calloc(sizeof(themis_secure_message_decrypter_t),1);
  assert(ctx);
  ctx->sk=malloc(private_key_length);
  assert(ctx->sk);
  ctx->sk_length=private_key_length;
  memcpy(ctx->sk, private_key, ctx->sk_length);
  if(peer_public_key && peer_public_key_length){
    ctx->pk=malloc(peer_public_key_length);
    assert(ctx->pk);
    ctx->pk_length=peer_public_key_length;
    memcpy(ctx->pk, peer_public_key, ctx->pk_length);
  }
  return ctx;  
}

themis_status_t themis_secure_message_decrypter_proceed(themis_secure_message_decrypter_t* ctx, const uint8_t* wrapped_message, const size_t wrapped_message_length, uint8_t* message, size_t* message_length){
  if(!ctx || !wrapped_message || wrapped_message_length<sizeof(themis_secure_encrypted_message_hdr_t)){
    return THEMIS_INVALID_PARAMETER;
  }
  switch(((themis_secure_encrypted_message_hdr_t*)wrapped_message)->message_hdr.message_type){
  case THEMIS_SECURE_MESSAGE_CHIPHER_ENCRYPTED:{
    themis_secure_message_chipher_encrypter_t* decrypter=themis_secure_message_chipher_decrypter_init(ctx->sk, ctx->sk_length);
    if(!decrypter){
      return THEMIS_FAIL;
    }
    soter_status_t res = themis_secure_message_chipher_decrypter_proceed(decrypter, wrapped_message, wrapped_message_length, message, message_length);
    themis_secure_message_chipher_decrypter_destroy(decrypter);
    return res;
  }
    break;
  case THEMIS_SECURE_MESSAGE_KA_ENCRYPTED:{
    themis_secure_message_ka_t* decrypter=themis_secure_message_ka_decrypter_init(ctx->sk, ctx->sk_length, ctx->pk, ctx->pk_length);
    if(!decrypter){
      return THEMIS_FAIL;
    }
    soter_status_t res = themis_secure_message_ka_decrypter_proceed(decrypter, wrapped_message, wrapped_message_length, message, message_length);
    themis_secure_message_ka_decrypter_destroy(decrypter);
    return res;    
  }
  default:
    return THEMIS_INVALID_PARAMETER;
  }
  return THEMIS_FAIL;
}

themis_status_t themis_secure_message_decrypter_destroy(themis_secure_message_decrypter_t* ctx){
  if(!ctx){
    return THEMIS_INVALID_PARAMETER;
  }
  free(ctx->sk);
  free(ctx->pk);
  free(ctx);
  return THEMIS_SUCCESS;
}









