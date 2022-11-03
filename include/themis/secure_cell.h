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
 * @brief Secure cell is a high-level cryptographic service which can protect arbitrary data being
 * stored in various types of storages (like databases, document archives, cloud storage etc)
 */

#ifndef THEMIS_SECURE_CELL_H
#define THEMIS_SECURE_CELL_H

#include <themis/themis_api.h>
#include <themis/themis_error.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup THEMIS
 * @{
 * @defgroup THEMIS_SECURE_CELL secure cell
 * @brief Secure cell is a high-level cryptographic service which can protect arbitrary data being
 * stored in various types of storages (like databases, document archives, cloud storage etc)
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
 * @param [out] encrypted_message buffer for encrypted message store. May be set to NULL for
 * encrypted message length determination
 * @param [in, out] encrypted_message_length length of encrypted_message
 * @return THEMIS_SUCCESS on success or THEMIS_FAIL on failure
 * @note If encrypted_message==NULL or encrypted_message_length is not enough for encrypted message
 * store then THEMIS_BUFFER_TOO_SMALL will return and encrypted_message_length will store length of
 * buffer needed for encrypted message store
 */
THEMIS_API
themis_status_t themis_secure_cell_encrypt_seal(const uint8_t* master_key,
                                                size_t master_key_length,
                                                const uint8_t* user_context,
                                                size_t user_context_length,
                                                const uint8_t* message,
                                                size_t message_length,
                                                uint8_t* encrypted_message,
                                                size_t* encrypted_message_length);

/**
 * @brief decrypt
 * @param [in] master_key master key
 * @param [in] master_key_length length of master_key
 * @param [in] user_context user defined context. May be set to NULL
 * @param [in] user_context_length length of user_context
 * @param [in] encrypted_message message to decrypt
 * @param [in] encrypted_message_length length of encrypted_message
 * @param [out] plain_message buffer for plain message store. May be set to NULL for plain message
 * length determination
 * @param [in, out] plain_message_length length of plain_message
 * @return THEMIS_SUCCESS on success or THEMIS_FAIL on failure
 * @note If plain_message==NULL or plain_message_length is not enough for plain message store then
 * THEMIS_BUFFER_TOO_SMALL will return and plain_message_length will store length of buffer needed
 * for plain1 message store
 */
THEMIS_API
themis_status_t themis_secure_cell_decrypt_seal(const uint8_t* master_key,
                                                size_t master_key_length,
                                                const uint8_t* user_context,
                                                size_t user_context_length,
                                                const uint8_t* encrypted_message,
                                                size_t encrypted_message_length,
                                                uint8_t* plain_message,
                                                size_t* plain_message_length);

/**
 * Encrypts and puts the provided message into a sealed cell.
 *
 * @param [in]      passphrase                  passphrase to use for security
 * @param [in]      passphrase_length           length of `passphrase` in bytes
 * @param [in]      user_context                associated context data, may be NULL
 * @param [in]      user_context_length         length of `user_context` in bytes, may be zero
 * @param [in]      message                     message to encrypt
 * @param [in]      message_length              length of `message` in bytes
 * @param [out]     encrypted_message           output buffer for encrypted message
 * @param [in,out]  encrypted_message_length    length of `encrypted_message` in bytes
 *
 * The passphrase is processed by a built-in key derivation function to obtain
 * a derived symmetric key. Input message is then combined with the provided
 * context (if any) and encrypted. Authentication data and key derivation
 * parameters are appended to the resulting data. Output is written into the
 * `encrypted_message` buffer that must have at least `encrypted_message_length`
 * bytes available. Total length of the output is written to the location
 * pointed by `encrypted_message_length`.
 *
 * You can pass NULL for `encrypted_message` in order to determine appropriate
 * buffer length. In this case no encryption is performed, the expected length
 * is written into provided location and THEMIS_BUFFER_TOO_SMALL is returned.
 *
 * `user_context` is optional _associated context data_, you may pass NULL here
 * if `user_context_length` is set to zero. This data is not included into the
 * message and you will need to provide the same data later for decryption.
 * This may be something associated with input data (e.g., database row number,
 * client name, protocol message ID, etc.)
 *
 * @returns THEMIS_SUCCESS if the message has been encrypted successfully
 * and written into `encrypted_message`.
 *
 * @returns THEMIS_BUFFER_TOO_SMALL if only the expected length of output data
 * has been written to `encrypted_message_length`.
 *
 * @exception THEMIS_INVALID_PARAMETER if `passphrase` is NULL or `passphrase_length` is zero.
 * @exception THEMIS_INVALID_PARAMETER if `user_context` is NULL but `user_context_length` is not zero.
 * @exception THEMIS_INVALID_PARAMETER if `message` is NULL or `message_length` is zero.
 * @exception THEMIS_INVALID_PARAMETER if `encrypted_message_length` is NULL.
 *
 * @exception THEMIS_BUFFER_TOO_SMALL if `encrypted_message_length` is too
 * small to hold the encrypted output.
 *
 * @exception THEMIS_FAIL if encryption failed for some reason.
 *
 * @see themis_secure_cell_decrypt_seal_with_passphrase
 */
THEMIS_API
themis_status_t themis_secure_cell_encrypt_seal_with_passphrase(const uint8_t* passphrase,
                                                                size_t passphrase_length,
                                                                const uint8_t* user_context,
                                                                size_t user_context_length,
                                                                const uint8_t* message,
                                                                size_t message_length,
                                                                uint8_t* encrypted_message,
                                                                size_t* encrypted_message_length);

/**
 * Extracts the original message from a sealed cell.
 *
 * @param [in]      passphrase                  passphrase used for security
 * @param [in]      passphrase_length           length of `passphrase` in bytes
 * @param [in]      user_context                associated context data, may be NULL
 * @param [in]      user_context_length         length of `user_context` in bytes, may be zero
 * @param [in]      encrypted_message           message to decrypt
 * @param [in]      encrypted_message_length    length of `encrypted_message` in bytes
 * @param [out]     plain_message               output buffer for decrypted message
 * @param [in,out]  plain_message_length        length of `plain_message` in bytes
 *
 * The passphrase is processed by a key derivation function to obtain a derived
 * symmetric key. Input message is then combined with the provided context
 * (if any) and decrypted. Embedded authentication data is verified to ensure
 * integrity of the message. If successful, output is written into the
 * `plain_message` buffer that must have at least `plain_message_length`
 * bytes available. Total length of the output is written to the location
 * pointed by `plain_message_length`.
 *
 * You can pass NULL for `plain_message` in order to determine appropriate
 * buffer length. In this case no decryption is performed, the expected length
 * is written into provided location and THEMIS_BUFFER_TOO_SMALL is returned.
 *
 * `user_context` is optional _associated context data_, it must be the same
 * data that was used for encryption. You may pass NULL here if `user_context_length`
 * is set to zero. Typically this is something associated with encrypted data
 * (e.g., database row number, client name, protocol message ID, etc.)
 *
 * @returns THEMIS_SUCCESS if the message has been decrypted successfully
 * and written into `plain_message`.
 *
 * @returns THEMIS_BUFFER_TOO_SMALL if only the expected length of output data
 * has been written to `plain_message_length`.
 *
 * @exception THEMIS_INVALID_PARAMETER if `passphrase` is NULL or `passphrase_length` is zero.
 * @exception THEMIS_INVALID_PARAMETER if `user_context` is NULL but `user_context_length` is not zero.
 * @exception THEMIS_INVALID_PARAMETER if `encrypted_message` is NULL or `encrypted_message_length` is zero.
 * @exception THEMIS_INVALID_PARAMETER if `plain_message_length` is NULL.
 *
 * @exception THEMIS_BUFFER_TOO_SMALL if `plain_message_length` is too
 * small to hold the decrypted output.
 *
 * @exception THEMIS_FAIL if decryption failed for any reason, be it invalid
 * passphrase, mismatched context data, corrupted encrypted message, or some
 * internal library failure.
 *
 * @see themis_secure_cell_encrypt_seal_with_passphrase
 */
THEMIS_API
themis_status_t themis_secure_cell_decrypt_seal_with_passphrase(const uint8_t* passphrase,
                                                                size_t passphrase_length,
                                                                const uint8_t* user_context,
                                                                size_t user_context_length,
                                                                const uint8_t* encrypted_message,
                                                                size_t encrypted_message_length,
                                                                uint8_t* plain_message,
                                                                size_t* plain_message_length);

/** @} */

/**
 * @defgroup THEMIS_SECURE_CELL_TOKEN_PROTECT_MODE token protect mode
 * @brief This API is designed for cases when underlying storage constraints do not allow the size
 * of the data to grow (so Secure cell seal API described above cannot be used), however the user
 * has access to a different storage location (ex. another table in the database) where he can store
 * needed security parameters
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
 * @param [out] context additional authentication info. May be set to NULL for additional
 * authentication info length determination
 * @param [in, out] context_length length of additional authentication info
 * @param [out] encrypted_message buffer for encrypted message store. May be set to NULL for
 * encrypted message length determination
 * @param [in, out] encrypted_message_length length of encrypted_message
 * @return THEMIS_SUCCESS on success or THEMIS_FAIL on failure
 * @note If encrypted_message==NULL or context==NULL or encrypted_message_length is not enough for
 * encrypted message or context_length is not enough for additional authentication info store then
 * THEMIS_BUFFER_TOO_SMALL will return and encrypted_message_length will store length of buffer
 * needed for encrypted message store and context_length will store length of buffer needed for
 * additional authentication info store
 */
THEMIS_API
themis_status_t themis_secure_cell_encrypt_token_protect(const uint8_t* master_key,
                                                         size_t master_key_length,
                                                         const uint8_t* user_context,
                                                         size_t user_context_length,
                                                         const uint8_t* message,
                                                         size_t message_length,
                                                         uint8_t* context,
                                                         size_t* context_length,
                                                         uint8_t* encrypted_message,
                                                         size_t* encrypted_message_length);

/**
 * @brief decrypt
 * @param [in] master_key master key
 * @param [in] master_key_length length of master_key
 * @param [in] user_context user defined context. May be set to NULL
 * @param [in] user_context_length length of user_context
 * @param [in] encrypted_message message to decrypt
 * @param [in] encrypted_message_length length of encrypted_message
 * @param [in] context additional authentication info
 * @param [in] context_length length of additional authentication info
 * @param [out] plain_message buffer for plain message store. May be set to NULL for plain message
 * length determination
 * @param [in, out] plain_message_length length of plain_message
 * @return THEMIS_SUCCESS on success or THEMIS_FAIL on failure
 * @note If plain_message==NULL or plain_message_length is not enough for plain message store then
 * THEMIS_BUFFER_TOO_SMALL will return and plain_message_length will store length of buffer needed
 * for plain1 message store
 */
THEMIS_API
themis_status_t themis_secure_cell_decrypt_token_protect(const uint8_t* master_key,
                                                         size_t master_key_length,
                                                         const uint8_t* user_context,
                                                         size_t user_context_length,
                                                         const uint8_t* encrypted_message,
                                                         size_t encrypted_message_length,
                                                         const uint8_t* context,
                                                         size_t context_length,
                                                         uint8_t* plain_message,
                                                         size_t* plain_message_length);

/** @} */

/**
 * @defgroup THEMIS_SECURE_CELL_CONTEXT_IMPRINT_MODE context imprint mode
 * @brief This API is for environments where storage constraints do not allow the size of the data
 * to grow and there is no auxiliary storage available
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
 * @param [out] encrypted_message buffer for encrypted message store. May be set to NULL for
 * encrypted message length determination
 * @param [in, out] encrypted_message_length length of encrypted_message
 * @return THEMIS_SUCCESS on success or THEMIS_FAIL on failure
 * @note If encrypted_message==NULL or encrypted_message_length is not enough for encrypted message
 * store then THEMIS_BUFFER_TOO_SMALL will return and encrypted_message_length will store length of
 * buffer needed for encrypted message store
 */
THEMIS_API
themis_status_t themis_secure_cell_encrypt_context_imprint(const uint8_t* master_key,
                                                           size_t master_key_length,
                                                           const uint8_t* message,
                                                           size_t message_length,
                                                           const uint8_t* context,
                                                           size_t context_length,
                                                           uint8_t* encrypted_message,
                                                           size_t* encrypted_message_length);

/**
 * @brief decrypt
 * @param [in] master_key master key
 * @param [in] master_key_length length of master_key
 * @param [in] encrypted_message message to decrypt
 * @param [in] encrypted_message_length length of encrypted_message
 * @param [in] context user defined context. May be set to NULL
 * @param [in] context_length length of user_context
 * @param [out] plain_message buffer for plain message store. May be set to NULL for plain message
 * length determination
 * @param [in, out] plain_message_length length of plain_message
 * @return THEMIS_SUCCESS on success or THEMIS_FAIL on failure
 * @note If plain_message==NULL or plain_message_length is not enough for plain message store then
 * THEMIS_BUFFER_TOO_SMALL will return and plain_message_length will store length of buffer needed
 * for plain1 message store
 */
THEMIS_API
themis_status_t themis_secure_cell_decrypt_context_imprint(const uint8_t* master_key,
                                                           size_t master_key_length,
                                                           const uint8_t* encrypted_message,
                                                           size_t encrypted_message_length,
                                                           const uint8_t* context,
                                                           size_t context_length,
                                                           uint8_t* plain_message,
                                                           size_t* plain_message_length);

/** @} */
/** @} */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* THEMIS_SECURE_CELL_H */
