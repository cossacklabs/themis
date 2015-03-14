/**
 * @file
 *
 * (c) CossackLabs
 */

#include <themis/themis.h>

#ifndef _SECURE_CELL_H_
#define _SECURE_CELL_H_

themis_status_t themis_secure_cell_encrypt_full(const uint8_t* master_key,
						const size_t master_key_length,
						const uint8_t* user_context,
						const size_t user_context_length,
						const uint8_t* message,
						const size_t message_length,
						uint8_t* encrypted_message,
						size_t* encrypted_message_length);

themis_status_t themis_secure_cell_decrypt_full(const uint8_t* master_key,
						const size_t master_key_length,
						const uint8_t* user_context,
						const size_t user_context_length,
						const uint8_t* encrypted_message,
						const size_t encrypted_message_length,
						uint8_t* plain_message,
						size_t* plain_message_length);

themis_status_t themis_secure_cell_encrypt_auto_split(const uint8_t* master_key,
						      const size_t master_key_length,
						      const uint8_t* user_context,
						      const size_t user_context_length,
						      const uint8_t* message,
						      const size_t message_length,
						      uint8_t* context,
						      size_t* context_length,
						      uint8_t* encrypted_message,
						      size_t* encrypted_message_length);

themis_status_t themis_secure_cell_decrypt_auto_split(const uint8_t* master_key,
						      const size_t master_key_length,
						      const uint8_t* user_context,
						      const size_t user_context_length,
						      const uint8_t* encrypted_message,
						      const size_t encrypted_message_length,
						      const uint8_t* context,
						      const size_t context_length,
						      uint8_t* plain_message,
						      size_t* plain_message_length);

themis_status_t themis_secure_cell_encrypt_user_split(const uint8_t* master_key,
						      const size_t master_key_length,
						      const uint8_t* message,
						      const size_t message_length,
						      const uint8_t* context,
						      const size_t context_length,
						      uint8_t* encrypted_message,
						      size_t* encrypted_message_length);

themis_status_t themis_secure_cell_decrypt_user_split(const uint8_t* master_key,
						      const size_t master_key_length,
						      const uint8_t* encrypted_message,
						      const size_t encrypted_message_length,
						      const uint8_t* context,
						      const size_t context_length,
						      uint8_t* plain_message,
						      size_t* plain_message_length);

#endif /* _SECURE_CELL_H_ */

















