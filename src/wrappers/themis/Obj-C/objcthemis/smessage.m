/**
 * @file
 *
 * Created by Andrey Mnatsakanov on 03/18/2015
 * (c) CossackLabs
 */

#import "smessage.h"

@implementation SMessage

- (id)init: (NSData*)private_key peer_pub_key:(NSData*)peer_pub_key{
  self = [super init];
  priv_key_=[[NSData alloc]initWithData:private_key];
  peer_pub_key_=[[NSData alloc]initWithData:peer_pub_key];
  return self;
}

- (NSData*)wrap: (NSData*)message{
  size_t wrapped_message_length=0;
  int res = themis_secure_message_wrap([priv_key_ bytes], [priv_key_ length], [peer_pub_key_ bytes], [peer_pub_key_ length], [message bytes], [message length], NULL, &wrapped_message_length);
  if(res!=-4)
    {
        NSException *e = [NSException
                          exceptionWithName:@"ThemisException"
                          reason:@"themis_secure_wrap (length detrmination) failed"
                          userInfo:nil];
        @throw e;
    }
  unsigned char* wrapped_message=malloc(wrapped_message_length);
  res = themis_secure_message_wrap([priv_key_ bytes], [priv_key_ length], [peer_pub_key_ bytes], [peer_pub_key_ length], [message bytes], [message length], wrapped_message, &wrapped_message_length);
  if(res!=0)
    {
        NSException *e = [NSException
                          exceptionWithName:@"ThemisException"
                          reason:@"themis_secure_wrap failed"
                          userInfo:nil];
        @throw e;
    }
  NSData* wr=[[NSData alloc]initWithBytes:wrapped_message length:wrapped_message_length];
  return wr;
}

- (NSData*)unwrap: (NSData*)message{
  size_t unwrapped_message_length=0;
  int res = themis_secure_message_unwrap([priv_key_ bytes], [priv_key_ length], [peer_pub_key_ bytes], [peer_pub_key_ length], [message bytes], [message length], NULL, &unwrapped_message_length);
  if(res!=-4)
    {
        NSException *e = [NSException
                          exceptionWithName:@"ThemisException"
                          reason:@"themis_secure_unwrap (length detrmination) failed"
                          userInfo:nil];
        @throw e;
    }
  unsigned char* unwrapped_message=malloc(unwrapped_message_length);
  res = themis_secure_message_unwrap([priv_key_ bytes], [priv_key_ length], [peer_pub_key_ bytes], [peer_pub_key_ length], [message bytes], [message length], unwrapped_message, &unwrapped_message_length);
  if(res!=0)
    {
        NSException *e = [NSException
                          exceptionWithName:@"ThemisException"
                          reason:@"themis_secure_unwrap failed"
                          userInfo:nil];
        @throw e;
    }
  NSData* wr=[[NSData alloc]initWithBytes:unwrapped_message length:unwrapped_message_length];
  return wr;
}


@end
