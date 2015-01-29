/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef _SECURE_CELL_H_
#define _SECURE_CELL_H_

typedef struct themis_secure_cell_type themis_secure_cell_t;

themis_secure_cell_t* themis_secure_cell_create(const uint8_t* master_key, const size_t master_key_length);

themis_status_t themis_secure_cell_encrypt_full(themis_secure_cell_t* ctx,
					       const uint8_t* message,
					       const size_t message_length,
					       uint8_t* encrypted_message,
					       size_t* encrypted_message_length);

themis_status_t themis_secure_cell_decrypt_full(themis_secure_cell_t* ctx,
					       const uint8_t* encrypted_message,
					       const size_t encrypted_message_length,
					       uint8_t* plain_message,
					       size_t* plain_message_length);

themis_status_t themis_secure_cell_encrypt_auto_split(themis_secure_cell_t* ctx,
					       const uint8_t* message,
					       const size_t message_length,
					       uint8_t* iv,
					       size_t* iv_length,
					       uint8_t* auth_tag,
					       size_t* auth_tag_length,
					       uint8_t* encrypted_message,
					       size_t* encrypted_message_length);

themis_status_t themis_secure_cell_decrypt_auto_split(themis_secure_cell_t* ctx,
					       const uint8_t* encrypted_message,
					       const size_t encrypted_message_length,
					       const uint8_t* iv,
					       const size_t iv_length,
					       const uint8_t* auth_tag,
					       const size_t auth_tag_length,
					       uint8_t* plain_message,
					       size_t* plain_message_length);

themis_status_t themis_secure_cell_encrypt_user_split(themis_secure_cell_t* ctx,
					       const uint8_t* message,
					       const size_t message_length,
					       const uint8_t* iv,
					       const size_t iv_length,
					       uint8_t* auth_tag,
					       size_t* auth_tag_length,
					       uint8_t* encrypted_message,
					       size_t* encrypted_message_length);

themis_status_t themis_secure_cell_decrypt_user_split(themis_secure_cell_t* ctx,
					       const uint8_t* encrypted_message,
					       const size_t encrypted_message_length,
					       const uint8_t* iv,
					       const size_t iv_length,
					       const uint8_t* auth_tag,
					       const size_t auth_tag_length,
					       uint8_t* plain_message,
					       size_t* plain_message_length);

themis_status_t themis_secure_cell_destroy(themis_secure_cell_t* ctx);


#endif /* _SECURE_CELL_H_ */

















