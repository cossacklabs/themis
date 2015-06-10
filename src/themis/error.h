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
 * @file themis/error.h
 * @brief return type, return codes and check macros
 */

#ifndef THEMIS_ERROR_H
#define THEMIS_ERROR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <soter/error.h>

/** @brief return type */
typedef int themis_status_t;

/**
 * @addtogroup THEMIS
 * @{
 * @defgroup SOTER_ERROR_CODES status codes
 * @{
 */
#define THEMIS_SSESSION_SEND_OUTPUT_TO_PEER 1
#define THEMIS_SUCCESS SOTER_SUCCESS
#define THEMIS_FAIL   SOTER_FAIL
#define THEMIS_INVALID_PARAMETER SOTER_INVALID_PARAMETER
#define THEMIS_NO_MEMORY SOTER_NO_MEMORY
#define THEMIS_BUFFER_TOO_SMALL SOTER_BUFFER_TOO_SMALL
#define THEMIS_DATA_CORRUPT SOTER_DATA_CORRUPT
#define THEMIS_INVALID_SIGNATURE SOTER_INVALID_SIGNATURE
#define THEMIS_NOT_SUPPORTED SOTER_NOT_SUPPORTED
#define THEMIS_SSESSION_KA_NOT_FINISHED -8
#define THEMIS_SSESSION_TRANSPORT_ERROR -9

#define THEMIS_SCOMPARE_SEND_OUTPUT_TO_PEER THEMIS_SSESSION_SEND_OUTPUT_TO_PEER
/** @} */

/**
 * @defgroup THEMIS_ERROR_OUT routines for error and debug output
 * @{
 */

#ifdef DEBUG
#define THEMIS_ERROR_OUT(message) SOTER_ERROR_OUT(message) 
#define THEMIS_DEBUG_OUT(message) SOTER_DEBUG_OUT(message)
#else
#define THEMIS_ERROR_OUT(message)  
#define THEMIS_DEBUG_OUT(message) 
#endif

/**@}*/

/**
 * @defgroup SOTER_CHECK_ROUTINES routines for parameters and variables checking
 * @{
 */

#define THEMIS_CHECK(x) SOTER_CHECK(x)
#define THEMIS_CHECK_(x) SOTER_CHECK_(x)

#define THEMIS_CHECK_PARAM(x) SOTER_CHECK_PARAM(x)
#define THEMIS_CHECK_PARAM_(x) SOTER_CHECK_PARAM_(x)

#define THEMIS_CHECK_MALLOC(x,y) SOTER_CHECK_MALLOC(x,y)

#define THEMIS_CHECK_MALLOC_(x) SOTER_CHECK_MALLOC_(x)

#define THEMIS_CHECK_FREE(x,y) SOTER_CHECK_FREE(x,y)

#define THEMIS_IF_FAIL(x,y) SOTER_IF_FAIL(x,y)

#define THEMIS_IF_FAIL_(x,y) SOTER_IF_FAIL_(x,y)

#define THEMIS_STATUS_CHECK(x,y) SOTER_STATUS_CHECK(x,y)

#define THEMIS_STATUS_CHECK_FREE(x,y,z) SOTER_STATUS_CHECK_FREE(x,y,z)

/** @} 
 * @}
 */

#endif /* THEMIS_ERROR_H */
