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
 * @file soter_asym_sign.h
 * @brief signature/verify routines
 */
#ifndef SOTER_ASYM_SIGN_H
#define SOTER_ASYM_SIGN_H

#include <soter/soter_api.h>
#include <soter/soter_error.h>

/** @addtogroup SOTER
 * @{
 * @defgroup SOTER_ASYM_SIGN signature/verify routines
 * @brief signature/verify routines
 * @{
 */

/** @brief supported signature algorithms */
enum soter_sign_alg_type {
    SOTER_SIGN_undefined,       /**< undefined */
    SOTER_SIGN_rsa_pss_pkcs8,   /**< RSA with PSS padding */
    SOTER_SIGN_ecdsa_none_pkcs8 /**< ECDSA */
};

/** @brief signature algorithm typedef */
typedef enum soter_sign_alg_type soter_sign_alg_t;

/** @defgroup SOTER_ASYM_SIGN_SIGN signing
 * @brief signing routines
 * @{
 */

/** @brief sign context typedef */
typedef struct soter_sign_ctx_type soter_sign_ctx_t;

/** @brief create sign context
 * @param [in] alg algorithm for signing. See @ref soter_sign_alg_type
 * @param [in] private_key buffer with private key
 * @param [in] private_key_length length of private_key
 * @param [in] public_key buffer with public key
 * @param [in] public_key_length length of public_key
 * @return result of operation, @ref SOTER_SUCCESS on success or SOTER_FAIL on failure
 * @note If private_key==NULL and public_key==NULL with creating of sign context will be generated
 * new key pair.
 */
SOTER_API
soter_sign_ctx_t* soter_sign_create(soter_sign_alg_t alg,
                                    const void* private_key,
                                    size_t private_key_length,
                                    const void* public_key,
                                    size_t public_key_length);

/** @brief update sign context
 * @param [in] ctx pointer to sign context previously created by soter_sign_create
 * @param [in] data data to sign
 * @param [in] data_length length of data
 * @return result of operation, @ref SOTER_SUCCESS on success or SOTER_FAIL on failure
 */
SOTER_API
soter_status_t soter_sign_update(soter_sign_ctx_t* ctx, const void* data, size_t data_length);

/** @brief final sign context
 * @param [in] ctx pointer to sign context previously created by soter_sign_create
 * @param [out] signature buffer to store signature. May be set to NULL for signature length
 * determination
 * @param [in, out] signature_length length of signature
 * @return result of operation, @ref SOTER_SUCCESS on success or SOTER_FAIL on failure
 * @note If signature==NULL or signature_length less than needed to store signature, @ref
 * SOTER_BUFFER_TOO_SMALL will return and signature_length will contain length of buffer needed
 * to store signature.
 */
SOTER_API
soter_status_t soter_sign_final(soter_sign_ctx_t* ctx, void* signature, size_t* signature_length);

/** @brief export key from sign context
 * @deprecated Use soter_sign_export_private_key() or soter_sign_export_public_key() instead.
 * @param [in] ctx pointer to sign context previously created by soter_sign_create
 * @param [out] key buffer to store exported key
 * @param [in,out] key_length length of key. May be set to NULL for key length determination
 * @param [in] isprivate if set private key will be exported. If not set public key will be exported
 * @return result of operation, @ref SOTER_SUCCESS on success or @ref SOTER_FAIL on failure
 * @note If key==NULL or key_length less than needed to store key, @ref SOTER_BUFFER_TOO_SMALL will
 * return and key_length will contain length of buffer needed to store key.
 */
DEPRECATED("use soter_sign_export_private_key() or soter_sign_export_public_key() instead")
SOTER_API
soter_status_t soter_sign_export_key(soter_sign_ctx_t* ctx, void* key, size_t* key_length, bool isprivate);

/**
 * Export private key from sign context.
 *
 * @param [in]     ctx        pointer to sign context
 * @param [out]    key        buffer to store exported key
 * @param [in,out] key_length length of key. May be set to NULL for key length determination.
 *
 * @return @ref SOTER_SUCCESS on success or @ref SOTER_FAIL on failure
 *
 * @note If key == NULL or if key_length is smaller than needed to store the key,
 * @ref SOTER_BUFFER_TOO_SMALL is returned and key_length is set to required length of the buffer.
 */
SOTER_API
soter_status_t soter_sign_export_private_key(const soter_sign_ctx_t* ctx, void* key, size_t* key_length);

/**
 * Export public key from sign context.
 *
 * @param [in]     ctx        pointer to sign context
 * @param [in]     compressed whether to export the key in compressed format, if applicable.
 * @param [out]    key        buffer to store exported key
 * @param [in,out] key_length length of key. May be set to NULL for key length determination.
 *
 * @return @ref SOTER_SUCCESS on success or @ref SOTER_FAIL on failure
 *
 * @note If key == NULL or if key_length is smaller than needed to store the key,
 * @ref SOTER_BUFFER_TOO_SMALL is returned and key_length is set to required length of the buffer.
 */
SOTER_API
soter_status_t soter_sign_export_public_key(const soter_sign_ctx_t* ctx,
                                            bool compressed,
                                            void* key,
                                            size_t* key_length);

/** @brief destroy sign context
 * @param [in] ctx pointer to sign context previously created by soter_sign_create
 * @return result of operation, @ref SOTER_SUCCESS on success or @ref SOTER_FAIL on failure
 */
SOTER_API
soter_status_t soter_sign_destroy(soter_sign_ctx_t* ctx);

/** @brief get used algorithm id from sign context
 * @param [in] ctx pointer to sign context previously created by soter_sign_create
 * @return used algorithm id (see @ref soter_sign_alg_type) on success or @ref SOTER_SIGN_undefined
 * on failure
 */
SOTER_API
soter_sign_alg_t soter_sign_get_alg_id(soter_sign_ctx_t* ctx);
/** @}*/

/** @defgroup SOTER_ASYM_SIGN_VERIFY verifying
 * @brief verifying routines
 * @{
 */

/** @brief verify context typedef */
typedef struct soter_sign_ctx_type soter_verify_ctx_t;

/** @brief create verify context
 * @param [in] alg algorithm for verifying. See @ref soter_sign_alg_type
 * @param [in] private_key buffer with private key
 * @param [in] private_key_length length of private_key
 * @param [in] public_key buffer with public key
 * @param [in] public_key_length length of public_key
 * @return result of operation, @ref SOTER_SUCCESS on success or @ref SOTER_FAIL on failure
 */
SOTER_API
soter_verify_ctx_t* soter_verify_create(soter_sign_alg_t alg,
                                        const void* private_key,
                                        size_t private_key_length,
                                        const void* public_key,
                                        size_t public_key_length);

/** @brief update verify context
 * @param [in] ctx pointer to verify context previously created by soter_verify_create
 * @param [in] data data to verify
 * @param [in] data_length length of data
 * @return result of operation, @ref SOTER_SUCCESS on success or @ref SOTER_FAIL on failure
 */
SOTER_API
soter_status_t soter_verify_update(soter_verify_ctx_t* ctx, const void* data, size_t data_length);

/** @brief final verify context
 * @param [in] ctx pointer to verify context previously created by soter_verify_create
 * @param [in] signature signature to verify.
 * @param [in] signature_length length of signature
 * @return result of operation, @ref SOTER_SUCCESS on success or @ref SOTER_INVALID_SIGNATURE on
 * incorrect signature or @ref SOTER_FAIL on other failure
 */
SOTER_API
soter_status_t soter_verify_final(soter_verify_ctx_t* ctx, const void* signature, size_t signature_length);

/** @brief destroy verify context
 * @param [in] ctx pointer to verify context previously created by soter_verify_create
 * @return result of operation, @ref SOTER_SUCCESS on success or @ref SOTER_FAIL on failure
 */
SOTER_API
soter_status_t soter_verify_destroy(soter_verify_ctx_t* ctx);

/** @brief get used algorithm id from verify context
 * @param [in] ctx pointer to verify context previously created by soter_verify_create
 * @return used algorithm id (see @ref soter_sign_alg_type) on success or @ref SOTER_SIGN_undefined
 * on failure
 */
SOTER_API
soter_sign_alg_t soter_verify_get_alg_id(soter_verify_ctx_t* ctx);

/** @} */
/** @} */

#endif /* SOTER_ASYM_SIGN_H */
