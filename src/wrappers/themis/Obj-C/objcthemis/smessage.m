/**
 * @file
 *
 * Created by Andrey Mnatsakanov on 03/18/2015
 * (c) CossackLabs
 */

#import "smessage.h"
#import "error.h"

@implementation SMessage

- (id)initWithPrivateKey: (NSData*)private_key peerPublicKey:(NSData*)peer_pub_key{
  self = [super init];
  if(self){
    _priv_key=[[NSData alloc]initWithData:private_key];
    _peer_pub_key=[[NSData alloc]initWithData:peer_pub_key];
  }
  return self;
}

- (NSData*)wrap: (NSData*)message error:(NSError**)errorPtr{
  size_t wrapped_message_length=0;
  int res = themis_secure_message_wrap([_priv_key bytes], [_priv_key length], [_peer_pub_key bytes], [_peer_pub_key length], [message bytes], [message length], NULL, &wrapped_message_length);
  if(res!=TErrorTypeBufferTooSmall)
    {
	*errorPtr=SCERROR(res, @"themis_secure_message_wrap (length detrmination) failed");
	return NULL;
    }
  unsigned char* wrapped_message=malloc(wrapped_message_length);
  res = themis_secure_message_wrap([_priv_key bytes], [_priv_key length], [_peer_pub_key bytes], [_peer_pub_key length], [message bytes], [message length], wrapped_message, &wrapped_message_length);
  if(res!=TErrorTypeSuccess)
    {
	*errorPtr=SCERROR(res, @"themis_secure_message_wrap failed");
	free(wrapped_message);
	return NULL;
    }
  NSData* wr=[[NSData alloc]initWithBytes:wrapped_message length:wrapped_message_length];
  free(wrapped_message);
  return wr;
}

- (NSData*)unwrap: (NSData*)message error:(NSError**)errorPtr{
  size_t unwrapped_message_length=0;
  int res = themis_secure_message_unwrap([_priv_key bytes], [_priv_key length], [_peer_pub_key bytes], [_peer_pub_key length], [message bytes], [message length], NULL, &unwrapped_message_length);
  if(res!=TErrorTypeBufferTooSmall)
    {
	*errorPtr=SCERROR(res, @"themis_secure_message_unwrap (length detrmination) failed");
	return NULL;
    }
  unsigned char* unwrapped_message=malloc(unwrapped_message_length);
  res = themis_secure_message_unwrap([_priv_key bytes], [_priv_key length], [_peer_pub_key bytes], [_peer_pub_key length], [message bytes], [message length], unwrapped_message, &unwrapped_message_length);
  if(res!=TErrorTypeSuccess)
    {
	*errorPtr=SCERROR(res, @"themis_secure_message_unwrap failed");
	free(unwrapped_message);
	return NULL;
    }
  NSData* wr=[[NSData alloc]initWithBytes:unwrapped_message length:unwrapped_message_length];
//  free(unwrapped_message);
  return wr;
}


@end
