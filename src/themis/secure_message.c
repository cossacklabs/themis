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

#include <themis/error.h>
#include <themis/secure_message.h>
#include <themis/secure_message_wrapper.h>
#include <soter/soter_t.h>

#ifndef THEMIS_RSA_KEY_LENGTH
#define THEMIS_RSA_KEY_LENGTH RSA_KEY_LENGTH_2048
#endif
themis_status_t themis_gen_key_pair(soter_sign_alg_t alg,
				    uint8_t* private_key,
				    size_t* private_key_length,
				    uint8_t* public_key,
				    size_t* public_key_length)
{
  soter_sign_ctx_t* ctx=soter_sign_create(alg,NULL,0,NULL,0);
  THEMIS_CHECK(ctx!=NULL);
  soter_status_t res=soter_sign_export_key(ctx, private_key, private_key_length, true);
  if(res!=THEMIS_SUCCESS && res != THEMIS_BUFFER_TOO_SMALL){
    soter_sign_destroy(ctx);
    return res;
  }
  soter_status_t res2=soter_sign_export_key(ctx, public_key, public_key_length, false);
  if(res2!=THEMIS_SUCCESS && res2!=THEMIS_BUFFER_TOO_SMALL){
    soter_sign_destroy(ctx);
    return res;
  }
  soter_sign_destroy(ctx);
  if(res==THEMIS_BUFFER_TOO_SMALL || res2==THEMIS_BUFFER_TOO_SMALL){
    return THEMIS_BUFFER_TOO_SMALL;
  }
  return THEMIS_SUCCESS;
}

themis_status_t themis_gen_rsa_key_pair(uint8_t* private_key,
				    size_t* private_key_length,
				    uint8_t* public_key,
				    size_t* public_key_length){
  soter_rsa_key_pair_gen_t* key_pair_ctx=soter_rsa_key_pair_gen_create(THEMIS_RSA_KEY_LENGTH);
  themis_status_t res=soter_rsa_key_pair_gen_export_key(key_pair_ctx, private_key, private_key_length, true);
  themis_status_t res1=soter_rsa_key_pair_gen_export_key(key_pair_ctx, public_key, public_key_length, false);
  return (res<res1)?res:res1;
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
  THEMIS_CHECK_PARAM(private_key!=NULL);
  THEMIS_CHECK_PARAM(private_key_length!=0);
  THEMIS_CHECK_PARAM(message!=NULL);
  THEMIS_CHECK_PARAM(message_length!=0);
  THEMIS_CHECK_PARAM(wrapped_message_length!=NULL);
  if(public_key==NULL && public_key_length==0){ 
    themis_secure_message_signer_t* ctx=NULL;
    ctx = themis_secure_message_signer_init(private_key, private_key_length);
    THEMIS_CHECK(ctx!=NULL);
    themis_status_t res=themis_secure_message_signer_proceed(ctx, message, message_length, wrapped_message, wrapped_message_length);
    themis_secure_message_signer_destroy(ctx);
    return res;
  } else {
    THEMIS_CHECK_PARAM(public_key!=NULL);
    THEMIS_CHECK_PARAM(public_key_length!=0);
    themis_secure_message_encrypter_t* ctx=NULL;
    ctx = themis_secure_message_encrypter_init(private_key, private_key_length, public_key, public_key_length);
    THEMIS_CHECK(ctx!=NULL);
    themis_status_t res=themis_secure_message_encrypter_proceed(ctx, message, message_length, wrapped_message, wrapped_message_length);
    themis_secure_message_encrypter_destroy(ctx);
    return res;    
  }
  return THEMIS_INVALID_PARAMETER;
}

themis_status_t themis_secure_message_unwrap(const uint8_t* private_key,
					   const size_t private_key_length,
					   const uint8_t* public_key,
					   const size_t public_key_length,
					   const uint8_t* wrapped_message,
					   const size_t wrapped_message_length,
					   uint8_t* message,
					   size_t* message_length){
  THEMIS_CHECK_PARAM(public_key!=NULL);
  THEMIS_CHECK_PARAM(public_key_length!=0);
  THEMIS_CHECK_PARAM(wrapped_message!=NULL);
  THEMIS_CHECK_PARAM(wrapped_message_length!=0);
  THEMIS_CHECK_PARAM(message_length!=NULL);
  themis_secure_message_hdr_t* message_hdr=(themis_secure_message_hdr_t*)wrapped_message;
  THEMIS_CHECK_PARAM(IS_THEMIS_SECURE_MESSAGE_SIGNED(message_hdr->message_type) || IS_THEMIS_SECURE_MESSAGE_ENCRYPTED(message_hdr->message_type));
  THEMIS_CHECK_PARAM(wrapped_message_length>=THEMIS_SECURE_MESSAGE_LENGTH(message_hdr));
  if(IS_THEMIS_SECURE_MESSAGE_SIGNED(message_hdr->message_type)){
    themis_secure_message_verifier_t* ctx=NULL;
    ctx = themis_secure_message_verifier_init(public_key, public_key_length);
    THEMIS_CHECK(ctx!=NULL);
    themis_status_t res=themis_secure_message_verifier_proceed(ctx, wrapped_message, wrapped_message_length, message, message_length);
    themis_secure_message_verifier_destroy(ctx);
    return res;
  } else{
    THEMIS_CHECK_PARAM(private_key!=NULL);
    THEMIS_CHECK_PARAM(private_key_length!=0);
    themis_secure_message_decrypter_t* ctx=NULL;
    ctx = themis_secure_message_decrypter_init(private_key, private_key_length, public_key, public_key_length);
    THEMIS_CHECK(ctx!=NULL);
    themis_status_t res=themis_secure_message_decrypter_proceed(ctx, wrapped_message, wrapped_message_length, message, message_length);
    themis_secure_message_decrypter_destroy(ctx);
    return res;    

  }
  return THEMIS_INVALID_PARAMETER;
}
