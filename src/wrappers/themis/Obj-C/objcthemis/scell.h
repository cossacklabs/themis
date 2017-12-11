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
 * @file objthemis/scell.h
 * @brief Base Secure Cell class
 */
#import <Foundation/Foundation.h>

/** @addtogroup WRAPPERS
 * @{
 * @addtogroup OBJC
 * @{
 */

/** @brief Base Secure cell interface
*
* Secure Сell is a high-level cryptographic service, aimed to protect arbitrary data being stored in various types of
* storages (like databases, filesystem files, document archives, cloud storage etc).
* It provides a simple way to secure your data using strong encryption and data authentication mechanisms,
* with easy-to-use interfaces for broad range of use-cases.
*
*  Implementing secure storage is often constrained by various practical matters - ability to store keys,
* existence of length-sensitive code bound to database structure, requirements to preserve structure.
* To cover a broader range of usage scenarios and provide highest security level for systems with such constraints,
* we've designed several types of interfaces and implementations of secure data container, Secure Cell.
* They slightly differ in overall security level and ease of use: more complicated and slightly less secure ones can
* cover more constrained environments though. Interfaces below are prioritized by our preference,
* which takes only security and ease of use into account.
*/

NS_ASSUME_NONNULL_BEGIN

@interface TSCell : NSObject

/** @brief store master key
*/
@property (nonatomic, readonly) NSData * key;

/** @brief Initialize Secure Cell object
* @param [in] key master key
*/
- (nullable instancetype)initWithKey:(NSData *)key;

@end

NS_ASSUME_NONNULL_END

/** @} */
/** @} */  
