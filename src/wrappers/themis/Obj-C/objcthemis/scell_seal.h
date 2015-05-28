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
 * @brief secure cell seal mode interface
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

/** @brief Secure Cell Seal mode interface */
@interface SCell_seal : SCell

/**
 * @brief Initialize Secure cell object in seal mode
 * @param [in] key master key
 */
- (id)initWithKey: (NSData*)key;

/**
 * @brief Wrap message
 * @param [in] message message to wrap
 * @param [in] errorPtr pointer to Error on failure
 * @return Wrapped message as NSData object on success or NULL on failure
 */
- (NSData*)wrap: (NSData*)message error:(NSError**)error;

/**
 * @brief Unwrap message
 * @param [in] message message to unwrap
 * @param [in] errorPtr pointer to Error on failure
 * @return Unwrapped message as NSData object on success or NULL on failure
 */
- (NSData*)unwrap: (NSData*)message error:(NSError**)error;


/**
 * @brief Wrap message with context
 * @param [in] message message to wrap
 * @param [in] context user context
 * @param [in] errorPtr pointer to Error on failure
 * @return Wrapped message as NSData object on success or NULL on failure
 */
- (NSData*)wrap: (NSData*)message context:(NSData*)contex  error:(NSError**)error;

/**
 * @brief Unwrap message
 * @param [in] message message to unwrap
 * @param [in] errorPtr pointer to Error on failure
 * @return Unwrapped message as NSData object on success or NULL on failure
 */
- (NSData*)unwrap: (NSData*)message context:(NSData*)contex  error:(NSError**)error;

@end

/** @} */
/** @} */