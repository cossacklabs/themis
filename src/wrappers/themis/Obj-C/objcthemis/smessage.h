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
#import <themis/themis.h>

/**
 * @addtogroup WRAPPERS
 * @{
 * @addtogroup OBJC
 * @{
 */

/** @brief Secure message modes */
typedef enum{
  /** @brief Encrypted message
   * 
   * Encrypted message is useful when you need full stack of protection for your data and in most cases you will use this flavor. Currently, internally, encrypted message uses @ref SCell_seal "Secure Cell in seal" mode for data protection.
   * @image html encrypted_message.png Secure Encrypted message
   */
  SMessageModeEncryptDecrypt, 

  /** @brief Signed message
   * 
   * Signed message is useful for cases where you do not need data confidentiality. It allows the receiver to verify the origin and integrity of the data while still allowing intermediate nodes to process it accordingly (for example, route data based on its type).
   * @image html signed_message.png Secure Signed message
   */
  SMessageModeSignVerify      /**< sign/verify mode */
} SMessageMode;

/** @brief Secure message interface 
 *
 * Secure message is a lightweight service which can help to deliver some message or data to your peer in a secure manner. It provides a simple way to protect your messages and bound them to credentials of communicating peers using strong cryptography. It adds data confidentiality, integrity and authentity to your message in one shot (single function call). Some features are:
 *    - strong data encryption
 *    - message integrity and authentication
 *    - key generation (both RSA and ECC)
 *    - stateless, easy-to-use API
 * Secure message only assumes that peers have each other's public key which they trust. Then they can freely exchange any messages with high security level and little overhead.
 * @image html secure_message.png Secure Message
 * Secure message comes in @ref SMessageMode "two flavors": signed message (integrity and authentity) and encrypted message (confidentiality, integrity and authentity).
 */
@interface SMessage : NSObject

{
  NSData* _priv_key; /**< private  key */
  NSData* _peer_pub_key; /**< peer public key */ 
  SMessageMode _mode; /**< mode. @see SMessageMode */
}

/**
 * @brief Initialize Secure message object in encrypt/decrypt mode
 * @param [in] private_key Private key
 * @param [in] peer_public_key Peer public key
 */
- (id)initWithPrivateKey: (NSData*)private_key peerPublicKey:(NSData*)peer_pub_key;

/**
 * @brief Initialize Secure message object in sign/verify mode
 * @param [in] private_key Private key
 * @param [in] peer_public_key Peer public key
 */
- (id)initSVWithPrivateKey: (NSData*)private_key peerPublicKey:(NSData*)peer_pub_key;


/**
 * @brief Wrap message
 * @param [in] message message to wrap
 * @param [in] errorPtr pointer to Error on failure
 * @return Wrapped message as NSData object on success or NULL on failure
 */
- (NSData*)wrap: (NSData*)message error:(NSError**)errorPtr;

/**
 * @brief Unwrap message
 * @param [in] message message to unwrap
 * @param [in] errorPtr pointer to Error on failure
 * @return Unwrapped message as NSData object on success or NULL on failure
 */
- (NSData*)unwrap: (NSData*)message error:(NSError**)errorPtr;
@end

/** @} */
/** @} */
