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
 * @file soter_asym_cipher.h 
 * @brief asymmetric encryption/decryption routines
 */
#ifndef SOTER_ASYM_CIPHER_H
#define SOTER_ASYM_CIPHER_H

#include <soter/soter.h>

/** 
 * @addtogroup SOTER
 * @{
 * @defgroup SOTER_ASYM_CIPHER asymmetric encryption/decryption routines
 * @brief asymmetric encryption/decryption routines
 * @{
 */

/**
 * @brief supported padding algorithms
 */
enum soter_asym_cipher_padding_type
{
  SOTER_ASYM_CIPHER_NOPAD, /**< no padding */
  SOTER_ASYM_CIPHER_OAEP /**< oaep padding */
};

/** @brief padding algorithm typedef  */
typedef enum soter_asym_cipher_padding_type soter_asym_cipher_padding_t;

/** @brief assymetric encoding/decoding context typedef */
typedef struct soter_asym_cipher_type soter_asym_cipher_t;

/**
 * @brief create asymmetric encription/decription context
 * @param [in] key cipher key. If key point to public key soter_asym_cipher_create return pointer to encrypter object. Owervise will return pointer to decrypter object.
 * @param [in] key_length length of key
 * @param [in] pad padding algorithm to be used. See @ref soter_asym_cipher_padding_type
 * @return pointer to created asymmetric encription/decription context on success or NULL on failure
 */
soter_asym_cipher_t* soter_asym_cipher_create(const void* key, const size_t key_length, soter_asym_cipher_padding_t pad);

/**
 * @brief encrypt data
 * @param [in] asym_cipher_ctx pointer to asymmetric encription/decription context previously created by soter_asym_cipher_create
 * @param [in] plain_data data to encrypt
 * @param [in] plain_data_length length of plain_data
 * @param [out] cipher_data buffer for cipher data store. May be set to NULL for cipher data length determination
 * @param [in, out] length of cipher_data
 * @return  SOTER_SUCESS on success or SOTER_FAIL on failure
 * @note If cipher_data==NULL or cipher_data_length less then need to store cipher data, @ref SOTER_BUFFER_TOO_SMALL will return and cipher_data_length will contain length of buffer thet need to store cipher data.
 */
soter_status_t soter_asym_cipher_encrypt(soter_asym_cipher_t* asym_cipher_ctx, const void* plain_data, size_t plain_data_length, void* cipher_data, size_t* cipher_data_length);

/**
 * @brief decrypt data
 * @param [in] asym_cipher_ctx pointer to asymmetric encription/decription context previously created by soter_asym_cipher_create
 * @param [in] cipher_data data to decrypt
 * @param [in] cipher_data_length length of cipher_data
 * @param [out] plain_data buffer for plain data store. May be set to NULL for plain data length determination
 * @param [in, out] length of plain_data
 * @return  SOTER_SUCESS on success or SOTER_FAIL on failure
 * @note If plain_data==NULL or plain_data_length less then need to store plain data, @ref SOTER_BUFFER_TOO_SMALL will return and plain_data_length will contain length of buffer thet need to store plain data.
 */
soter_status_t soter_asym_cipher_decrypt(soter_asym_cipher_t* asym_cipher_ctx, const void* cipher_data, size_t cipher_data_length, void* plain_data, size_t* plain_data_length);

/**
 * @brief import key to asymmetric encription/decription context
 * @param [in] asym_cipher_ctx pointer to asymmetric encription/decription context previously created by soter_asym_cipher_create
 * @param [in] key buffer with stored key
 * @param [in] key_length length of key
 * @return  SOTER_SUCESS on success or SOTER_FAIL on failure
 */
//soter_status_t soter_asym_cipher_import_key(soter_asym_cipher_t* asym_cipher_ctx, const void* key, size_t key_length);

/**
 * @brief destroy asymmetric encription/decription context
 * @param [in] asym_cipher_ctx pointer to asymmetric encription/decription context previously created by soter_asym_cipher_create
 * @return  SOTER_SUCESS on success or SOTER_FAIL on failure
 */
soter_status_t soter_asym_cipher_destroy(soter_asym_cipher_t* asym_cipher_ctx);

/** @} */
/** @} */

#endif /* SOTER_ASYM_CIPHER_H */
