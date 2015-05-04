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

#import <objcthemis/ssession.h>
#import <objcthemis/error.h>

@implementation SSession

-(instancetype)initWithId: (NSData*)id andPrivateKey:(NSData*)private_key andCallbacks:(SSession_transport_interface*)callbacks{
  self=[super init];
  if(self){
    _session=secure_session_create([id bytes], [id length], [private_key bytes], [private_key length], [callbacks callbacks]);
  }
  return self;
}

-(void)connect: (NSError**)errorPtr{
  TErrorType res=secure_session_connect(_session);
  if(res!=TErrorTypeSuccess){
    *errorPtr=SCERROR(res, @"secure_session_connect fail");
  }
}
-(NSData*)connect_request: (NSError**)errorPtr{
  size_t connect_request_length=0;
  TErrorType res=secure_session_generate_connect_request(_session, NULL, &connect_request_length);
  if(res!=TErrorTypeBufferTooSmall){
    *errorPtr=SCERROR(res, @"secure_session_generate_connect_request (length determination) fail");
    return NULL;
  }
  NSMutableData* connect_request=[[NSMutableData alloc]initWithLength:connect_request_length];
  res=secure_session_generate_connect_request(_session, [connect_request mutableBytes], &connect_request_length);
  if(res!=TErrorTypeSuccess){
    *errorPtr=SCERROR(res, @"secure_session_generate_connect_request fail");
    return NULL;
  }
  return connect_request;
}

-(NSData*)wrap: (NSData*)message error:(NSError**)errorPtr{
  size_t wrapped_message_length=0;
  TErrorType res=secure_session_wrap(_session, [message bytes], [message length], NULL, &wrapped_message_length);
  if(res!=TErrorTypeBufferTooSmall){
    *errorPtr=SCERROR(res, @"secure_session_wrap (length determination) fail");
    return NULL;
  }
  NSMutableData* wrapped_message=[[NSMutableData alloc]initWithLength:wrapped_message_length];
  res=secure_session_wrap(_session, [message bytes], [message length], [wrapped_message mutableBytes], &wrapped_message_length);
  if(res!=TErrorTypeSuccess){
    *errorPtr=SCERROR(res, @"secure_session_wrap fail");
    return NULL;
  }
  return wrapped_message; 
}
-(NSData*)unwrap: (NSData*)message error:(NSError**)errorPtr{
  size_t unwrapped_message_length=0;
  TErrorType res=secure_session_unwrap(_session, [message bytes], [message length], NULL, &unwrapped_message_length);
  if(res!=TErrorTypeBufferTooSmall){
    if(res==TErrorTypeSuccess){
      return NULL;
    }
    *errorPtr=SCERROR(res, @"secure_session_unwrap (length determination) fail");
    return NULL;
  }
  NSMutableData* unwrapped_message=[[NSMutableData alloc]initWithLength:unwrapped_message_length];
  res=secure_session_unwrap(_session, [message bytes], [message length], [unwrapped_message mutableBytes], &unwrapped_message_length);
  if(res!=TErrorTypeSuccess){
    if(res==TErrorTypeSendAsIs){
      *errorPtr=SCERROR(res, @"secure_session_unwrap send as is");
      return unwrapped_message;      
    }
    else{
      *errorPtr=SCERROR(res, @"secure_session_unwrap fail");
      return NULL;
    }
  }
  return unwrapped_message; 
}

-(void)send: (NSData*)message error:(NSError**)errorPtr{
  TErrorType res=secure_session_send(_session, [message bytes], [message length]);
  if(res!=TErrorTypeSuccess){
    *errorPtr=SCERROR(res, @"secure_session_send fail");    
  }
}

-(NSData*)recv: (NSInteger)length error:(NSError**)errorPtr{
  NSMutableData* received_data=[[NSMutableData alloc]initWithLength:length];
  TErrorType res=secure_session_receive(_session, [received_data mutableBytes], [received_data length]);
  if(res!=TErrorTypeSuccess){
    *errorPtr=SCERROR(res, @"secure_session_receive fail");
    return NULL;    
  }
  return received_data;
}

-(bool)is_established{
  return secure_session_is_established(_session);
}

@end
