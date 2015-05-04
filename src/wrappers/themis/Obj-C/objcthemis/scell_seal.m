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

#import <objcthemis/scell_seal.h>
#import <objcthemis/error.h>

@implementation SCell_seal

- (id)initWithKey: (NSData*)key{
  self = [super initWithKey: key];
  return self;
}

- (NSData*)wrap: (NSData*)message error:(NSError**)errorPtr{
  return [self wrap:message context:NULL error:errorPtr];  
}

- (NSData*)unwrap: (NSData*)message error:(NSError**)errorPtr{
  return [self unwrap:message context:NULL error:errorPtr];
}

- (NSData*)wrap: (NSData*)message context:(NSData*)context error:(NSError**)errorPtr{
  size_t wrapped_message_length=0;
  const void* context_data=(context!=NULL)?[context bytes]:NULL;
  size_t context_length=(context!=NULL)?[context length]:0;
  TErrorType res = themis_secure_cell_encrypt_full([_key bytes], [_key length], context_data, context_length, [message bytes], [message length], NULL, &wrapped_message_length);
  if(res!=TErrorTypeBufferTooSmall)
    {
      *errorPtr = SCERROR(res, @"themis_secure_cell_encrypt_full (length determination) fail");
      return NULL;
    }
  unsigned char* wrapped_message=malloc(wrapped_message_length);
  res = themis_secure_cell_encrypt_full([_key bytes], [_key length], context_data, context_length, [message bytes], [message length], wrapped_message, &wrapped_message_length);
  if(res!=TErrorTypeSuccess)
    {
      *errorPtr = SCERROR(res, @"themis_secure_cell_encrypt_full fail");
      free(wrapped_message);
      return NULL;
    }
  NSData* wr=[[NSData alloc]initWithBytes:wrapped_message length:wrapped_message_length];
  free(wrapped_message);
  return wr;

}
- (NSData*)unwrap: (NSData*)message context:(NSData*)context error:(NSError**)errorPtr{
  size_t unwrapped_message_length=0;
  const void* context_data=(context!=NULL)?[context bytes]:NULL;
  size_t context_length=(context!=Nil)?[context length]:0;
  TErrorType res = themis_secure_cell_decrypt_full([_key bytes], [_key length], context_data, context_length, [message bytes], [message length], NULL, &unwrapped_message_length);
  if(res!=TErrorTypeBufferTooSmall)
    {
      *errorPtr = SCERROR(res, @"themis_secure_cell_decrypt_full (length determination) fail");
      return NULL;
    }
  unsigned char* unwrapped_message=malloc(unwrapped_message_length);
  res = themis_secure_cell_decrypt_full([_key bytes], [_key length], context_data, context_length, [message bytes], [message length], unwrapped_message, &unwrapped_message_length);
  if(res!=TErrorTypeSuccess)
    {
      *errorPtr = SCERROR(res, @"themis_secure_cell_decrypt_full fail");
      free(unwrapped_message);
      return NULL;
    }
  NSData* wr=[[NSData alloc]initWithBytes:unwrapped_message length:unwrapped_message_length];
  free(unwrapped_message);
  return wr;
}

@end
