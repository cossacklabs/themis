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

#import <objcthemis/scomparator.h>
#import <objcthemis/serror.h>


@interface TSComparator ()

@property (nonatomic) secure_comparator_t * comparator;

@end


@implementation TSComparator

- (nullable instancetype)initWithMessageToCompare:(NSData *)message {
    self = [super init];
    if (self) {
        self.comparator = secure_comparator_create();
        if (self.comparator) {
            if (secure_comparator_append_secret(self.comparator, [message bytes], [message length]) == TSErrorTypeSuccess) {
                return self;
            }
            secure_comparator_destroy(self.comparator);
        }
    }
    return nil;
}

// TODO: check for memory leak?
- (void)dealloc {
    if(self.comparator) {
        secure_comparator_destroy(self.comparator);
    }
}


- (nullable NSData *)beginCompare:(NSError * __autoreleasing *)error {
    size_t comparationRequestLength = 0;
    TSErrorType result = (TSErrorType) secure_comparator_begin_compare(self.comparator, NULL, &comparationRequestLength);

    if (result != TSErrorTypeBufferTooSmall) {
        if (error) {
        	*error = SCERROR(result, @"Secure Comparator failed making initialisation message");
		}
        return nil;
    }

    NSMutableData * requestData = [[NSMutableData alloc] initWithLength:comparationRequestLength];
    result = (TSErrorType) secure_comparator_begin_compare(self.comparator, [requestData mutableBytes], &comparationRequestLength);

    if (result != TSErrorTypeSuccess && result !=TSErrorTypeSendAsIs) {
		if (error) {
        	*error = SCERROR(result, @"Secure Comparator failed making initialisation message");
		}
        return nil;
    }
    return [requestData copy];
}

- (nullable NSData *)proceedCompare:(nullable NSData *)message error:(NSError * __autoreleasing *)error {
    size_t unwrappedMessageLength = 0;
    TSErrorType result = (TSErrorType) secure_comparator_proceed_compare(self.comparator, [message bytes], [message length], NULL, &unwrappedMessageLength);

    if (result != TSErrorTypeBufferTooSmall) {
        if (result == TSErrorTypeSuccess) {
            return nil;
        }
		if (error) {
        	*error = SCERROR(result, @"Secure Comparator failed proceeding message");
		}
        return nil;
    }

    NSMutableData * unwrappedMessage = [[NSMutableData alloc] initWithLength:unwrappedMessageLength];
    result = (TSErrorType) secure_comparator_proceed_compare(self.comparator, [message bytes], [message length], [unwrappedMessage mutableBytes], &unwrappedMessageLength);

    if (result != TSErrorTypeSuccess) {
        if (result == TSErrorTypeSendAsIs) {
            return unwrappedMessage;
        }
        else {
			if (error) {
            	*error = SCERROR(result, @"Secure Comparator failed proceeding message");
			}
            return nil;
        }
    }
    return [unwrappedMessage copy];
}

- (TSComparatorStateType)status {
    return secure_comparator_get_result(self.comparator);
}

@end
