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

#import <objcthemis/ssession_transport_interface.h>
#import <objcthemis/serror.h>


ssize_t on_send_callback(const uint8_t * data, size_t data_length, void * user_data) {
    return TSErrorTypeFail;
}


ssize_t on_receive_callback(uint8_t * data, size_t data_length, void * user_data) {
    return TSErrorTypeFail;
}


void on_state_changed_callback(int event, void * user_data) {
    return;
}


int on_get_public_key_for_id_callback(const void * id, size_t id_length, void * key_buffer, size_t key_buffer_length, void * user_data) {
    TSSessionTransportInterface * referenceObject = (__bridge TSSessionTransportInterface *) user_data;
    NSError * error = nil;
    NSData * publicKey = [referenceObject publicKeyFor:[[NSData alloc] initWithBytes:id length:id_length] error:&error];

    if (error || key_buffer_length < [publicKey length]) {
        return TSErrorTypeFail;
    }
    memcpy(key_buffer, [publicKey bytes], [publicKey length]);
    return TSErrorTypeSuccess;
}


@implementation TSSessionTransportInterface

- (nullable instancetype)init {
    self = [super init];
    if (self) {
        _callbacks.send_data = &on_send_callback;
        _callbacks.receive_data = &on_receive_callback;
        _callbacks.state_changed = &on_state_changed_callback;
        _callbacks.get_public_key_for_id = &on_get_public_key_for_id_callback;
        _callbacks.user_data = (__bridge void *) self;
    }
    return self;
}


// TODO: implement
- (void)sendData:(nullable NSData *)data error:(NSError * __autoreleasing *)error {
	if (error) {
    	*error = SCERROR(TSErrorTypeFail, @"secure session send callback");
	}
    return;
}


// TODO: implement
- (nullable NSData *)receiveDataWithError:(NSError **)error {
	if (error) {
    	*error = SCERROR(TSErrorTypeFail, @"secure session receive callback");
	}
    return nil;
}


- (nullable NSData *)publicKeyFor:(nullable NSData *)binaryId error:(NSError * __autoreleasing *)error {
    // TODO: approve
    NSMutableData * key = [[NSMutableData alloc] initWithLength:1024];
    TSErrorType result = (TSErrorType) on_get_public_key_for_id_callback([binaryId bytes], [binaryId length],
        [key mutableBytes], [key length], (__bridge void *) self);

    if (result != TSErrorTypeSuccess) {
		if (error) {
        	*error = SCERROR(TSErrorTypeFail, @"Secure Session failed getting of public key");
		}
        return nil;
    }
    return [key copy];
}


- (secure_session_user_callbacks_t *)callbacks {
    return &_callbacks;
}

@end
