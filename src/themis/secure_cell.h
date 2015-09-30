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

/**
 * @file secure_cell.h
 * @brief Secure cell is a high-level cryptographic service which can protect arbitrary data being stored in various types of storages (like databases, document archives, cloud storage etc)
 */
#include <themis/themis.h>

#ifndef _SECURE_CELL_H_
#define _SECURE_CELL_H_

#ifdef __cplusplus
extern "C"{
#endif

/** 
 * @addtogroup THEMIS
 * @{
 * @defgroup THEMIS_SECURE_CELL secure cell
 * @brief Secure cell is a high-level cryptographic service which can protect arbitrary data being stored in various types of storages (like databases, document archives, cloud storage etc)
 * @{
 * @defgroup THEMIS_SECURE_CELL_SEAL_MODE seal mode
 * @brief This is the most secure and easy way to protect stored data
 * @{
 */

/**
 * @brief encrypt
 * @param [in] master_key master key
 * @param [in] master_key_length length of master_key
 * @param [in] user_context user defined context. May be set to NULL
 * @param [in] user_context_length length of user_context
 * @param [in] message message to encrypt
 * @param [in] message_length length of message
 * @param [out] encrypted_message buffer for encrypted message store. May be set to NULL for encrypted message length determination
 * @param [in, out] encrypted_message_length length of encrypted_message
 * @return THEMIS_SUCCESS on success or THEMIS_FAIL on failure
 * @note If encrypted_message==NULL or encrypted_message_length is not enought for encrypted message store then THEMIS_BUFFER_TOO_SMALL will return and encrypted_message_length will store length of buffer needed for encrypted message store
 */
themis_status_t themis_secure_cell_encrypt_seal(const uint8_t* master_key,
						const size_t master_key_length,
						const uint8_t* user_context,
						const size_t user_context_length,
						const uint8_t* message,
						const size_t message_length,
						uint8_t* encrypted_message,
						size_t* encrypted_message_length);

/**
 * @brief decrypt
 * @param [in] master_key master key
 * @param [in] master_key_length length of master_key
 * @param [in] user_context user defined context. May be set to NULL
 * @param [in] user_context_length length of user_context
 * @param [in] encrypted message to decrypt
 * @param [in] encrypted_message_length length of encrypted_message
 * @param [out] plain_message buffer for plain message store. May be set to NULL for plain message length determination
 * @param [in, out] plain_message_length length of plain_message
 * @return THEMIS_SUCCESS on success or THEMIS_FAIL on failure
 * @note If plain_message==NULL or plain_message_length is not enought for plain message store then THEMIS_BUFFER_TOO_SMALL will return and plain_message_length will store length of buffer needed for plain1 message store
 */
themis_status_t themis_secure_cell_decrypt_seal(const uint8_t* master_key,
						const size_t master_key_length,
						const uint8_t* user_context,
						const size_t user_context_length,
						const uint8_t* encrypted_message,
						const size_t encrypted_message_length,
						uint8_t* plain_message,
						size_t* plain_message_length);

/** @} */

/** 
 * @defgroup THEMIS_SECURE_CELL_TOKEN_PROTECT_MODE token protect mode
 * @brief This API is designed for cases when underlying storage constraints do not allow the size of the data to grow (so Secure cell seal API described above cannot be used), however the user has access to a different storage location (ex. another table in the database) where he can store needed security parameters
 * @{
 */

/**
 * @brief encrypt
 * @param [in] master_key master key
 * @param [in] master_key_length length of master_key
 * @param [in] user_context user defined context. May be set to NULL
 * @param [in] user_context_length length of user_context
 * @param [in] message message to encrypt
 * @param [in] message_length length of message
 * @param [out] token additional authentication info. May be set to NULL for additional authentication info length determination
 * @param [in, out] length of additional authentication info
 * @param [out] encrypted_message buffer for encrypted message store. May be set to NULL for encrypted message length determination
 * @param [in, out] encrypted_message_length length of encrypted_message
 * @return THEMIS_SUCCESS on success or THEMIS_FAIL on failure
 * @note If encrypted_message==NULL or context==NULL or encrypted_message_length is not enought for encrypted message or context_length is not enougth for additional authentication info store then THEMIS_BUFFER_TOO_SMALL will return and encrypted_message_length will store length of buffer needed for encrypted message store and context_length will store length of buuffer needed for additional authentication info store
 */
themis_status_t themis_secure_cell_encrypt_token_protect(const uint8_t* master_key,
						      const size_t master_key_length,
						      const uint8_t* user_context,
						      const size_t user_context_length,
						      const uint8_t* message,
						      const size_t message_length,
						      uint8_t* token,
						      size_t* token_length,
						      uint8_t* encrypted_message,
						      size_t* encrypted_message_length);

/**
 * @brief decrypt
 * @param [in] master_key master key
 * @param [in] master_key_length length of master_key
 * @param [in] user_context user defined context. May be set to NULL
 * @param [in] user_context_length length of user_context
 * @param [in] encrypted message to decrypt
 * @param [in] encrypted_message_length length of encrypted_message
 * @param [in] token additional authentication info
 * @param [in] context_length length of additional authentication info
 * @param [out] plain_message buffer for plain message store. May be set to NULL for plain message length determination
 * @param [in, out] plain_message_length length of plain_message
 * @return THEMIS_SUCCESS on success or THEMIS_FAIL on failure
 * @note If plain_message==NULL or plain_message_length is not enought for plain message store then THEMIS_BUFFER_TOO_SMALL will return and plain_message_length will store length of buffer needed for plain1 message store
 */
themis_status_t themis_secure_cell_decrypt_token_protect(const uint8_t* master_key,
						      const size_t master_key_length,
						      const uint8_t* user_context,
						      const size_t user_context_length,
						      const uint8_t* encrypted_message,
						      const size_t encrypted_message_length,
						      const uint8_t* token,
						      const size_t token_length,
						      uint8_t* plain_message,
						      size_t* plain_message_length);

/** @} */

/** 
 * @defgroup THEMIS_SECURE_CELL_CONEXT_IMPIRIT_MODE context imprint mode
 * @brief This API is for environments where storage constraints do not allow the size of the data to grow and there is no auxiliary storage available
 * @{
 */

/**
 * @brief encrypt
 * @param [in] master_key master key
 * @param [in] master_key_length length of master_key
 * @param [in] message message to encrypt
 * @param [in] message_length length of message
 * @param [in] context user defined context. May be set to NULL
 * @param [in] context_length length of user_context
 * @param [out] encrypted_message buffer for encrypted message store. May be set to NULL for encrypted message length determination
 * @param [in, out] encrypted_message_length length of encrypted_message
 * @return THEMIS_SUCCESS on success or THEMIS_FAIL on failure
 * @note If encrypted_message==NULL or encrypted_message_length is not enought for encrypted message store then THEMIS_BUFFER_TOO_SMALL will return and encrypted_message_length will store length of buffer needed for encrypted message store
 */
themis_status_t themis_secure_cell_encrypt_context_imprint(const uint8_t* master_key,
						      const size_t master_key_length,
						      const uint8_t* message,
						      const size_t message_length,
						      const uint8_t* context,
						      const size_t context_length,
						      uint8_t* encrypted_message,
						      size_t* encrypted_message_length);

/**
 * @brief decrypt
 * @param [in] master_key master key
 * @param [in] master_key_length length of master_key
 * @param [in] encrypted message to decrypt
 * @param [in] encrypted_message_length length of encrypted_message
 * @param [in] context user defined context. May be set to NULL
 * @param [in] context_length length of user_context
 * @param [out] plain_message buffer for plain message store. May be set to NULL for plain message length determination
 * @param [in, out] plain_message_length length of plain_message
 * @return THEMIS_SUCCESS on success or THEMIS_FAIL on failure
 * @note If plain_message==NULL or plain_message_length is not enought for plain message store then THEMIS_BUFFER_TOO_SMALL will return and plain_message_length will store length of buffer needed for plain1 message store
 */
themis_status_t themis_secure_cell_decrypt_context_imprint(const uint8_t* master_key,
						      const size_t master_key_length,
						      const uint8_t* encrypted_message,
						      const size_t encrypted_message_length,
						      const uint8_t* context,
						      const size_t context_length,
						      uint8_t* plain_message,
						      size_t* plain_message_length);

/** @} */
/** @} */
/** @} */

#ifdef __cplusplus
}
#endif

/* for backward compatibility */
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
						      uint8_t* token,
						      size_t* token_length,
						      uint8_t* encrypted_message,
						      size_t* encrypted_message_length);

themis_status_t themis_secure_cell_decrypt_auto_split(const uint8_t* master_key,
						      const size_t master_key_length,
						      const uint8_t* user_context,
						      const size_t user_context_length,
						      const uint8_t* encrypted_message,
						      const size_t encrypted_message_length,
						      const uint8_t* token,
						      const size_t token_length,
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

















