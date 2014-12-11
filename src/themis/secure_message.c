/**
 * @file
 *
 * (c) CossackLabs
 */

#include "themis/secure_message.h"

themis_secure_message_t* themis_secure_message_init(const uint8_t* private_key,
						    const size_t private_key_length,
						    const uint8_t* public_key,
						    const size_t public_key_length){


}

themis_status_t themis_secure_message_wrap(themis_secure_message_t* ctx,
					   const uint8_t* message,
					   const size_t message_length);

themis_status_t themis_secure_message_unwrap(themis_secure_message_t* ctx,
					     uint8_t* message,
					     const size_t message_length);

themis_status_t themis_secure_message_export(themis_secure_message_t* ctx,
					     uint8_t* message,
					     size_t* message_length);

themis_status_t themis_secure_message_destroy(themis_secure_message_t* ctx);


