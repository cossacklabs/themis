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
 * @file soter_rand.h
 * @brief random bits generator
 */

#ifndef SOTER_RAND_H
#define SOTER_RAND_H

#include <soter/soter_api.h>
#include <soter/soter_error.h>

/**
 * @addtogroup SOTER
 * @{
 * @defgroup SOTER_RAND generating random data
 * @brief Routines for generating random data
 * @{
 */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Generates pseudo-random bytes
 *
 * @param [out] buffer pointer to the output buffer for random data
 * @param [in]  length length of the buffer
 * @return success code
 *
 * This function generates cryptographically strong pseudo-random bytes
 * and fills the provided buffer with them.
 *
 * SOTER_FAIL indicates that there is not enough entropy available
 * to fill the entire buffer. Please try again later.
 */
SOTER_MUST_USE
SOTER_API
soter_status_t soter_rand(uint8_t* buffer, size_t length);

#ifdef __cplusplus
}
#endif

/**@}@}*/

#endif /* SOTER_RAND_H */
