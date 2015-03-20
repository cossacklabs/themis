/**
 * @file
 *
 * Created by Andrey Mnatsakanov on 03/18/2015
 * (c) CossackLabs
 */
#import "scell_context_imprint.h"

@implementation SCell_context_imprint

- (id)init: (NSData*)key{
  self = [super init];
  key_=[[NSData alloc]initWithData:key];
  return self;
}

- (NSData*)wrap: (NSData*)message context:(NSData*)context{
  size_t wrapped_message_length=0;
  int res = themis_secure_cell_encrypt_user_split([key_ bytes], [key_ length], [message bytes], [message length], [context bytes], [context length], NULL, &wrapped_message_length);
  if(res!=-4)
    {
        NSException *e = [NSException
                          exceptionWithName:@"ThemisException"
                          reason:@"themis_scell_context_imprint_wrap (length detrmination) failed"
                          userInfo:nil];
        @throw e;
    }
  unsigned char* wrapped_message=malloc(wrapped_message_length);
  res = themis_secure_cell_encrypt_user_split([key_ bytes], [key_ length], [message bytes], [message length], [context bytes], [context length], wrapped_message, &wrapped_message_length);
  if(res!=0)
    {
        NSException *e = [NSException
                          exceptionWithName:@"ThemisException"
                          reason:@"themis_scell_context_imprint_wrap failed"
                          userInfo:nil];
        @throw e;
    }
  NSData* wr=[[NSData alloc]initWithBytes:wrapped_message length:wrapped_message_length];
  return wr;

}

- (NSData*)unwrap: (NSData*)message context:(NSData*)context{
  size_t unwrapped_message_length=0;
  int res = themis_secure_cell_decrypt_user_split([key_ bytes], [key_ length], [message bytes], [message length], [context bytes], [context length], NULL, &unwrapped_message_length);
  if(res!=-4)
    {
        NSException *e = [NSException
                          exceptionWithName:@"ThemisException"
                          reason:@"themis_scell_context_imprint_unwrap (length detrmination) failed"
                          userInfo:nil];
        @throw e;
    }
  unsigned char* unwrapped_message=malloc(unwrapped_message_length);
  res = themis_secure_cell_decrypt_user_split([key_ bytes], [key_ length], [message bytes], [message length], [context bytes], [context length], unwrapped_message, &unwrapped_message_length);
  if(res!=0)
    {
        NSException *e = [NSException
                          exceptionWithName:@"ThemisException"
                          reason:@"themis_scell_context_imprint_unwrap failed"
                          userInfo:nil];
        @throw e;
    }
  NSData* wr=[[NSData alloc]initWithBytes:unwrapped_message length:unwrapped_message_length];
  return wr;
}

@end
