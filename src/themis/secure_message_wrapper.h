/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef _THEMIS_SECURE_MESSAGE_WRAPPER_H_
#define _THEMIS_SECURE_MESSAGE_WRAPPER_H_

#include "common/error.h"

struct themis_secure_message_wrapper_type{
  uint8_t* private_key;
  size_t private_key_length;
  uint8_t* public_key;
  size_t public_key_length;
};

typedef struct themis_secure_message_wrapper_type themis_secure_message_wrapper_t;

themis_secure_message_wrapper_t* themis_secure_message_wrapper_init(const uint8_t* private_key, const size_t private_key_length, const uint8_t* peer_public_key, const size_t peer_public_key_length){

    
}

themis_status_t secure_message_wrapper_destroy(themis_secure_message_wrapper_t* ctx){
  THEMIS_CHECK(ctx);
  free(ctx);
  return HERMESS_SUCCESS;
}


#endif /* _THEMIS_SECURE_MESSAGE_WRAPPER_H_ */










