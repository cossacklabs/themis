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

/** @brief Secure Cell Seal mode interface
*
* This is the most secure and easy way to protect stored data. All one have to do is to provide some secret
* (password, secret key etc) to the API and the data itself.
* The data will be encrypted and an authentication tag will be appended to the data, so the size of the encrypted data
* will be larger than original. Also, users of this object mode can bind the data to some context
* (for example, database row number), so decryption of the data with incorrect context will fail
* (even if the secret will be correct). This allows establishing cryptographically secure associations between
* protected data and its context. In example with database row numbers, it will prevent encrypted data from tampering
* by attacker (for example, forcing the system to accept wrong hash to check credentials by displacing
* row numbers or primary key values).
* @image html scell-seal.png "Secure Cell Seal mode"
*/
NS_ASSUME_NONNULL_BEGIN

@interface TSCellSeal : TSCell

/**
* @brief Initialize Secure cell object in seal mode
* @param [in] key master key
*/
- (nullable instancetype)initWithKey:(NSData *)key;

/**
* @brief Wrap message
* @param [in] message message to wrap
* @param [in] error pointer to Error on failure
* @return Wrapped message as NSData object on success or nil on failure
*/
- (nullable NSData *)wrapData:(NSData *)message error:(NSError * __autoreleasing *)error;

/**
* @brief Unwrap message
* @param [in] message message to unwrap
* @param [in] error pointer to Error on failure
* @return Unwrapped message as NSData object on success or nil on failure
*/
- (nullable NSData *)unwrapData:(NSData *)message error:(NSError * __autoreleasing *)error;

/**
* @brief Wrap message with context
* @param [in] message message to wrap
* @param [in] context user context
* @param [in] error pointer to Error on failure
* @return Wrapped message as NSData object on success or nil on failure
*/
- (nullable NSData *)wrapData:(NSData *)message context:(nullable NSData *)context error:(NSError * __autoreleasing *)error;

/**
* @brief Unwrap message
* @param [in] message message to unwrap
* @param [in] error pointer to Error on failure
* @return Unwrapped message as NSData object on success or nil on failure
*/
- (nullable NSData *)unwrapData:(NSData *)message context:(nullable NSData *)context error:(NSError * __autoreleasing *)error;

@end

NS_ASSUME_NONNULL_END

/** @} */
/** @} */
