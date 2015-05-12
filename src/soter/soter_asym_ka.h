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
 * @file soter_asym_ka.h
 * @brief asymmetric key agreement routines
 */
#ifndef SOTER_ASYM_KA_H
#define SOTER_ASYM_KA_H

#include <soter/soter.h>

/** @addtogroup SOTER
 * @{
 * @defgroup SOTER_ASYM_KA asymmetric key agreement routines
 * @brief asymmetric key agreement routines
 * @{
 */

/** @brief supported key agreement algorithms */
enum soter_asym_ka_alg_type
{
  SOTER_ASYM_KA_EC_P256 /**< elliptic curve 256 */
};

/** @brief key agreement algorims typedef */
typedef enum soter_asym_ka_alg_type soter_asym_ka_alg_t;

/** @brief key agreement context typedef */
typedef struct soter_asym_ka_type soter_asym_ka_t;


/** @brief create key agreement context
 * @param [in] alg algorithm to use. See @ref soter_asym_ka_alg_type
 * @return pointer to created key agreement context on success or NULL on failure
 */
soter_asym_ka_t* soter_asym_ka_create(soter_asym_ka_alg_t alg);

/**
 * @brief asymmetric keys pair generation for key agreement context
 * @param [in] asym_ka_ctx pointer to key agreement context previously created by soter_asym_ka_create
 * @return SOTER_SUCESS on success or SOTER_FAIL on failure
 */
soter_status_t soter_asym_ka_gen_key(soter_asym_ka_t* asym_ka_ctx);

/**
 * @brief export key from key agreement context
 * @param [in] asym_ka_ctx pointer to key agreement context previously created by soter_asym_ka_create
 * @param [out] key buffer to store exported key
 * @param [in,out] key_length length of key. May be set to NULL for key length determination
 * @param [in] isprivate if set private key will be exported. If not set public key will be exported
 * @return  SOTER_SUCESS on success or SOTER_FAIL on failure
 * @note If key==NULL or key_length less then need to store key, @ref SOTER_BUFFER_TOO_SMALL will return and key_length will contain length of buffer thet need to store key.
 */
soter_status_t soter_asym_ka_export_key(soter_asym_ka_t* asym_ka_ctx, void* key, size_t* key_length, bool isprivate);

/**
 * @brief import key to key agreement context
 * @param [in] asym_ka_ctx pointer to key agreement context previously created by soter_asym_ka_create
 * @param [in] key buffer with stored key
 * @param [in] key_length length of key
 * @return  SOTER_SUCESS on success or SOTER_FAIL on failure
 */
soter_status_t soter_asym_ka_import_key(soter_asym_ka_t* asym_ka_ctx, const void* key, size_t key_length);

/**
 * @brief derive shared secret from key agreement context
 * @param [in] asym_ka_ctx pointer to key agreement context previously created by soter_asym_ka_create
 * @param [in] peer_key buffer with peer public key
 * @param [in] peer_key_length length of peer_key
 * @param [out] shared_secret buffer to store shared secret. May be set to NULL for shared secret length determination
 * @param [in,out] shared_secret_length length of shared secret
 * @return  SOTER_SUCESS on success or SOTER_FAIL on failure
 * @note If shared_secret==NULL or shared_secret_length less then need to store shared secret, @ref SOTER_BUFFER_TOO_SMALL will return and shared_secret_length will contain length of buffer thet need to store shared secret.
 */
soter_status_t soter_asym_ka_derive(soter_asym_ka_t* asym_ka_ctx, const void* peer_key, size_t peer_key_length, void *shared_secret, size_t* shared_secret_length);

/**
 * @brief destroy key agreement context
 * @param [in] asym_cipher_ctx pointer to key agreement context previously created by soter_asym_ka_create
 * @return  SOTER_SUCESS on success or SOTER_FAIL on failure
 */
soter_status_t soter_asym_ka_destroy(soter_asym_ka_t* asym_ka_ctx);

/** @} */
/** @} */

#endif /* SOTER_ASYM_KA_H */
