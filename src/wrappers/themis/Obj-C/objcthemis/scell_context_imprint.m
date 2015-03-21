/**
 * @file
 *
 * Created by Andrey Mnatsakanov on 03/18/2015
 * (c) CossackLabs
 */
#import "scell_context_imprint.h"
#import "error.h"

@implementation SCell_context_imprint

- (id)initWithKey: (NSData*)key{
  self = [super initWithKey: key];
  return self;
}

- (NSData*)wrapData: (NSData*)message context:(NSData*)context error:(NSError**)errorPtr{
  size_t wrapped_message_length=0;
  int res = themis_secure_cell_encrypt_user_split([_key bytes], [_key length], [message bytes], [message length], [context bytes], [context length], NULL, &wrapped_message_length);
  if(res!=TErrorTypeBufferTooSmall)
    {
      *errorPtr = [NSError errorWithDomain:@"com.CossackLabs.Themis.ErrorDomain"
				      code:res 
				  userInfo:[[NSDictionary alloc] 
					     initWithObjectsAndKeys:@"themis_secure_cell_encrypt_user_split (length determination) fail",
					     @"NSLocalizedDescriptionKey",NULL]];
      return NULL;
    }
  unsigned char* wrapped_message=malloc(wrapped_message_length);
  res = themis_secure_cell_encrypt_user_split([_key bytes], [_key length], [message bytes], [message length], [context bytes], [context length], wrapped_message, &wrapped_message_length);
  if(res!=TErrorTypeSuccess)
    {
      free(wrapped_message);
      *errorPtr = [NSError errorWithDomain:@"com.CossackLabs.Themis.ErrorDomain"
				      code:res 
				  userInfo:[[NSDictionary alloc] 
					     initWithObjectsAndKeys:@"themis_secure_cell_encrypt_user_split fail",
					     @"NSLocalizedDescriptionKey",NULL]];
      return NULL;
    }
  NSData* wr=[[NSData alloc]initWithBytes:wrapped_message length:wrapped_message_length];
  free(wrapped_message);
  return wr;

}

- (NSData*)unwrapData: (NSData*)message context:(NSData*)context error:(NSError**)errorPtr{
  size_t unwrapped_message_length=0;
  int res = themis_secure_cell_decrypt_user_split([_key bytes], [_key length], [message bytes], [message length], [context bytes], [context length], NULL, &unwrapped_message_length);
  if(res!=TErrorTypeBufferTooSmall)
    {
      *errorPtr = [NSError errorWithDomain:@"com.CossackLabs.Themis.ErrorDomain"
				      code:res 
				  userInfo:[[NSDictionary alloc] 
					     initWithObjectsAndKeys:@"themis_secure_cell_encrypt_user_split (length determination) fail",
					     @"NSLocalizedDescriptionKey",NULL]];
      return NULL;
    }
  unsigned char* unwrapped_message=malloc(unwrapped_message_length);
  res = themis_secure_cell_decrypt_user_split([_key bytes], [_key length], [message bytes], [message length], [context bytes], [context length], unwrapped_message, &unwrapped_message_length);
  if(res!=TErrorTypeSuccess)
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
