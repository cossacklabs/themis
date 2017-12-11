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

#include <soter/soter_sign_rsa.h>
#include <soter/soter.h>
#include <soter/soter_rsa_key.h>
#include "soter_engine.h"
#include "soter_rsa_common.h"
#include <openssl/evp.h>
#include <openssl/rsa.h>


soter_status_t soter_sign_init_rsa_pss_pkcs8(soter_sign_ctx_t* ctx, const void* private_key, const size_t private_key_length, const void* public_key, const size_t public_key_length)
{
  /* pkey_ctx init */
  EVP_PKEY *pkey;
  pkey = EVP_PKEY_new();
  if (!pkey){
    return SOTER_NO_MEMORY;
  }
  if (!EVP_PKEY_set_type(pkey, EVP_PKEY_RSA)){
    EVP_PKEY_free(pkey);
    return SOTER_FAIL;
  }
  ctx->pkey_ctx = EVP_PKEY_CTX_new(pkey, NULL);
  if (!(ctx->pkey_ctx)){
    EVP_PKEY_free(pkey);
    return SOTER_FAIL;
  }
  if((!private_key)&&(!public_key)){
    if(soter_rsa_gen_key(ctx->pkey_ctx)!=SOTER_SUCCESS){
      soter_sign_cleanup_rsa_pss_pkcs8(ctx);
      return SOTER_FAIL;
    }
  }else{
    if(private_key!=NULL){
      if(soter_rsa_import_key(pkey, private_key, private_key_length)!=SOTER_SUCCESS){
        soter_sign_cleanup_rsa_pss_pkcs8(ctx);
	return SOTER_FAIL;
      }
    }
    if(public_key!=NULL){
      if(soter_rsa_import_key(pkey, public_key, public_key_length)!=SOTER_SUCCESS){
        soter_sign_cleanup_rsa_pss_pkcs8(ctx);
	return SOTER_FAIL;
      }
    }
  }

  /*md_ctx init*/
  ctx->md_ctx = EVP_MD_CTX_create();
  if(!(ctx->md_ctx)){
    soter_sign_cleanup_rsa_pss_pkcs8(ctx);
    return SOTER_NO_MEMORY;
  }
  EVP_PKEY_CTX *md_pkey_ctx=NULL;
  if(!EVP_DigestSignInit(ctx->md_ctx, &md_pkey_ctx, EVP_sha256(), NULL, pkey)){
    soter_sign_cleanup_rsa_pss_pkcs8(ctx);
    return SOTER_FAIL;
  }
  if(!EVP_PKEY_CTX_set_rsa_padding(md_pkey_ctx, RSA_PKCS1_PSS_PADDING)){
    soter_sign_cleanup_rsa_pss_pkcs8(ctx);
    return SOTER_FAIL;
  }
  if(!EVP_PKEY_CTX_set_rsa_pss_saltlen(md_pkey_ctx, -2)){
    soter_sign_cleanup_rsa_pss_pkcs8(ctx);
    return SOTER_FAIL;
  }  
  return SOTER_SUCCESS;
}

soter_status_t soter_sign_export_key_rsa_pss_pkcs8(soter_sign_ctx_t* ctx, void* key, size_t* key_length, bool isprivate)
{
    return soter_rsa_export_key(ctx,key,key_length, isprivate);
}

soter_status_t soter_sign_update_rsa_pss_pkcs8(soter_sign_ctx_t* ctx, const void* data, const size_t data_length)
{
  if(!EVP_DigestSignUpdate(ctx->md_ctx, data, data_length)){
    return SOTER_FAIL;
  }
  return SOTER_SUCCESS;
}

soter_status_t soter_sign_final_rsa_pss_pkcs8(soter_sign_ctx_t* ctx, void* signature, size_t *signature_length)
{
  EVP_PKEY *pkey = EVP_PKEY_CTX_get0_pkey(ctx->pkey_ctx);
  if (!pkey){
    return SOTER_INVALID_PARAMETER;
  }
  int key_size = EVP_PKEY_size(pkey);
  if(key_size < 0){
    return SOTER_FAIL;
  }
  if((*signature_length)< (size_t)key_size){
    (*signature_length)=(size_t)key_size;
    return SOTER_BUFFER_TOO_SMALL;
  }
  if(!EVP_DigestSignFinal(ctx->md_ctx, signature, signature_length)){
    return SOTER_FAIL;
  }
  return SOTER_SUCCESS;
}

soter_status_t soter_sign_cleanup_rsa_pss_pkcs8(soter_sign_ctx_t* ctx)
{
  if(!ctx){
    return SOTER_INVALID_PARAMETER;
  }
  if(ctx->pkey_ctx){
    EVP_PKEY *pkey = EVP_PKEY_CTX_get0_pkey(ctx->pkey_ctx);
    if(pkey){
        EVP_PKEY_free(pkey);
    }
    EVP_PKEY_CTX_free(ctx->pkey_ctx);
    ctx->pkey_ctx=NULL;
  }
  if(ctx->md_ctx){
    EVP_MD_CTX_destroy(ctx->md_ctx);
    ctx->md_ctx=NULL;
  }
  return SOTER_SUCCESS;
}
