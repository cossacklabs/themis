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
 * @file objthemis/scell_context_imprint.h
 * @brief secure cell context imprint mode interface
 */

#import <Foundation/Foundation.h>
#import <themis/themis.h>
#import <objcthemis/scell.h>

/**
 * @addtogroup WRAPPERS
 * @{
 * @addtogroup OBJC
 * @{
 */

/** @brief Secure Cell Context Imprint mode interface
*
* This object mode is for environments where storage constraints do not allow the size of the data to grow and
* there is no auxiliary storage available. Secure Cell context imprint relies on the user to provide the data context
* along with the secret to protect the information. Also, no authentication tag is computed or verified.
* This means the integrity of the data is not enforced, so the overall security level is slightly lower than preceding
* two cases.
*
* @image html scell-context_imprint.png Secure Cell Context imprint mode
* @note To ensure highest security level possible user has to supply different context for each encryption invocation
* of the object for the same secret.
*/

NS_ASSUME_NONNULL_BEGIN

@interface TSCellContextImprint : TSCell

/**
* @brief Initialize Secure cell object in context imprint mode
* @param [in] key master key
*/
- (nullable instancetype)initWithKey:(NSData *)key;

/**
* @brief Wrap message with context
* @param [in] message message to wrap
* @param [in] context user context
* @param [in] error pointer to Error on failure
* @return Wrapped message as NSData object on success or nil on failure
*/
- (nullable NSData *)wrapData:(NSData *)message context:(NSData *)context error:(NSError **)error;

/**
* @brief Unwrap message with context
* @param [in] message message to unwrap
* @param [in] context user context
* @param [in] error pointer to Error on failure
* @return Unwrapped message as NSData object on success or nil on failure
*/
- (nullable NSData *)unwrapData:(NSData *)message context:(NSData *)context error:(NSError **)error;

@end

NS_ASSUME_NONNULL_END

/** @} */
/** @} */
