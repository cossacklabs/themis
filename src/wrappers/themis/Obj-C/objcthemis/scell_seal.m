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
#import <themis/themis.h>

@interface TSCellSealWithPassphrase : TSCellSeal
@end

@implementation TSCellSeal

- (nullable instancetype)initWithKey:(NSData *)key {
    self = [super initWithKey:key];
    return self;
}

- (nullable instancetype)initWithPassphrase:(NSString *)passphrase
{
    NSData *passphraseUTF8 = [passphrase dataUsingEncoding:NSUTF8StringEncoding];
    return [self initWithPassphraseData:passphraseUTF8];
}

- (nullable instancetype)initWithPassphraseData:(NSData *)passphrase
{
    // Avoid a warning about "Convenience initializer missing a 'self' call
    // to another initializer" by assigning the new instance to self first.
    self = [TSCellSealWithPassphrase alloc];
    self = [self initWithPassphraseData:passphrase];
    return self;
}

#pragma mark - Encryption

- (nullable NSData *)encrypt:(NSData *)message
                     context:(nullable NSData *)context
                       error:(NSError **)error
{
    size_t wrappedMessageLength = 0;

    const void *contextData = [context bytes];
    size_t contextLength = [context length];

    TSErrorType result = (TSErrorType) themis_secure_cell_encrypt_seal([self.key bytes], [self.key length],
            contextData, contextLength, [message bytes], [message length], NULL, &wrappedMessageLength);

    if (result != TSErrorTypeBufferTooSmall) {
        if (error) {
            *error = SCERROR(result, @"Secure Cell (Seal) encrypted message length determination failed");
        }
        return nil;
    }

    unsigned char *wrappedMessage = malloc(wrappedMessageLength);
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

- (nullable NSData *)encrypt:(NSData *)message context:(nullable NSData *)context
{
    return [self encrypt:message context:context error:nil];
}

- (nullable NSData *)encrypt:(NSData *)message error:(NSError **)error
{
    return [self encrypt:message context:nil error:error];
}

- (nullable NSData *)encrypt:(NSData *)message
{
    return [self encrypt:message context:nil error:nil];
}

- (nullable NSData *)wrapData:(NSData *)message context:(nullable NSData *)context error:(NSError **) error
{
    return [self encrypt:message context:context error:error];
}

- (nullable NSData *)wrapData:(NSData *)message error:(NSError **)error
{
    return [self encrypt:message context:nil error:error];
}

#pragma mark - Decryption

- (nullable NSData *)decrypt:(NSData *)message
                     context:(nullable NSData *)context
                       error:(NSError **)error
{
    size_t unwrappedMessageLength = 0;

    const void *contextData = [context bytes];
    size_t contextLength = [context length];

    TSErrorType result = (TSErrorType) themis_secure_cell_decrypt_seal([self.key bytes], [self.key length],
            contextData, contextLength, [message bytes], [message length], NULL, &unwrappedMessageLength);

    if (result != TSErrorTypeBufferTooSmall) {
        if (error) {
            *error = SCERROR(result, @"Secure Cell (Seal) decrypted message length determination failed");
        }
        return nil;
    }

    unsigned char *unwrappedMessage = malloc(unwrappedMessageLength);
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

- (nullable NSData *)decrypt:(NSData *)message context:(nullable NSData *)context
{
    return [self decrypt:message context:context error:nil];
}

- (nullable NSData *)decrypt:(NSData *)message error:(NSError **)error
{
    return [self decrypt:message context:nil error:error];
}

- (nullable NSData *)decrypt:(NSData *)message
{
    return [self decrypt:message context:nil error:nil];
}

- (nullable NSData *)unwrapData:(NSData *)message context:(nullable NSData *)context error:(NSError **)error
{
    return [self decrypt:message context:context error:error];
}

- (nullable NSData *)unwrapData:(NSData *)message error:(NSError **)error
{
    return [self decrypt:message context:nil error:error];
}

@end

@implementation TSCellSealWithPassphrase

- (nullable instancetype)initWithPassphraseData:(NSData *)passphrase
{
    // Call grandparent TSCell initializer. We store the passphrase as a "key".
    self = [super initWithKey:passphrase];
    return self;
}

- (nullable NSData *)encrypt:(NSData *)data
                     context:(nullable NSData *)context
                       error:(NSError **)error
{
    themis_status_t res = THEMIS_FAIL;

    size_t encryptedLength = 0;
    res = themis_secure_cell_encrypt_seal_with_passphrase(self.key.bytes,
                                                          self.key.length,
                                                          context.bytes,
                                                          context.length,
                                                          data.bytes,
                                                          data.length,
                                                          NULL,
                                                          &encryptedLength);
    if (res != THEMIS_BUFFER_TOO_SMALL) {
        if (error) {
            *error = SCERROR(res, @"Secure Cell encryption failed");
        }
        return nil;
    }

    NSMutableData *encryptedData = [NSMutableData dataWithLength:encryptedLength];

    res = themis_secure_cell_encrypt_seal_with_passphrase(self.key.bytes,
                                                          self.key.length,
                                                          context.bytes,
                                                          context.length,
                                                          data.bytes,
                                                          data.length,
                                                          encryptedData.mutableBytes,
                                                          &encryptedLength);
    if (res != THEMIS_SUCCESS) {
        if (error) {
            *error = SCERROR(res, @"Secure Cell encryption failed");
        }
        return nil;
    }

    [encryptedData setLength:encryptedLength];
    return encryptedData;
}

- (nullable NSData *)decrypt:(NSData *)data
                     context:(nullable NSData *)context
                       error:(NSError **)error
{
    themis_status_t res = THEMIS_FAIL;

    size_t decryptedLength = 0;
    res = themis_secure_cell_decrypt_seal_with_passphrase(self.key.bytes,
                                                          self.key.length,
                                                          context.bytes,
                                                          context.length,
                                                          data.bytes,
                                                          data.length,
                                                          NULL,
                                                          &decryptedLength);
    if (res != THEMIS_BUFFER_TOO_SMALL) {
        if (error) {
            *error = SCERROR(res, @"Secure Cell decryption failed");
        }
        return nil;
    }

    NSMutableData *decryptedData = [NSMutableData dataWithLength:decryptedLength];

    res = themis_secure_cell_decrypt_seal_with_passphrase(self.key.bytes,
                                                          self.key.length,
                                                          context.bytes,
                                                          context.length,
                                                          data.bytes,
                                                          data.length,
                                                          decryptedData.mutableBytes,
                                                          &decryptedLength);
    if (res != THEMIS_SUCCESS) {
        if (error) {
            *error = SCERROR(res, @"Secure Cell decryption failed");
        }
        return nil;
    }

    [decryptedData setLength:decryptedLength];
    return decryptedData;
}

@end
