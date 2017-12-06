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

#import <objcthemis/skeygen.h>
#import <objcthemis/serror.h>


@interface TSKeyGen ()

@property (nonatomic, readwrite) TSKeyGenAsymmetricAlgorithm algorithm;

/** @brief private key */
@property (nonatomic, readwrite) NSMutableData * privateKey;
/** @brief public key */
@property (nonatomic, readwrite) NSMutableData * publicKey;

@end


@implementation TSKeyGen

- (nullable instancetype)initWithAlgorithm:(TSKeyGenAsymmetricAlgorithm)algorithm {
    self = [super init];
    if (self) {
        self.algorithm = algorithm;
        if ([self generateKeys] != TSErrorTypeSuccess) {
            return nil;
        }
    }
    return self;
}


- (TSErrorType)generateKeys {
    size_t privateKeyLength = 0;
    size_t publicKeyLength = 0;

    // Initialize key length for both keys
    TSErrorType result = TSErrorTypeFail;
    switch (self.algorithm) {
        case TSKeyGenAsymmetricAlgorithmEC:
            result = (TSErrorType) themis_gen_ec_key_pair(NULL, &privateKeyLength, NULL, &publicKeyLength);
            break;
        case TSKeyGenAsymmetricAlgorithmRSA:
            result = (TSErrorType) themis_gen_rsa_key_pair(NULL, &privateKeyLength, NULL, &publicKeyLength);
    }
    if (result != TSErrorTypeBufferTooSmall) {
        return result;
    }

    // Initialize keys
    self.privateKey = [[NSMutableData alloc] initWithLength:privateKeyLength];
    self.publicKey = [[NSMutableData alloc] initWithLength:publicKeyLength];

    // Fill keys with binary data
    result = TSErrorTypeFail;
    switch (self.algorithm) {
        case TSKeyGenAsymmetricAlgorithmEC:
            result = (TSErrorType) themis_gen_ec_key_pair([self.privateKey mutableBytes], &privateKeyLength,
                            [self.publicKey mutableBytes], &publicKeyLength);
            break;
        case TSKeyGenAsymmetricAlgorithmRSA:
            result = (TSErrorType) themis_gen_rsa_key_pair([self.privateKey mutableBytes], &privateKeyLength,
                            [self.publicKey mutableBytes], &publicKeyLength);
    }
    return result;
}

@end
