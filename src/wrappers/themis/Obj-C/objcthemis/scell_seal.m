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
#import <objcthemis/serror.h>


@implementation TSCellSeal

- (nullable instancetype)initWithKey:(NSData *)key {
    self = [super initWithKey:key];
    return self;
}


- (nullable NSData *)wrapData:(NSData *)message error:(NSError * __autoreleasing *)error {
    return [self wrapData:message context:nil error:error];
}


- (nullable NSData *)unwrapData:(NSData *)message error:(NSError * __autoreleasing *)error {
    return [self unwrapData:message context:nil error:error];
}


- (nullable NSData *)wrapData:(NSData *)message context:(nullable NSData *)context error:(NSError * __autoreleasing *)error {
    size_t wrappedMessageLength = 0;

    const void * contextData = [context bytes];
    size_t contextLength = [context length];

    TSErrorType result = (TSErrorType) themis_secure_cell_encrypt_seal([self.key bytes], [self.key length],
        contextData, contextLength, [message bytes], [message length], NULL, &wrappedMessageLength);

    if (result != TSErrorTypeBufferTooSmall) {
		if (error) {
        	*error = SCERROR(result, @"Secure Cell (Seal) encrypted message length determination failed");
		}
        return nil;
    }

    unsigned char * wrappedMessage = malloc(wrappedMessageLength);
    if (!wrappedMessage) {
		if (error) {
        	*error = SCERROR(result, @"Secure Cell (Seal) encryption failed, not enough memory");
		}
        return nil;
    }
    result = (TSErrorType) themis_secure_cell_encrypt_seal([self.key bytes], [self.key length],
        contextData, contextLength, [message bytes], [message length], wrappedMessage, &wrappedMessageLength);

    if (result != TSErrorTypeSuccess) {
		if (error) {
        	*error = SCERROR(result, @"Secure Cell (Seal) encryption failed");
		}
        free(wrappedMessage);
        return nil;
    }

    return [NSData dataWithBytesNoCopy:wrappedMessage length:wrappedMessageLength];
}

- (nullable NSData *)unwrapData:(NSData *)message context:(nullable NSData *)context error:(NSError * __autoreleasing *)error {
    size_t unwrappedMessageLength = 0;

    const void * contextData = [context bytes];
    size_t contextLength = [context length];

    TSErrorType result = (TSErrorType) themis_secure_cell_decrypt_seal([self.key bytes], [self.key length],
        contextData, contextLength, [message bytes], [message length], NULL, &unwrappedMessageLength);

    if (result != TSErrorTypeBufferTooSmall) {
		if (error) {
        	*error = SCERROR(result, @"Secure Cell (Seal) decrypted message length determination failed");
		}
        return nil;
    }

    unsigned char * unwrappedMessage = malloc(unwrappedMessageLength);
    if (!unwrappedMessage) {
		if (error) {
        	*error = SCERROR(result, @"Secure Cell (Seal) decryption failed, not enough memory");
		}
        return nil;
    }

    result = (TSErrorType) themis_secure_cell_decrypt_seal([self.key bytes], [self.key length],
        contextData, contextLength, [message bytes], [message length], unwrappedMessage, &unwrappedMessageLength);

    if (result != TSErrorTypeSuccess) {
		if (error) {
        	*error = SCERROR(result, @"Secure Cell (Seal) decryption failed");
		}
        free(unwrappedMessage);
        return nil;
    }

    return [NSData dataWithBytesNoCopy:unwrappedMessage length:unwrappedMessageLength];
}

@end
