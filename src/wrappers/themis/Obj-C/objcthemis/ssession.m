/**
 * @file
 *
 * Created by Andrey Mnatsakanov on 03/21/2015
 * (c) CossackLabs
 */

#import "ssession.h"
#import "error.h"

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
    *errorPtr=SCERROR(res, @"secure_session_unwrap (length determination) fail");
    return NULL;
  }
  NSMutableData* unwrapped_message=[[NSMutableData alloc]initWithLength:unwrapped_message_length];
  res=secure_session_unwrap(_session, [message bytes], [message length], [unwrapped_message mutableBytes], &unwrapped_message_length);
  if(res!=TErrorTypeSuccess){
    *errorPtr=SCERROR(res, @"secure_session_unwrap fail");
    return NULL;
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

@end
