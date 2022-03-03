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

/**
* @addtogroup WRAPPERS
* @{
* @addtogroup OBJC
* @{
*/

NS_ASSUME_NONNULL_BEGIN

/** @brief supported asymmetric cryptography algorithms */
typedef NS_ENUM(NSInteger, TSKeyGenAsymmetricAlgorithm) {
    TSKeyGenAsymmetricAlgorithmRSA, /**< RSA */
    TSKeyGenAsymmetricAlgorithmEC /**< Elliptic Curve */
};


/** @brief Key Pair generation interface */
@interface TSKeyGen : NSObject


/** @brief private key */
@property(nonatomic, readonly) NSMutableData *privateKey;

/** @brief public key */
@property(nonatomic, readonly) NSMutableData *publicKey;

/**
* @brief initialise key pair generator, generates privateKey and publicKey
* @param [in] algorithm RSA or EC. @see TSKeyGenAsymmetricAlgorithm
*/
- (nullable instancetype)initWithAlgorithm:(TSKeyGenAsymmetricAlgorithm)algorithm;


@end

/**
 * Generates new symmetric master key.
 *
 * Securely generate a new master key suitable for Secure Cell.
 *
 * @returns a newly allocated key of default size.
 */
NSData* __nullable TSGenerateSymmetricKey(void);

NS_ASSUME_NONNULL_END

/** @} */
/** @} */
