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
#include "soter_ecdsa_common.h"
#include <openssl/evp.h>
#include <openssl/ecdsa.h>
#include <openssl/err.h>


soter_status_t soter_sign_init_ecdsa_none_pkcs8(soter_sign_ctx_t* ctx, const void* private_key, const size_t private_key_length, const void* public_key, const size_t public_key_length)
{
  /* pkey_ctx init */
  EVP_PKEY *pkey;
  pkey = EVP_PKEY_new();
  if (!pkey){
    return SOTER_NO_MEMORY;
  }
  if (!EVP_PKEY_set_type(pkey, EVP_PKEY_EC)){
    EVP_PKEY_free(pkey);
    return SOTER_FAIL;
  }
  ctx->pkey_ctx = EVP_PKEY_CTX_new(pkey, NULL);
  if (!(ctx->pkey_ctx)){
    EVP_PKEY_free(pkey);
    return SOTER_FAIL;
  }
  if (!EVP_PKEY_paramgen_init(ctx->pkey_ctx)){
    EVP_PKEY_free(pkey);
    return SOTER_FAIL;
  }
  if (!EVP_PKEY_CTX_set_ec_paramgen_curve_nid(ctx->pkey_ctx, NID_X9_62_prime256v1)){
    EVP_PKEY_free(pkey);
    return SOTER_FAIL;
  }
  if (!EVP_PKEY_paramgen(ctx->pkey_ctx, &pkey)){
    EVP_PKEY_free(pkey);
    return SOTER_FAIL;
  }
  if((!private_key)&&(!public_key)){
    if(soter_ec_gen_key(ctx->pkey_ctx)!=SOTER_SUCCESS){
      EVP_PKEY_free(pkey);
      return SOTER_FAIL;
    }
  }else{
    if(private_key!=NULL){
      if(soter_ec_import_key(pkey, private_key, private_key_length)!=SOTER_SUCCESS){
	EVP_PKEY_free(pkey);
	return SOTER_FAIL;
      }
    }
    if(public_key!=NULL){
      if(soter_ec_import_key(pkey, public_key, public_key_length)!=SOTER_SUCCESS){
	EVP_PKEY_free(pkey);
	return SOTER_FAIL;
      }
    }
  }

  /*md_ctx init*/
  ctx->md_ctx = EVP_MD_CTX_create();
  if(!(ctx->md_ctx)){
    EVP_PKEY_CTX_free(ctx->pkey_ctx);
    return SOTER_NO_MEMORY;
  }
  if(EVP_DigestSignInit(ctx->md_ctx, &(ctx->pkey_ctx), EVP_sha256(), NULL, pkey)!=1){
    EVP_PKEY_CTX_free(ctx->pkey_ctx);
    return SOTER_FAIL;
  }
  return SOTER_SUCCESS;
}

soter_status_t soter_sign_export_key_ecdsa_none_pkcs8(soter_sign_ctx_t* ctx, void* key, size_t* key_length, bool isprivate)
{
    return soter_ec_export_key(ctx,key,key_length, isprivate);
}

soter_status_t soter_sign_update_ecdsa_none_pkcs8(soter_sign_ctx_t* ctx, const void* data, const size_t data_length)
{
  if(EVP_DigestSignUpdate(ctx->md_ctx, data, data_length)!=1){
    return SOTER_FAIL;
  }
  return SOTER_SUCCESS;
}

soter_status_t soter_sign_final_ecdsa_none_pkcs8(soter_sign_ctx_t* ctx, void* signature, size_t *signature_length)
{
  EVP_PKEY *pkey = EVP_PKEY_CTX_get0_pkey(ctx->pkey_ctx);
  if (!pkey && EVP_PKEY_type(pkey->type)!=EVP_PKEY_EC){
    return SOTER_INVALID_PARAMETER;
  } /* TODO: need review */
  if(!signature || (*signature_length)<EVP_PKEY_size(pkey)){
    (*signature_length)=EVP_PKEY_size(pkey);
    return SOTER_BUFFER_TOO_SMALL;
  }

  if(EVP_DigestSignFinal(ctx->md_ctx, signature, signature_length)!=1){
    return SOTER_INVALID_SIGNATURE;
  }
  return SOTER_SUCCESS;
}
