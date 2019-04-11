/*
 * Copyright (c) 2019 Cossack Labs Limited
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
 * @file secure_keygen.h
 * @brief secure key generation
 */

#ifndef THEMIS_SECURE_KEYGEN_H
#define THEMIS_SECURE_KEYGEN_H

#include <themis/themis_api.h>
#include <themis/themis_error.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup THEMIS
 * @{
 * @defgroup THEMIS_KEYS secure key generation
 * @brief securely generating random key pairs
 * @{
 */

/**
 * @brief generate RSA key pair
 * @param [out] private_key buffer for private key to store. May be set to NULL for private key
 * length determination
 * @param [in, out] private_key_length length of private_key
 * @param [out] public_key buffer for public key to store. May be set to NULL for public key length
 * determination
 * @param [in, out] public_key_length length of public key
 * @return THEMIS_SUCCESS on success or THEMIS_FAIL on failure
 * @note If private_key==NULL or public_key==NULL or private_key_length is not enought for private
 * key storage or public_key_length is not enought for public key storage then
 * THEMIS_BUFFER_TOO_SMALL will return and private_key_length and public_key_length will store
 * lengths of buffers needed for private key and public key store respectively
 */
THEMIS_API
themis_status_t themis_gen_rsa_key_pair(uint8_t* private_key,
                                        size_t* private_key_length,
                                        uint8_t* public_key,
                                        size_t* public_key_length);

/**
 * @brief generate EC key pair
 * @param [out] private_key buffer for private key to store. May be set to NULL for private key
 * length determination
 * @param [in, out] private_key_length length of private_key
 * @param [out] public_key buffer for public key to store. May be set to NULL for public key length
 * determination
 * @param [in, out] public_key_length length of public key
 * @return THEMIS_SUCCESS on success or THEMIS_FAIL on failure
 * @note If private_key==NULL or public_key==NULL or private_key_length is not enought for private
 * key storage or public_key_length is not enought for public key storage then
 * THEMIS_BUFFER_TOO_SMALL will return and private_key_length and public_key_length will store
 * lengths of buffers needed for private key and public key store respectively
 */
THEMIS_API
themis_status_t themis_gen_ec_key_pair(uint8_t* private_key,
                                       size_t* private_key_length,
                                       uint8_t* public_key,
                                       size_t* public_key_length);

enum themis_key_kind {
    THEMIS_KEY_INVALID,
    THEMIS_KEY_RSA_PRIVATE,
    THEMIS_KEY_RSA_PUBLIC,
    THEMIS_KEY_EC_PRIVATE,
    THEMIS_KEY_EC_PUBLIC,
};

typedef enum themis_key_kind themis_key_kind_t;

/**
 * @brief get Themis key kind
 * @param [in]  key     key buffer
 * @param [in]  length  length of key
 * @return corresponding key kind if the buffer contains a key, or THEMIS_KEY_INVALID otherwise
 */
THEMIS_API
themis_key_kind_t themis_get_asym_key_kind(const uint8_t* key, size_t length);

/**
 * @brief validate a Themis key
 * @param [in]  key     key buffer to validate
 * @param [in]  length  length of key
 * @return THEMIS_SUCCESS if the buffer contains a valid Themis key, or an error code otherwise
 */
THEMIS_API
themis_status_t themis_is_valid_asym_key(const uint8_t* key, size_t length);

/** @} */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* THEMIS_SECURE_KEYGEN_H */
