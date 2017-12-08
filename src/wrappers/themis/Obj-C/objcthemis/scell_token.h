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
NS_ASSUME_NONNULL_BEGIN

/** @brief encrypted message by secure cell in token protect mode */
@interface TSCellTokenEncryptedData : NSObject

/**< @breaf cipher text */
@property (nonatomic, strong) NSMutableData * cipherText;

/**< @breaf token */
@property (nonatomic, strong) NSMutableData * token;

@end


/** @brief Secure Cell Context Token Protect interface
*
* This object mode is designed for cases when underlying storage constraints do not allow the size of the data to grow
* (so @ref TSCellSeal "Secure cell seal" cannot be used), however the user has access to a different storage location
* (ex. another table in the database) where he can store needed security parameters.
* The Secure Cell object puts authentication tag and other auxiliary information (aka data token) to a separate buffer,
* so user can store it elsewhere, while keeping the original encrypted data size.
* The same token has to be provided along with the correct secret for data to be decrypted successfully.
* Since the same security parameters are used (just stored in a different location) this object mode has same security
* level as @ref TSCellSeal "Secure cell seal" but requires slightly more effort from the user.
* Also, user has the ability to bind the data to its context as before.
* @image html scell-token_protect.png "Secure Cell Token protect mode"
*/

@interface TSCellToken : TSCell

/**
* @brief Initialize Secure cell object in context token mode
* @param [in] key master key
*/
- (nullable instancetype)initWithKey:(NSData *)key;

/**
* @brief Wrap message
* @param [in] message message to wrap
* @param [in] error pointer to Error on failure
* @return Wrapped message as NSData object on success or nil on failure
*/
- (nullable TSCellTokenEncryptedData *)wrapData:(NSData *)message error:(NSError * __autoreleasing *)error;

/**
* @brief Unwrap message
* @param [in] message message to unwrap
* @param [in] error pointer to Error on failure
* @return Unwrapped message as NSData object on success or nil on failure
*/
- (nullable NSData *)unwrapData:(TSCellTokenEncryptedData *)message error:(NSError * __autoreleasing *)error;

/**
* @brief Wrap message with context
* @param [in] message message to wrap
* @param [in] context user context
* @param [in] error pointer to Error on failure
* @return Wrapped message as NSData object on success or nil on failure
*/
- (nullable TSCellTokenEncryptedData *)wrapData:(NSData *)message context:(nullable NSData *)context error:(NSError * __autoreleasing *)error;

/**
* @brief Unwrap message with context
* @param [in] message message to unwrap
* @param [in] context user context
* @param [in] error pointer to Error on failure
* @return Unwrapped message as NSData object on success or nil on failure
*/
- (nullable NSData *)unwrapData:(TSCellTokenEncryptedData *)message context:(nullable NSData *)context error:(NSError * __autoreleasing *)error;

@end

NS_ASSUME_NONNULL_END

/** @} */
/** @} */
