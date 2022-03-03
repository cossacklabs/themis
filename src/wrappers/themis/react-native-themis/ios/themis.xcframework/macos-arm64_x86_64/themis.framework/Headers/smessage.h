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
* @file objthemis/smessage.h
* @brief secure message interface
*/

#import <Foundation/Foundation.h>

/**
* @addtogroup WRAPPERS
* @{
* @addtogroup OBJC
* @{
*/

NS_ASSUME_NONNULL_BEGIN

/** @brief Secure message modes */
typedef NS_ENUM(NSInteger, TSMessageMode) {

    /** @brief Encrypted message
    *
    * Encrypted message is useful when you need full stack of protection for your data and in most cases you will use
    * this flavor. Currently, internally, encrypted message uses @ref TSCellSeal "Secure Cell in seal" mode for data
    * protection.
    * @image html encrypted_message.png Secure Encrypted message
    */
            TSMessageModeEncryptDecrypt,

    /** @brief Signed message
    *
    * Signed message is useful for cases where you do not need data confidentiality. It allows the receiver to verify
    * the origin and integrity of the data while still allowing intermediate nodes to process it accordingly
    * (for example, route data based on its type).
    * @image html signed_message.png Secure Signed message
    */
            TSMessageModeSignVerify
};


/** @brief Secure message interface
*
* Secure message is a lightweight service which can help to deliver some message or data to your peer in a secure manner.
* It provides a simple way to protect your messages and bound them to credentials of communicating peers using strong
* cryptography. It adds data confidentiality, integrity and authenticity to your message in one shot
* (single function call). Some features are:
*    - strong data encryption
*    - message integrity and authentication
*    - key generation (both RSA and ECC)
*    - stateless, easy-to-use API
* Secure message only assumes that peers have each other's public key which they trust.
* Then they can freely exchange any messages with high security level and little overhead.
*
* @image html secure_message.png Secure Message
* Secure message comes in @ref SMessageMode "two flavors": signed message (integrity and authenticity) and
* encrypted message (confidentiality, integrity and authenticity).
*/
@interface TSMessage : NSObject


/** @brief private key */
@property(nonatomic, readonly) NSData *privateKey;

/** @brief public key */
@property(nonatomic, readonly) NSData *publicKey;

/** @brief mode */
@property(nonatomic, readonly) TSMessageMode mode;


/**
* @brief Initialize Secure message object in encrypt/decrypt mode
* @param [in] privateKey Private key
* @param [in] peerPublicKey Peer public key
*/
- (nullable instancetype)initInEncryptModeWithPrivateKey:(nonnull NSData *)privateKey peerPublicKey:(nonnull NSData *)peerPublicKey;

/**
* @brief Initialize Secure message object in sign/verify mode.
* @param [in] privateKey Private key
* @param [in] peerPublicKey Peer public key
*/
- (nullable instancetype)initInSignVerifyModeWithPrivateKey:(nullable NSData *)privateKey peerPublicKey:(nullable NSData *)peerPublicKey;

/**
* @brief Wrap message (encrypt using both keys or sign using own private key)
* @param [in] message message to wrap
* @param [in] error pointer to Error on failure
* @return Wrapped message as NSData object on success or nil on failure
 */
- (nullable NSData *)wrapData:(nullable NSData *)message error:(NSError *__autoreleasing *)error;

/**
* @brief Unwrap message (decrypt using both keys or verify using peer's public key)
* @param [in] message message to unwrap
* @param [in] error pointer to Error on failure
* @return Unwrapped message as NSData object on success or nil on failure
*/
- (nullable NSData *)unwrapData:(nullable NSData *)message error:(NSError *__autoreleasing *)error;

@end

NS_ASSUME_NONNULL_END

/** @} */
/** @} */
