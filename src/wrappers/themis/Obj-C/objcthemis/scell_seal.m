/**
 * @file
 *
 * Created by Andrey Mnatsakanov on 03/18/2015
 * (c) CossackLabs
 */

#import "scell_seal.h"

@implementation SCell_seal

- (id)init: (NSData*)key{
  self = [super init];
  key_=[[NSData alloc]initWithData:key];
  return self;
}

- (NSData*)wrap: (NSData*)message{
  return [self wrap:message context:NULL];  
}

- (NSData*)unwrap: (NSData*)message{
  return [self unwrap:message context:NULL];
}

- (NSData*)wrap: (NSData*)message context:(NSData*)context{
  size_t wrapped_message_length=0;
  const void* context_data=(context!=NULL)?[context bytes]:NULL;
  size_t context_length=(context!=NULL)?[context length]:0;
  int res = themis_secure_cell_encrypt_full([key_ bytes], [key_ length], context_data, context_length, [message bytes], [message length], NULL, &wrapped_message_length);
  if(res!=-4)
    {
        NSException *e = [NSException
                          exceptionWithName:@"ThemisException"
                          reason:@"themis_scell_seal_wrap (length detrmination) failed"
                          userInfo:nil];
        @throw e;
    }
  unsigned char* wrapped_message=malloc(wrapped_message_length);
  res = themis_secure_cell_encrypt_full([key_ bytes], [key_ length], context_data, context_length, [message bytes], [message length], wrapped_message, &wrapped_message_length);
  if(res!=0)
    {
        NSException *e = [NSException
                          exceptionWithName:@"ThemisException"
                          reason:@"themis_scell_seal_wrap failed"
                          userInfo:nil];
        @throw e;
    }
  NSData* wr=[[NSData alloc]initWithBytes:wrapped_message length:wrapped_message_length];
  return wr;

}
- (NSData*)unwrap: (NSData*)message context:(NSData*)context{
  size_t unwrapped_message_length=0;
  const void* context_data=(context!=NULL)?[context bytes]:NULL;
  size_t context_length=(context!=Nil)?[context length]:0;
  int res = themis_secure_cell_decrypt_full([key_ bytes], [key_ length], context_data, context_length, [message bytes], [message length], NULL, &unwrapped_message_length);
  if(res!=-4)
    {
        NSException *e = [NSException
                          exceptionWithName:@"ThemisException"
                          reason:@"themis_scell_seal_unwrap (length detrmination) failed"
                          userInfo:nil];
        @throw e;
    }
  unsigned char* unwrapped_message=malloc(unwrapped_message_length);
  res = themis_secure_cell_decrypt_full([key_ bytes], [key_ length], context_data, context_length, [message bytes], [message length], unwrapped_message, &unwrapped_message_length);
  if(res!=0)
    {
        NSException *e = [NSException
                          exceptionWithName:@"ThemisException"
                          reason:@"themis_scell_seal_unwrap failed"
                          userInfo:nil];
        @throw e;
    }
  NSData* wr=[[NSData alloc]initWithBytes:unwrapped_message length:unwrapped_message_length];
  return wr;
}

@end
