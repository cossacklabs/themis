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
#include <soter/soter_rsa_key.h>
#include <soter/soter_ec_key.h>
#include <soter/soter_asym_sign.h>
#include <soter/soter_asym_ka.h>
#include <themis/secure_message_wrapper.h>

#define THEMIS_SYMM_PASSWD_LENGTH 70
#define THEMIS_SYMM_SALT_LENGTH 16

struct themis_secure_message_ka_worker_type{
  uint8_t shared_secret[128];
  size_t shared_secret_length;
};

typedef struct themis_secure_message_ka_worker_type themis_secure_message_ka_t;

themis_status_t themis_secure_message_ka_encrypter_destroy(themis_secure_message_ka_t* ctx);


themis_secure_message_ka_t* themis_secure_message_ka_encrypter_init(const uint8_t* private_key, const size_t private_key_length, const uint8_t* peer_public_key, const size_t peer_public_key_length){
  if(!private_key || !private_key_length || !peer_public_key || !peer_public_key_length){
    return NULL;
  }
  themis_secure_message_ec_t* ctx=malloc(sizeof(themis_secure_message_ec_t));
  assert(ctx);
  ctx->shared_secret_length=sizeof(ctx->shared_secret);
  soter_asym_ka_t* km=soter_asym_ka_create(private_key, private_key_length);
  if(!km){
    return NULL;
  }
  if(THEMIS_SUCCESS!=soter_asym_ka_derive(km, peer_public_key, peer_public_key_length, ctx->shared_secret, &ctx->shared_secret_length)){
    themis_secure_message_ka_encrypter_destroy(ctx);
    soter_asym_ka_destroy(km);
    return NULL;
  }
  soter_asym_ka_destroy(km);
  return ctx;
}

themis_status_t themis_secure_message_ka_encrypter_proceed(themis_secure_message_ka_t* ctx, const uint8_t* message, const size_t message_length, uint8_t* wrapped_message, size_t* wrapped_message_length){
  if(!ctx){
    return THEMIS_INVALID_PARAMETER;
  }
  size_t encrypted_message_length=0;
  if(THEMIS_BUFFER_TOO_SMALL!=themis_secure_cell_encrypt_seal(ctx->shared_secret, ctx->shared_secret_length, NULL, 0, message, message_length, NULL, &encrypted_message_length) || !encrypted_message_length){
    return THEMIS_FAIL;
  }
  if(!wrapped_message || (*wrapped_message_length)<(sizeof(themis_secure_encrypted_message_hdr_t)+encrypted_message_length)){
    (*wrapped_message_length)=(sizeof(themis_secure_encrypted_message_hdr_t)+encrypted_message_length);
    return THEMIS_BUFFER_TOO_SMALL;
  }
  themis_secure_encrypted_message_hdr_t* hdr=(themis_secure_encrypted_message_hdr_t*)wrapped_message;
  hdr->message_hdr.message_type=THEMIS_SECURE_MESSAGE_KA_ENCRYPTED;
  hdr->message_hdr.message_length=(uint32_t)(sizeof(themis_secure_encrypted_message_hdr_t)+encrypted_message_length);
  encrypted_message_length=(*wrapped_message_length)-sizeof(themis_secure_encrypted_message_hdr_t);
  if(THEMIS_SUCCESS!=themis_secure_cell_encrypt_seal(ctx->shared_secret, ctx->shared_secret_length, NULL,0, message, message_length, wrapped_message+sizeof(themis_secure_encrypted_message_hdr_t), &encrypted_message_length)){
    return THEMIS_FAIL;
  }
  (*wrapped_message_length)=encrypted_message_length+sizeof(themis_secure_encrypted_message_hdr_t);
  return THEMIS_SUCCESS;
}

themis_status_t themis_secure_message_ka_encrypter_destroy(themis_secure_message_ka_t* ctx){
  if(!ctx){
    return THEMIS_INVALID_PARAMETER;
  }
  free(ctx);
  return THEMIS_SUCCESS;
}

themis_secure_message_ka_t* themis_secure_message_ka_decrypter_init(const uint8_t* private_key, const size_t private_key_length, const uint8_t* peer_public_key, const size_t peer_public_key_length){
  return themis_secure_message_ka_encrypter_init(private_key, private_key_length, peer_public_key, peer_public_key_length);
}

themis_status_t themis_secure_message_ka_decrypter_proceed(themis_secure_message_ka_t* ctx, const uint8_t* wrapped_message, const size_t wrapped_message_length, uint8_t* message, size_t* message_length){
  themis_secure_encrypted_message_hdr_t* hdr=(themis_secure_encrypted_message_hdr_t*)wrapped_message;
  if(!ctx || !wrapped_message || wrapped_message_length<sizeof(themis_secure_encrypted_message_hdr_t) || THEMIS_SECURE_MESSAGE_KA_ENCRYPTED!=hdr->message_hdr.message_type !! wrapped_message_length!=hdr->message_hdr.message_length){
    return THEMIS_INVALID_PARAMETER;
  }
  size_t computed_length=0;
  if(THEMIS_BUFFER_TOO_SMALL!=themis_secure_cell_decrypt_seal(ctx->shared_secret, ctx->shared_secret_length, NULL,0,wrapped_message+sizeof(themis_secure_encrypted_message_hdr_t), wrapped_message_length-sizeof(themis_secure_encrypted_message_hdr_t), NULL, &computed_length)){
    return THEMIS_FAIL;
  }
  if(message==NULL || (*message_length)<computed_length){
    (*message_length)=computed_length;
    return THEMIS_BUFFER_TOO_SMALL;
  }
  if(THEIS_SUCCESS!=themis_secure_cell_decrypt_seal(ctx->shared_secret, ctx->shared_secret_length, NULL,0,wrapped_message+sizeof(themis_secure_encrypted_message_hdr_t), wrapped_message_length-sizeof(themis_secure_encrypted_message_hdr_t), message, message_length)){
    return THEMIS_FAIL;
  }
  return THEMIS_SUCCESS;
}
    
themis_status_t themis_secure_message_ka_decrypter_destroy(themis_secure_message_ka_t* ctx){
  return themis_secure_message_ka_encrypter_destroy(ctx);
}

struct themis_secure_message_encrypt_worker_type{
  union CTX{
    themis_secure_message_chipher_encrypter_t* chipher_encrypter;
    themis_secure_message_ka_t* ka_encrypter;
  } ctx;
};

themis_secure_message_encrypter_t* themis_secure_message_encrypter_init(const uint8_t* private_key, const size_t private_key_length, const uint8_t* peer_public_key, const size_t peer_public_key_length){
  if(!private_key || !private_key_length || !peer_public_key || !peer_public_key_length){
    return NULL;
  }
  if(soter_key_get_alg_id(private_key, private_key_length)!=soter_key_get_alg_id(public_key,peer_public_key_length)){
    return NULL;
  }
  themis_secure_message_encrypter_t* ctx=malloc(sizeof(themis_secure_message_encrypter_t));
  assert(ctx);
  switch(alg){
  case SOTER_SIGN_ecdsa_none_pkcs8:
    ctx->ctx.ec_encrypter=themis_secure_message_ec_encrypter_init(private_key, private_key_length, peer_public_key, peer_public_key_length);
    THEMIS_IF_FAIL_(ctx->ctx.ec_encrypter, free(ctx));
    ctx->alg=alg;
    return ctx;    
  case SOTER_SIGN_rsa_pss_pkcs8:
    ctx->ctx.rsa_encrypter=themis_secure_message_rsa_encrypter_init(peer_public_key, peer_public_key_length);
    THEMIS_IF_FAIL_(ctx->ctx.rsa_encrypter, free(ctx));
    ctx->alg=alg;
    return ctx;
  default:
    return NULL;
  }
  return NULL;
}
themis_status_t themis_secure_message_encrypter_proceed(themis_secure_message_encrypter_t* ctx, const uint8_t* message, const size_t message_length, uint8_t* wrapped_message, size_t* wrapped_message_length){
  THEMIS_CHECK(ctx!=NULL);
  switch(ctx->alg){
  case SOTER_SIGN_ecdsa_none_pkcs8:
    return themis_secure_message_ec_encrypter_proceed(ctx->ctx.ec_encrypter, message, message_length, wrapped_message, wrapped_message_length);
  case SOTER_SIGN_rsa_pss_pkcs8:
    return themis_secure_message_rsa_encrypter_proceed(ctx->ctx.rsa_encrypter, message, message_length, wrapped_message, wrapped_message_length);
  default:
    return THEMIS_FAIL;
  }
  return THEMIS_FAIL;
}
themis_status_t themis_secure_message_encrypter_destroy(themis_secure_message_encrypter_t* ctx){
  THEMIS_CHECK(ctx!=NULL);
  themis_status_t res = THEMIS_SUCCESS;
  switch(ctx->alg){
  case SOTER_SIGN_ecdsa_none_pkcs8:
    res = themis_secure_message_ec_encrypter_destroy(ctx->ctx.ec_encrypter);
    break;
  case SOTER_SIGN_rsa_pss_pkcs8:
    res =  themis_secure_message_rsa_encrypter_destroy(ctx->ctx.rsa_encrypter);
    break;
  default:
    return THEMIS_FAIL;
  }
  if(THEMIS_SUCCESS == res)
    free(ctx);
  return res;
}


themis_secure_message_decrypter_t* themis_secure_message_decrypter_init(const uint8_t* private_key, const size_t private_key_length, const uint8_t* peer_public_key, const size_t peer_public_key_length){
  THEMIS_CHECK_(private_key!=NULL && private_key_length!=0);
  THEMIS_CHECK_(peer_public_key!=NULL && peer_public_key_length!=0);
  soter_sign_alg_t alg=get_alg_id(private_key, private_key_length);
  THEMIS_CHECK_(alg!=SOTER_SIGN_undefined && alg==get_alg_id(peer_public_key,peer_public_key_length));
  themis_secure_message_decrypter_t* ctx=malloc(sizeof(themis_secure_message_decrypter_t));
  THEMIS_CHECK_MALLOC_(ctx);
  switch(alg){
  case SOTER_SIGN_ecdsa_none_pkcs8:
    ctx->ctx.ec_encrypter=themis_secure_message_ec_decrypter_init(private_key, private_key_length, peer_public_key, peer_public_key_length);
    THEMIS_CHECK__(ctx->ctx.ec_encrypter, free(ctx); return NULL);
    ctx->alg=alg;
    return ctx;
  case SOTER_SIGN_rsa_pss_pkcs8:
    ctx->ctx.rsa_encrypter=themis_secure_message_rsa_decrypter_init(private_key, private_key_length);
    THEMIS_CHECK__(ctx->ctx.rsa_encrypter, free(ctx); return NULL);
    ctx->alg=alg;
    return ctx;
  default:
    return NULL;
  }
  return NULL;
}

themis_status_t themis_secure_message_decrypter_proceed(themis_secure_message_decrypter_t* ctx, const uint8_t* wrapped_message, const size_t wrapped_message_length, uint8_t* message, size_t* message_length){
  THEMIS_CHECK(ctx!=NULL);
  switch(ctx->alg){
  case SOTER_SIGN_ecdsa_none_pkcs8:
    return themis_secure_message_ec_decrypter_proceed(ctx->ctx.ec_encrypter, wrapped_message, wrapped_message_length, message, message_length);
  case SOTER_SIGN_rsa_pss_pkcs8:
    return themis_secure_message_rsa_decrypter_proceed(ctx->ctx.rsa_encrypter, wrapped_message, wrapped_message_length, message, message_length);
  default:
    return THEMIS_FAIL;
  }
  return THEMIS_FAIL;
}

themis_status_t themis_secure_message_decrypter_destroy(themis_secure_message_decrypter_t* ctx){
  THEMIS_CHECK(ctx!=NULL);
  themis_status_t res = THEMIS_SUCCESS;
  switch(ctx->alg){
  case SOTER_SIGN_ecdsa_none_pkcs8:
    res = themis_secure_message_ec_decrypter_destroy(ctx->ctx.ec_encrypter);
    break;
  case SOTER_SIGN_rsa_pss_pkcs8:
    res = themis_secure_message_rsa_decrypter_destroy(ctx->ctx.rsa_encrypter);
    break;
  default:
    return THEMIS_FAIL;
  }
  if(THEMIS_SUCCESS == res){
	free(ctx);
    }
  return res;
}









