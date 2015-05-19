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

#import <objcthemis/scell_token.h>
#import <objcthemis/error.h>

@implementation SCellTokenEncryptedData

- (id)init{
    self = [super init];
    return self;
}

- (NSMutableData *)getCipherText{
    return cipher_text;
}
- (NSMutableData *)getToken{
    return token;
}

@end


@implementation SCell_token

- (id)initWithKey: (NSData*)key{
  self = [super initWithKey: key];
  return self;
}

- (SCellTokenEncryptedData*)wrap: (NSData*)message error:(NSError**)errorPtr{
  return [self wrap:message context:NULL error:errorPtr];
}

- (NSData*)unwrap: (SCellTokenEncryptedData*)message error:(NSError**)errorPtr{
  return [self unwrap:message context:NULL error:errorPtr];
}

- (SCellTokenEncryptedData*)wrap: (NSData*)message context:(NSData*)context error:(NSError**)errorPtr{
  size_t wrapped_message_length=0;
  size_t token_length=0;
  const void* context_data=(context!=NULL)?[context bytes]:NULL;
  size_t context_length=(context!=NULL)?[context length]:0;
  SCellTokenEncryptedData* encrypted_message = [[SCellTokenEncryptedData alloc] init];
  int res = themis_secure_cell_encrypt_auto_split([_key bytes], [_key length], context_data, context_length, [message bytes], [message length], NULL, &token_length, NULL, &wrapped_message_length);
  if(res!=TErrorTypeBufferTooSmall)
    {
        *errorPtr = SCERROR(res, @"themis_scell_token_wrap (length detrmination) failed");
        struct Encrypted_data ed={NULL, NULL};
        return ed;
    }
   [encrypted_message getCipherText] = [[NSMutableData alloc]initWithLength:wrapped_message_length];
   [encrypted_message getToken] = [[NSMutableData alloc]initWithLength:token_length];
   res = themis_secure_cell_encrypt_auto_split([_key bytes], [_key length], context_data, context_length, [message bytes], [message length], [encrypted_message getToken], &token_length, [encrypted_message getCipherText], &wrapped_message_length);
   if(res!=TErrorTypeSuccess)
     {
       *errorPtr=SCERROR(res, @"themis_scell_token_wrap failed");
       return NULL;
     }
   return encrypted_message;
}
- (NSData*)unwrap: (SCellTokenEncryptedData*)message context:(NSData*)context error:(NSError**)errorPtr{
  size_t unwrapped_message_length=0;
  const void* context_data=(context!=NULL)?[context bytes]:NULL;
  size_t context_length=(context!=NULL)?[context length]:0;
  int res = themis_secure_cell_decrypt_auto_split([_key bytes], [_key length], context_data, context_length, [[message getCipherText] bytes], [[message getCipherText] length], [[message getToken] bytes], [[message getToken] length], NULL, &unwrapped_message_length);
  if(res!=TErrorTypeBufferTooSmall)
    {
      *errorPtr=SCERROR(res,@"themis_scell_token_unwrap (length detrmination) failed");
      return NULL;
    }
  NSMutableData* unwrapped_message=[[NSMutableBytes alloc]initWithLength: unwrapped_message_length];
  res = themis_secure_cell_decrypt_auto_split([_key bytes], [_key length], context_data, context_length, [[message getCipherText] bytes], [[message getCipherText] length], [[message getToken] bytes], [[message getToken] length], [unwrapped_message mutableBytes], &unwrapped_message_length);
  if(res!=TErrorTypeSuccess)
    {
      *errorPtr=SCERROR(res,@"themis_scell_token_unwrap failed");
      return NULL;
    }
  return unwrapped_message;
}

@end
