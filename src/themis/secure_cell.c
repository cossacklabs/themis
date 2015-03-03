/**
 * @file
 *
 * (c) CossackLabs
 */

#include <themis/secure_cell.h>
#include <common/error.h>
#include "sym_enc_message.h"
themis_status_t themis_secure_cell_encrypt_full(const uint8_t* master_key,
						const size_t master_key_length,
						const uint8_t* message,
						const size_t message_length,
						uint8_t* encrypted_message,
						size_t* encrypted_message_length){
  size_t ctx_length_;
  size_t msg_length_;
  HERMES_STATUS_CHECK(themis_auth_sym_encrypt_message(master_key, master_key_length, message, message_length, NULL, 0, NULL, &ctx_length_, NULL, &msg_length_),HERMES_BUFFER_TOO_SMALL);
  if(encrypted_message==NULL || (*encrypted_message_length)<(ctx_length_+msg_length_)){
    (*encrypted_message_length)=(ctx_length_+msg_length_);
    return HERMES_BUFFER_TOO_SMALL;
  }
  return themis_auth_sym_encrypt_message(master_key, master_key_length, message, message_length, NULL, 0, encrypted_message, &ctx_length_, encrypted_message+ctx_length_, &msg_length_);
}

themis_status_t themis_secure_cell_decrypt_full(const uint8_t* master_key,
						const size_t master_key_length,
						const uint8_t* encrypted_message,
						const size_t encrypted_message_length,
						uint8_t* plain_message,
						size_t* plain_message_length){
  size_t ctx_length_=0;
  size_t msg_length_=0;
  HERMES_STATUS_CHECK(themis_auth_sym_decrypt_message(master_key, master_key_length, encrypted_message, encrypted_message_length, NULL, 0, NULL, &msg_length_),HERMES_BUFFER_TOO_SMALL);
  ctx_length_=encrypted_message_length-msg_length_;
  return themis_auth_sym_decrypt_message(master_key, master_key_length, encrypted_message, ctx_length_, encrypted_message+ctx_length_, msg_length_, plain_message, plain_message_length);
}

themis_status_t themis_secure_cell_encrypt_auto_split(const uint8_t* master_key,
						      const size_t master_key_length,
						      const uint8_t* message,
						      const size_t message_length,
						      uint8_t* context,
						      size_t* context_length,
						      uint8_t* encrypted_message,
						      size_t* encrypted_message_length){
  return themis_auth_sym_encrypt_message(master_key, master_key_length, message, message_length, NULL, 0, context, context_length, encrypted_message, encrypted_message_length);  
}

themis_status_t themis_secure_cell_decrypt_auto_split(const uint8_t* master_key,
						      const size_t master_key_length,
						      const uint8_t* encrypted_message,
						      const size_t encrypted_message_length,
						      const uint8_t* context,
						      const size_t context_length,
						      uint8_t* plain_message,
						      size_t* plain_message_length){
  return themis_auth_sym_decrypt_message(master_key, master_key_length, context, context_length, encrypted_message, encrypted_message_length, plain_message, plain_message_length);
}

themis_status_t themis_secure_cell_encrypt_user_split(const uint8_t* master_key,
						      const size_t master_key_length,
						      const uint8_t* message,
						      const size_t message_length,
						      const uint8_t* context,
						      const size_t context_length,
						      uint8_t* encrypted_message,
						      size_t* encrypted_message_length){
  return themis_sym_encrypt_message_u(master_key, master_key_length, context, context_length, message, message_length, encrypted_message, encrypted_message_length);
}

themis_status_t themis_secure_cell_decrypt_user_split(const uint8_t* master_key,
						      const size_t master_key_length,
						      const uint8_t* encrypted_message,
						      const size_t encrypted_message_length,
						      const uint8_t* context,
						      const size_t context_length,
						      uint8_t* plain_message,
						      size_t* plain_message_length){
  return themis_sym_decrypt_message_u(master_key, master_key_length, context, context_length, encrypted_message, encrypted_message_length, plain_message, plain_message_length);
}

