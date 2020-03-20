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
#import <objcthemis/serror.h>
#import <themis/themis.h>

@implementation TSCellTokenEncryptedData

+ (instancetype)withResult:(TSCellTokenEncryptedResult *)result
{
    TSCellTokenEncryptedData *data = [TSCellTokenEncryptedData new];
    data.cipherText = [NSMutableData dataWithData:result.encrypted];
    data.token = [NSMutableData dataWithData:result.token];
    return data;
}

@end

@interface TSCellTokenEncryptedResult ()

// Make these properties read-write in this file
@property (nonatomic) NSData *encrypted;
@property (nonatomic) NSData *token;

@end

@implementation TSCellTokenEncryptedResult

+ (instancetype)withEncrypted:(NSData *)message andToken:(NSData *)token
{
    TSCellTokenEncryptedResult *result = [TSCellTokenEncryptedResult new];
    result.encrypted = message;
    result.token = token;
    return result;
}

- (NSData*)cipherText
{
    return self.encrypted;
}

@end

@implementation TSCellToken

- (nullable instancetype)initWithKey:(NSData *)key {
    self = [super initWithKey:key];
    return self;
}

#pragma mark - Encryption

- (nullable TSCellTokenEncryptedResult *)encrypt:(NSData *)message
                                         context:(nullable NSData *)context
                                           error:(NSError **)error
{
    size_t wrappedMessageLength = 0;
    size_t tokenLength = 0;

    const void *contextData = [context bytes];
    size_t contextLength = [context length];

    TSErrorType result = (TSErrorType) themis_secure_cell_encrypt_token_protect([self.key bytes], [self.key length],
            contextData, contextLength, [message bytes], [message length], NULL, &tokenLength,
            NULL, &wrappedMessageLength);

    if (result != TSErrorTypeBufferTooSmall) {
        if (error) {
            *error = SCERROR(result, @"Secure Cell (Token Protect) encrypted message length determination failed");
        }
        return nil;
    }

    NSMutableData *cipherText = [[NSMutableData alloc] initWithLength:wrappedMessageLength];
    NSMutableData *token = [[NSMutableData alloc] initWithLength:tokenLength];

    result = (TSErrorType) themis_secure_cell_encrypt_token_protect([self.key bytes], [self.key length], contextData, contextLength,
            [message bytes], [message length], [token mutableBytes], &tokenLength,
            [cipherText mutableBytes], &wrappedMessageLength);

    if (result != TSErrorTypeSuccess) {
        if (error) {
            *error = SCERROR(result, @"Secure Cell (Token Protect) encryption failed");
        }
        return nil;
    }

    [cipherText setLength:wrappedMessageLength];
    [token setLength:tokenLength];
    return [TSCellTokenEncryptedResult withEncrypted:cipherText andToken:token];
}

- (nullable TSCellTokenEncryptedResult *)encrypt:(NSData *)message context:(nullable NSData *)context
{
    return [self encrypt:message context:context error:nil];
}

- (nullable TSCellTokenEncryptedResult *)encrypt:(NSData *)message error:(NSError **)error
{
    return [self encrypt:message context:nil error:error];
}

- (nullable TSCellTokenEncryptedResult *)encrypt:(NSData *)message
{
    return [self encrypt:message context:nil error:nil];
}

- (nullable TSCellTokenEncryptedData *)wrapData:(NSData *)message context:(nullable NSData *)context error:(NSError **)error
{
    return [TSCellTokenEncryptedData withResult:[self encrypt:message context:context error:error]];
}

- (nullable TSCellTokenEncryptedData *)wrapData:(NSData *)message error:(NSError **)error
{
    return [TSCellTokenEncryptedData withResult:[self encrypt:message context:nil error:error]];
}

#pragma mark - Decryption

- (nullable NSData *)decrypt:(NSData *)message
                       token:(NSData *)token
                     context:(nullable NSData *)context
                       error:(NSError **)error
{
    size_t unwrappedMessageLength = 0;
    const void *contextData = [context bytes];
    size_t contextLength = [context length];

    TSErrorType result = (TSErrorType) themis_secure_cell_decrypt_token_protect([self.key bytes], [self.key length], contextData, contextLength,
            [message bytes], [message length], [token bytes], [token length],
            NULL, &unwrappedMessageLength);

    if (result != TSErrorTypeBufferTooSmall) {
        if (error) {
            *error = SCERROR(result, @"Secure Cell (Token Protect) decrypted message length determination failed");
        }
        return nil;
    }

    NSMutableData *unwrapped_message = [[NSMutableData alloc] initWithLength:unwrappedMessageLength];
    result = (TSErrorType) themis_secure_cell_decrypt_token_protect([self.key bytes], [self.key length], contextData, contextLength,
            [message bytes], [message length], [token bytes], [token length],
            [unwrapped_message mutableBytes], &unwrappedMessageLength);

    if (result != TSErrorTypeSuccess) {
        if (error) {
            *error = SCERROR(result, @"Secure Cell (Token Protect) decryption failed");
        }
        return nil;
    }
    return [unwrapped_message copy];
}

- (nullable NSData *)decrypt:(NSData *)message token:(NSData *)token context:(nullable NSData *)context
{
    return [self decrypt:message token:token context:context error:nil];
}

- (nullable NSData *)decrypt:(NSData *)message token:(NSData *)token error:(NSError **)error
{
    return [self decrypt:message token:token context:nil error:error];
}

- (nullable NSData *)decrypt:(NSData *)message token:(NSData *)token
{
    return [self decrypt:message token:token context:nil error:nil];
}

- (nullable NSData *)unwrapData:(TSCellTokenEncryptedData *)message context:(nullable NSData *)context error:(NSError **)error
{
    return [self decrypt:message.cipherText token:message.token context:context error:error];
}

- (nullable NSData *)unwrapData:(TSCellTokenEncryptedData *)message error:(NSError **)error
{
    return [self decrypt:message.cipherText token:message.token context:nil error:error];
}

@end
