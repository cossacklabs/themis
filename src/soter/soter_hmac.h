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
 * @file soter_hmac.h
 * @brief HMAC calculation routines
 */
#ifndef SOTER_HMAC_H
#define SOTER_HMAC_H

#include <soter/soter_hash.h>

/**
 * @addtogroup SOTER 
 * @{
 * @defgroup SOTER_HMAC HMAC
 * @brief HMAC calculation routines
 * @details Usage example:
 * @code
 * #include <soter/soter.h>
 * ...
 * uint8_t data[data_length];
 * uint8_t *hmac=NULL;
 * size_t hmac_length;
 * soter_hmac_ctx_t* ctx=soter_hmac_create(SOTER_HASH_SHA512);
 * if(ctx){
 *	soter_status_t res=soter_hmac_update(ctx, data, data_length);
 *	if(res==SOTER_SUCCESS){
 *		res=soter_hmac_final(ctx, NULL, &hmac_length);
 *		if(res==SOTER_BUFFER_TOO_SMALL){
 *			hmac=malloc(hmac_length);
 *			if(hmac){
 *				res=soter_hmac_final(ctx, hash, &hash_length);
 *				if(res==SOTER_SUCCESS){
 *					//output hmac
 *				}
 *				free(hmac);
 *			}
 *		}
 *	}
 *	soter_hmac_destroy(ctx);
 * }
 * @endcode
 * @{
 * 
*/

/**
 * @typedef soter_hmac_ctx_t
 * @brief  HMAC context typedef
 */
typedef struct soter_hmac_ctx_type soter_hmac_ctx_t;

/**
 * @brief creating of HMAC context
 * @param [in] algo hash algorithm to be used; see @ref soter_hash_algo_type
 * @return pointer to HMAC context on sussecc and  NULL on failure
 */
soter_hmac_ctx_t* soter_hmac_create(soter_hash_algo_t algo, const uint8_t* key, size_t key_length);

/**
 * @brief destroy HMAC context
 * @param [in] hmac_ctx pointer to HMAC context previosly created by @ref soter_hmac_create
 * @return @ref SOTER_SUCCESS on success and @ref SOTER_FAIL on failure
 */
soter_status_t soter_hmac_destroy(soter_hmac_ctx_t *hmac_ctx);

/**
 * @brief update HMAC context with data
 * @param [in] hmac_ctx pointer to HMAC context previosly created by @ref soter_hmac_create
 * @param [in] data pointer to buffer with data to HMAC update
 * @param [in] length of data buffer
 * @return @ref SOTER_SUCCESS on success and @ref SOTER_FAIL on failure
 */
soter_status_t soter_hmac_update(soter_hmac_ctx_t *hmac_ctx, const void *data, size_t length);

/**
 * @brief final HMAC context and get hash value
 * @param [in] hmac_ctx pointer to hash context previosly created by @ref soter_hmac_create
 * @param [out] hmac_value pointer to buffer for HMAC value retrieve, may be set to NULL for HMAC value length determination
 * @param [in, out] hmac_length length of hmac_value buffer
 * @return @ref SOTER_SUCCESS on success and @ref SOTER_FAIL on failure. 
 * @note If hmac_value==NULL or hmac_length less then need to store HMAC value, @ref SOTER_BUFFER_TOO_SMALL will return and hmac_length will contain length of buffer thet need to store HMAC value.
 */
soter_status_t soter_hmac_final(soter_hmac_ctx_t *hmac_ctx, uint8_t* hmac_value, size_t* hmac_length);

/**@}@}*/

#endif /* SOTER_HMAC_H */
