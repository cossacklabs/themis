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

#import <Foundation/Foundation.h>
#import <themis/themis.h>

typedef enum{
    SMessageModeEncryptDecrypt,
    SMessageModeSignVerify
} SMessageMode;

@interface SMessage : NSObject

{
  NSData* _priv_key;
  NSData* _peer_pub_key;
  SMessageMode _mode;
}

- (id)initWithPrivateKey: (NSData*)private_key peerPublicKey:(NSData*)peer_pub_key;
- (id)initSVWithPrivateKey: (NSData*)private_key peerPublicKey:(NSData*)peer_pub_key;
- (NSData*)wrap: (NSData*)message error:(NSError**)errorPtr;
- (NSData*)unwrap: (NSData*)message error:(NSError**)errorPtr;
@end
