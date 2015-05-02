/*
* Copyright (C) 2015 CossackLabs
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

#import <Foundation/Foundation.h>
#import <themis/themis.h>
#import "scell.h"

struct Encrypted_data{
  NSData* cipher_text;
  NSData* token;
};


@interface SCell_token : SCell

- (id)initWithKey: (NSData*)key;
- (struct Encrypted_data)wrap: (NSData*)message error:(NSError**)errorPtr;
- (NSData*)unwrap: (struct Encrypted_data)message error:(NSError**)errorPtr;
- (struct Encrypted_data)wrap: (NSData*)message context:(NSData*)contex error:(NSError**)errorPtr;
- (NSData*)unwrap: (struct Encrypted_data)message context:(NSData*)contex error:(NSError**)errorPtr;

@end
