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
 * @file objthemis/scell_token.h
 * @brief secure cell token protect mode interface
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

/** @brief encrypted message by secure cell in token protect mode */ 
@interface SCellTokenEncryptedData : NSObject{
  NSMutableData* cipher_text; /**< chipher text */
  NSMutableData* token; /**< token */
};

- (id)init; /**< @breaf initialize */
- (NSMutableData *)getCipherText; /**< @breaf return chipher text */
- (NSMutableData *)getToken;      /**< @breaf return token */
- (void)setCipherData:(NSMutableData*)data;  /**< @breaf set chipher text */
- (void)setToken:(NSMutableData*)data;       /**< @breaf set token */

@end

/** @brief Secure Cell Context Token Protect interface */
@interface SCell_token : SCell

/**
 * @brief Initialize Secure cell object in context imprint mode
 * @param [in] key master key
 */
- (id)initWithKey: (NSData*)key;

/**
 * @brief Wrap message
 * @param [in] message message to wrap
 * @param [in] errorPtr pointer to Error on failure
 * @return Wrapped message as NSData object on success or NULL on failure
 */
- (SCellTokenEncryptedData*)wrap: (NSData*)message error:(NSError**)errorPtr;

/**
 * @brief Unwrap message
 * @param [in] message message to unwrap
 * @param [in] errorPtr pointer to Error on failure
 * @return Unwrapped message as NSData object on success or NULL on failure
 */
- (NSData*)unwrap: (SCellTokenEncryptedData*)message error:(NSError**)errorPtr;

/**
 * @brief Wrap message with context
 * @param [in] message message to wrap
 * @param [in] context user context
 * @param [in] errorPtr pointer to Error on failure
 * @return Wrapped message as NSData object on success or NULL on failure
 */
- (SCellTokenEncryptedData*)wrap: (NSData*)message context:(NSData*)contex error:(NSError**)errorPtr;

/**
 * @brief Unwrap message with context
 * @param [in] message message to unwrap
 * @param [in] context user context
 * @param [in] errorPtr pointer to Error on failure
 * @return Unwrapped message as NSData object on success or NULL on failure
 */
- (NSData*)unwrap: (SCellTokenEncryptedData*)message context:(NSData*)contex error:(NSError**)errorPtr;

@end

/** @} */
/** @} */
