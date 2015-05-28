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
#import <themis/themis.h>


/** @brief Secure session trancport callbacs interface */
@interface SSession_transport_interface: NSObject
{
  secure_session_user_callbacks_t _callbacks;
}

/** @brief Initialisation */
-(instancetype)init;

/** @brief Send \b data to peer */ 
-(void)send: (NSData*)data error:(NSError**)errorPtr;

/** @brief Receive data fron peer and return it in NSData object */ 
-(NSData*) receive: (NSError**)errorPtr;

/** @brief Return public key assosiated with \b Id as NSData object or NULL on failure */ 
-(NSData*) get_public_key: (NSData*)Id error:(NSError**)errorPtr;

/** @brief Get callbacks */
-(secure_session_user_callbacks_t*)callbacks;
@end
