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
 * @file secure_message.h
 * @brief main Secure Session interface
 */

#ifndef THEMIS_SECURE_MESSAGE_H
#define THEMIS_SECURE_MESSAGE_H

#include <themis/themis_api.h>
#include <themis/themis_error.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup THEMIS
 * @{
 * @defgroup THEMIS_SECURE_MESSAGE secure message
 * @brief Secure message is a lightweight service which can help to deliver some message or data to
 * your peer in a secure manner.
 * @{
 */

/**
 * @brief encrypt message to secure message
 * @param [in]      private_key                 private key
 * @param [in]      private_key_length          length of private_key
 * @param [in]      public_key                  peer public key
 * @param [in]      public_key_length           length of public_key
 * @param [in]      message                     message to encrypt
 * @param [in]      message_length              length of message
 * @param [out]     encrypted_message           buffer for encrypted message.
 *                                              May be set to NULL to determine expected length of
 * encrypted message
 * @param [in, out] encrypted_message_length    length of encrypted_message
 * @return THEMIS_SUCCESS on success or an error code on failure
 * @note If encrypted_message is NULL or encrypted_message_length is not enough to store the
 * encrypted message then THEMIS_BUFFER_TOO_SMALL will be returned and encrypted_message_length will
 * contain the length of the buffer needed to store the encrypted message.
 */
THEMIS_API
themis_status_t themis_secure_message_encrypt(const uint8_t* private_key,
                                              size_t private_key_length,
                                              const uint8_t* public_key,
                                              size_t public_key_length,
                                              const uint8_t* message,
                                              size_t message_length,
                                              uint8_t* encrypted_message,
                                              size_t* encrypted_message_length);

/**
 * @brief decrypt secure message to plaintext message
 * @param [in]      private_key                 private key
 * @param [in]      private_key_length          length of private_key
 * @param [in]      public_key                  peer public key
 * @param [in]      public_key_length           length of public_key
 * @param [in]      encrypted_message           encrypted message to decrypt
 * @param [in]      encrypted_message_length    length of encrypted_message
 * @param [out]     message                     buffer for plaintext message.
 *                                              May be set to NULL to determine expected length of
 * plaintext message
 * @param [in, out] message_length              length of message
 * @return THEMIS_SUCCESS on success or an error code on failure
 * @note If message is NULL or message_length is not enough to store the plaintext message
 *       then THEMIS_BUFFER_TOO_SMALL will be returned and message_length will contain
 *       the length of the buffer needed to store the encrypted message.
 */
THEMIS_API
themis_status_t themis_secure_message_decrypt(const uint8_t* private_key,
                                              size_t private_key_length,
                                              const uint8_t* public_key,
                                              size_t public_key_length,
                                              const uint8_t* encrypted_message,
                                              size_t encrypted_message_length,
                                              uint8_t* message,
                                              size_t* message_length);

/**
 * @brief securely sign a message
 * @param [in]      private_key             private key
 * @param [in]      private_key_length      length of private_key
 * @param [in]      message                 message to sign
 * @param [in]      message_length          length of message
 * @param [out]     signed_message          buffer for signed message.
 *                                          May be set to NULL to determine expected length of
 * signed message
 * @param [in, out] signed_message_length   length of signed_message
 * @return THEMIS_SUCCESS on success or an error code on failure
 * @note If signed_message is NULL or signed_message_length is not enough to store the signed
 * message then THEMIS_BUFFER_TOO_SMALL will be returned and signed_message_length will contain the
 * length of the buffer needed to store the signed message.
 */
THEMIS_API
themis_status_t themis_secure_message_sign(const uint8_t* private_key,
                                           size_t private_key_length,
                                           const uint8_t* message,
                                           size_t message_length,
                                           uint8_t* signed_message,
                                           size_t* signed_message_length);

/**
 * @brief verify signature on a signed message
 * @param [in]      public_key              peer public key
 * @param [in]      public_key_length       length of public_key
 * @param [in]      signed_message          signed message to verify
 * @param [in]      signed_message_length   length of signed_message
 * @param [out]     message                 buffer for original message (without signature).
 *                                          May be set to NULL to determine expected length of
 * original message
 * @param [in, out] message_length          length of message
 * @return THEMIS_SUCCESS on success or an error code on failure
 * @note If message is NULL or message_length is not enough to store the original message
 *       then THEMIS_BUFFER_TOO_SMALL will be returned and message_length will contain
 *       the length of the buffer needed to store the original message.
 */
THEMIS_API
themis_status_t themis_secure_message_verify(const uint8_t* public_key,
                                             size_t public_key_length,
                                             const uint8_t* signed_message,
                                             size_t signed_message_length,
                                             uint8_t* message,
                                             size_t* message_length);

/**
 * @brief wrap message to secure message
 * @param [in] private_key private key
 * @param [in] private_key_length length of private_key
 * @param [in] public_key peer public key
 * @param [in] public_key_length length of public_key
 * @param [in] message message to wrap
 * @param [in] message_length length of message
 * @param [out] wrapped_message buffer for wrapped message store. May be set to NULL for wrapped
 * message length determination
 * @param [in, out] wrapped_message_length length of wrapped_message
 * @return THEMIS_SUCCESS on success or THEMIS_FAIL on failure
 * @note If wrapped_message==NULL or wrapped_message_length is not enough for wrapped message
 * storage then THEMIS_BUFFER_TOO_SMALL will return and wrapped_message_length will store length of
 * buffer needed for wrapped message store
 */
DEPRECATED("use 'themis_secure_message_encrypt' with private and public keys to encrypt message, or 'themis_secure_message_sign' with private key to sign message")
THEMIS_API
themis_status_t themis_secure_message_wrap(const uint8_t* private_key,
                                           size_t private_key_length,
                                           const uint8_t* public_key,
                                           size_t public_key_length,
                                           const uint8_t* message,
                                           size_t message_length,
                                           uint8_t* wrapped_message,
                                           size_t* wrapped_message_length);

/**
 * @brief unwrap secure message to plain message
 * @param [in] private_key private key
 * @param [in] private_key_length length of private_key
 * @param [in] public_key peer public key
 * @param [in] public_key_length length of public_key
 * @param [in] wrapped_message wrapped message to unwrap
 * @param [in] wrapped_message_length length of wrapped_message
 * @param [out] message buffer for plain message store. May be set to NULL for plain message length
 * determination
 * @param [in, out] message_length length of message
 * @return THEMIS_SUCCESS on success or THEMIS_FAIL on failure
 * @note If message==NULL or message_length is not enough for plain message storage then
 * THEMIS_BUFFER_TOO_SMALL will return and message_length will store length of buffer needed for
 * plain message store
 */
DEPRECATED("use 'themis_secure_message_decrypt' with private and public key to decrypt message or 'themis_secure_message_verify' with public key to verify signed message")
THEMIS_API
themis_status_t themis_secure_message_unwrap(const uint8_t* private_key,
                                             size_t private_key_length,
                                             const uint8_t* public_key,
                                             size_t public_key_length,
                                             const uint8_t* wrapped_message,
                                             size_t wrapped_message_length,
                                             uint8_t* message,
                                             size_t* message_length);
/** @} */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* THEMIS_SECURE_MESSAGE_H */
