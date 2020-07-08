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
#import "scell.h"

/**
 * @addtogroup WRAPPERS
 * @{
 * @addtogroup OBJC
 * @{
 */

NS_ASSUME_NONNULL_BEGIN

/**
 * Secure Cell in Context Imprint mode.
 *
 * Context Imprint mode is intended for environments where storage constraints
 * do not allow the size of the data to grow and there is no auxiliary storage
 * available. Context Imprint mode requires an additional "associated context"
 * to be provided along with the secret in order to protect the data.
 *
 * In Context Imprint mode no authentication token is computed or verified.
 * This means the integrity of the data is not enforced, so the overall
 * security level is slightly lower than in Seal or Token Protect modes.
 *
 * @note To ensure highest security level possible, supply a different
 * associated context for each encryption invocation with the same secret.
 *
 * Read more about Context Imprint mode:
 *
 * https://docs.cossacklabs.com/pages/secure-cell-cryptosystem/#context-imprint-mode
 */
@interface TSCellContextImprint : TSCell

/**
 * Initialise Secure Cell in Context Imprint mode with a master key.
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
 * @param [in]  context associated context, must not be empty
 * @param [out] error   optional error output
 *
 * Data is encrypted and context is cryptographically mixed with the data, but not included
 * into the resulting encrypted message. You will have to provide the same context again
 * during decryption. Usually this is some plaintext data associated with encrypted data,
 * such as database row number, protocol message ID, etc.
 *
 * @returns Encrypted data has the same length as input and cannot be authenticated.
 *
 * @returns @c nil will be returned on failure, such as if @c message or @c context is empty
 * or in case of some internal failure in cryptographic backend.
 *
 * @returns If @c error is not @c nil, it will describe the reason of failure.
 */
- (nullable NSData *)encrypt:(NSData *)message
                     context:(NSData *)context
                       error:(NSError **)error;

/**
 * Encrypt data.
 * @see encrypt:context:error:
 */
- (nullable NSData *)encrypt:(NSData *)message context:(NSData *)context
    NS_SWIFT_UNAVAILABLE("use throwing encrypt(_:,context:)");

/**
 * Encrypt data.
 * @deprecated since Themis 0.13
 * @see encrypt:context:error:
 */
- (nullable NSData *)wrapData:(NSData *)message context:(NSData *)context error:(NSError **)error
    __deprecated_msg("use 'encrypt:context:error:' instead");

/**
 * Decrypt data.
 *
 * @param [in]  message data to decrypt, cannot be empty
 * @param [in]  context associated context, cannot be empty
 * @param [out] error   optional error output
 *
 * Secure Cell validates association with the context data and decrypts the message.
 * You need to provide the same context as it was used during encryption.
 *
 * @returns Decrypted data is returned if everything goes well.
 *
 * @returns Note that data integrity is not verified by Context Imprint mode:
 * garbage in — garbage out. If data has been corrupted or context is incorrect
 * then Secure Cell will most likely successfully return corrupted output.
 *
 * @returns @c nil will be returned on failure, such as if @c message or @c context
 * is empty, or in case of some internal failure in cryptographic backend.
 *
 * @returns If @c error is not @c nil, it will describe the reason of failure.
 */
- (nullable NSData *)decrypt:(NSData *)message
                     context:(NSData *)context
                       error:(NSError **)error;

/**
 * Decrypt data.
 * @see decrypt:context:error:
 */
- (nullable NSData *)decrypt:(NSData *)message context:(NSData *)context
    NS_SWIFT_UNAVAILABLE("use throwing decrypt(_:,context:)");

/**
 * Decrypt data.
 * @deprecated since Themis 0.13
 * @see decrypt:context:error:
 */
- (nullable NSData *)unwrapData:(NSData *)message context:(NSData *)context error:(NSError **)error
    __deprecated_msg("use 'decrypt:context:error:' instead");

@end

NS_ASSUME_NONNULL_END

/** @} */
/** @} */
