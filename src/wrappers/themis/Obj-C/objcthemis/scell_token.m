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


@implementation TSCellTokenEncryptedData

@end


@implementation TSCellToken

- (id)initWithKey:(NSData *)key {
    self = [super initWithKey:key];
    return self;
}


- (TSCellTokenEncryptedData *)wrap:(NSData *)message error:(NSError **)error {
    return [self wrap:message context:nil error:error];
}


- (NSData *)unwrap:(TSCellTokenEncryptedData *)message error:(NSError **)error {
    return [self unwrap:message context:nil error:error];
}


- (TSCellTokenEncryptedData *)wrap:(NSData *)message context:(NSData *)context error:(NSError **)error {
    size_t wrappedMessageLength = 0;
    size_t tokenLength = 0;

    const void * contextData = (context != nil) ? [context bytes] : NULL;
    size_t contextLength = (context != nil) ? [context length] : 0;

    TSCellTokenEncryptedData * encryptedMessage = [[TSCellTokenEncryptedData alloc] init];
    TErrorType result = (TErrorType) themis_secure_cell_encrypt_auto_split([self.key bytes], [self.key length],
            contextData, contextLength, [message bytes], [message length], NULL, &tokenLength,
            NULL, &wrappedMessageLength);

    if (result != TErrorTypeBufferTooSmall) {
        *error = SCERROR(result, @"themis_scell_token_wrap (length determination) failed");
        return nil;
    }

    encryptedMessage.cipherText = [[NSMutableData alloc] initWithLength:wrappedMessageLength];
    encryptedMessage.token = [[NSMutableData alloc] initWithLength:tokenLength];

    result = (TErrorType) themis_secure_cell_encrypt_auto_split([self.key bytes], [self.key length], contextData, contextLength,
            [message bytes], [message length], [encryptedMessage.token mutableBytes], &tokenLength,
            [encryptedMessage.cipherText mutableBytes], &wrappedMessageLength);

    if (result != TErrorTypeSuccess) {
        *error = SCERROR(result, @"themis_scell_token_wrap failed");
        return nil;
    }
    return encryptedMessage;
}


- (NSData *)unwrap:(TSCellTokenEncryptedData *)message context:(NSData *)context error:(NSError **)error {
    size_t unwrappedMessageLength = 0;
    const void * contextData = (context != nil) ? [context bytes] : NULL;
    size_t contextLength = (context != nil) ? [context length] : 0;

    TErrorType result = (TErrorType) themis_secure_cell_decrypt_auto_split([self.key bytes], [self.key length], contextData, contextLength,
            [message.cipherText bytes], [message.cipherText length], [message.token bytes], [message.token length],
            NULL, &unwrappedMessageLength);

    if (result != TErrorTypeBufferTooSmall) {
        *error = SCERROR(result, @"themis_scell_token_unwrap (length determination) failed");
        return nil;
    }

    NSMutableData * unwrapped_message = [[NSMutableData alloc] initWithLength:unwrappedMessageLength];
    result = (TErrorType) themis_secure_cell_decrypt_auto_split([self.key bytes], [self.key length], contextData, contextLength,
            [message.cipherText bytes], [message.cipherText length], [message.token bytes], [message.token length],
            [unwrapped_message mutableBytes], &unwrappedMessageLength);

    if (result != TErrorTypeSuccess) {
        *error = SCERROR(result, @"themis_scell_token_unwrap failed");
        return nil;
    }
    return unwrapped_message;
}

@end
