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

#ifndef _THEMIS_SECURE_MESSAGE_H_
#define _THEMIS_SECURE_MESSAGE_H_

#include <themis/themis.h>

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @addtogroup THEMIS
 * @{
 * @defgroup THEMIS_SECURE_MESSAGE secure message
 * @brief Secure message is a lightweight service which can help to deliver some message or data to your peer in a secure manner. 
 * @{
 */

/**
 * @brief generate RSA key pair
 * @param [out] private_key buffer for private key to store. May be set to NULL for private key length determination
 * @param [in, out] private_key_length length of private_key
 * @param [out] public_key buffer for public key to store. May be set to NULL for public key length determination
 * @param [in, out] public_key_length length of public key
 * @return THEMIS_SUCCESS on success or THEMIS_FAIL on failure
 * @note If private_key==NULL or public_key==NULL or private_key_length is not enought for private key storage or public_key_length is not enought for public key storage then THEMIS_BUFFER_TOO_SMALL will return and private_key_length and public_key_length will store lengths of buffers needed for private key and public key store respectively
 */  
themis_status_t themis_gen_rsa_key_pair(uint8_t* private_key,
					size_t* private_key_length,
					uint8_t* public_key,
					size_t* public_key_length);

/**
 * @brief generate EC key pair
 * @param [out] private_key buffer for private key to store. May be set to NULL for private key length determination
 * @param [in, out] private_key_length length of private_key
 * @param [out] public_key buffer for public key to store. May be set to NULL for public key length determination
 * @param [in, out] public_key_length length of public key
 * @return THEMIS_SUCCESS on success or THEMIS_FAIL on failure
 * @note If private_key==NULL or public_key==NULL or private_key_length is not enought for private key storage or public_key_length is not enought for public key storage then THEMIS_BUFFER_TOO_SMALL will return and private_key_length and public_key_length will store lengths of buffers needed for private key and public key store respectively
 */  
themis_status_t themis_gen_ec_key_pair(uint8_t* private_key,
				       size_t* private_key_length,
				       uint8_t* public_key,
				       size_t* public_key_length);

/** 
 * @brief wrap message to secure message
 * @param [in] private_key private key
 * @param [in] pribate_key_length length of private_key
 * @param [in] public_key peer public key
 * @param [in] public_key_length length of public_key
 * @param [in] message message to wrap
 * @param [in] message_length length of message
 * @param [out] wrapped_message buffer for wrapped message store. May be set to NULL for wrapped message length determination
 * @param [in, out] wrapped_message_length length of wrapped_message
 * @return THEMIS_SUCCESS on success or THEMIS_FAIL on failure
 * @note If wrapped_message==NULL or wrapped_message_length is not enought for wrapped message storage then THEMIS_BUFFER_TOO_SMALL will return and wrapped_message_length will store length of buffer needed for wrapped message store
 */
themis_status_t themis_secure_message_wrap(const uint8_t* private_key,
					   const size_t private_key_length,
					   const uint8_t* public_key,
					   const size_t public_key_length,
					   const uint8_t* message,
					   const size_t message_length,
					   uint8_t* wrapped_message,
					   size_t* wrapped_message_length);

/** 
 * @brief unwrap secure message to plain message
 * @param [in] private_key private key
 * @param [in] pribate_key_length length of private_key
 * @param [in] public_key peer public key
 * @param [in] public_key_length length of public_key
 * @param [in] wrapped_message wrapped message to unwrap
 * @param [in] wrapped_message_length length of wrapped_message
 * @param [out] message buffer for plain message store. May be set to NULL for plain message length determination
 * @param [in, out] message_length length of message
 * @return THEMIS_SUCCESS on success or THEMIS_FAIL on failure
 * @note If message==NULL or message_length is not enought for plain message storage then THEMIS_BUFFER_TOO_SMALL will return and message_length will store length of buffer needed for plain message store
 */
themis_status_t themis_secure_message_unwrap(const uint8_t* private_key,
					   const size_t private_key_length,
					   const uint8_t* public_key,
					   const size_t public_key_length,
					   const uint8_t* wrapped_message,
					   const size_t wrapped_message_length,
					   uint8_t* message,
					   size_t* message_length);
/** @} */
/** @} */

#ifdef __cplusplus
}
#endif


#endif /* _THEMIS_SECURE_MESSAGE_H_ */
















