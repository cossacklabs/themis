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

#include <assert.h>

#include <soter/error.h>
#include <soter/soter.h>

#include "soter_engine.h"

#include <soter/soter_container.h>
#include <soter/soter_asym_key.h>
#include <soter/soter_asym_sign.h>
#include "soter_engine_consts.h"

#include <string.h>

soter_status_t soter_sign_init(soter_sign_ctx_t* ctx, const void* private_key, const size_t private_key_length){
  SOTER_CHECK_IN_PARAM(ctx);
  SOTER_CHECK_IN_PARAM(private_key);
  SOTER_CHECK_IN_PARAM(private_key_length>sizeof(soter_container_hdr_t));
  SOTER_CHECK_IN_PARAM(private_key_length==ntohl(((soter_container_hdr_t*)private_key)->size));
  SOTER_CHECK_IN_PARAM(SOTER_SUCCESS==soter_verify_container_checksum((soter_container_hdr_t*)private_key));
  soter_status_t res=SOTER_FAIL;
  res = soter_hash_init(&(ctx->hash), SOTER_HASH_SHA_256);
  if(SOTER_SUCCESS!=res){
    return res;
  }
  ctx->alg=soter_key_get_alg_id(private_key, private_key_length);
  switch(ctx->alg){
  case SOTER_ASYM_EC_P256_M31:
    res=soter_ec_p256_m31_byte_array_to_priv_key(private_key, private_key_length, &(ctx->key.sk.ec_p256_m31));
    break;
  default:
    return SOTER_INVALID_PARAMETER;
  }
  return res;
}

soter_status_t soter_verify_init(soter_sign_ctx_t* ctx, const void* public_key, const size_t public_key_length){
  SOTER_CHECK_IN_PARAM(ctx);
  SOTER_CHECK_IN_PARAM(public_key);
  SOTER_CHECK_IN_PARAM(public_key_length>sizeof(soter_container_hdr_t));
  SOTER_CHECK_IN_PARAM(public_key_length==ntohl(((soter_container_hdr_t*)public_key)->size));
  SOTER_CHECK_IN_PARAM(SOTER_SUCCESS==soter_verify_container_checksum((soter_container_hdr_t*)public_key));
  soter_status_t res=SOTER_FAIL;
  res = soter_hash_init(&(ctx->hash), SOTER_HASH_SHA_256);
  if(SOTER_SUCCESS!=res){
    return res;
  }
  ctx->alg=soter_key_get_alg_id(public_key, public_key_length);
  switch(ctx->alg){
  case SOTER_ASYM_EC_P256_M31:
    res=soter_ec_p256_m31_byte_array_to_pub_key(public_key, public_key_length, &(ctx->key.pk.ec_p256_m31));
    break;
  default:
    return SOTER_INVALID_PARAMETER;
  }
  return res;
}

soter_status_t soter_sign_cleanup(soter_sign_ctx_t* ctx){
  SOTER_CHECK_IN_PARAM(ctx);
  memset(ctx, 0, sizeof(soter_sign_ctx_t));
  return SOTER_SUCCESS;
}

soter_status_t soter_verify_cleanup(soter_sign_ctx_t* ctx){
  SOTER_CHECK_IN_PARAM(ctx);
  memset(ctx, 0, sizeof(soter_sign_ctx_t));
  return SOTER_SUCCESS;
}

soter_status_t soter_sign(const void* private_key, const size_t private_key_length, const uint8_t* data, const size_t data_length, uint8_t* signature, size_t* signature_length){
  soter_sign_ctx_t ctx;
  soter_status_t res = SOTER_SUCCESS;
  res = soter_sign_init(&ctx, private_key, private_key_length) ;
  if(SOTER_SUCCESS == res){
    res = soter_sign_update(&ctx, data, data_length);
    if(SOTER_SUCCESS == res){
      res = soter_sign_final(&ctx, signature, signature_length);
    }
  }
  soter_sign_cleanup(&ctx);
  return res;
}

soter_status_t soter_verify(const void* public_key, const size_t public_key_length, const int8_t* data, const size_t data_length, const int8_t* signature, const size_t signature_length){
  soter_verify_ctx_t ctx;
  soter_status_t res = SOTER_SUCCESS;
  res = soter_verify_init(&ctx, public_key, public_key_length) ;
  if(SOTER_SUCCESS == res){
    res = soter_verify_update(&ctx, data, data_length);
    if(SOTER_SUCCESS == res){
      res = soter_verify_final(&ctx, signature, signature_length);
    }
  }
  soter_verify_cleanup(&ctx);
  return res;
}


soter_status_t soter_sign_update(soter_sign_ctx_t* ctx, const void* data, const size_t data_length){
  SOTER_CHECK_IN_PARAM(ctx);
  SOTER_CHECK_IN_BUF_PARAM_NON_EMPTY(data, data_length);
  return soter_hash_update(&(ctx->hash), data, data_length);
}

soter_status_t soter_verify_update(soter_sign_ctx_t* ctx, const void* data, const size_t data_length){
  return soter_sign_update(ctx, data,data_length);
}

soter_status_t soter_sign_final(soter_sign_ctx_t* ctx, void* signature, size_t* signature_length){
  SOTER_CHECK_IN_PARAM(ctx);
  switch(ctx->alg){
  case SOTER_ASYM_EC_P256_M31:{
    SOTER_CHECK_OUT_BUF_PARAM(signature, signature_length, SOTER_ASYM_EC_P256_M31_SIGNATURE_LENGTH);
    uint8_t hash[SOTER_HASH_SHA_256_LENGTH];
    size_t hash_length=sizeof(hash);
    if(SOTER_SUCCESS!=soter_hash_final(&(ctx->hash), hash, &hash_length)){
      return SOTER_FAIL;
    }
    size_t sig_length = br_ecdsa_i31_sign_raw(&br_ec_prime_i31, &br_sha256_vtable, hash, &(ctx->key.sk.ec_p256_m31.impl),signature);
    if(!sig_length){
      return SOTER_FAIL;
    }
    *signature_length=sig_length;
  }
    break;
  default:
    return SOTER_INVALID_PARAMETER;
  }
  return SOTER_SUCCESS;
}

soter_status_t soter_verify_final(soter_sign_ctx_t* ctx, const void* signature, const size_t signature_length){
  SOTER_CHECK_IN_PARAM(ctx);
  switch(ctx->alg){
  case SOTER_ASYM_EC_P256_M31:{
    SOTER_CHECK_IN_BUF_PARAM(signature, signature_length, SOTER_ASYM_EC_P256_M31_SIGNATURE_LENGTH);
    uint8_t hash[SOTER_HASH_SHA_256_LENGTH];
    size_t hash_length=sizeof(hash);
    if(SOTER_SUCCESS!=soter_hash_final(&(ctx->hash), hash, &hash_length)){
      return SOTER_FAIL;
    }
    if(1 != br_ecdsa_i31_vrfy_raw(&br_ec_prime_i31, hash, hash_length, &(ctx->key.pk.ec_p256_m31.impl), signature, signature_length)){
      return SOTER_INVALID_SIGNATURE;
    }
  }
    break;
  default:
    return SOTER_INVALID_PARAMETER;
  }
  return SOTER_SUCCESS;
}

soter_sign_ctx_t* soter_sign_create(const void* private_key, const size_t private_key_length){
  if(!private_key || !private_key_length){
    return NULL;
  }
  soter_sign_ctx_t* ctx=calloc(sizeof(soter_sign_ctx_t), 1);
  assert(ctx);
  if(SOTER_SUCCESS!=soter_sign_init(ctx, private_key, private_key_length)){
    free(ctx);
    return NULL;
  }
  return ctx;
}

soter_sign_ctx_t* soter_verify_create(const void* public_key, const size_t public_key_length){
  if(!public_key || !public_key_length){
    return NULL;
  }
  soter_sign_ctx_t* ctx=calloc(sizeof(soter_sign_ctx_t), 1);
  assert(ctx);
  if(SOTER_SUCCESS!=soter_verify_init(ctx, public_key, public_key_length)){
    free(ctx);
    return NULL;
  }
  return ctx;
}

soter_status_t soter_sign_destroy(soter_sign_ctx_t* ctx){
  if(!ctx){
    return SOTER_INVALID_PARAMETER;
  }
  soter_sign_cleanup(ctx);
  free(ctx);
  return SOTER_SUCCESS;
}

soter_status_t soter_verify_destroy(soter_sign_ctx_t* ctx){
  if(!ctx){
    return SOTER_INVALID_PARAMETER;
  }
  soter_verify_cleanup(ctx);
  free(ctx);
  return SOTER_SUCCESS;
}
