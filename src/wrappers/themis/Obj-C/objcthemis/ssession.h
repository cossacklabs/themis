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
 * @file objthemis/ssession.h
 * @brief secure session interface
 */

#import <objcthemis/ssession_transport_interface.h>
#import <themis/themis.h>

/**
 * @addtogroup WRAPPERS
 * @{
 * @addtogroup OBJC
 * @{
 */

/** @brief Secure session interface 
 *
 * Secure session is a lightweight mechanism to secure any network communications (both private and public networks including Internet). It is protocol agnostic and operates on the 5th layer of the network OSI model. Some features:
 *    - secure end-to-end communication
 *    - perfect forward secrecy
 *    - strong mutual peer authentication
 *    - replay protection
 *    - low negotiation round-trip
 *    - uses strong cryptography (including ECC)
 *    - lightweight, easy to use
 *    - easy to integrate into existing applications
 *
 *  Communications over secure session contains 2 stages: negotiation (key agreement) stage and actual data exchange.
 *
 */
@interface SSession : NSObject
{
  secure_session_t* _session; /**< session */
}

/**
 * @brief Initialise Secure session object
 * @param [in] id User id
 * @param [in] private_key User private key
 * @param [in] callbacks Reference to SSession_transport_interface object
 */
-(instancetype)initWithId: (NSData*)id andPrivateKey:(NSData*)private_key andCallbacks:(SSession_transport_interface*)callbacks;


/** @brief Connection initialization message 
 * @return Connection initialization message on success or NULL on failure
*/
-(NSData*)connect_request: (NSError**)errorPtr;

/** @brief Create connection initialization message and send it to peer by \b send method from callbacks object. @see SSession_transport_interface. */
-(void)connect: (NSError**)errorPtr;

/** @brief Wrap message
 * @return Wrapped message in NSData object on success or NULL on failure.
 */ 
-(NSData*)wrap: (NSData*)message error:(NSError**)errorPtr;

/** @brief Unwrap message
 * @return Unwrapped message in NSData object on success or NULL on failure.
 */ 
-(NSData*)unwrap: (NSData*)message error:(NSError**)errorPtr;

/** @brief Wrap message and sent it to peer by \b send method from callbacks object. @see SSession_transport_interface. */
-(void)send: (NSData*)message error:(NSError**)errorPtr;

/** @brief Unwrap received from peer by \b receive method from callbacks object message. @see SSession_transport_interface. 
 * @return Plain message in NSData object on success or NULL on failure. 
 */
-(NSData*)recv: (NSInteger)length error:(NSError**)errorPtr;

@end
/** @} */
/** @} */
