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
 * @file soter_kdf.h
 * @brief key derivation function
 */
#ifndef SOTER_KDF_H
#define SOTER_KDF_H

#include <soter/soter_api.h>
#include <soter/soter_error.h>

/** @addtogroup SOTER
 * @{
 * @defgroup SOTER_KDF key derivation function
 * @brief key derivation function
 * @{
 */

/** @brief context buffer type */
struct soter_kdf_context_buf_type {
    const uint8_t* data;
    size_t length;
};

/** @brief context buffer typedef */
typedef struct soter_kdf_context_buf_type soter_kdf_context_buf_t;

/** @brief derive
 * @param [in] key master key
 * @param [in] key_length length of key
 * @param [in] context pointer to array of context buffers
 * @param [in] context_count count of context buffers in context
 * @param [out] output buffer for derived data store
 * @param [in] output_length length of data to derive
 * @return SOTER_SUCCESS on success or SOTER_FAIL on failure
 */
SOTER_API
soter_status_t soter_kdf(const void* key,
                         size_t key_length,
                         const char* label,
                         const soter_kdf_context_buf_t* context,
                         size_t context_count,
                         void* output,
                         size_t output_length);

/**
 * Computes PKCS#5 PBKDF2 HMAC-SHA-256 for a passphrase.
 *
 * @param [in]  passphrase          passphrase used for derivation
 * @param [in]  passphrase_length   length of `passphrase` in bytes
 * @param [in]  salt                additional salt for derivation, may be NULL
 * @param [in]  salt_length         length of `salt` in bytes
 * @param [in]  iterations          PBKDF2 iteration count
 * @param [out] key                 output key buffer
 * @param [in]  key_length          length of `key` in bytes
 *
 * This function derives a key from a passphrase using a salt and iteration
 * count as specified in RFC 8018. It uses HMAC-SHA-256 as the hash function.
 *
 * The iteration count must be a positive number. The bigger it is, the slower
 * the derivation, and the harder it gets for an attacker to perform a brute
 * force attack with candidate passphrases. RFC 8018 suggests at least 1000.
 * We suggest using at least 100,000. Generally, you experiment with values,
 * use the biggest one that you can tolerate at the moment, and periodically
 * reevaluate your decision and increase the count as machines get faster.
 *
 * @returns SOTER_SUCCESS on successful key derivation.
 *
 * @exception SOTER_FAIL on critical backend failure.
 *
 * @exception SOTER_INVALID_PARAM if `passphrase` is NULL or `passphrase_length` is zero.
 * @exception SOTER_INVALID_PARAM if `salt` is NULL but `salt_length` is not zero.
 * @exception SOTER_INVALID_PARAM if `iterations` count is zero.
 * @exception SOTER_INVALID_PARAM if `key` is NULL or `key_length` is zero.
 */
SOTER_API
soter_status_t soter_pbkdf2_sha256(const uint8_t* passphrase,
                                   size_t passphrase_length,
                                   const uint8_t* salt,
                                   size_t salt_length,
                                   size_t iterations,
                                   uint8_t* key,
                                   size_t key_length);

/** @} */
/** @} */

#endif /* SOTER_KDF_H */
