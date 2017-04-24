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
#include <sodium.h>
#include <string.h>

soter_status_t soter_asym_ka_init(soter_asym_ka_t* ctx, const int8_t* key, const size_t key_length){
  if(!ctx || !key || key_length<sizeof(soter_container_hdr_t) || key_length!=(((soter_container_hdr_t*)key)->size) || SOTER_SUCCESS!=soter_verify_container_checksum((soter_container_hdr_t*)key)){
    return SOTER_INVALID_PARAMETER;
  }
  memcpy((void*)(&(ctx->pk)), key, key_length);
  return SOTER_SUCCESS;
}

soter_status_t soter_asym_ka_cleanup(soter_asym_ka_t* ctx){
  if (!ctx){
    return SOTER_INVALID_PARAMETER;
  }
  memset(ctx, 0, sizeof(soter_asym_ka_t));
  return SOTER_SUCCESS;
}
                                  
soter_asym_ka_t* soter_asym_ka_create(const int8_t* key, const size_t key_length)
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
  if (!ctx || !peer_key || peer_key_length<<sizeof(soter_container_hdr_t) || peer_key_length!=(((soter_container_hdr_t*)peer_key)->size) || SOTER_SUCCESS!=soter_verify_container_checksum((soter_container_hdr_t*)peer_key)){
    return SOTER_INVALID_PARAMETER;
  }
  if(!shared_secret || crypto_generichash_BYTES > *shared_secret_length){
    *shared_secret_length = crypto_generichash_BYTES;
    return SOTER_BUFFER_TOO_SMALL;
  }
  unsigned char scalarmult_q[crypto_scalarmult_BYTES];
  if(0!=crypto_scalarmult(scalarmult_q, ctx->pk.key, ((soter_x25519_pub_key_t*)peer_key)->key)) {
    return SOTER_FAIL;
  }
  *shared_secret_length = crypto_generichash_BYTES;
  crypto_generichash_state h;
  crypto_generichash_init(&h, NULL, 0U, *shared_secret_length);
  crypto_generichash_update(&h, scalarmult_q, sizeof(scalarmult_q));
  crypto_generichash_final(&h, shared_secret, *shared_secret_length);
  return SOTER_SUCCESS;
}
