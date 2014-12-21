/**
 * @file
 *
 * (c) CossackLabs
 */

#include <string.h>


#include <common/error.h>
#include <soter/soter.h>
#include <soter/soter_rsa_key.h>
#include <soter/soter_ec_key.h>
#include <soter/soter_asym_sign.h>
#include <themis/secure_message_wrapper.h>

soter_sign_alg_t get_alg_id(const uint8_t* key, size_t key_length)
{
  if (key_length < sizeof(soter_container_hdr_t) && key_length < ((const soter_container_hdr_t*)key)->size){
    return (soter_sign_alg_t)(-1);
  }
  if (memcmp(((const soter_container_hdr_t*)key)->tag,EC_PRIV_KEY_PREF,3)==0 || memcmp(((const soter_container_hdr_t*)key)->tag,EC_PUB_KEY_PREF,3)==0){
      return SOTER_SIGN_ecdsa_none_pkcs8;
  }
  if (memcmp(((const soter_container_hdr_t*)key)->tag,RSA_PRIV_KEY_PREF,3)==0 || memcmp(((const soter_container_hdr_t*)key)->tag,RSA_PUB_KEY_PREF,3)==0){
      return SOTER_SIGN_rsa_pss_pkcs8;
  }
  return (soter_sign_alg_t)(-1);
}

themis_secure_message_signer_t* themis_secure_message_signer_init(const uint8_t* key, const size_t key_length)
{
  themis_secure_message_signer_t* ctx=malloc(sizeof(themis_secure_message_signer_t));
  if(!ctx){
    return NULL;
  }
  ctx->sign_ctx=NULL;
  ctx->sign_ctx=soter_sign_create(get_alg_id(key,key_length),NULL,0,key, key_length);
  if(!(ctx->sign_ctx)){
    free(ctx);
    return NULL;
  }
  return ctx;
}

themis_status_t themis_secure_message_signer_proceed(themis_secure_message_signer_t* ctx, const uint8_t* message, const size_t message_length, uint8_t* wrapped_message, size_t* wrapped_message_length)
{
  HERMES_CHECK(ctx!=NULL && ctx->sign_ctx!=NULL);
  HERMES_CHECK(message!=NULL && message_length!=0 && wrapped_message_length!=NULL);
  uint8_t* signature=NULL;
  size_t signature_length=0;
  //  if(!ctx->precompute_signature_present){
    HERMES_CHECK(soter_sign_update(ctx->sign_ctx, message, message_length)==HERMES_SUCCESS);
    //}
  HERMES_CHECK(soter_sign_final(ctx->sign_ctx, signature, &signature_length)==HERMES_BUFFER_TOO_SMALL);
  if((message_length+signature_length+sizeof(themis_secure_message_hdr_t)>(*wrapped_message_length))){
    (*wrapped_message_length)=message_length+signature_length+sizeof(themis_secure_message_hdr_t);
    return HERMES_BUFFER_TOO_SMALL;
  }
  signature=malloc(signature_length);
  HERMES_CHECK(signature!=NULL);
  if(soter_sign_final(ctx->sign_ctx, signature, &signature_length)!=HERMES_SUCCESS){
    free(signature);
    return HERMES_FAIL;
  }
  themis_secure_message_hdr_t hdr;
  switch(soter_sign_get_alg_id(ctx->sign_ctx)){
  case SOTER_SIGN_ecdsa_none_pkcs8:
    hdr.message_type=THEMIS_SECURE_MESSAGE_EC_SIGNED;
    break;
  case SOTER_SIGN_rsa_pss_pkcs8:
    hdr.message_type=THEMIS_SECURE_MESSAGE_RSA_SIGNED;
    break;
  default:
    return HERMES_INVALID_PARAMETER;  
  };
  hdr.message_length=message_length;
  hdr.signature_length=signature_length;
  memcpy(wrapped_message,&hdr,sizeof(themis_secure_message_hdr_t));
  memcpy(wrapped_message+sizeof(themis_secure_message_hdr_t),message,message_length);
  memcpy(wrapped_message+sizeof(themis_secure_message_hdr_t)+message_length,signature,signature_length);
  (*wrapped_message_length)=message_length+signature_length+sizeof(themis_secure_message_hdr_t);
  return HERMES_SUCCESS;
}

themis_status_t secure_message_signer_destroy(themis_secure_message_signer_t* ctx)
{
  soter_sign_destroy(ctx->sign_ctx);
  ctx->sign_ctx=NULL;
  free(ctx);
  return HERMES_SUCCESS;
}

themis_secure_message_verifier_t* themis_secure_message_verifier_init(const uint8_t* key, const size_t key_length)
{
  themis_secure_message_verifier_t* ctx=malloc(sizeof(themis_secure_message_verifier_t));
  if(!ctx){
    return NULL;
  }
  ctx->verify_ctx=soter_verify_create(get_alg_id(key,key_length),key, key_length, NULL,0);
  if(!ctx->verify_ctx){
    free(ctx);
    return NULL;
  }
  return ctx;
}
 
themis_status_t themis_secure_message_verifier_proceed(themis_secure_message_verifier_t* ctx, const uint8_t* wrapped_message, const size_t wrapped_message_length, uint8_t* message, size_t* message_length)
{
  HERMES_CHECK(ctx!=NULL);
  HERMES_CHECK(wrapped_message!=NULL && wrapped_message_length!=0 && message_length!=NULL);
  themis_secure_message_hdr_t* msg=(themis_secure_message_hdr_t*)wrapped_message;
  if(((msg->message_type==THEMIS_SECURE_MESSAGE_RSA_SIGNED && soter_verify_get_alg_id(ctx->verify_ctx)!=SOTER_SIGN_rsa_pss_pkcs8)
      || (msg->message_type==THEMIS_SECURE_MESSAGE_EC_SIGNED && soter_verify_get_alg_id(ctx->verify_ctx)!=SOTER_SIGN_ecdsa_none_pkcs8))
     && (msg->message_length+msg->signature_length+sizeof(themis_secure_message_hdr_t)>wrapped_message_length)){
    return HERMES_INVALID_PARAMETER;
  }
  if((*message_length)<msg->message_length){
    (*message_length)=msg->message_length;
    return HERMES_BUFFER_TOO_SMALL;
  }
  HERMES_CHECK(soter_verify_update(ctx->verify_ctx, wrapped_message+sizeof(themis_secure_message_hdr_t), msg->message_length)==HERMES_SUCCESS);
  HERMES_CHECK(soter_verify_final(ctx->verify_ctx, wrapped_message+sizeof(themis_secure_message_hdr_t)+msg->message_length, msg->signature_length)==HERMES_SUCCESS);
  memcpy(message,wrapped_message+sizeof(themis_secure_message_hdr_t),msg->message_length);
  (*message_length)=msg->message_length;
  return HERMES_SUCCESS;
}

themis_status_t secure_message_verifier_destroy(themis_secure_message_verifier_t* ctx){
  soter_verify_destroy(ctx->verify_ctx);
  free(ctx);
  return HERMES_SUCCESS;  
}

