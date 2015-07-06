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
#include <soter/soter_rsa_key.h>
#include "soter_openssl.h"
#include <openssl/evp.h>
#include <openssl/rsa.h>

#ifndef SOTER_RSA_KEY_LENGTH
#define SOTER_RSA_KEY_LENGTH 2048
#endif

soter_status_t soter_rsa_gen_key(EVP_PKEY_CTX *pkey_ctx)
{
  /* it is copy-paste from /src/soter/openssl/soter_asym_cipher.c */
  BIGNUM *pub_exp;
  EVP_PKEY *pkey = EVP_PKEY_CTX_get0_pkey(pkey_ctx);
  if (!pkey){
    return SOTER_INVALID_PARAMETER;
  }
  
  if (EVP_PKEY_RSA != EVP_PKEY_id(pkey)){
    return SOTER_INVALID_PARAMETER;
  }
  
  if (!EVP_PKEY_keygen_init(pkey_ctx)){
    return SOTER_INVALID_PARAMETER;
  }
  
  /* Although it seems that OpenSSL/LibreSSL use 0x10001 as default public exponent, we will set it explicitly just in case */
  pub_exp = BN_new();
  if (!pub_exp){
    return SOTER_NO_MEMORY;
  }
  
  if (!BN_set_word(pub_exp, RSA_F4)){
    BN_free(pub_exp);
    return SOTER_FAIL;
  }
  
  if (1 > EVP_PKEY_CTX_ctrl(pkey_ctx, -1, -1, EVP_PKEY_CTRL_RSA_KEYGEN_PUBEXP, 0, pub_exp)){
    BN_free(pub_exp);
    return SOTER_FAIL;
  }
  
  /* Override default key size for RSA key. Currently OpenSSL has default key size of 1024. LibreSSL has 2048. We will put 2048 explicitly */
  if (1 > EVP_PKEY_CTX_ctrl(pkey_ctx, -1, -1, EVP_PKEY_CTRL_RSA_KEYGEN_BITS, SOTER_RSA_KEY_LENGTH, NULL)){
    return SOTER_FAIL;
  }
  
  if(!EVP_PKEY_keygen(pkey_ctx, &pkey)){
    return SOTER_FAIL;
  }
  return SOTER_SUCCESS;
  /* end of copy-paste from /src/soter/openssl/soter_asym_cipher.c*/
}

soter_status_t soter_rsa_import_key(EVP_PKEY *pkey, const void* key, const size_t key_length)
{
  const soter_container_hdr_t *hdr = key;
 
  if (!pkey){
    return SOTER_INVALID_PARAMETER;
  }
  if (EVP_PKEY_RSA != EVP_PKEY_id(pkey) || key_length < sizeof(soter_container_hdr_t)){
    return SOTER_INVALID_PARAMETER;
  }
  switch (hdr->tag[0]){
  case 'R':
    return soter_rsa_priv_key_to_engine_specific(hdr, key_length, ((soter_engine_specific_rsa_key_t **)&pkey));
  case 'U':
    return soter_rsa_pub_key_to_engine_specific(hdr, key_length, ((soter_engine_specific_rsa_key_t **)&pkey));
  }
  return SOTER_INVALID_PARAMETER;
}

soter_status_t soter_rsa_export_key(soter_sign_ctx_t* ctx, void* key, size_t* key_length, bool isprivate)
{
  EVP_PKEY *pkey = EVP_PKEY_CTX_get0_pkey(ctx->pkey_ctx);
  
  if (!pkey){
    return SOTER_INVALID_PARAMETER;
  }
  if(isprivate)
    {
      return soter_engine_specific_to_rsa_priv_key((const soter_engine_specific_rsa_key_t *)pkey, (soter_container_hdr_t *)key, key_length);
    }
  else
    {
      return soter_engine_specific_to_rsa_pub_key((const soter_engine_specific_rsa_key_t *)pkey, (soter_container_hdr_t *)key, key_length);
    }  
}

