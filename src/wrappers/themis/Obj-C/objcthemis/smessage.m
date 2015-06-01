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

#import <objcthemis/smessage.h>
#import <objcthemis/error.h>


@interface TSMessage ()

/** @brief private key */
@property (nonatomic, readwrite) NSData * privateKey;

/** @brief public key */
@property (nonatomic, readwrite) NSData * publicKey;

/** @brief mode */
@property (nonatomic, readwrite) TSMessageMode mode;

@end


@implementation TSMessage

- (instancetype)initWithPrivateKey:(NSData *)privateKey peerPublicKey:(NSData *)peerPublicKey {
    self = [super init];
    if (self) {
        self.privateKey = [privateKey copy];
        self.publicKey = [peerPublicKey copy];
        self.mode = TSMessageModeEncryptDecrypt;
    }
    return self;
}


- (instancetype)initSVWithPrivateKey:(NSData *)privateKey peerPublicKey:(NSData *)peerPublicKey {
    self = [super init];
    if (self) {
        self.privateKey = [privateKey copy];
        self.publicKey = [peerPublicKey copy];
        self.mode = TSMessageModeSignVerify;
    }
    return self;
}


- (NSData *)wrap:(NSData *)message error:(NSError **)error {
    size_t wrappedMessageLength = 0;
    TSErrorType result = TSErrorTypeFail;

    switch (self.mode) {
        case TSMessageModeEncryptDecrypt:
            result = (TSErrorType) themis_secure_message_wrap([self.privateKey bytes], [self.privateKey length],
                [self.publicKey bytes], [self.publicKey length], [message bytes], [message length],
                NULL, &wrappedMessageLength);
            break;

        case TSMessageModeSignVerify:
            result = (TSErrorType) themis_secure_message_wrap([self.privateKey bytes], [self.privateKey length], NULL, 0,
                [message bytes], [message length], NULL, &wrappedMessageLength);
            break;
        default:
            *error = SCERROR(TSErrorTypeFail, @"themis_secure_message_wrap (undefined secure session mode) failed");
            return nil;
    }

    if (result != TSErrorTypeBufferTooSmall) {
        *error = SCERROR(result, @"themis_secure_message_wrap (length determination) failed");
        return nil;
    }

    unsigned char * wrappedMessage = malloc(wrappedMessageLength);
    switch (self.mode) {
        case TSMessageModeEncryptDecrypt:
            result = (TSErrorType) themis_secure_message_wrap([self.privateKey bytes], [self.privateKey length],
                [self.publicKey bytes], [self.publicKey length], [message bytes], [message length],
                wrappedMessage, &wrappedMessageLength);
            break;

        case TSMessageModeSignVerify:
            result = (TSErrorType) themis_secure_message_wrap([self.privateKey bytes], [self.privateKey length], NULL, 0,
                [message bytes], [message length], wrappedMessage, &wrappedMessageLength);
            break;
        default:
            *error = SCERROR(TSErrorTypeFail, @"themis_secure_message_wrap (undefined secure session mode) failed");
            return NULL;
    }

    if (result != TSErrorTypeSuccess) {
        *error = SCERROR(result, @"themis_secure_message_wrap failed");
        free(wrappedMessage);
        return NULL;
    }

    NSData * wrappedData = [[NSData alloc] initWithBytes:wrappedMessage length:wrappedMessageLength];
    free(wrappedMessage);
    return wrappedData;
}


- (NSData *)unwrap:(NSData *)message error:(NSError **)error {
    size_t unwrappedMessageLength = 0;

    TSErrorType result = (TSErrorType) themis_secure_message_unwrap([self.privateKey bytes], [self.privateKey length],
        [self.publicKey bytes], [self.publicKey length], [message bytes], [message length],
        NULL, &unwrappedMessageLength);

    if (result != TSErrorTypeBufferTooSmall) {
        *error = SCERROR(result, @"themis_secure_message_unwrap (length determination) failed");
        return nil;
    }

    unsigned char * unwrappedMessage = malloc(unwrappedMessageLength);
    result = (TSErrorType) themis_secure_message_unwrap([self.privateKey bytes], [self.privateKey length],
        [self.publicKey bytes], [self.publicKey length], [message bytes], [message length],
        unwrappedMessage, &unwrappedMessageLength);

    if (result != TSErrorTypeSuccess) {
        *error = SCERROR(result, @"themis_secure_message_unwrap failed");
        free(unwrappedMessage);
        return nil;
    }

    NSData * unwrappedData = [[NSData alloc] initWithBytes:unwrappedMessage length:unwrappedMessageLength];
    free(unwrappedMessage);
    return unwrappedData;
}


@end
