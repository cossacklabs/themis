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

#import <objcthemis/scell_seal.h>
#import <objcthemis/error.h>


@implementation TSCellSeal

- (instancetype)initWithKey:(NSData *)key {
    self = [super initWithKey:key];
    return self;
}


- (NSData *)wrap:(NSData *)message error:(NSError **)error {
    return [self wrap:message context:nil error:error];
}


- (NSData *)unwrap:(NSData *)message error:(NSError **)error {
    return [self unwrap:message context:nil error:error];
}


- (NSData *)wrap:(NSData *)message context:(NSData *)context error:(NSError **)error {
    size_t wrappedMessageLength = 0;

    const void * contextData = (context != nil) ? [context bytes] : NULL;
    size_t contextLength = (context != nil) ? [context length] : 0;

    TSErrorType result = (TSErrorType) themis_secure_cell_encrypt_full([self.key bytes], [self.key length],
        contextData, contextLength, [message bytes], [message length], NULL, &wrappedMessageLength);

    if (result != TSErrorTypeBufferTooSmall) {
        *error = SCERROR(result, @"themis_secure_cell_encrypt_full (length determination) fail");
        return nil;
    }

    unsigned char * wrappedMessage = malloc(wrappedMessageLength);
    result = (TSErrorType) themis_secure_cell_encrypt_full([self.key bytes], [self.key length],
        contextData, contextLength, [message bytes], [message length], wrappedMessage, &wrappedMessageLength);

    if (result != TSErrorTypeSuccess) {
        *error = SCERROR(result, @"themis_secure_cell_encrypt_full fail");
        free(wrappedMessage);
        return nil;
    }

    NSData * wrappedData = [[NSData alloc] initWithBytes:wrappedMessage length:wrappedMessageLength];
    free(wrappedMessage);
    return wrappedData;

}


- (NSData *)unwrap:(NSData *)message context:(NSData *)context error:(NSError **)error {
    size_t unwrappedMessageLength = 0;

    const void * contextData = (context != nil) ? [context bytes] : nil;
    size_t contextLength = (context != Nil) ? [context length] : 0;

    TSErrorType result = (TSErrorType) themis_secure_cell_decrypt_full([self.key bytes], [self.key length],
        contextData, contextLength, [message bytes], [message length], NULL, &unwrappedMessageLength);

    if (result != TSErrorTypeBufferTooSmall) {
        *error = SCERROR(result, @"themis_secure_cell_decrypt_full (length determination) fail");
        return nil;
    }

    unsigned char * unwrappedMessage = malloc(unwrappedMessageLength);
    result = (TSErrorType) themis_secure_cell_decrypt_full([self.key bytes], [self.key length],
        contextData, contextLength, [message bytes], [message length], unwrappedMessage, &unwrappedMessageLength);

    if (result != TSErrorTypeSuccess) {
        *error = SCERROR(result, @"themis_secure_cell_decrypt_full fail");
        free(unwrappedMessage);
        return nil;
    }

    NSData * unwrappedData = [[NSData alloc] initWithBytes:unwrappedMessage length:unwrappedMessageLength];
    free(unwrappedMessage);
    return unwrappedData;
}

@end
