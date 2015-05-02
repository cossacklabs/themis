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

#import "scell_token.h"
#import "error.h"

@implementation SCell_token

- (id)initWithKey: (NSData*)key{
  self = [super initWithKey: key];
  return self;
}

- (struct Encrypted_data)wrap: (NSData*)message error:(NSError**)errorPtr{
  return [self wrap:message context:NULL error:errorPtr];
}

- (NSData*)unwrap: (struct Encrypted_data)message error:(NSError**)errorPtr{
  return [self unwrap:message context:NULL error:errorPtr];
}

- (struct Encrypted_data)wrap: (NSData*)message context:(NSData*)context error:(NSError**)errorPtr{
  size_t wrapped_message_length=0;
  size_t token_length=0;
  const void* context_data=(context!=NULL)?[context bytes]:NULL;
  size_t context_length=(context!=NULL)?[context length]:0;
  int res = themis_secure_cell_encrypt_auto_split([_key bytes], [_key length], context_data, context_length, [message bytes], [message length], NULL, &token_length, NULL, &wrapped_message_length);
  if(res!=TErrorTypeBufferTooSmall)
    {
        *errorPtr = SCERROR(res, @"themis_scell_token_wrap (length detrmination) failed");
        struct Encrypted_data ed={NULL, NULL};
        return ed;
    }
   unsigned char* wrapped_message=malloc(wrapped_message_length);
   unsigned char* token=malloc(token_length);
   res = themis_secure_cell_encrypt_auto_split([_key bytes], [_key length], context_data, context_length, [message bytes], [message length], token, &token_length, wrapped_message, &wrapped_message_length);
   if(res!=TErrorTypeSuccess)
     {
       *errorPtr=SCERROR(res, @"themis_scell_token_wrap failed");
       free(wrapped_message);
       free(token);
       struct Encrypted_data ed={NULL, NULL};
       return ed;
     }
   struct Encrypted_data ed={[[NSData alloc]initWithBytes:wrapped_message length:wrapped_message_length],[[NSData alloc]initWithBytes:token length:token_length]};
   free(wrapped_message);
   free(token);
   return ed;
}
- (NSData*)unwrap: (struct Encrypted_data)message context:(NSData*)context error:(NSError**)errorPtr{
  size_t unwrapped_message_length=0;
  const void* context_data=(context!=NULL)?[context bytes]:NULL;
  size_t context_length=(context!=NULL)?[context length]:0;
  int res = themis_secure_cell_decrypt_auto_split([_key bytes], [_key length], context_data, context_length, [message.cipher_text bytes], [message.cipher_text length], [message.token bytes], [message.token length], NULL, &unwrapped_message_length);
  if(res!=TErrorTypeBufferTooSmall)
    {
      *errorPtr=SCERROR(res,@"themis_scell_token_unwrap (length detrmination) failed");
      return NULL;
    }
  unsigned char* unwrapped_message=malloc(unwrapped_message_length);
  res = themis_secure_cell_decrypt_auto_split([_key bytes], [_key length], context_data, context_length, [message.cipher_text bytes], [message.cipher_text length], [message.token bytes], [message.token length], unwrapped_message, &unwrapped_message_length);
  if(res!=TErrorTypeSuccess)
    {
      *errorPtr=SCERROR(res,@"themis_scell_token_unwrap failed");
      free(unwrapped_message);
      return NULL;
    }
  NSData* rez=[[NSData alloc]initWithBytes:unwrapped_message length:unwrapped_message_length];
  free(unwrapped_message);
  return rez;
}

@end
