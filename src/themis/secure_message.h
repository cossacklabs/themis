/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef _THEMIS_SECURE_MESSAGE_H_
#define _THEMIS_SECURE_MESSAGE_H_

#include "themis/message.h"

struct themis_secure_message_type{
  themis_message_t* message;
  themis_message_t* signature;
};

typedef struct themis_secure_message_type themis_secure_message_t;



themis_secure_message_t* themis_secure_message_init(const uint8_t* private_key,
						    const size_t private_key_length,
						    const uint8_t* public_key,
						    const size_t public_key_length);

themis_status_t themis_secure_message_set(themis_secure_message_t* ctx,
					   const uint8_t* message,
					   const size_t message_length);

themis_status_t themis_secure_message_set_plain(themis_secure_message_t* ctx,
						const uint8_t* message,
						const size_t message_length);

themis_status_t themis_secure_message_get(themis_secure_message_t* ctx,
					     uint8_t* message,
					     size_t* message_length);

themis_status_t themis_secure_message_get_clean(themis_secure_message_t* ctx,
						uint8_t* message,
						size_t* message_length);

themis_status_t themis_secure_message_destroy(themis_secure_message_t* ctx);



#endif /* _MESSAGE_H_ */
















