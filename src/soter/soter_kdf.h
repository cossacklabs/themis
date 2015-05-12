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

#include <soter/soter.h>

/** @addtogroup SOTER
 * @{
 * @defgroup SOTER_KDF key derivation function
 * @brief key derivation function
 * @{
 */

/** @brief context buffer type */ 
struct soter_kdf_context_buf_type
{
	const uint8_t *data;
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
soter_status_t soter_kdf(const void *key, size_t key_length, const char *label, const soter_kdf_context_buf_t *context, size_t context_count, void *output, size_t output_length);
/** @} */
/** @} */

#endif /* SOTER_KDF_H */
