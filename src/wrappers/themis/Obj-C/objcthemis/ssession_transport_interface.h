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
* @file objthemis/ssession_transport_interface.h
* @brief secure session trancport callbacs interface
*/
#import <Foundation/Foundation.h>

typedef struct secure_session_user_callbacks_type secure_session_user_callbacks_t;

NS_ASSUME_NONNULL_BEGIN

/** @brief Secure session transport callbacks interface */
@interface TSSessionTransportInterface : NSObject

/** @brief Send binary data to peer
* @param [in] data binary data
* @param [in] error pointer to Error on failure
*/
- (void)sendData:(nullable NSData *)data error:(NSError *__autoreleasing *)error;


/** @brief Receive data from peer and return it in NSData object
* @param [in] error pointer to Error on failure
* @return data object or nil on failure
*/
- (nullable NSData *)receiveDataWithError:(NSError *__autoreleasing *)error;


/** @brief Return public key associated with binaryId as NSData object or nil on failure
* @param [in] binaryId usually server id
* @param [in] error pointer to Error on failure
* @return binary public key associated with binaryId or nil on failure
*/
- (nullable NSData *)publicKeyFor:(nullable NSData *)binaryId error:(NSError *__autoreleasing *)error;


/** @brief Get callbacks */
- (secure_session_user_callbacks_t *)callbacks;

NS_ASSUME_NONNULL_END

@end
