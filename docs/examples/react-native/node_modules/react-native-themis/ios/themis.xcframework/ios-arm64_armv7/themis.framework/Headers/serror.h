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

#ifndef OBJCTHEMIS_S_ERROR_H
#define OBJCTHEMIS_S_ERROR_H

/**
 * @file objthemis/serror.h
 * @brief Status codes defenitions for Obj-C wrapper for themis
 */

/**
 * @defgroup WRAPPERS wrappers 
 * @brief Themis wrappers for high level languages
 * @{
 * @defgroup OBJC Objective C
 * @brief Themis wrapper fro Obj-C
 * @{
 */

/** @brief Status codes */
typedef NS_ENUM(NSInteger, TSErrorType) {
    TSErrorTypeSuccess = 0, /**< Success */
    TSErrorTypeBufferTooSmall = 14, /**< Buffer is too small */
    TSErrorTypeFail = 11, /**< Fail */
    TSErrorTypeSendAsIs = 1   /**< Send as is */
};


/** @brief Error generation macro
* @param [in] error_code error code
* @param [in] error_message human readable error message
*/

#define SCERROR_DOMAIN @"com.CossackLabs.Themis.ErrorDomain"

#define SCERROR(error_code, error_message)                \
  [NSError errorWithDomain:SCERROR_DOMAIN    \
  code:error_code \
  userInfo:@{ NSLocalizedDescriptionKey : error_message}]

/** @} */
/** @} */

#endif //OBJCTHEMIS_S_ERROR_H