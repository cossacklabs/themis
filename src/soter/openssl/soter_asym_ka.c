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
#include "soter_ec_key.h"
#include <soter/soter_container.h>
#include <openssl/ec.h>

static int soter_alg_to_curve_nid(const int32_t alg_id, const int32_t length_id)
{
  switch (alg_id&0x00ffff00){
  case SOTER_ASYM_EC:
          switch(length_id){
          case 2:
            return NID_X9_62_prime256v1;
          case 3:
            return NID_secp384r1; 
          case 5:
            return NID_secp521r1;
          default:
            return 0;
          }
  default:
    return 0;
  }
  return 0;
}

soter_status_t soter_asym_ka_init(soter_asym_ka_t* asym_ka_ctx, const int8_t* key, const size_t key_length){
  EVP_PKEY *pkey;
  if(!asym_ka_ctx || !key || key_length<sizeof(soter_container_hdr_t)){
    return SOTER_INVALID_PARAMETER;
  }
  int nid = soter_alg_to_curve_nid(soter_key_get_alg_id(key, key_length), soter_key_get_length_id(key, key_length));
  if(!nid){
    return SOTER_INVALID_PARAMETER;  
  }
  pkey = EVP_PKEY_new();
  if(!pkey){
    return SOTER_NO_MEMORY;
  }
  if(!EVP_PKEY_set_type(pkey, EVP_PKEY_EC))
    {
      EVP_PKEY_free(pkey);
      return SOTER_FAIL;
    }
  asym_ka_ctx->pkey_ctx = EVP_PKEY_CTX_new(pkey, NULL);
  if (!(asym_ka_ctx->pkey_ctx)){
    EVP_PKEY_free(pkey);
    return SOTER_FAIL;
  }
  if (1 != EVP_PKEY_paramgen_init(asym_ka_ctx->pkey_ctx))
    {
      EVP_PKEY_free(pkey);
      EVP_PKEY_CTX_free(asym_ka_ctx->pkey_ctx);
      return SOTER_FAIL;
    }
  if (1 != EVP_PKEY_CTX_set_ec_paramgen_curve_nid(asym_ka_ctx->pkey_ctx, nid)){
    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(asym_ka_ctx->pkey_ctx);
    return SOTER_FAIL;
  }

  if (1 != EVP_PKEY_paramgen(asym_ka_ctx->pkey_ctx, &pkey)){
    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(asym_ka_ctx->pkey_ctx);
    return SOTER_FAIL;
  }
  if (soter_key_is_private(key, key_length)){
    return soter_ec_priv_key_to_engine_specific((soter_container_hdr_t*)key, key_length, ((soter_engine_specific_ec_key_t **)&pkey));
  }
  return soter_ec_pub_key_to_engine_specific((soter_container_hdr_t*)key, key_length, ((soter_engine_specific_ec_key_t **)&pkey));
}

soter_status_t soter_asym_ka_cleanup(soter_asym_ka_t* asym_ka_ctx)
{
  if (!asym_ka_ctx){
    return SOTER_INVALID_PARAMETER;
  }
  if (asym_ka_ctx->pkey_ctx){
    EVP_PKEY *pkey = EVP_PKEY_CTX_get0_pkey(asym_ka_ctx->pkey_ctx);
    if(pkey){
      EVP_PKEY_free(pkey);
    }
    EVP_PKEY_CTX_free(asym_ka_ctx->pkey_ctx);
  }
  return SOTER_SUCCESS;
}
                                  
soter_asym_ka_t* soter_asym_ka_create(const int8_t* key, const size_t key_length)
{
  soter_asym_ka_t *ctx = malloc(sizeof(soter_asym_ka_t));
  assert(ctx);
  if(SOTER_SUCCESS != soter_asym_ka_init(ctx, key, key_length)) {
    soter_asym_ka_destroy(ctx);
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

soter_status_t soter_asym_ka_derive(soter_asym_ka_t* asym_ka_ctx, const void* peer_key, size_t peer_key_length, void *shared_secret, size_t* shared_secret_length)
{
  soter_status_t res;
  size_t out_length;
  if ((!asym_ka_ctx) || (!shared_secret_length)){
    return SOTER_INVALID_PARAMETER;
  }
  EVP_PKEY *peer_pkey = EVP_PKEY_new();
  assert(peer_pkey);
  if (SOTER_SUCCESS != soter_ec_pub_key_to_engine_specific((const soter_container_hdr_t *)peer_key, peer_key_length, ((soter_engine_specific_ec_key_t **)&peer_pkey))){
    EVP_PKEY_free(peer_pkey);
    return SOTER_INVALID_PARAMETER;
  }
  if (1 != EVP_PKEY_derive_init(asym_ka_ctx->pkey_ctx)){
    EVP_PKEY_free(peer_pkey);
    return SOTER_FAIL;
  }

  if (1 != EVP_PKEY_derive_set_peer(asym_ka_ctx->pkey_ctx, peer_pkey)){
    EVP_PKEY_free(peer_pkey);
    return SOTER_FAIL;
  }
  
  if (1 != EVP_PKEY_derive(asym_ka_ctx->pkey_ctx, NULL, &out_length)){
    EVP_PKEY_free(peer_pkey);
    return SOTER_FAIL;
  }

  if (out_length > *shared_secret_length){
    EVP_PKEY_free(peer_pkey);
    *shared_secret_length = out_length;
    return SOTER_BUFFER_TOO_SMALL;
  }

  if (1 != EVP_PKEY_derive(asym_ka_ctx->pkey_ctx, (unsigned char *)shared_secret, shared_secret_length)){
    EVP_PKEY_free(peer_pkey);
    return SOTER_FAIL;
  }

  EVP_PKEY_free(peer_pkey);
  return SOTER_SUCCESS;
}
