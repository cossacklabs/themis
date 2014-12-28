/**
 * @file
 *
 * (c) CossackLabs
 */

#include <themis/secure_message.h>
#include <themis/secure_message_wrapper.h>
#include <soter/soter_t.h>

themis_status_t themis_gen_key_pair(soter_sign_alg_t alg,
				    uint8_t* private_key,
				    size_t* private_key_length,
				    uint8_t* public_key,
				    size_t* public_key_length)
{
  soter_sign_ctx_t* ctx=soter_sign_create(alg,NULL,0,NULL,0);
  HERMES_CHECK(ctx!=NULL);
  soter_status_t res=soter_sign_export_key(ctx, private_key, private_key_length, true);
  if(res!=HERMES_SUCCESS){
    soter_sign_destroy(ctx);
    return res;
  }
  res=soter_sign_export_key(ctx, public_key, public_key_length, false);
  if(res!=HERMES_SUCCESS){
    soter_sign_destroy(ctx);
    return res;
  }
  soter_sign_destroy(ctx);
  return HERMES_SUCCESS;
}

themis_status_t themis_gen_rsa_key_pair(uint8_t* private_key,
				    size_t* private_key_length,
				    uint8_t* public_key,
				    size_t* public_key_length){
  return themis_gen_key_pair(SOTER_SIGN_rsa_pss_pkcs8, private_key, private_key_length, public_key, public_key_length);
}

themis_status_t themis_gen_ec_key_pair(uint8_t* private_key,
				    size_t* private_key_length,
				    uint8_t* public_key,
				    size_t* public_key_length){
  return themis_gen_key_pair(SOTER_SIGN_ecdsa_none_pkcs8, private_key, private_key_length, public_key, public_key_length);
}

themis_status_t themis_secure_message_wrap(const uint8_t* private_key,
					   const size_t private_key_length,
					   const uint8_t* public_key,
					   const size_t public_key_length,
					   const uint8_t* message,
					   const size_t message_length,
					   uint8_t* wrapped_message,
					   size_t* wrapped_message_length){
  HERMES_CHECK_PARAM(private_key!=NULL);
  HERMES_CHECK_PARAM(private_key_length!=0);
  HERMES_CHECK_PARAM(message!=NULL);
  HERMES_CHECK_PARAM(message_length!=0);
  HERMES_CHECK_PARAM(wrapped_message_length!=NULL);
  if(public_key==NULL && public_key_length==0){ 
    themis_secure_message_signer_t* ctx=NULL;
    ctx = themis_secure_message_signer_init(private_key, private_key_length);
    HERMES_CHECK(ctx!=NULL);
    themis_status_t res=themis_secure_message_signer_proceed(ctx, message, message_length, wrapped_message, wrapped_message_length);
    secure_message_signer_destroy(ctx);
    return res;
  } else {
    HERMES_CHECK_PARAM(public_key);
    HERMES_CHECK_PARAM(public_key_length);
    
    
  }
  return HERMES_INVALID_PARAMETER;
}

themis_status_t themis_secure_message_unwrap(const uint8_t* private_key,
					   const size_t private_key_length,
					   const uint8_t* public_key,
					   const size_t public_key_length,
					   const uint8_t* wrapped_message,
					   const size_t wrapped_message_length,
					   uint8_t* message,
					   size_t* message_length){
  HERMES_CHECK_PARAM(public_key!=NULL);
  HERMES_CHECK_PARAM(public_key_length!=0);
  HERMES_CHECK_PARAM(wrapped_message!=NULL);
  HERMES_CHECK_PARAM(wrapped_message_length!=0);
  HERMES_CHECK_PARAM(message_length!=NULL);
  themis_secure_message_hdr_t* message_hdr=(themis_secure_message_hdr_t*)wrapped_message;
  HERMES_CHECK_PARAM(IS_THEMIS_SECURE_MESSAGE_SIGNED(message_hdr->message_type) || IS_THEMIS_SECURE_MESSAGE_SIGNED(message_hdr->message_type));
  HERMES_CHECK_PARAM(wrapped_message_length>=THEMIS_SECURE_MESSAGE_LENGTH(message_hdr));
  if(IS_THEMIS_SECURE_MESSAGE_SIGNED(message_hdr->message_type)){
    themis_secure_message_verifier_t* ctx=NULL;
    ctx = themis_secure_message_verifier_init(public_key, public_key_length);
    HERMES_CHECK(ctx!=NULL);
    themis_status_t res=themis_secure_message_verifier_proceed(ctx, wrapped_message, wrapped_message_length, message, message_length);
    secure_message_verifier_destroy(ctx);
    return res;
  }
  return HERMES_INVALID_PARAMETER;
}
