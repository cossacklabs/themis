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


- (TSCellTokenEncryptedData *)wrapData:(NSData *)message error:(NSError **)error {
    return [self wrapData:message context:nil error:error];
}


- (NSData *)unwrapData:(TSCellTokenEncryptedData *)message error:(NSError **)error {
    return [self unwrapData:message context:nil error:error];
}


- (TSCellTokenEncryptedData *)wrapData:(NSData *)message context:(NSData *)context error:(NSError **)error {
    size_t wrappedMessageLength = 0;
    size_t tokenLength = 0;

    const void * contextData = [context bytes];
    size_t contextLength = [context length];

    TSCellTokenEncryptedData * encryptedMessage = [[TSCellTokenEncryptedData alloc] init];
    TSErrorType result = (TSErrorType) themis_secure_cell_encrypt_token_protect([self.key bytes], [self.key length],
            contextData, contextLength, [message bytes], [message length], NULL, &tokenLength,
            NULL, &wrappedMessageLength);

    if (result != TSErrorTypeBufferTooSmall) {
        *error = SCERROR(result, @"Secure Cell (Token Protect) encrypted message length determination failed");
        return nil;
    }

    encryptedMessage.cipherText = [[NSMutableData alloc] initWithLength:wrappedMessageLength];
    encryptedMessage.token = [[NSMutableData alloc] initWithLength:tokenLength];

    result = (TSErrorType) themis_secure_cell_encrypt_token_protect([self.key bytes], [self.key length], contextData, contextLength,
            [message bytes], [message length], [encryptedMessage.token mutableBytes], &tokenLength,
            [encryptedMessage.cipherText mutableBytes], &wrappedMessageLength);

    if (result != TSErrorTypeSuccess) {
        *error = SCERROR(result, @"Secure Cell (Token Protect) encryption failed");
        return nil;
    }
    return encryptedMessage;
}


- (NSData *)unwrapData:(TSCellTokenEncryptedData *)message context:(NSData *)context error:(NSError **)error {
    size_t unwrappedMessageLength = 0;
    const void * contextData = [context bytes];
    size_t contextLength = [context length];

    TSErrorType result = (TSErrorType) themis_secure_cell_decrypt_token_protect([self.key bytes], [self.key length], contextData, contextLength,
            [message.cipherText bytes], [message.cipherText length], [message.token bytes], [message.token length],
            NULL, &unwrappedMessageLength);

    if (result != TSErrorTypeBufferTooSmall) {
        *error = SCERROR(result, @"Secure Cell (Token Protect) decrypted message length determination failed");
        return nil;
    }

    NSMutableData * unwrapped_message = [[NSMutableData alloc] initWithLength:unwrappedMessageLength];
    result = (TSErrorType) themis_secure_cell_decrypt_token_protect([self.key bytes], [self.key length], contextData, contextLength,
            [message.cipherText bytes], [message.cipherText length], [message.token bytes], [message.token length],
            [unwrapped_message mutableBytes], &unwrappedMessageLength);

    if (result != TSErrorTypeSuccess) {
        *error = SCERROR(result, @"Secure Cell (Token Protect) decryption failed");
        return nil;
    }
    return [unwrapped_message copy];
}

@end
