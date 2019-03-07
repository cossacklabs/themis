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

#import "ssession_transport_interface.h"

/**
* @addtogroup WRAPPERS
* @{
* @addtogroup OBJC
* @{
*/

NS_ASSUME_NONNULL_BEGIN

/** @brief Secure session interface
*
* Secure session is a lightweight mechanism to secure any network communications (both private and public networks
* including Internet). It is protocol agnostic and operates on the 5th layer of the network OSI model.
* Some features:
*    - secure end-to-end communication
*    - perfect forward secrecy
*    - strong mutual peer authentication
*    - replay protection
*    - low negotiation round-trip
*    - uses strong cryptography (including ECC)
*    - lightweight, easy to use
*    - easy to integrate into existing applications
*
*  Communications over secure session contains 2 stages: negotiation (key agreement) stage and actual data exchange.*
*/
@interface TSSession : NSObject

/**
* @brief Initialise Secure session object
* @param [in] userId User id
* @param [in] privateKey User private key
* @param [in] callbacks Reference to TSSessionTransportInterface object
*/
- (nullable instancetype)initWithUserId:(NSData *)userId
                             privateKey:(NSData *)privateKey
                              callbacks:(TSSessionTransportInterface *)callbacks;


/**
* @brief Connection initialization message
* @return Connection initialization message on success or nil on failure
*/
// TODO: rename method to reflect it's goal
- (nullable NSData *)connectRequest:(NSError *__autoreleasing *)error;


/**
* @brief Create connection initialization message and send it to peer by \b send method from callbacks object.
* @see TSSessionTransportInterface.
* @return YES if success, NO if fail.
*/
// TODO: rename method to reflect it's goal
- (BOOL)connect:(NSError *__autoreleasing *)error;


/**
* @brief Wrap message
* @param [in] message message to wrap
* @param [in] error pointer to Error on failure
* @return Wrapped message in NSData object on success or nil on failure.
*/
- (nullable NSData *)wrapData:(nullable NSData *)message error:(NSError *__autoreleasing *)error;


/** @brief Unwrap message
* @param [in] message message to unwrap
* @param [in] error pointer to Error on failure
* @return Unwrapped message in NSData object on success or nil on failure.
*/
- (nullable NSData *)unwrapData:(nullable NSData *)message error:(NSError *__autoreleasing *)error;


/** @brief Wrap message and sent it to peer by \b send method from callbacks object.
* @param [in] message message to wrap
* @param [in] error pointer to Error on failure
* @see TSSessionTransportInterface.
* @return YES if success, NO if fail.
*/
// TODO: rename method to reflect it's goal
- (BOOL)wrapAndSend:(nullable NSData *)message error:(NSError *__autoreleasing *)error;


/** @brief Unwrap received from peer by \b receive method from callbacks object message.
* @see TSSessionTransportInterface.
* @param [in] length of data to receive
* @param [in] error pointer to Error on failure
* @return Plain message in NSData object on success or nil on failure.
*/
// TODO: rename method to reflect it's goal
- (nullable NSData *)unwrapAndReceive:(NSUInteger)length error:(NSError *__autoreleasing *)error;


/** @brief indicate session established state.
* @return YES is session establishing phase complete or NO overwise.
*/
- (BOOL)isSessionEstablished;

@end

NS_ASSUME_NONNULL_END

/** @} */
/** @} */
