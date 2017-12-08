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
#import <objcthemis/serror.h>


@interface TSMessage ()

/** @brief private key */
@property (nonatomic, readwrite) NSData * privateKey;

/** @brief public key */
@property (nonatomic, readwrite) NSData * publicKey;

/** @brief mode */
@property (nonatomic, readwrite) TSMessageMode mode;

@end


@implementation TSMessage

- (nullable instancetype)initInEncryptModeWithPrivateKey:(NSData *)privateKey peerPublicKey:(NSData *)peerPublicKey {
    self = [super init];
    if (self) {
        if (!privateKey || [privateKey length] == 0 || !peerPublicKey || [peerPublicKey length] == 0) {
            return nil;
        }
        self.privateKey = [privateKey copy];
        self.publicKey = [peerPublicKey copy];
        self.mode = TSMessageModeEncryptDecrypt;
    }
    return self;
}


- (nullable instancetype)initInSignVerifyModeWithPrivateKey:(NSData *)privateKey peerPublicKey:(NSData *)peerPublicKey {
    self = [super init];
    if (self) {
        if (!privateKey || [privateKey length] == 0 || !peerPublicKey || [peerPublicKey length] == 0) {
            return nil;
        }
        self.privateKey = [privateKey copy];
        self.publicKey = [peerPublicKey copy];
        self.mode = TSMessageModeSignVerify;
    }
    return self;
}


- (nullable NSData *)wrapData:(nullable NSData *)message error:(NSError * __autoreleasing *)error {
    size_t wrappedMessageLength = 0;
    TSErrorType result = TSErrorTypeFail;

    switch (self.mode) {
        case TSMessageModeEncryptDecrypt:
            result = (TSErrorType) themis_secure_message_wrap([self.privateKey bytes], [self.privateKey length],
                [self.publicKey bytes], [self.publicKey length], [message bytes], [message length],
                NULL, &wrappedMessageLength);
            break;

        case TSMessageModeSignVerify:
            if (!(self.privateKey) || [self.privateKey length] == 0) {
				if (error) {
                	*error = SCERROR(TSErrorTypeFail, @"Secure Message failed wraping");
				}
                return nil;
            }
            result = (TSErrorType) themis_secure_message_wrap([self.privateKey bytes], [self.privateKey length], NULL, 0,
                [message bytes], [message length], NULL, &wrappedMessageLength);
            break;
        default:
			if (error) {
            	*error = SCERROR(TSErrorTypeFail, @"Secure Message failed wraping, mode unknown");
			}
            return nil;
    }

    if (result != TSErrorTypeBufferTooSmall) {
		if (error) {
        	*error = SCERROR(result, @"Secure Message failed wraping");
		}
        return nil;
    }

    unsigned char * wrappedMessage = malloc(wrappedMessageLength);
    if (!wrappedMessage) {
		if (error) {
        	*error = SCERROR(TSErrorTypeFail, @"Secure Message failed, not enough memory");
		}
        return nil;
    }

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
			if (error) {
            	*error = SCERROR(TSErrorTypeFail, @"Secure Message failed wraping, mode unknown");
			}
            free(wrappedMessage);
            return NULL;
    }

    if (result != TSErrorTypeSuccess) {
		if (error) {
        	*error = SCERROR(result, @"Secure Message failed wrapping");
		}
        free(wrappedMessage);
        return NULL;
    }

    return [NSData dataWithBytesNoCopy:wrappedMessage length:wrappedMessageLength];
}


- (nullable NSData *)unwrapData:(nullable NSData *)message error:(NSError * __autoreleasing *)error {
    size_t unwrappedMessageLength = 0;

    TSErrorType result = (TSErrorType) themis_secure_message_unwrap([self.privateKey bytes], [self.privateKey length],
        [self.publicKey bytes], [self.publicKey length], [message bytes], [message length],
        NULL, &unwrappedMessageLength);

    if (result != TSErrorTypeBufferTooSmall) {
		if (error) {
        	*error = SCERROR(result, @"Secure Message failed unwraping");
		}
        return nil;
    }

    unsigned char * unwrappedMessage = malloc(unwrappedMessageLength);
    if (!unwrappedMessage) {
		if (error) {
        	*error = SCERROR(TSErrorTypeFail, @"Secure Message failed, not enough memory");
		}
        return nil;
    }

    result = (TSErrorType) themis_secure_message_unwrap([self.privateKey bytes], [self.privateKey length],
        [self.publicKey bytes], [self.publicKey length], [message bytes], [message length],
        unwrappedMessage, &unwrappedMessageLength);

    if (result != TSErrorTypeSuccess) {
		if (error) {
        	*error = SCERROR(result, @"Secure Message failed unwraping");
		}
        free(unwrappedMessage);
        return nil;
    }

    return [NSData dataWithBytesNoCopy:unwrappedMessage length:unwrappedMessageLength];
}


@end
