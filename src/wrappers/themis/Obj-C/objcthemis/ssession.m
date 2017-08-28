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

#import <objcthemis/ssession.h>
#import <objcthemis/error.h>


@interface TSSession ()

@property (nonatomic) secure_session_t * session;

@end


@implementation TSSession

- (instancetype)initWithUserId:(NSData *)userId privateKey:(NSData *)privateKey
                     callbacks:(TSSessionTransportInterface *)callbacks {
    self = [super init];
    if (self) {
        self.session = secure_session_create([userId bytes], [userId length],
            [privateKey bytes], [privateKey length], [callbacks callbacks]);
    }
    return self;
}


- (void)connect:(NSError **)error {
    TSErrorType result = (TSErrorType) secure_session_connect(self.session);
    if (result != TSErrorTypeSuccess) {
        *error = SCERROR(result, @"Secure Session failed connection");
    }
}


- (NSData *)connectRequest:(NSError **)error {
    size_t connectRequestLength = 0;
    TSErrorType result = (TSErrorType) secure_session_generate_connect_request(self.session, NULL, &connectRequestLength);

    if (result != TSErrorTypeBufferTooSmall) {
        *error = SCERROR(result, @"Secure Session failed making connection request");
        return nil;
    }

    NSMutableData * requestData = [[NSMutableData alloc] initWithLength:connectRequestLength];
    result = (TSErrorType) secure_session_generate_connect_request(self.session, [requestData mutableBytes], &connectRequestLength);

    if (result != TSErrorTypeSuccess) {
        *error = SCERROR(result, @"Secure Session failed making connection request");
        return nil;
    }
    return [requestData copy];
}


- (NSData *)wrapData:(NSData *)message error:(NSError **)error {
    size_t wrappedMessageLength = 0;

    TSErrorType result = (TSErrorType) secure_session_wrap(self.session, [message bytes], [message length],
        NULL, &wrappedMessageLength);

    if (result != TSErrorTypeBufferTooSmall) {
        *error = SCERROR(result, @"Secure Session failed encryption");
        return nil;
    }

    NSMutableData * wrappedMessage = [[NSMutableData alloc] initWithLength:wrappedMessageLength];
    result = (TSErrorType) secure_session_wrap(self.session, [message bytes], [message length],
        [wrappedMessage mutableBytes], &wrappedMessageLength);

    if (result != TSErrorTypeSuccess) {
        *error = SCERROR(result, @"Secure Session failed encryption");
        return nil;
    }
    return [wrappedMessage copy];
}


- (NSData *)unwrapData:(NSData *)message error:(NSError **)error {
    size_t unwrappedMessageLength = 0;
    TSErrorType result = (TSErrorType) secure_session_unwrap(self.session, [message bytes], [message length],
        NULL, &unwrappedMessageLength);

    if (result != TSErrorTypeBufferTooSmall) {
        if (result == TSErrorTypeSuccess) {
            return nil;
        }
        *error = SCERROR(result, @"Secure Session failed decryption");
        return nil;
    }

    NSMutableData * unwrappedMessage = [[NSMutableData alloc] initWithLength:unwrappedMessageLength];
    result = (TSErrorType) secure_session_unwrap(self.session, [message bytes], [message length],
        [unwrappedMessage mutableBytes], &unwrappedMessageLength);

    if (result != TSErrorTypeSuccess) {
        if (result == TSErrorTypeSendAsIs) {
            return unwrappedMessage;
        }
        else {
            *error = SCERROR(result, @"Secure Session failed decryption");
            return nil;
        }
    }

    return [unwrappedMessage copy];
}


- (void)wrapAndSend:(NSData *)message error:(NSError **)error {
    TSErrorType result = (TSErrorType) secure_session_send(self.session, [message bytes], [message length]);
    if (result != TSErrorTypeSuccess) {
        *error = SCERROR(result, @"Secure Session failed sending");
    }
}


- (NSData *)unwrapAndReceive:(NSUInteger)length error:(NSError **)error {
    NSMutableData * receivedData = [[NSMutableData alloc] initWithLength:length];
    TSErrorType result = (TSErrorType) secure_session_receive(self.session, [receivedData mutableBytes],
        [receivedData length]);

    if (result != TSErrorTypeSuccess) {
        *error = SCERROR(result, @"Secure Session failed receiving");
        return nil;
    }
    return [receivedData copy];
}


- (BOOL)isSessionEstablished {
    return secure_session_is_established(self.session);
}

@end
