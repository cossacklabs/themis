/**
 * @file
 *
 * (c) CossackLabs
 */

#include <themis/secure_message.h>
#include <themis/secure_message_wrapper.h>

themis_status_t themis_secure_message_wrap(const uint8_t* private_key,
					   const size_t private_key_length,
					   const uint8_t* public_key,
					   const size_t public_key_length,
					   const uint8_t* message,
					   const size_t message_length,
					   uint8_t* wrapped_message,
					   size_t* wrapped_message_length){
  HERMES_CHECK(private_key!=NULL && private_key_length!=0 && message!=NULL && message_length!=0 && wrapped_message_length!=NULL);
  themis_secure_message_signer_t* ctx=NULL;
  if(public_key==NULL && public_key_length==0){ 
    ctx = themis_secure_message_signer_init(private_key, private_key_length);
    HERMES_CHECK(ctx!=NULL);
    themis_status_t res=themis_secure_message_singer_proceed(ctx, message, message_length, wrapped_message, wrapped_message_length);
    if(res!=HERMES_SUCCESS){
      return res;
    }
    HERMES_CHECK(secure_message_singer_destroy(ctx)==HERMES_SUCCESS);
    return HERMES_SUCCESS;
  }else{
    return HERMES_INVALID_PARAMETER;
  }
  return HERMES_SUCCESS;
}

themis_status_t themis_secure_message_unwrap(const uint8_t* private_key,
					   const size_t private_key_length,
					   const uint8_t* public_key,
					   const size_t public_key_length,
					   const uint8_t* wrapped_message,
					   const size_t wrapped_message_length,
					   uint8_t* message,
					   size_t* message_length){


}
