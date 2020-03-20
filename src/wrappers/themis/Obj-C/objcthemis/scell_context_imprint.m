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

#import <objcthemis/scell_context_imprint.h>
#import <objcthemis/serror.h>
#import <themis/themis.h>

@implementation TSCellContextImprint

- (nullable instancetype)initWithKey:(NSData *)key {
    self = [super initWithKey:key];
    return self;
}

#pragma mark - Encryption

- (nullable NSData *)encrypt:(NSData *)message
                     context:(NSData *)context
                       error:(NSError **)error
{
    size_t wrappedMessageLength = 0;

    TSErrorType encryptionResult = (TSErrorType) themis_secure_cell_encrypt_context_imprint([self.key bytes], [self.key length],
            [message bytes], [message length], [context bytes], [context length], NULL, &wrappedMessageLength);

    if (encryptionResult != TSErrorTypeBufferTooSmall) {
        *error = SCERROR(encryptionResult, @"Secure Cell (Context Imprint) encrypted message length determination failed");
        return nil;
    }

    unsigned char *wrappedMessage = malloc(wrappedMessageLength);
    if (!wrappedMessage) {
        *error = SCERROR(encryptionResult, @"Secure Cell (Context Imprint) encryption failed, not enough memory");
        return nil;
    }

    encryptionResult = (TSErrorType) themis_secure_cell_encrypt_context_imprint([self.key bytes], [self.key length],
            [message bytes], [message length], [context bytes], [context length], wrappedMessage, &wrappedMessageLength);

    if (encryptionResult != TSErrorTypeSuccess) {
        free(wrappedMessage);
        *error = SCERROR(encryptionResult, @"Secure Cell (Context Imprint) encryption failed ");
        return nil;
    }

    return [NSData dataWithBytesNoCopy:wrappedMessage length:wrappedMessageLength];
}

- (nullable NSData *)encrypt:(NSData *)message context:(NSData *)context
{
    return [self encrypt:message context:context error:nil];
}

- (nullable NSData *)wrapData:(NSData *)message context:(NSData *)context error:(NSError **)error
{
    return [self encrypt:message context:context error:error];
}

#pragma mark - Decryption

- (nullable NSData *)decrypt:(NSData *)message
                     context:(NSData *)context
                       error:(NSError **)error
{
    size_t unwrappedMessageLength = 0;

    int decryptionResult = themis_secure_cell_decrypt_context_imprint([self.key bytes], [self.key length],
            [message bytes], [message length], [context bytes], [context length], NULL, &unwrappedMessageLength);

    if (decryptionResult != TSErrorTypeBufferTooSmall) {
        *error = SCERROR(decryptionResult, @"Secure Cell (Context Imprint) decrypted message length determination failed");
        return nil;
    }

    unsigned char *unwrappedMessage = malloc(unwrappedMessageLength);
    if (!unwrappedMessage) {
        *error = SCERROR(decryptionResult, @"Secure Cell (Context Imprint) decryption failed, not enough memory");
        return nil;
    }

    decryptionResult = themis_secure_cell_decrypt_context_imprint([self.key bytes], [self.key length],
            [message bytes], [message length], [context bytes], [context length], unwrappedMessage, &unwrappedMessageLength);

    if (decryptionResult != TSErrorTypeSuccess) {
        free(unwrappedMessage);
        *error = SCERROR(decryptionResult, @"Secure Cell (Context Imprint) decryption failed");
        return nil;
    }

    return [NSData dataWithBytesNoCopy:unwrappedMessage length:unwrappedMessageLength];
}

- (nullable NSData *)decrypt:(NSData *)message context:(NSData *)context
{
    return [self decrypt:message context:context error:nil];
}

- (nullable NSData *)unwrapData:(NSData *)message context:(NSData *)context error:(NSError **)error
{
    return [self decrypt:message context:context error:error];
}

@end
