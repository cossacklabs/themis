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

#import <objcthemis/ssession_transport_interface.h>
#import <themis/themis.h>

@interface SSession : NSObject
{
  secure_session_t* _session;
}

-(instancetype)initWithId: (NSData*)id andPrivateKey:(NSData*)private_key andCallbacks:(SSession_transport_interface*)callbacks;

-(void)connect: (NSError**)errorPtr;
-(NSData*)connect_request: (NSError**)errorPtr;

-(NSData*)wrap: (NSData*)message error:(NSError**)errorPtr;
-(NSData*)unwrap: (NSData*)message error:(NSError**)errorPtr;

-(void)send: (NSData*)message error:(NSError**)errorPtr;
-(NSData*)recv: (NSInteger)length error:(NSError**)errorPtr;

@end
