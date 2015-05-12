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
 * @file soter_hash.h
 * @brief routines for hash calculation
 */

#ifndef SOTER_HASH_H
#define SOTER_HASH_H

#include <soter/soter.h>

/**
 * @addtogroup SOTER
 * @{
 * @defgroup HASH hash
 * @brief Hash calculation routines
 * @details Usage example:
 * @code
 * #include <soter/soter.h>
 * ...
 * uint8_t data[data_length];
 * uint8_t *hash=NULL;
 * size_t hash_length;
 * soter_hash_ctx_t* ctx=soter_hash_create(SOTER_HASH_SHA512);
 * if(ctx){
 *	soter_status_t res=soter_hash_update(ctx, data, data_length);
 *	if(res==SOTER_SUCCESS){
 *		res=soter_hash_final(ctx, NULL, &hash_length);
 *		if(res==SOTER_BUFFER_TOO_SMALL){
 *			hash=malloc(hash_length);
 *			if(hash){
 *				res=soter_hash_final(ctx, hash, &hash_length);
 *				if(res==SOTER_SUCCESS){
 *					//output hash
 *				}
 *				free(hash);
 *			}
 *		}
 *	}
 *	soter_hash_destroy(ctx);
 * }
 * @endcode
 * @{
 */

/**
 * @enum soter_hash_algo_type
 * @brief Supported hash algorithms
 */
enum soter_hash_algo_type
{
	SOTER_HASH_SHA1,    /**< sha1   */
	SOTER_HASH_SHA256,  /**< sha256 */
	SOTER_HASH_SHA512,  /**< sha512 */
};

/** 
 * @brief hash algorithm typedef
 */
typedef enum soter_hash_algo_type soter_hash_algo_t;

/**
 * @typedef soter_hash_ctx_t
 * @brief hash context typedef
 */
typedef struct soter_hash_ctx_type soter_hash_ctx_t;

/**
 * @brief creating of hash context
 * @param [in] algo hash algorithm to be used; see @ref soter_hash_algo_type
 * @return pointer to hash context on sussecc and  NULL on failure
 */
soter_hash_ctx_t* soter_hash_create(soter_hash_algo_t algo);

/**
 * @brief destroy hash context
 * @param [in] hash_ctx pointer to hash context previosly created by @ref soter_hash_create
 * @return @ref SOTER_SUCCESS on success and @ref SOTER_FAIL on failure
 */
soter_status_t soter_hash_destroy(soter_hash_ctx_t *hash_ctx);

/**
 * @brief update hash context with data
 * @param [in] hash_ctx pointer to hash context previosly created by @ref soter_hash_create
 * @param [in] data pointer to buffer with data to hash update
 * @param [in] length of data buffer
 * @return @ref SOTER_SUCCESS on success and @ref SOTER_FAIL on failure
 */
soter_status_t soter_hash_update(soter_hash_ctx_t *hash_ctx, const void *data, size_t length);

/**
 * @brief final hash context and get hash value
 * @param [in] hash_ctx pointer to hash context previosly created by @ref soter_hash_create
 * @param [out] hash_value pointer to buffer for hash value retrieve, may be set to NULL for hash value length determination
 * @param [in, out] hash_length length of hash_value buffer
 * @return @ref SOTER_SUCCESS on success and @ref SOTER_FAIL on failure. 
 * @note If hash_value==NULL or hash_length less then need to store hash value, @ref SOTER_BUFFER_TOO_SMALL will return and hash_length will contain length of buffer thet need to store hash value.
 */
soter_status_t soter_hash_final(soter_hash_ctx_t *hash_ctx, uint8_t* hash_value, size_t* hash_length);

/**@}@}*/

#endif /* SOTER_HASH_H */
