//
//  SecureMessageTests.m
//  objthemis
//
//  Created by Anastasiia on 9/19/17.
//

#import <XCTest/XCTest.h>
#import <objcthemis/objcthemis.h>
#import "StaticKeys.h"

@interface SecureMessageTests : XCTestCase

@end

@implementation SecureMessageTests

#pragma MARK -

- (void)testGeneratingKeys {
    // Generating RSA keys
    TSKeyGen *keygenRSA = [[TSKeyGen alloc] initWithAlgorithm:TSKeyGenAsymmetricAlgorithmRSA];
    XCTAssertNotNil(keygenRSA);
    XCTAssertNotNil(keygenRSA.privateKey);
    XCTAssertNotNil(keygenRSA.publicKey);

    //    NSLog(@"client private key %@", [keygenRSA.privateKey base64EncodedStringWithOptions:NSDataBase64Encoding64CharacterLineLength]);
    //    NSLog(@"client public key %@", [keygenRSA.publicKey base64EncodedStringWithOptions:NSDataBase64Encoding64CharacterLineLength]);

    TSKeyGen *keygenEC = [[TSKeyGen alloc] initWithAlgorithm:TSKeyGenAsymmetricAlgorithmEC];
    XCTAssertNotNil(keygenEC);
    XCTAssertNotNil(keygenEC.privateKey);
    XCTAssertNotNil(keygenEC.publicKey);

    TSKeyGen *keygenInvalid = [[TSKeyGen alloc] initWithAlgorithm:200];
    XCTAssertNil(keygenInvalid);
}

- (void)testSecureMessageEC256 {
    [self wrapUnwrapWithPrivateKey:privateKeyEc256 publicKey:publicKeyEc256];
    [self signVerifyWithPrivateKey:privateKeyEc256 publicKey:publicKeyEc256];
}

- (void)testSecureMessageRSA1024 {
    [self wrapUnwrapWithPrivateKey:privateKeyRSA1024 publicKey:publicKeyRSA1024];
    [self signVerifyWithPrivateKey:privateKeyRSA1024 publicKey:publicKeyRSA1024];
}

- (void)testSecureMessageRSA2048 {
    [self wrapUnwrapWithPrivateKey:privateKeyRSA2048 publicKey:publicKeyRSA2048];
    [self signVerifyWithPrivateKey:privateKeyRSA2048 publicKey:publicKeyRSA2048];
}

- (void)testSecureMessageRSA4096 {
    [self wrapUnwrapWithPrivateKey:privateKeyRSA4096 publicKey:publicKeyRSA4096];
    [self signVerifyWithPrivateKey:privateKeyRSA4096 publicKey:publicKeyRSA4096];
}


- (void)wrapUnwrapWithPrivateKey:(NSString *)privateKey publicKey:(NSString *)publicKey {
    NSData *publicKeyData = [[NSData alloc] initWithBase64EncodedString:publicKey
                                                                options:NSDataBase64DecodingIgnoreUnknownCharacters];
    NSData *privateKeyData = [[NSData alloc] initWithBase64EncodedString:privateKey
                                                                 options:NSDataBase64DecodingIgnoreUnknownCharacters];

    TSMessage *encrypter = [[TSMessage alloc] initInEncryptModeWithPrivateKey:privateKeyData peerPublicKey:publicKeyData];

    NSString *message = @"- Knock, knock.\n- Who’s there?\n*very long pause...*\n- Java.";


    NSError *themisError = nil;
    NSData *encryptedMessage = [encrypter wrapData:[message dataUsingEncoding:NSUTF8StringEncoding] error:&themisError];
    XCTAssertNil(themisError, @"wrapping data should be successful");
    XCTAssertNotNil(encryptedMessage, @"wrapping data should return encrypted data");

    NSData *decryptedMessage = [encrypter unwrapData:encryptedMessage error:&themisError];
    XCTAssertNil(themisError, @"unwrapping data should be successful");
    XCTAssertNotNil(decryptedMessage, @"unwrapping data should return decrypted data");

    NSString *resultString = [[NSString alloc] initWithData:decryptedMessage encoding:NSUTF8StringEncoding];
    XCTAssertTrue([message isEqualToString:resultString], @"initial string and decrypted string should be the same");
}

- (void)signVerifyWithPrivateKey:(NSString *)privateKey publicKey:(NSString *)publicKey {
    NSData *publicKeyData = [[NSData alloc] initWithBase64EncodedString:publicKey
                                                                options:NSDataBase64DecodingIgnoreUnknownCharacters];
    NSData *privateKeyData = [[NSData alloc] initWithBase64EncodedString:privateKey
                                                                 options:NSDataBase64DecodingIgnoreUnknownCharacters];

    TSMessage *encrypter = [[TSMessage alloc] initInSignVerifyModeWithPrivateKey:privateKeyData peerPublicKey:publicKeyData];

    NSString *message = @"- Knock, knock.\n- Who’s there?\n*very long pause...*\n- Java.";


    NSError *themisError = nil;
    NSData *encryptedMessage = [encrypter wrapData:[message dataUsingEncoding:NSUTF8StringEncoding] error:&themisError];
    XCTAssertNil(themisError, @"wrapping data should be successful");
    XCTAssertNotNil(encryptedMessage, @"wrapping data should return encrypted data");

    NSData *decryptedMessage = [encrypter unwrapData:encryptedMessage error:&themisError];
    XCTAssertNil(themisError, @"unwrapping data should be successful");
    XCTAssertNotNil(decryptedMessage, @"unwrapping data should return decrypted data");

    NSString *resultString = [[NSString alloc] initWithData:decryptedMessage encoding:NSUTF8StringEncoding];
    XCTAssertTrue([message isEqualToString:resultString], @"initial string and decrypted string should be the same");
}


@end
