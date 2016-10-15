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

#import <themis/themis.h>

/**
* @addtogroup WRAPPERS
* @{
* @addtogroup OBJC
* @{
*/

/** @brief Secure comparator states */
typedef NS_ENUM(NSInteger, TSComparatorStateType){
  TSComparatorNotReady = 0,
  TSCompatarorNotMatch = -1,
  TSComparatorMatch = -252645136
};
/** @brief Secure comparator interface
*
* Secure comparator is a lightweight mechanism
*
*/
@interface TSComparator : NSObject

/**
* @brief Initialise Secure comparator object
* @param [in] message message to compare
*/
- (instancetype)initWithMessageToCompare:(NSData *)message;

/**
* @brief Deinitialise Secure comparator object
*/
-(void)dealloc;
/**
* @brief Comparation initialization message
* @param [in] error pointer to Error on failure
* @return Comparation initialization message on success or nil on failure
*/
// TODO: rename method to reflect it's goal
- (NSData *)beginCompare:(NSError **)error;


/** @brief Proceed comparation message 
* @param [in] message message to proceed
* @param [in] error pointer to Error on failure
* @return Next comparation message in NSData object on success or nil on failure.
*/
- (NSData *)proceedCompare:(NSData *)message error:(NSError **)error;

/** @brief indicate comparation state.
* @return comparation state.
*/
- (TSComparatorStateType)status;

@end
/** @} */
/** @} */
