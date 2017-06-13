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
#include "soter_engine.h"
#include "soter_engine_consts.h"
#include <soter/soter_container.h>

#include "soter_ec_p256_m31_key.h"

soter_status_t soter_asym_ka_init(soter_asym_ka_t* ctx, const uint8_t* key, const size_t key_length){
  SOTER_CHECK_IN_PARAM(ctx);
  SOTER_CHECK_IN_PARAM(key);
  SOTER_CHECK_IN_PARAM(sizeof(soter_container_hdr_t)<key_length);
  soter_status_t res=SOTER_FAIL;
  switch(soter_key_get_alg_id(key, key_length)){
  case SOTER_ASYM_EC_P256_M31:
    res=soter_ec_p256_m31_byte_array_to_priv_key(key, key_length, &(ctx->pk.ec_p256_m31));
    break;
  default:
    return SOTER_INVALID_PARAMETER;
  }
  return res;
}

soter_status_t soter_asym_ka_cleanup(soter_asym_ka_t* ctx){
  if (!ctx){
    return SOTER_INVALID_PARAMETER;
  }
  memset(ctx, 0, sizeof(soter_asym_ka_t));
  return SOTER_SUCCESS;
}
                                  
soter_asym_ka_t* soter_asym_ka_create(const uint8_t* key, const size_t key_length)
{
  soter_asym_ka_t *ctx = malloc(sizeof(soter_asym_ka_t));
  assert(ctx);
  if(SOTER_SUCCESS != soter_asym_ka_init(ctx, key, key_length)) {
    free(ctx);
    return NULL;
  }
  return ctx;
}

soter_status_t soter_asym_ka_destroy(soter_asym_ka_t* asym_ka_ctx)
{
  if (!asym_ka_ctx){
    return SOTER_INVALID_PARAMETER;
  }
  if (SOTER_SUCCESS != soter_asym_ka_cleanup(asym_ka_ctx)){
    return SOTER_FAIL;
  }
  free(asym_ka_ctx);
  return SOTER_SUCCESS;
}

soter_status_t soter_asym_ka_derive(soter_asym_ka_t* ctx, const void* peer_key, size_t peer_key_length, void *shared_secret, size_t* shared_secret_length){
  SOTER_CHECK_IN_PARAM(ctx);
  SOTER_CHECK_IN_PARAM(shared_secret_length);
  soter_status_t res=SOTER_FAIL;
  switch(soter_key_get_alg_id((uint8_t*)&(ctx->pk), sizeof(ctx->pk))){
  case SOTER_ASYM_EC_P256_M31:{
    SOTER_CHECK_OUT_BUF_PARAM(shared_secret, shared_secret_length, SOTER_ASYM_EC_P256_M31_DH_SHARED_SECRET_LENGTH);
    soter_ec_p256_m31_pub_key_t pk;
    res=soter_ec_p256_m31_byte_array_to_pub_key(peer_key, peer_key_length, &pk);
    if(SOTER_SUCCESS!=res){
      return res;
    }
    memcpy(shared_secret, pk.key, SOTER_EC_P256_M31_PUB_SIZE);
    *shared_secret_length=SOTER_EC_P256_M31_PUB_SIZE;
    if(!br_ec_p256_m31.mul(shared_secret, *shared_secret_length, ctx->pk.ec_p256_m31.key, sizeof(ctx->pk.ec_p256_m31.key), BR_EC_secp256r1)){
      return SOTER_FAIL;
    }
    return SOTER_SUCCESS;
  }
    break;
  default:
    return SOTER_INVALID_PARAMETER;
  }
  return SOTER_SUCCESS;
}
