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
#import "scell.h"

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
 * Initialise Secure Cell in Seal mode with a master key.
 *
 * @param [in] key  non-empty master key
 *
 * @returns @c nil if key is empty.
 */
- (nullable instancetype)initWithKey:(NSData *)key;

/**
 * Encrypt data.
 *
 * @param [in]  message data to encrypt, must not be empty
 * @param [in]  context optional associated context
 * @param [out] error   optional error output
 *
 * Data is encrypted and authentication token is appended to form a single sealed buffer.
 *
 * The context, if provided, is cryptographically mixed with the data, but is not included
 * into the resulting encrypted message. You will have to provide the same context again
 * during decryption. Usually this is some plaintext data associated with encrypted data,
 * such as database row number, protocol message ID, etc.
 *
 * @returns Encrypted data is returned as a single buffer.
 *
 * @returns @c nil will be returned on failure, such as if @c message is empty
 * or in case of some internal failure in cryptographic backend.
 *
 * @returns If @c error is not @c nil, it will describe the reason of failure.
 */
- (nullable NSData *)encrypt:(NSData *)message
                     context:(nullable NSData *)context
                       error:(NSError **)error;

/**
 * Encrypt data.
 *
 * @param [in]  message data to encrypt, must not be empty
 * @param [out] error   optional error output
 *
 * Data is encrypted and authentication token is appended to form a single sealed buffer.
 *
 * This call is equivalent to encryption with @c nil associated context.
 *
 * @returns Encrypted data is returned as a single buffer.
 *
 * @returns @c nil will be returned on failure, such as if @c message is empty
 * or in case of some internal failure in cryptographic backend.
 *
 * @returns If @c error is not @c nil, it will describe the reason of failure.
 */
- (nullable NSData *)encrypt:(NSData *)message error:(NSError **)error;

/**
 * Encrypt data.
 * @see encrypt:context:error:
 */
- (nullable NSData *)encrypt:(NSData *)message context:(nullable NSData *)context
    NS_SWIFT_UNAVAILABLE("use throwing encrypt(_:,context:)");

/**
 * Encrypt data.
 * @see encrypt:context:error:
 */
- (nullable NSData *)encrypt:(NSData *)message
    NS_SWIFT_UNAVAILABLE("use throwing encrypt");

/**
 * Encrypt data.
 * @deprecated since Themis 0.13
 * @see encrypt:context:error:
 */
- (nullable NSData *)wrapData:(NSData *)message context:(nullable NSData *)context error:(NSError **)error
    __deprecated_msg("use 'encrypt:context:error:' instead");

/**
 * Encrypt data.
 * @deprecated since Themis 0.13
 * @see encrypt:error:
 */
- (nullable NSData *)wrapData:(NSData *)message error:(NSError **)error
    __deprecated_msg("use 'encrypt:error:' instead");

/**
 * Decrypt data.
 *
 * @param [in]  message data to decrypt, cannot be empty
 * @param [in]  context optional associated context
 * @param [out] error   optional error output
 *
 * Secure Cell validates association with the context data, decrypts the message,
 * and verifies data integrity using authentication data embedded into the message.
 *
 * You need to provide the same context as used during encryption
 * (or use @c nil or empty context if there was no context).
 *
 * @returns Decrypted data is returned if everything goes well.
 *
 * @returns @c nil will be returned on failure, such as if @c message is empty,
 * or if the data has been tampered with, or the secret or associated context
 * are not the same as were used to encrypt the data.
 *
 * @returns If @c error is not @c nil, it will describe the reason of failure.
 */
- (nullable NSData *)decrypt:(NSData *)message
                     context:(nullable NSData *)context
                       error:(NSError **)error;

/**
 * Decrypt data.
 *
 * @param [in]  message data to decrypt, cannot be empty
 * @param [out] error   optional error output
 *
 * Secure Cell decrypts the message and verifies data integrity using
 * authentication data embedded into the message.
 *
 * This call is equivalent to decryption with @c nil associated context.
 *
 * @returns Decrypted data is returned if everything goes well.
 *
 * @returns @c nil will be returned on failure, such as if @c message is empty,
 * or if the data has been tampered with, or the secret or associated context
 * are not the same as were used to encrypt the data.
 *
 * @returns If @c error is not @c nil, it will describe the reason of failure.
 */
- (nullable NSData *)decrypt:(NSData *)message error:(NSError **)error;

/**
 * Decrypt data.
 * @see decrypt:context:error:
 */
- (nullable NSData *)decrypt:(NSData *)message context:(nullable NSData *)context
    NS_SWIFT_UNAVAILABLE("use throwing decrypt(_:,context:)");

/**
 * Decrypt data.
 * @see decrypt:context:error:
 */
- (nullable NSData *)decrypt:(NSData *)message
    NS_SWIFT_UNAVAILABLE("use throwing decrypt");

/**
 * Decrypt data.
 * @deprecated since Themis 0.13
 * @see decrypt:context:error:
 */
- (nullable NSData *)unwrapData:(NSData *)message context:(nullable NSData *)context error:(NSError **)error
    __deprecated_msg("use 'decrypt:context:error:' instead");

/**
 * Decrypt data.
 * @deprecated since Themis 0.13
 * @see decrypt:error:
 */
- (nullable NSData *)unwrapData:(NSData *)message error:(NSError **)error
    __deprecated_msg("use 'decrypt:error:' instead");

@end

NS_ASSUME_NONNULL_END

/** @} */
/** @} */
