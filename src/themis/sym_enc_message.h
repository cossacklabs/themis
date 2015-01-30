/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef _SYM_ENC_MESSAGE_H_
#define _SYM_ENC_MESSAGE_H_

#include <themis/themis.h>

themis_status_t themis_auth_sym_encrypt(const uint8_t* key,
					const size_t key_length,
					const uint8_t* message,
					const size_t message_length,
					uint8_t* context,
					size_t* context_length,
					uint8_t* encrypted_message,
					size_t* encrypted_message_length);
  

themis_status_t themis_auth_sym_decrypt(const uint8_t* key,
					const size_t key_length,
					const uint8_t* context,
					const size_t context_length,
					const uint8_t* encrypted_message,
					const size_t encrypted_message_length,
					uint8_t* message,
					size_t* message_length);

#endif /* _SYM_ENC_MESSAGE_H_ */
