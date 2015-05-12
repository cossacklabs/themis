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

#include <soter/error.h>
#include <soter/soter.h>
#include <soter/soter_ec_key.h>
#include "soter_openssl.h"
#include <openssl/evp.h>
#include <openssl/ec.h>

soter_status_t soter_ec_gen_key(EVP_PKEY_CTX *pkey_ctx)
{
  EVP_PKEY *pkey;
  EC_KEY *ec;
  if (!pkey_ctx){
    return SOTER_INVALID_PARAMETER;
  }
  pkey = EVP_PKEY_CTX_get0_pkey(pkey_ctx);
  if (!pkey){
    return SOTER_INVALID_PARAMETER;
  }
  if (EVP_PKEY_EC != EVP_PKEY_id(pkey)){
    return SOTER_INVALID_PARAMETER;
  }
  ec = EVP_PKEY_get0(pkey);
  if (NULL == ec){
    return SOTER_INVALID_PARAMETER;
  }
  if (1 == EC_KEY_generate_key(ec)){
    return SOTER_SUCCESS;
  }
  return SOTER_FAIL;
}

soter_status_t soter_ec_import_key(EVP_PKEY *pkey, const void* key, const size_t key_length)
{
  const soter_container_hdr_t *hdr = key;
  if (!pkey || !key){
    return SOTER_INVALID_PARAMETER;
  }
  if (key_length < sizeof(soter_container_hdr_t)){
    return SOTER_INVALID_PARAMETER;
  }
  if (EVP_PKEY_EC != EVP_PKEY_id(pkey)){
    return SOTER_INVALID_PARAMETER;
  }
  switch (hdr->tag[0]){
  case 'R':
    return soter_ec_priv_key_to_engine_specific(hdr, key_length, ((soter_engine_specific_ec_key_t **)&pkey));
  case 'U':
    return soter_ec_pub_key_to_engine_specific(hdr, key_length, ((soter_engine_specific_ec_key_t **)&pkey));
  }
  return SOTER_INVALID_PARAMETER;
}

soter_status_t soter_ec_export_key(soter_sign_ctx_t* ctx, void* key, size_t* key_length, bool isprivate)
{
  EVP_PKEY *pkey = EVP_PKEY_CTX_get0_pkey(ctx->pkey_ctx);
  if (!pkey){
    return SOTER_INVALID_PARAMETER;
  }
  if(isprivate){
    return soter_engine_specific_to_ec_priv_key((const soter_engine_specific_ec_key_t *)pkey, (soter_container_hdr_t *)key, key_length);
  }
  return soter_engine_specific_to_ec_pub_key((const soter_engine_specific_ec_key_t *)pkey, (soter_container_hdr_t *)key, key_length);
}
