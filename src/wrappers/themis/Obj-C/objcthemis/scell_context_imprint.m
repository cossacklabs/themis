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
#import <objcthemis/error.h>
#import "error.h"


@implementation TSCellContextImprint

- (id)initWithKey:(NSData *)key {
    self = [super initWithKey:key];
    return self;
}


- (NSData *)wrapData:(NSData *)message context:(NSData *)context error:(NSError **)error {
    size_t wrappedMessageLength = 0;

    int encryptionResult = themis_secure_cell_encrypt_user_split([self.key bytes], [self.key length],
        [message bytes], [message length], [context bytes], [context length], NULL, &wrappedMessageLength);

    if (encryptionResult != TErrorTypeBufferTooSmall) {
        *error = SCERROR(encryptionResult, @"themis_secure_cell_encrypt_user_split (length determination) fail");
        return nil;
    }

    unsigned char * wrappedMessage = malloc(wrappedMessageLength);

    encryptionResult = themis_secure_cell_encrypt_user_split([self.key bytes], [self.key length],
        [message bytes], [message length], [context bytes], [context length], wrappedMessage, &wrappedMessageLength);

    if (encryptionResult != TErrorTypeSuccess) {
        free(wrappedMessage);
        *error = SCERROR(encryptionResult, @"themis_secure_cell_encrypt_user_split fail");
        return nil;
    }

    NSData * wrappedData = [[NSData alloc] initWithBytes:wrappedMessage length:wrappedMessageLength];
    free(wrappedMessage);
    return wrappedData;
}


- (NSData *)unwrapData:(NSData *)message context:(NSData *)context error:(NSError **)error {
    size_t unwrappedMessageLength = 0;

    int decryptionResult = themis_secure_cell_decrypt_user_split([self.key bytes], [self.key length],
        [message bytes], [message length], [context bytes], [context length], NULL, &unwrappedMessageLength);

    if (decryptionResult != TErrorTypeBufferTooSmall) {
        *error = SCERROR(decryptionResult, @"themis_secure_cell_encrypt_user_split (length determination) fail");
        return nil;
    }

    unsigned char * unwrappedMessage = malloc(unwrappedMessageLength);

    decryptionResult = themis_secure_cell_decrypt_user_split([self.key bytes], [self.key length],
        [message bytes], [message length], [context bytes], [context length], unwrappedMessage, &unwrappedMessageLength);

    if (decryptionResult != TErrorTypeSuccess) {
        *error = SCERROR(decryptionResult, @"themis_scell_context_imprint_unwrap failed");
        return nil;
    }

    NSData * unwrappedData = [[NSData alloc] initWithBytes:unwrappedMessage length:unwrappedMessageLength];
    return unwrappedData;
}

@end
