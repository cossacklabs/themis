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
#include <soter/soter_container.h>
#include <soter/soter_rand.h>
#include "soter_engine.h"
#include "soter_ec_p256_m31_key_pair_gen.h"

#include <bearssl.h>

#include <string.h>

struct soter_ec_p256_m31_key_pair_gen_type{
    soter_ec_p256_m31_priv_key_t priv;
    soter_ec_p256_m31_pub_key_t pub;
};

soter_status_t soter_ec_p256_m31_key_pair_gen_init(soter_ec_p256_m31_key_pair_gen_t* ctx){
  SOTER_CHECK_IN_PARAM(ctx);
  if(SOTER_SUCCESS!=soter_rand(ctx->priv.key, sizeof(ctx->priv.key))){
    return SOTER_FAIL;
  }
  ctx->priv.impl.curve=BR_EC_secp256r1;
  ctx->priv.impl.x=ctx->priv.key;
  ctx->priv.impl.xlen=sizeof(ctx->priv.key);
  ctx->pub.impl.curve=BR_EC_secp256r1;
  ctx->pub.impl.q=ctx->pub.key;
  ctx->pub.impl.qlen=sizeof(ctx->pub.key);
  br_ec_p256_m31.mulgen(ctx->pub.key, ctx->priv.impl.x, ctx->priv.impl.xlen, BR_EC_secp256r1);

  memcpy(ctx->priv.hdr.tag, SOTER_EC_P256_M31_PRIV_KEY_TAG, SOTER_CONTAINER_TAG_LENGTH);
  ctx->priv.hdr.size=htonl(sizeof(soter_ec_p256_m31_priv_key_t));
  soter_update_container_checksum((soter_container_hdr_t*)&(ctx->priv));
  memcpy(ctx->pub.hdr.tag, SOTER_EC_P256_M31_PUB_KEY_TAG, SOTER_CONTAINER_TAG_LENGTH);
  ctx->pub.hdr.size=htonl(sizeof(soter_ec_p256_m31_pub_key_t));
  soter_update_container_checksum((soter_container_hdr_t*)&(ctx->pub));
  return SOTER_SUCCESS;
}

soter_ec_p256_m31_key_pair_gen_t* soter_ec_p256_m31_key_pair_gen_create(){
  soter_ec_p256_m31_key_pair_gen_t *ctx = calloc(1, sizeof(soter_ec_p256_m31_key_pair_gen_t));
  assert(ctx);
  if(SOTER_SUCCESS != soter_ec_p256_m31_key_pair_gen_init(ctx)){
    soter_ec_p256_m31_key_pair_gen_destroy(ctx);
    return NULL;
  }
  return ctx;
}

soter_status_t soter_ec_p256_m31_key_pair_gen_cleanup(soter_ec_p256_m31_key_pair_gen_t* ctx){
  SOTER_CHECK_IN_PARAM(ctx);
  memset(ctx, 0, sizeof(soter_ec_p256_m31_key_pair_gen_t));
  return SOTER_SUCCESS;
}

soter_status_t soter_ec_p256_m31_key_pair_gen_destroy(soter_ec_p256_m31_key_pair_gen_t* ctx){
  SOTER_CHECK_IN_PARAM(ctx);
  soter_ec_p256_m31_key_pair_gen_cleanup(ctx);
  free(ctx);
  return SOTER_SUCCESS;
}

soter_status_t soter_ec_p256_m31_key_pair_gen_export_key(soter_ec_p256_m31_key_pair_gen_t* ctx, void* key, size_t* key_length, bool isprivate){
    if(!ctx){
	return SOTER_INVALID_PARAMETER;
    }
    if (isprivate)
    {
      if(!key || (*key_length)<(sizeof(soter_ec_p256_m31_priv_key_t))){
        *key_length=sizeof(soter_ec_p256_m31_priv_key_t);
        return SOTER_BUFFER_TOO_SMALL;
      }
      memcpy(key, &(ctx->priv), sizeof(soter_ec_p256_m31_priv_key_t));
      *key_length=sizeof(soter_ec_p256_m31_priv_key_t);
      return SOTER_SUCCESS;
    }
    if(!key || (*key_length)<(sizeof(soter_ec_p256_m31_pub_key_t))){
        *key_length=sizeof(soter_ec_p256_m31_pub_key_t);
        return SOTER_BUFFER_TOO_SMALL;
    }
    memcpy(key, &(ctx->pub), sizeof(soter_ec_p256_m31_pub_key_t));
    *key_length=sizeof(soter_ec_p256_m31_pub_key_t);
    return SOTER_SUCCESS;
}
