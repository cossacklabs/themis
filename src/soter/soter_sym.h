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
 * @file soter_sym.h
 * @brief symmetric encryption/decription routines
 */

#ifndef SOTER_SYM_H
#define SOTER_SYM_H

#include <soter/soter.h>

/**
 * @addtogroup SOTER
 * @{
 * @defgroup SOTER_SYM symmetric encryption/decription routines
 * @brief symmetric encryption/decription routines
 * @{
 * @defgroup SOTER_SYM_ALGORYTHMS symmetric encription/decryption  algorithms
 * @brief supported symmetric encryption/decription algorithms definitions
 * @details Algorithm definition example:
 * @code
 * SOTER_SYM_AES_GCM|SOTER_SYM_NOKDF|SOTER_SYM_256_KEY_LENGTH  //AES in GCM mode with 256 bits key length without kdf
 * SOTER_SYM_AES_CTR|SOTER_SYM_PBKDF2|SOTER_SYM_128_KEY_LENGTH //AES in CTR mode with 128 bits key length with pbkdf2
 * @endcode
 * @{
 *
 * @defgroup SOTER_SYM_ALGORYTHMS_IDS symmetric encription/decryption  algorithms ids
 * @brief supported symmetric encryption/decription algorithms definitions
 * @{
 */

/** AES in ECB mode with pkcs7 padding */
#define SOTER_SYM_AES_ECB_PKCS7     0x10010000
/** AES in CTR mode */
#define SOTER_SYM_AES_CTR           0x20000000
/** AES in XTS mode */
#define SOTER_SYM_AES_XTS           0x30000000
/** AES in GCM mode (with authenticated encryption) */
#define SOTER_SYM_AES_GCM           0x40010000

/** @} */

/** 
 * @defgroup SOTER_KDF_ALGS kdf algorithms
 * @brief supported kdf algorithms
 * @{
*/
/** do not use kdf */
#define SOTER_SYM_NOKDF             0x00000000
/** pbkdf2 algorythm */
#define SOTER_SYM_PBKDF2            0x01000000
/** @} */

/** 
 * @defgroup SOTER_KEY_LENGTH supported lengths of keys
 * @brief supported lengths of keys
 * @{
*/
/** 256 bits */
#define SOTER_SYM_256_KEY_LENGTH    0x00000100
/** 192 bits */
#define SOTER_SYM_192_KEY_LENGTH    0x000000c0
/** 512 bits */
#define SOTER_SYM_128_KEY_LENGTH    0x00000080

/** @} */

/** 
 * @defgroup SOTER_SYM_MASK masks definition for symmetryc algorithm id
 * @brief masks definition for symmetryc algorithm id
 * @{
 */
/** key length mask */
#define SOTER_SYM_KEY_LENGTH_MASK   0x00000fff
/** padding algorithm */
#define SOTER_SYM_PADDING_MASK      0x000f0000
/** encryption algorithm */
#define SOTER_SYM_ALG_MASK          0xf0000000
/** kdf algorithm */
#define SOTER_SYM_KDF_MASK          0x0f000000

/** @} */
/** @} */

/**
 * @defgroup SOTER_SYM_ROUTINES symmetric encryption/decryption routines
 * @brief symmetric encryption/decryption routines
 * @{
 */

/** @brief symmetric context typedef */
typedef struct soter_sym_ctx_type soter_sym_ctx_t;

/** 
 * @defgroup SOTER_SYM_ROUTINES_NOAUTH without authenticated encription
 * @brief symmetric encryption/decryption without authenticated encription
 * @{
 */
/** 
 * @defgroup SOTER_SYM_ROUTINES_NOAUTH_ENCRYPT encryption
 * @brief symmetric encryption without authenticated encription
 * @{
 */

/**
 * @brief create symmetric encryption context
 * @param [in] alg algorithm id for usage. See @ref SOTER_SYM_ALGORYTHMS
 * @param [in] key pointer to key buffer
 * @param [in] key_length length of key
 * @param [in] salt pointer to salt buffer
 * @param [in] salt_length length of salt 
 * @param [in] iv pointer to iv buffer
 * @param [in] iv_length length of iv
 * @return pointer to new symmetric encryption context on success or NULL on failure
 */
soter_sym_ctx_t* soter_sym_encrypt_create(const uint32_t alg, const void* key, const size_t key_length, const void* salt, const size_t salt_length, const void* iv, const size_t iv_length);

/**
 * @brief update symmetric encryption context
 * @param [in] ctx pointer to symmetric encryption context prerviosly created by soter_sym_encrypt_create
 * @param [in] plain_data pointer to data buffer to encrypt
 * @param [in] data_length length of plain_data
 * @param [out] cipher_data pointer to buffer to cipher data store, may be set to NULL for cipher data length determination
 * @param [in, out] cipher_data_length length of cipher_data
 * @return @ref SOTER_SUCCESS on success and @ref SOTER_FAIL on failure. 
 * @note If cipher_data==NULL or cipher_data_length less then need to store cipher data, @ref SOTER_BUFFER_TOO_SMALL will return and cipher_data_length will contain length of buffer thet need to store cipher data.
 */
soter_status_t soter_sym_encrypt_update(soter_sym_ctx_t *ctx, const void* plain_data,  const size_t data_length, void* cipher_data, size_t* cipher_data_length);

/** 
 * @brief final symmetric encryption context
 * @param [in] ctx pointer to symmetric encryption context prerviosly created by soter_sym_encrypt_create
 * @param [out] cipher_data pointer to buffer to cipher data store, may be set to NULL for cipher data length determination
 * @param [in, out] cipher_data_length length of cipher_data
 * @return @ref SOTER_SUCCESS on success and @ref SOTER_FAIL on failure. 
 * @note If cipher_data==NULL or cipher_data_length less then need to store cipher data, @ref SOTER_BUFFER_TOO_SMALL will return and cipher_data_length will contain length of buffer thet need to store cipher data.
 */
soter_status_t soter_sym_encrypt_final(soter_sym_ctx_t *ctx, void* cipher_data, size_t* cipher_data_length);

/**
 * @brief destroy symmetric encryption context
 * @param [in] ctx pointer to symmetric encryption context prerviosly created by soter_sym_encrypt_create
 * @return @ref SOTER_SUCCESS on success and @ref SOTER_FAIL on failure. 
 */
soter_status_t soter_sym_encrypt_destroy(soter_sym_ctx_t *ctx);
/** @} */

/** 
 * @defgroup SOTER_SYM_ROUTINES_NOAUTH_DECRYPT decription
 * @brief symmetric decryption without authenticated encription
 * @{
 */

/**
 * @brief create symmetric decryption context
 * @param [in] alg algorithm id for usage. See @ref SOTER_SYM_ALGORYTHMS
 * @param [in] key pointer to key buffer
 * @param [in] key_length length of key
 * @param [in] salt pointer to salt buffer
 * @param [in] salt_length length of salt 
 * @param [in] iv pointer to iv buffer
 * @param [in] iv_length length of iv
 * @return pointer to new symmetric decryption context on success or NULL on failure
 */
soter_sym_ctx_t* soter_sym_decrypt_create(const uint32_t alg, const void* key, const size_t key_length, const void* salt, const size_t salt_length, const void* iv, const size_t iv_length);

/**
 * @brief update symmetric decryption context
 * @param [in] ctx pointer to symmetric decryption context prerviosly created by soter_sym_decrypt_create
 * @param [in] cipher_data pointer to data buffer to decrypt
 * @param [in] data_length length of cipher_data
 * @param [out] plain_data pointer to buffer to plain data store, may be set to NULL for plain data length determination
 * @param [in, out] plain_data_length 
 * @return @ref SOTER_SUCCESS on success and @ref SOTER_FAIL on failure. 
 * @note If plain_data==NULL or plain_data_length less then need to store plain data, @ref SOTER_BUFFER_TOO_SMALL will return and plain_data_length will contain length of buffer thet need to store plain data.
 */
soter_status_t soter_sym_decrypt_update(soter_sym_ctx_t *ctx, const void* cipher_data,  const size_t data_length, void* plain_data, size_t* plain_data_length);

/**
 * @brief final symmetric decryption context
 * @param [in] ctx pointer to symmetric decryption context prerviosly created by soter_sym_decrypt_create
 * @param [out] plain_data pointer to buffer to plain data store, may be set to NULL for plain data length determination
 * @param [in, out] plain_data_length 
 * @return @ref SOTER_SUCCESS on success and @ref SOTER_FAIL on failure. 
 * @note If plain_data==NULL or plain_data_length less then need to store plain data, @ref SOTER_BUFFER_TOO_SMALL will return and plain_data_length will contain length of buffer thet need to store plain data.
 */
soter_status_t soter_sym_decrypt_final(soter_sym_ctx_t *ctx, void* plain_data, size_t* plain_data_length);

/**
 * @brief destroy symmetric decryption context
 * @param [in] ctx pointer to symmetric decryption context prerviosly created by soter_sym_decrypt_create
 * @return @ref SOTER_SUCCESS on success and @ref SOTER_FAIL on failure.
 */
soter_status_t soter_sym_decrypt_destroy(soter_sym_ctx_t *ctx);
/** @} */
/** @} */

/** 
 * @defgroup SOTER_SYM_ROUTINES_AUTH with authenticated encription
 * @brief symmetric encryption/decryption with authenticated encription
 * @{
 */

/** 
 * @defgroup SOTER_SYM_ROUTINES_AUTH_ENCRYPT encryption
 * @brief symmetric encryption with authenticated encription
 * @{
 */

/**
 * @brief create symmetric encryption context
 * @param [in] alg algorithm id for usage. See @ref SOTER_SYM_ALGORYTHMS
 * @param [in] key pointer to key buffer
 * @param [in] key_length length of key
 * @param [in] salt pointer to salt buffer
 * @param [in] salt_length length of salt 
 * @param [in] iv pointer to iv buffer
 * @param [in] iv_length length of iv
 * @return pointer to new symmetric encryption context on success or NULL on failure
 */
soter_sym_ctx_t* soter_sym_aead_encrypt_create(const uint32_t alg, const void* key, const size_t key_length, const void* salt, const size_t salt_length, const void* iv, const size_t iv_length);

/**
 * @brief Add AAD data to symmetric encryption context
 * @param [in] ctx pointer to symmetric encryption context prerviosly created by soter_sym_encrypt_create
 * @param [in] aad_data pointer to buffer with AAD data
 * @param [in] aad_length length of AAD data
 * @return @ref SOTER_SUCCESS on success and @ref SOTER_FAIL on failure. 
 */
soter_status_t soter_sym_aead_encrypt_aad(soter_sym_ctx_t *ctx, const void* plain_data,  const size_t data_length);

/**
 * @brief update symmetric encryption context
 * @param [in] ctx pointer to symmetric encryption context prerviosly created by soter_sym_encrypt_create
 * @param [in] plain_data pointer to data buffer to encrypt
 * @param [in] data_length length of plain_data
 * @param [out] cipher_data pointer to buffer to cipher data store, may be set to NULL for cipher data length determination
 * @param [in, out] cipher_data_length  length of cipher data
 * @return @ref SOTER_SUCCESS on success and @ref SOTER_FAIL on failure. 
 * @note If cipher_data==NULL or cipher_data_length less then need to store cipher data, @ref SOTER_BUFFER_TOO_SMALL will return and cipher_data_length will contain length of buffer thet need to store cipher data.
 */
soter_status_t soter_sym_aead_encrypt_update(soter_sym_ctx_t *ctx, const void* plain_data,  const size_t data_length, void* cipher_data, size_t* cipher_data_length);

/** 
 * @brief final symmetric encryption context
 * @param [in] ctx pointer to symmetric encryption context prerviosly created by soter_sym_encrypt_create
 * @param [out] auth_tag pointer to buffer for auth tag store, may be set to NULL for auth tag length determination
 * @param [in, out] auth_tag_length length of auth_tag
 * @return @ref SOTER_SUCCESS on success and @ref SOTER_FAIL on failure. 
 * @note If auth_tag==NULL or auth_tag_length less then need to store auth tag, @ref SOTER_BUFFER_TOO_SMALL will return and auth_tag_length will contain length of buffer thet need to store auth_tag.
 */
soter_status_t soter_sym_aead_encrypt_final(soter_sym_ctx_t *ctx, void* auth_tag, size_t* auth_tag_length);

/**
 * @brief destroy symmetric encryption context
 * @param [in] ctx pointer to symmetric encryption context prerviosly created by soter_sym_encrypt_create
 * @return @ref SOTER_SUCCESS on success and @ref SOTER_FAIL on failure. 
 */
soter_status_t soter_sym_aead_encrypt_destroy(soter_sym_ctx_t *ctx);
/** @} */

/** 
 * @defgroup SOTER_SYM_ROUTINES_AUTH_DECRYPT decription
 * @brief symmetric decryption with authenticated encription
 * @{
 */

/**
 * @brief create symmetric decryption context
 * @param [in] alg algorithm id for usage. See @ref SOTER_SYM_ALGORYTHMS
 * @param [in] key pointer to key buffer
 * @param [in] key_length length of key
 * @param [in] salt pointer to salt buffer
 * @param [in] salt_length length of salt 
 * @param [in] iv pointer to iv buffer
 * @param [in] iv_length length of iv
 * @return pointer to new symmetric decryption context on success or NULL on failure
 */
soter_sym_ctx_t* soter_sym_aead_decrypt_create(const uint32_t alg, const void* key, const size_t key_length, const void* salt, const size_t salt_length, const void* iv, const size_t iv_length);

/**
 * @brief Add AAD data to symmetric decryption context
 * @param [in] ctx pointer to symmetric decryption context prerviosly created by soter_sym_decrypt_create
 * @param [in] aad_data pointer to buffer with AAD data
 * @param [in] aad_length length of AAD data
 * @return @ref SOTER_SUCCESS on success and @ref SOTER_FAIL on failure. 
 */
soter_status_t soter_sym_aead_decrypt_aad(soter_sym_ctx_t *ctx, const void* plain_data,  const size_t data_length);

/**
 * @brief update symmetric decryption context
 * @param [in] ctx pointer to symmetric decryption context prerviosly created by soter_sym_decrypt_create
 * @param [in] cipher_data pointer to data buffer to decrypt
 * @param [in] data_length length of cipher_data
 * @param [out] plain_data pointer to buffer to plain data store, may be set to NULL for plain data length determination
 * @param [in, out] plain_data_length 
 * @return @ref SOTER_SUCCESS on success and @ref SOTER_FAIL on failure. 
 * @note If plain_data==NULL or plain_data_length less then need to store plain data, @ref SOTER_BUFFER_TOO_SMALL will return and plain_data_length will contain length of buffer thet need to store plain data.
 */
soter_status_t soter_sym_aead_decrypt_update(soter_sym_ctx_t *ctx, const void* plain_data,  const size_t data_length, void* chiper_data, size_t* chipher_data_length);

/** 
 * @brief final symmetric decryption context
 * @param [in] ctx pointer to symmetric decryption context prerviosly created by soter_sym_decrypt_create
 * @param [in] auth_tag pointer to buffer of auth tag
 * @param [in] auth_tag_length length of auth_tag
 * @return @ref SOTER_SUCCESS on success and @ref SOTER_FAIL on failure. 
 */
soter_status_t soter_sym_aead_decrypt_final(soter_sym_ctx_t *ctx, const void* auth_tag, const size_t auth_tag_length);

/**
 * @brief destroy symmetric decryption context
 * @param [in] ctx pointer to symmetric decryption context prerviosly created by soter_sym_decrypt_create
 * @return @ref SOTER_SUCCESS on success and @ref SOTER_FAIL on failure.
 */
soter_status_t soter_sym_aead_decrypt_destroy(soter_sym_ctx_t *ctx);
/** @} */
/** @} */
/** @} */

/** @}@} */

#endif /* SOTER_SYM_H */

