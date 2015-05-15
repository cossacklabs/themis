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
#import <objcthemis/error.h>

@implementation SKeyGen

- (id)init: (AsymAlg)alg{
  self = [super init];
  if(self){
    size_t private_key_length=0;
    size_t public_key_length=0;
    int res=TErrorTypeFail;
    switch(alg){
    case EC:
    	res = themis_gen_ec_key_pair(NULL, &private_key_length, NULL, &public_key_length);
    	break;
    case RSA:
	res = themis_gen_rsa_key_pair(NULL, &private_key_length, NULL, &public_key_length);
    }
    if (res != TErrorTypeBufferTooSmall) {
	return nil;
    }

    _priv_key = [[NSMutableData alloc] initWithLength:private_key_length];
    _pub_key = [[NSMutableData alloc] initWithLength:public_key_length];
    res=TErrorTypeFail;
    switch(alg){
    case EC:
	res = themis_gen_ec_key_pair([_priv_key mutableBytes], &private_key_length, [_pub_key mutableBytes], &public_key_length);
	break;
    case RSA:
	res = themis_gen_rsa_key_pair([_priv_key mutableBytes], &private_key_length, [_pub_key mutableBytes], &public_key_length);
    }
    if (res != TErrorTypeSuccess) {
	return nil;
    }
  }
  return self;
}

- (NSData*)getPrivKey{
    return _priv_key;
}

- (NSData*)getPubKey{
    return _pub_key;
}


@end
