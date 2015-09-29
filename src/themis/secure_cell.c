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

#include <themis/secure_cell.h>
#include <themis/error.h>
#include "sym_enc_message.h"

themis_status_t themis_secure_cell_encrypt_seal(const uint8_t* master_key,
						const size_t master_key_length,
						const uint8_t* user_context,
						const size_t user_context_length,
						const uint8_t* message,
						const size_t message_length,
						uint8_t* encrypted_message,
						size_t* encrypted_message_length){
  size_t ctx_length_;
  size_t msg_length_;
  THEMIS_STATUS_CHECK(themis_auth_sym_encrypt_message(master_key, master_key_length, message, message_length, user_context, user_context_length, NULL, &ctx_length_, NULL, &msg_length_),THEMIS_BUFFER_TOO_SMALL);
  if(encrypted_message==NULL || (*encrypted_message_length)<(ctx_length_+msg_length_)){
    (*encrypted_message_length)=(ctx_length_+msg_length_);
    return THEMIS_BUFFER_TOO_SMALL;
  }
  return themis_auth_sym_encrypt_message(master_key, master_key_length, message, message_length, user_context, user_context_length, encrypted_message, &ctx_length_, encrypted_message+ctx_length_, &msg_length_);
}

themis_status_t themis_secure_cell_decrypt_seal(const uint8_t* master_key,
						const size_t master_key_length,
						const uint8_t* user_context,
						const size_t user_context_length,
						const uint8_t* encrypted_message,
						const size_t encrypted_message_length,
						uint8_t* plain_message,
						size_t* plain_message_length){
  size_t ctx_length_=0;
  size_t msg_length_=0;
  THEMIS_STATUS_CHECK(themis_auth_sym_decrypt_message(master_key, master_key_length, user_context, user_context_length, encrypted_message, encrypted_message_length, NULL, 0, NULL, &msg_length_),THEMIS_BUFFER_TOO_SMALL);
  ctx_length_=encrypted_message_length-msg_length_;
  return themis_auth_sym_decrypt_message(master_key, master_key_length, user_context, user_context_length, encrypted_message, ctx_length_, encrypted_message+ctx_length_, msg_length_, plain_message, plain_message_length);
}

themis_status_t themis_secure_cell_encrypt_token_protect(const uint8_t* master_key,
							 const size_t master_key_length,
							 const uint8_t* user_context,
							 const size_t user_context_length,
							 const uint8_t* message,
							 const size_t message_length,
							 uint8_t* context,
							 size_t* context_length,
							 uint8_t* encrypted_message,
							 size_t* encrypted_message_length){
  return themis_auth_sym_encrypt_message(master_key, master_key_length, message, message_length, user_context, user_context_length, context, context_length, encrypted_message, encrypted_message_length);  
}

themis_status_t themis_secure_cell_decrypt_token_protect(const uint8_t* master_key,
							 const size_t master_key_length,
							 const uint8_t* user_context,
							 const size_t user_context_length,
							 const uint8_t* encrypted_message,
							 const size_t encrypted_message_length,
							 const uint8_t* context,
							 const size_t context_length,
							 uint8_t* plain_message,
							 size_t* plain_message_length){
  return themis_auth_sym_decrypt_message(master_key, master_key_length, user_context, user_context_length, context, context_length, encrypted_message, encrypted_message_length, plain_message, plain_message_length);
}

themis_status_t themis_secure_cell_encrypt_context_imprint(const uint8_t* master_key,
							   const size_t master_key_length,
							   const uint8_t* message,
							   const size_t message_length,
							   const uint8_t* context,
							   const size_t context_length,
							   uint8_t* encrypted_message,
							   size_t* encrypted_message_length){
  return themis_sym_encrypt_message_u(master_key, master_key_length, context, context_length, message, message_length, encrypted_message, encrypted_message_length);
}

themis_status_t themis_secure_cell_decrypt_context_imprint(const uint8_t* master_key,
							   const size_t master_key_length,
							   const uint8_t* encrypted_message,
							   const size_t encrypted_message_length,
							   const uint8_t* context,
							   const size_t context_length,
							   uint8_t* plain_message,
							   size_t* plain_message_length){
  return themis_sym_decrypt_message_u(master_key, master_key_length, context, context_length, encrypted_message, encrypted_message_length, plain_message, plain_message_length);
}


/* for backward compatibility */
themis_status_t themis_secure_cell_encrypt_full(const uint8_t* master_key,
						const size_t master_key_length,
						const uint8_t* user_context,
						const size_t user_context_length,
						const uint8_t* message,
						const size_t message_length,
						uint8_t* encrypted_message,
						size_t* encrypted_message_length){
  return  themis_secure_cell_encrypt_seal(master_key, master_key_length, user_context, user_context_length, message, message_length, encrypted_message, encrypted_message_length);
}

themis_status_t themis_secure_cell_decrypt_full(const uint8_t* master_key,
						const size_t master_key_length,
						const uint8_t* user_context,
						const size_t user_context_length,
						const uint8_t* encrypted_message,
						const size_t encrypted_message_length,
						uint8_t* plain_message,
						size_t* plain_message_length){
  return  themis_secure_cell_decrypt_seal(master_key, master_key_length, user_context, user_context_length, encrypted_message, encrypted_message_length, plain_message, plain_message_length);
}

themis_status_t themis_secure_cell_encrypt_auto_split(const uint8_t* master_key,
						      const size_t master_key_length,
						      const uint8_t* user_context,
						      const size_t user_context_length,
						      const uint8_t* message,
						      const size_t message_length,
						      uint8_t* token,
						      size_t* token_length,
						      uint8_t* encrypted_message,
						      size_t* encrypted_message_length){
  return themis_secure_cell_encrypt_token_protect(master_key, master_key_length, user_context, user_context_length, message, message_length, token, token_length, encrypted_message, encrypted_message_length);
}

themis_status_t themis_secure_cell_decrypt_auto_split(const uint8_t* master_key,
						      const size_t master_key_length,
						      const uint8_t* user_context,
						      const size_t user_context_length,
						      const uint8_t* encrypted_message,
						      const size_t encrypted_message_length,
						      const uint8_t* token,
						      const size_t token_length,
						      uint8_t* plain_message,
						      size_t* plain_message_length){
  return themis_secure_cell_decrypt_token_protect(master_key, master_key_length, user_context, user_context_length, encrypted_message, encrypted_message_length, token, token_length, plain_message, plain_message_length);  
}

themis_status_t themis_secure_cell_encrypt_user_split(const uint8_t* master_key,
						      const size_t master_key_length,
						      const uint8_t* message,
						      const size_t message_length,
						      const uint8_t* context,
						      const size_t context_length,
						      uint8_t* encrypted_message,
						      size_t* encrypted_message_length){
  return themis_secure_cell_encrypt_context_imprint(master_key, master_key_length, message, message_length, context, context_length, encrypted_message, encrypted_message_length);
}

themis_status_t themis_secure_cell_decrypt_user_split(const uint8_t* master_key,
						      const size_t master_key_length,
						      const uint8_t* encrypted_message,
						      const size_t encrypted_message_length,
						      const uint8_t* context,
						      const size_t context_length,
						      uint8_t* plain_message,
						      size_t* plain_message_length){
  return themis_secure_cell_decrypt_context_imprint(master_key, master_key_length, encrypted_message, encrypted_message_length, context, context_length, plain_message, plain_message_length);
}
