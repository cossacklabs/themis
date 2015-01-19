/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef _SECURE_RECORD_H_
#define _SECURE_RECORD_H_

typedef struct themis_secure_record_type themis_secure_record_t;

themis_secure_record_t* themis_secure_record_create(const uint8_t* master_key, const size_t master_key_length);

themis_status_t themis_secure_record_encrypt_1(themis_secure_record_t* ctx,
					       const uint8_t* message,
					       const size_t message_length,
					       uint8_t* encrypted_message,
					       size_t* encrypted_message_length);

themis_status_t themis_secure_record_decrypt_1(themis_secure_record_t* ctx,
					       const uint8_t* encrypted_message,
					       const size_t encrypted_message_length,
					       uint8_t* plain_message,
					       size_t* plain_message_length);

themis_status_t themis_secure_record_encrypt_2(themis_secure_record_t* ctx,
					       const uint8_t* message,
					       const size_t message_length,
					       uint8_t* auth_tag_iv,
					       size_t* auth_tag_iv_length,
					       uint8_t* encrypted_message,
					       size_t* encrypted_message_length);

themis_status_t themis_secure_record_decrypt_2(themis_secure_record_t* ctx,
					       const uint8_t* encrypted_message,
					       const size_t encrypted_message_length,
					       const uint8_t* auth_tag_iv,
					       const size_t auth_tag_iv_length,
					       uint8_t* plain_message,
					       size_t* plain_message_length);

themis_status_t themis_secure_record_encrypt_3(themis_secure_record_t* ctx,
					       const uint8_t* message,
					       const size_t message_length,
					       const uint8_t* auth_tag_iv,
					       const size_t auth_tag_iv_length,
					       uint8_t* encrypted_message,
					       size_t* encrypted_message_length);

themis_status_t themis_secure_record_decrypt_3(themis_secure_record_t* ctx,
					       const uint8_t* encrypted_message,
					       const size_t encrypted_message_length,
					       const uint8_t* auth_tag_iv,
					       const size_t auth_tag_iv_length,
					       uint8_t* plain_message,
					       size_t* plain_message_length);

themis_status_t themis_secure_record_destroy(themis_secure_record_t* ctx);


#endif /* _SECURE_RECORD_H_ */

















