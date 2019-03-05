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
* @file objthemis/scomparator.h
* @brief secure comparator interface
*/

#import <Foundation/Foundation.h>

/**
* @addtogroup WRAPPERS
* @{
* @addtogroup OBJC
* @{
*/

NS_ASSUME_NONNULL_BEGIN

/** @brief Secure comparator states */
typedef NS_ENUM(NSInteger, TSComparatorStateType) {
    TSComparatorNotReady = 0,
    TSComparatorNotMatch = 22,
    TSComparatorMatch = 21
};

/** @brief Secure comparator interface
*
* Secure comparator is a lightweight mechanism
*
*/
@interface TSComparator : NSObject

/**
* @brief Initialise Secure comparator object
* @param [in] message we want to compare
*/
- (nullable instancetype)initWithMessageToCompare:(NSData *)message;

/**
* @brief Comparation initialization message
* @param [in] error pointer to Error on failure
* @return Comparation initialization message on success or nil on failure
*/
- (nullable NSData *)beginCompare:(NSError *__autoreleasing *)error;


/** @brief Proceed comparation message 
* @param [in] message message to proceed
* @param [in] error pointer to Error on failure
* @return Next comparation message in NSData object on success or nil on failure.
*/
- (nullable NSData *)proceedCompare:(nullable NSData *)message error:(NSError *__autoreleasing *)error;

/** @brief indicate comparation state.
* @return comparation state.
*/
- (TSComparatorStateType)status;

@end

NS_ASSUME_NONNULL_END

/** @} */
/** @} */
