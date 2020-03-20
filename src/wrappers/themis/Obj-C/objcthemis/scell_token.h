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
#import "scell.h"

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
@property(nonatomic, strong) NSMutableData *cipherText;

/**< @breaf token */
@property(nonatomic, strong) NSMutableData *token;

@end

/**
 * Encryption result returned by Secure Cell in Token Protect mode.
 */
@interface TSCellTokenEncryptedResult : NSObject

/** Encrypted data. */
@property (nonatomic, readonly) NSData *encrypted;
/** Authentication token. */
@property (nonatomic, readonly) NSData *token;

/**
 * Encrypted data.
 * @deprecated since Themis 0.13
 * @see encrypted
 */
@property (nonatomic, readonly) NSData *cipherText
    __deprecated_msg("use 'encrypted' instead");

@end


/**
 * Secure Cell in Token Protect mode.
 *
 * Token Protect mode is designed for cases when underlying storage constraints
 * do not allow the size of the data to grow (so @c TSCellSeal cannot be used).
 * However, if you have access to a different storage location (e.g., another
 * table in the database) where additional security parameters can be stored
 * then Token Protect mode can be used instead of Seal mode.
 *
 * Token Protect mode produces authentication tag and other auxiliary data
 * (aka "authentication token") in a detached buffer. This keeps the original
 * size of the encrypted data while enabling separate storage of security
 * information. Note that the same token must be provided along with the
 * correct secret and matching associated context in order for the data
 * to be decrypted successfully.
 *
 * Since @c TSCellToken uses the same security parameters as @c TSCellSeal
 * (just stored in a different location), these modes have the same highest
 * security level. Token Protect mode only requires slightly more effort
 * in exchange for preserving the original data size.
 *
 * @note Read more about Token Protect mode:
 *
 * https://docs.cossacklabs.com/pages/secure-cell-cryptosystem/#token-protect-mode
 */
@interface TSCellToken : TSCell

/**
 * Initialise Secure Cell in Token Protect mode with a master key.
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
 * Data is encrypted and authentication token is produced separately.
 *
 * The context, if provided, is cryptographically mixed with the data, but it not included
 * into the resulting encrypted message. You will have to provide the same context again
 * during decryption. Usually this is some plaintext data associated with encrypted data,
 * such as database row number, protocol message ID, etc.
 *
 * @returns Successful encryption returns an object containing encrypted data and
 * authentication token.
 *
 * @returns @c nil will be returned on failure, such as if @c message is empty
 * or in case of some internal failure in cryptographic backend.
 *
 * @returns If @c error is not @c nil, it will describe the reason of failure.
 */
- (nullable TSCellTokenEncryptedResult *)encrypt:(NSData *)message
                                         context:(nullable NSData *)context
                                           error:(NSError **)error;

/**
 * Encrypt data.
 *
 * @param [in]  message data to encrypt, must not be empty
 * @param [out] error   optional error output
 *
 * Data is encrypted and authentication token is produced separately.
 *
 * This call is equivalent to encryption with @c nil associated context.
 *
 * @returns Successful encryption returns an object containing encrypted data and
 * authentication token.
 *
 * @returns @c nil will be returned on failure, such as if @c message is empty
 * or in case of some internal failure in cryptographic backend.
 *
 * @returns If @c error is not @c nil, it will describe the reason of failure.
 */
- (nullable TSCellTokenEncryptedResult *)encrypt:(NSData *)message error:(NSError **)error;

/**
 * Encrypt data.
 * @see encrypt:context:error:
 */
- (nullable TSCellTokenEncryptedResult *)encrypt:(NSData *)message context:(nullable NSData *)context
    NS_SWIFT_UNAVAILABLE("use throwing encrypt(_:,context:)");

/**
 * Encrypt data.
 * @see encrypt:context:error:
 */
- (nullable TSCellTokenEncryptedResult *)encrypt:(NSData *)message
    NS_SWIFT_UNAVAILABLE("use throwing encrypt");

/**
 * Encrypt data.
 * @deprecated since Themis 0.13
 * @see encrypt:context:error:
 */
- (nullable TSCellTokenEncryptedData *)wrapData:(NSData *)message context:(nullable NSData *)context error:(NSError **)error
    __deprecated_msg("use 'encrypt:context:error:' instead");

/**
 * Encrypt data.
 * @deprecated since Themis 0.13
 * @see encrypt:error:
 */
- (nullable TSCellTokenEncryptedData *)wrapData:(NSData *)message error:(NSError **)error
    __deprecated_msg("use 'encrypt:error:' instead");

/**
 * Decrypt data.
 *
 * @param [in]  message data to decrypt, cannot be empty
 * @param [in]  token   authentication token to verify, cannot be empty
 * @param [in]  context optional associated context
 * @param [out] error   optional error output
 *
 * Secure Cell validates association with the context data, decrypts the message,
 * and verifies data integrity using the provided authentication token.
 *
 * You need to provide the same context as it was used during encryption
 * (or use @c nil or empty context if there was no context).
 *
 * @returns Decrypted data is returned if everything goes well.
 *
 * @returns @c nil will be returned on failure, such as if @c message or @c token
 * is empty, or if the data has been tampered with, or the secret or associated context
 * are not the same as were used to encrypt the data.
 *
 * @returns If @c error is not @c nil, it will describe the reason of failure.
 */
- (nullable NSData *)decrypt:(NSData *)message
                       token:(NSData *)token
                     context:(nullable NSData *)context
                       error:(NSError **)error;

/**
 * Decrypt data.
 *
 * @param [in]  message data to decrypt, cannot be empty
 * @param [in]  token   authentication token to verify, cannot be empty
 * @param [out] error   optional error output
 *
 * Secure Cell decrypts the message and verifies data integrity using
 * the provided authentication token.
 *
 * This call is equivalent to decryption with @c nil associated context.
 *
 * @returns Decrypted data is returned if everything goes well.
 *
 * @returns @c nil will be returned on failure, such as if @c message or @c token
 * is empty, or if the data has been tampered with, or the secret or associated context
 * are not the same as were used to encrypt the data.
 *
 * @returns If @c error is not @c nil, it will describe the reason of failure.
 */
- (nullable NSData *)decrypt:(NSData *)message token:(NSData *)token error:(NSError **)error;

/**
 * Decrypt data.
 * @see decrypt:token:context:error:
 */
- (nullable NSData *)decrypt:(NSData *)message token:(NSData *)token context:(nullable NSData *)context
    NS_SWIFT_UNAVAILABLE("use throwing decrypt(_:,token:,context:)");

/**
 * Decrypt data.
 * @see decrypt:token:context:error:
 */
- (nullable NSData *)decrypt:(NSData *)message token:(NSData *)token
    NS_SWIFT_UNAVAILABLE("use throwing decrypt(_:,context:)");

/**
 * Decrypt data.
 * @deprecated since Themis 0.13
 * @see decrypt:token:context:error:
 */
- (nullable NSData *)unwrapData:(TSCellTokenEncryptedData *)message context:(nullable NSData *)context error:(NSError **)error
    __deprecated_msg("use 'decrypt:token:context:error:' instead");

/**
 * Decrypt data.
 * @deprecated since Themis 0.13
 * @see decrypt:token:error:
 */
- (nullable NSData *)unwrapData:(TSCellTokenEncryptedData *)message error:(NSError **)error
    __deprecated_msg("use 'decrypt:token:error:' instead");

@end

NS_ASSUME_NONNULL_END

/** @} */
/** @} */
