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
  SMessageModeEncryptDecrypt, /**< encrypt/decrypt mode */
  SMessageModeSignVerify      /**< sign/verify mode */
} SMessageMode;

/** @brief Secure message interface */
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
