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

NS_ASSUME_NONNULL_BEGIN

/**
 * Secure Cell in Seal mode.
 *
 * This is the most secure and easy way to protect stored data. You provide
 * some secret (a master key) and the data then receive encrypted data back.
 *
 * Secure Cell in Seal mode will encrypt the data and append an "authentication
 * tag" to it with auxiliary security information. This means that that size
 * of the encrypted data will be larger than the original input.
 *
 * Additionally, it is possible to bind the encrypted data to some "associated
 * context" (for example, database row number). In this case decryption of the
 * data with incorrect context will fail (even if the secret is correct and
 * the data has not been tampered). This establishes cryptographically secure
 * association between the protected data and the context in which it is used.
 * With database row numbers, for example, this prevents the attacker from
 * swapping encrypted password hashes in the database so the system will not
 * accept credentials of a different user.
 *
 * @note Use @c TSGenerateSymmetricKey() to generate master keys suitable
 * for Secure Cell.
 *
 * Read more about Seal mode:
 *
 * https://docs.cossacklabs.com/pages/secure-cell-cryptosystem/#seal-mode
 */
@interface TSCellSeal : TSCell

/**
 * Initialise Secure Cell in Seal mode with a master key.
 *
 * @param [in] key  non-empty master key
 *
 * @returns @c nil if key is empty.
 */
- (nullable instancetype)initWithKey:(NSData *)key
    NS_DESIGNATED_INITIALIZER;

/**
 * Initialise Secure Cell in Seal mode with a passphrase.
 *
 * @param [in] passphrase   non-empty passphrase to use
 *
 * The passphrase string will be encoded in UTF-8.
 *
 * @returns @c nil if passphrase is empty.
 */
- (nullable instancetype)initWithPassphrase:(NSString *)passphrase;

/**
 * Initialise Secure Cell in Seal mode with raw passphrase data.
 *
 * @param [in] passphrase   non-empty passphrase to use
 *
 * @returns @c nil if passphrase is empty.
 */
- (nullable instancetype)initWithPassphraseData:(NSData *)passphrase;

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
