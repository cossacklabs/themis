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
    [self wrapUnwrapWithPrivateStringKey:privateKeyEc256 publicStringKey:publicKeyEc256];
    [self signVerifyWithPrivateStringKey:privateKeyEc256 publicStringKey:publicKeyEc256];
}

- (void)testSecureMessageRSA1024 {
    [self wrapUnwrapWithPrivateStringKey:privateKeyRSA1024 publicStringKey:publicKeyRSA1024];
    [self signVerifyWithPrivateStringKey:privateKeyRSA1024 publicStringKey:publicKeyRSA1024];
}

- (void)testSecureMessageRSA2048 {
    [self wrapUnwrapWithPrivateStringKey:privateKeyRSA2048 publicStringKey:publicKeyRSA2048];
    [self signVerifyWithPrivateStringKey:privateKeyRSA2048 publicStringKey:publicKeyRSA2048];
}

- (void)testSecureMessageRSA4096 {
    [self wrapUnwrapWithPrivateStringKey:privateKeyRSA4096 publicStringKey:publicKeyRSA4096];
    [self signVerifyWithPrivateStringKey:privateKeyRSA4096 publicStringKey:publicKeyRSA4096];
}

- (void)testSecureMessageKeygenRSA {
  TSKeyGen *keygenRSA = [[TSKeyGen alloc] initWithAlgorithm:TSKeyGenAsymmetricAlgorithmRSA];
  [self wrapUnwrapWithPrivateKey:keygenRSA.privateKey publicKey:keygenRSA.publicKey];
  [self signVerifyWithPrivateKey:keygenRSA.privateKey publicKey:keygenRSA.publicKey];
}

- (void)testSecureMessageKeygenEC {
  TSKeyGen *keygenEC = [[TSKeyGen alloc] initWithAlgorithm:TSKeyGenAsymmetricAlgorithmEC];
  [self wrapUnwrapWithPrivateKey:keygenEC.privateKey publicKey:keygenEC.publicKey];
  [self signVerifyWithPrivateKey:keygenEC.privateKey publicKey:keygenEC.publicKey];
}


- (void)wrapUnwrapWithPrivateStringKey:(NSString *)privateKey publicStringKey:(NSString *)publicKey {
  NSData *publicKeyData = [[NSData alloc] initWithBase64EncodedString:publicKey
                                                              options:NSDataBase64DecodingIgnoreUnknownCharacters];
  NSData *privateKeyData = [[NSData alloc] initWithBase64EncodedString:privateKey
                                                               options:NSDataBase64DecodingIgnoreUnknownCharacters];
  
  return [self wrapUnwrapWithPrivateKey:privateKeyData publicKey:publicKeyData];
}

- (void)wrapUnwrapWithPrivateKey:(NSData *)privateKeyData publicKey:(NSData *)publicKeyData {
    TSMessage *encrypter = [[TSMessage alloc] initInEncryptModeWithPrivateKey:privateKeyData peerPublicKey:publicKeyData];

    NSString *message = @"- Knock, knock.\n- Who’s there?\n*very long pause...*\n- Java.";


    NSError *themisError = nil;
    NSData *encryptedMessage = [encrypter wrapData:[message dataUsingEncoding:NSUTF8StringEncoding] error:&themisError];
    XCTAssertNil(themisError, @"encrypting data should be successful");
    XCTAssertNotNil(encryptedMessage, @"encrypting data should return encrypted data");
  
    NSData *decryptedMessage = [encrypter unwrapData:encryptedMessage error:&themisError];
    XCTAssertNil(themisError, @"decrypting data should be successful");
    XCTAssertNotNil(decryptedMessage, @"decrypting data should return decrypted data");

    NSString *resultString = [[NSString alloc] initWithData:decryptedMessage encoding:NSUTF8StringEncoding];
    XCTAssertTrue([message isEqualToString:resultString], @"initial string and decrypted string should be the same");
  
    // empty message
    NSError *themisEmptyErrorEn = nil;
    NSData *emEmptyMessageEn = [encrypter wrapData:NULL error:&themisEmptyErrorEn];
    XCTAssertNotNil(themisEmptyErrorEn, @"encrypting data with empty message is error");
    XCTAssertNil(emEmptyMessageEn, @"encrypting data with empty message should return nil message");
  
    NSError *themisEmptyErrorDe = nil;
    NSData *emEmptyMessageDe = [encrypter unwrapData:NULL error:&themisEmptyErrorDe];
    XCTAssertNotNil(themisEmptyErrorDe, @"decrypting data with empty message is error");
    XCTAssertNil(emEmptyMessageDe, @"decrypting data with empty message should return nil message");
  
    // empty keys
    TSMessage *encrypterEPriv = [[TSMessage alloc] initInEncryptModeWithPrivateKey:NULL peerPublicKey:publicKeyData];
    XCTAssertNil(encrypterEPriv, @"init SMessage in Encrypt/Decrypt mode with empty private key should return nil");
  
    TSMessage *encrypterEPub = [[TSMessage alloc] initInEncryptModeWithPrivateKey:privateKeyData peerPublicKey:NULL];
    XCTAssertNil(encrypterEPub, @"init SMessage in Encrypt/Decrypt mode with empty public key should return nil");
  
    TSMessage *encrypterEPrivEPub = [[TSMessage alloc] initInEncryptModeWithPrivateKey:NULL peerPublicKey:NULL];
    XCTAssertNil(encrypterEPrivEPub, @"init SMessage in Encrypt/Decrypt mode with both empty keys should return nil");
  
    // another encryptor
    TSKeyGen *keygenRSA = [[TSKeyGen alloc] initWithAlgorithm:TSKeyGenAsymmetricAlgorithmRSA];
    TSMessage *anotherEncryptor = [[TSMessage alloc] initInSignVerifyModeWithPrivateKey:keygenRSA.privateKey peerPublicKey:keygenRSA.publicKey];
    NSError *themisAnotherEncrError = nil;
    NSData *verifiedBadMessage = [anotherEncryptor unwrapData:encryptedMessage error:&themisAnotherEncrError];
    XCTAssertNotNil(themisAnotherEncrError, @"another encryptor can't decrypt other encrypted message");
    XCTAssertNil(verifiedBadMessage, @"another encryptor can't decrypt other encrypted message");
}

- (void)signVerifyWithPrivateStringKey:(NSString *)privateKey publicStringKey:(NSString *)publicKey {
  NSData *publicKeyData = [[NSData alloc] initWithBase64EncodedString:publicKey
                                                              options:NSDataBase64DecodingIgnoreUnknownCharacters];
  NSData *privateKeyData = [[NSData alloc] initWithBase64EncodedString:privateKey
                                                               options:NSDataBase64DecodingIgnoreUnknownCharacters];
  
  return [self signVerifyWithPrivateKey:privateKeyData publicKey:publicKeyData];
}

- (void)signVerifyWithPrivateKey:(NSData *)privateKeyData publicKey:(NSData *)publicKeyData {

    TSMessage *encrypter = [[TSMessage alloc] initInSignVerifyModeWithPrivateKey:privateKeyData peerPublicKey:publicKeyData];

    NSString *message = @"- Knock, knock.\n- Who’s there?\n*very long pause...*\n- Java.";


    NSError *themisError = nil;
    NSData *encryptedMessage = [encrypter wrapData:[message dataUsingEncoding:NSUTF8StringEncoding] error:&themisError];
    XCTAssertNil(themisError, @"signing data should be successful");
    XCTAssertNotNil(encryptedMessage, @"signing data should return encrypted data");

    NSData *decryptedMessage = [encrypter unwrapData:encryptedMessage error:&themisError];
    XCTAssertNil(themisError, @"verifying data should be successful");
    XCTAssertNotNil(decryptedMessage, @"verifying data should return original data");

    NSString *resultString = [[NSString alloc] initWithData:decryptedMessage encoding:NSUTF8StringEncoding];
    XCTAssertTrue([message isEqualToString:resultString], @"initial string and decrypted string should be the same");
  
    // empty message
    NSError *themisEmptyErrorEn = nil;
    NSData *emEmptyMessageEn = [encrypter wrapData:NULL error:&themisEmptyErrorEn];
    XCTAssertNotNil(themisEmptyErrorEn, @"signing data with empty message is error");
    XCTAssertNil(emEmptyMessageEn, @"signing data with empty message should return nil message");
  
    NSError *themisEmptyErrorDe = nil;
    NSData *emEmptyMessageDe = [encrypter unwrapData:NULL error:&themisEmptyErrorDe];
    XCTAssertNotNil(themisEmptyErrorDe, @"verifying data with empty message is error");
    XCTAssertNil(emEmptyMessageDe, @"verifying data with empty message should return nil message");
  
    // empty keys
    TSMessage *encrypterEPriv = [[TSMessage alloc] initInSignVerifyModeWithPrivateKey:NULL peerPublicKey:publicKeyData];
    XCTAssertNotNil(encrypterEPriv, @"init SMessage in Sign/Verify mode with empty private key should be ok");
  
    TSMessage *encrypterEPub = [[TSMessage alloc] initInSignVerifyModeWithPrivateKey:privateKeyData peerPublicKey:NULL];
    XCTAssertNotNil(encrypterEPub, @"init SMessage in Sign/Verify mode with empty public key should be ok");
  
    TSMessage *encrypterEPrivEPub = [[TSMessage alloc] initInSignVerifyModeWithPrivateKey:NULL peerPublicKey:NULL];
    XCTAssertNil(encrypterEPrivEPub, @"init SMessage in Sign/Verify mode with both empty keys should return nil");
  
  
    // sign only
    message = @"sign verify test";

    TSMessage *signer = [[TSMessage alloc] initInSignVerifyModeWithPrivateKey:privateKeyData peerPublicKey:NULL];
    NSError *themisSignError = nil;
    NSData *signedMessage = [signer wrapData:[message dataUsingEncoding:NSUTF8StringEncoding] error:&themisSignError];
    XCTAssertNil(themisSignError, @"signing data should be successful");
    XCTAssertNotNil(signedMessage, @"signing data should return encrypted data");
  
    NSData *badSignedMessage = [signer unwrapData:[message dataUsingEncoding:NSUTF8StringEncoding] error:&themisSignError];
    XCTAssertNotNil(themisSignError, @"signer can't verify the message");
    XCTAssertNil(badSignedMessage, @"signer can't verify the message");
  
    TSMessage *veryfier = [[TSMessage alloc] initInSignVerifyModeWithPrivateKey:NULL peerPublicKey:publicKeyData];
    NSError *themisVerError = nil;
    NSData *verifiedMessage = [veryfier unwrapData:signedMessage error:&themisVerError];
    XCTAssertNil(themisVerError, @"verifying data should be successful");
    XCTAssertNotNil(verifiedMessage, @"verifying data should return encrypted data");
  
    NSString * verResult = [[NSString alloc] initWithData:verifiedMessage encoding:NSUTF8StringEncoding];
    XCTAssertTrue([message isEqualToString:verResult], @"initial string and verifying string should be the same");
  
    NSError *themisBadVerError = nil;
    NSData *badVerifiedMessage = [veryfier wrapData:[message dataUsingEncoding:NSUTF8StringEncoding] error:&themisBadVerError];
    XCTAssertNotNil(themisBadVerError, @"verifer can't sign the message");
    XCTAssertNil(badVerifiedMessage, @"verifer can't sign the message");
    themisBadVerError = nil;
  
    // another verifier
    TSKeyGen *keygenRSA = [[TSKeyGen alloc] initWithAlgorithm:TSKeyGenAsymmetricAlgorithmRSA];
    TSMessage *anotherVeryfier = [[TSMessage alloc] initInSignVerifyModeWithPrivateKey:keygenRSA.privateKey peerPublicKey:keygenRSA.publicKey];
    NSData *verifiedBadMessage = [anotherVeryfier unwrapData:signedMessage error:&themisBadVerError];
    XCTAssertNotNil(themisBadVerError, @"wrong verifier can't verify another message");
    XCTAssertNil(verifiedBadMessage, @"wrong verifier can't verify another message");
}


@end
