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
#include <soter/soter_asym_sign.h>
#include <themis/secure_message_wrapper.h>

themis_secure_message_signer_t* themis_secure_message_signer_init(const uint8_t* key, const size_t key_length)
{
  themis_secure_message_signer_t* ctx=malloc(sizeof(themis_secure_message_signer_t));
  if(!ctx){
    return NULL;
  }
  ctx->sign_ctx=NULL;
  ctx->sign_ctx=soter_sign_create(key, key_length);
  if(!(ctx->sign_ctx)){
    free(ctx);
    return NULL;
  }
  ctx->alg=soter_key_get_alg_id(key, key_length);
  return ctx;
}

themis_status_t themis_secure_message_signer_proceed(themis_secure_message_signer_t* ctx, const uint8_t* message, const size_t message_length, uint8_t* wrapped_message, size_t* wrapped_message_length)
{
  THEMIS_CHECK(ctx!=NULL && ctx->sign_ctx!=NULL);
  THEMIS_CHECK(message!=NULL && message_length!=0 && wrapped_message_length!=NULL);
  uint8_t* signature=NULL;
  size_t signature_length=0;
  THEMIS_CHECK(soter_sign_update(ctx->sign_ctx, message, message_length)==THEMIS_SUCCESS);
  THEMIS_CHECK(soter_sign_final(ctx->sign_ctx, signature, &signature_length)==THEMIS_BUFFER_TOO_SMALL);
  if(wrapped_message==NULL ||(message_length+signature_length+sizeof(themis_secure_signed_message_hdr_t)>(*wrapped_message_length))){
    (*wrapped_message_length)=message_length+signature_length+sizeof(themis_secure_signed_message_hdr_t);
    return THEMIS_BUFFER_TOO_SMALL;
  }
  signature=malloc(signature_length);
  THEMIS_CHECK(signature!=NULL);
  if(soter_sign_final(ctx->sign_ctx, signature, &signature_length)!=THEMIS_SUCCESS){
    free(signature);
    return THEMIS_FAIL;
  }
  themis_secure_signed_message_hdr_t hdr;

  hdr.message_hdr.message_type=THEMIS_SECURE_MESSAGE_SIGNED;
  hdr.message_hdr.alg=ctx->alg;
  hdr.message_hdr.message_length=(uint32_t)message_length;
  hdr.signature_length=(uint32_t)signature_length;
  memcpy(wrapped_message,&hdr,sizeof(themis_secure_signed_message_hdr_t));
  memcpy(wrapped_message+sizeof(themis_secure_signed_message_hdr_t),message,message_length);
  memcpy(wrapped_message+sizeof(themis_secure_signed_message_hdr_t)+message_length,signature,signature_length);
  (*wrapped_message_length)=message_length+signature_length+sizeof(themis_secure_signed_message_hdr_t);
  free(signature);
  return THEMIS_SUCCESS;
}

themis_status_t themis_secure_message_signer_destroy(themis_secure_message_signer_t* ctx)
{
  soter_sign_destroy(ctx->sign_ctx);
  ctx->sign_ctx=NULL;
  free(ctx);
  return THEMIS_SUCCESS;
}

themis_secure_message_verifier_t* themis_secure_message_verifier_init(const uint8_t* key, const size_t key_length)
{
  themis_secure_message_verifier_t* ctx=malloc(sizeof(themis_secure_message_verifier_t));
  if(!ctx){
    return NULL;
  }
  ctx->verify_ctx=soter_verify_create(key, key_length);
  if(!ctx->verify_ctx){
    free(ctx);
    return NULL;
  }
  ctx->alg=soter_key_get_alg_id(key, key_length);
  return ctx;
}
 
themis_status_t themis_secure_message_verifier_proceed(themis_secure_message_verifier_t* ctx, const uint8_t* wrapped_message, const size_t wrapped_message_length, uint8_t* message, size_t* message_length)
{
  THEMIS_CHECK(ctx!=NULL);
  THEMIS_CHECK(wrapped_message!=NULL && wrapped_message_length!=0 && message_length!=NULL);
  themis_secure_signed_message_hdr_t* msg=(themis_secure_signed_message_hdr_t*)wrapped_message;
  if(msg->message_hdr.message_type!=THEMIS_SECURE_MESSAGE_SIGNED && (ctx->alg) != msg->message_hdr.alg){
    return THEMIS_INVALID_PARAMETER;
  }
  if(message == NULL || (*message_length)<msg->message_hdr.message_length){
    (*message_length)=msg->message_hdr.message_length;
    return THEMIS_BUFFER_TOO_SMALL;
  }
  soter_status_t res=THEMIS_FAIL;
  if(THEMIS_SUCCESS!=(res=soter_verify_update(ctx->verify_ctx, wrapped_message+sizeof(themis_secure_signed_message_hdr_t), msg->message_hdr.message_length))){
    return res;
  }
  if(THEMIS_SUCCESS!=(res=soter_verify_final(ctx->verify_ctx, wrapped_message+sizeof(themis_secure_signed_message_hdr_t)+msg->message_hdr.message_length, msg->signature_length))){
    return res;
  }
  memcpy(message,wrapped_message+sizeof(themis_secure_signed_message_hdr_t),msg->message_hdr.message_length);
  (*message_length)=msg->message_hdr.message_length;
  return THEMIS_SUCCESS;
}

themis_status_t themis_secure_message_verifier_destroy(themis_secure_message_verifier_t* ctx){
  soter_verify_destroy(ctx->verify_ctx);
  free(ctx);
  return THEMIS_SUCCESS;  
}
