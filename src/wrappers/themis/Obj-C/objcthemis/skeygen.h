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
 * @file objthemis/skeygen.h
 * @brief key pair generation interface
 */

#import <Foundation/Foundation.h>
#import <themis/themis.h>

/**
 * @addtogroup WRAPPERS
 * @{
 * @addtogroup OBJC
 * @{
 */

/** @brief supported asymmetric cryptography algorithms */
typedef enum{
  RSA, /**< RSA */
  EC /**< Elliptic Curve */
} AsymAlg;

/** @brief Key Pair generation interface */
@interface SKeyGen : NSObject
{
  NSMutableData* _priv_key; /**< private key */
  NSMutableData* _pub_key; /**< public key */
}

/**
 * @brief initialise key pair generator
 * @param [in] alg algorithm. @see AsymAlg
 */ 
- (id)init: (AsymAlg)alg;

/** @brief return private key
 * @return Private Key as NSData object
 */
- (NSData*)getPrivKey;

/** @brief return public key
 * @return Public Key as NSData object
 */
- (NSData*)getPubKey;

@end

/** @} */
/** @} */
