//
//  SecureMessageTests.m
//  objthemis
//
//  Created by Anastasiia on 9/19/17.
//

#import <XCTest/XCTest.h>

#import "StaticKeys.h"

// TODO: use a unified import here
// CocoaPods tests do not work with canonical import of Themis for some reason.
// Please fix this if you have any idea how.
#if COCOAPODS
#import <objcthemis/objcthemis.h>
#else
@import themis;
#endif

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
    [self encryptDecryptWithPrivateStringKey:privateKeyEc256 publicStringKey:publicKeyEc256];
    [self signVerifyWithPrivateStringKey:privateKeyEc256 publicStringKey:publicKeyEc256];
}

- (void)testSecureMessageRSA1024 {
    [self encryptDecryptWithPrivateStringKey:privateKeyRSA1024 publicStringKey:publicKeyRSA1024];
    [self signVerifyWithPrivateStringKey:privateKeyRSA1024 publicStringKey:publicKeyRSA1024];
}

- (void)testSecureMessageRSA2048 {
    [self encryptDecryptWithPrivateStringKey:privateKeyRSA2048 publicStringKey:publicKeyRSA2048];
    [self signVerifyWithPrivateStringKey:privateKeyRSA2048 publicStringKey:publicKeyRSA2048];
}

- (void)testSecureMessageRSA4096 {
    [self encryptDecryptWithPrivateStringKey:privateKeyRSA4096 publicStringKey:publicKeyRSA4096];
    [self signVerifyWithPrivateStringKey:privateKeyRSA4096 publicStringKey:publicKeyRSA4096];
}

- (void)testSecureMessageKeygenRSA {
  TSKeyGen *keygenRSA = [[TSKeyGen alloc] initWithAlgorithm:TSKeyGenAsymmetricAlgorithmRSA];
  [self encryptDecryptWithPrivateKey:keygenRSA.privateKey publicKey:keygenRSA.publicKey];
  [self signVerifyWithPrivateKey:keygenRSA.privateKey publicKey:keygenRSA.publicKey];
}

- (void)testSecureMessageKeygenEC {
  TSKeyGen *keygenEC = [[TSKeyGen alloc] initWithAlgorithm:TSKeyGenAsymmetricAlgorithmEC];
  [self encryptDecryptWithPrivateKey:keygenEC.privateKey publicKey:keygenEC.publicKey];
  [self signVerifyWithPrivateKey:keygenEC.privateKey publicKey:keygenEC.publicKey];
}


- (void)encryptDecryptWithPrivateStringKey:(NSString *)privateKey publicStringKey:(NSString *)publicKey {
  NSData *publicKeyData = [[NSData alloc] initWithBase64EncodedString:publicKey
                                                              options:NSDataBase64DecodingIgnoreUnknownCharacters];
  NSData *privateKeyData = [[NSData alloc] initWithBase64EncodedString:privateKey
                                                               options:NSDataBase64DecodingIgnoreUnknownCharacters];
  
  return [self encryptDecryptWithPrivateKey:privateKeyData publicKey:publicKeyData];
}

- (void)encryptDecryptWithPrivateKey:(NSData *)privateKeyData publicKey:(NSData *)publicKeyData {
    TSMessage *encrypter = [[TSMessage alloc] initInEncryptModeWithPrivateKey:privateKeyData peerPublicKey:publicKeyData];

    NSString *message = @"- Knock, knock.\n- Who’s there?\n*very long pause...*\n- Java.";
    NSData *emptyData = [@"" dataUsingEncoding:NSUTF8StringEncoding];


    NSError *themisError = nil;
    NSData *encryptedMessage = [encrypter wrapData:[message dataUsingEncoding:NSUTF8StringEncoding] error:&themisError];
    XCTAssertNil(themisError, @"encrypting data should be successful");
    XCTAssertNotNil(encryptedMessage, @"encrypting data should return encrypted data");
  
    NSData *decryptedMessage = [encrypter unwrapData:encryptedMessage error:&themisError];
    XCTAssertNil(themisError, @"decrypting data should be successful");
    XCTAssertNotNil(decryptedMessage, @"decrypting data should return decrypted data");

    NSString *resultString = [[NSString alloc] initWithData:decryptedMessage encoding:NSUTF8StringEncoding];
    XCTAssertTrue([message isEqualToString:resultString], @"initial string and decrypted string should be the same");
  
    // NULL message
    NSError *nullErrorEn = nil;
    NSData *nullMessageEn = [encrypter wrapData:NULL error:&nullErrorEn];
    XCTAssertNotNil(nullErrorEn, @"encrypting data with NULL message is error");
    XCTAssertNil(nullMessageEn, @"encrypting data with NULL message should return nil message");
    
    NSError *nullErrorDe = nil;
    NSData *nullMessageDe = [encrypter unwrapData:NULL error:&nullErrorDe];
    XCTAssertNotNil(nullErrorDe, @"decrypting data with NULL message is error");
    XCTAssertNil(nullMessageDe, @"decrypting data with NULL message should return nil message");
    
    // empty message
    NSError *emptyErrorEn = nil;
    NSData *emptyMessageEn = [encrypter wrapData:emptyData error:&emptyErrorEn];
    XCTAssertNotNil(emptyErrorEn, @"encrypting data with empty message is error");
    XCTAssertNil(emptyMessageEn, @"encrypting data with empty message should return nil message");
    
    NSError *emptyErrorDe = nil;
    NSData *emptyMessageDe = [encrypter unwrapData:emptyData error:&emptyErrorDe];
    XCTAssertNotNil(emptyErrorDe, @"decrypting data with empty message is error");
    XCTAssertNil(emptyMessageDe, @"decrypting data with empty message should return nil message");
  
    // empty keys
    // Key parameters here are marked with "nonnull" and normally issue warnings.
    // Suppress the warnings and verify that we don't crash if they are ignored.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnonnull"
    TSMessage *encrypterEPriv = [[TSMessage alloc] initInEncryptModeWithPrivateKey:NULL peerPublicKey:publicKeyData];
    XCTAssertNil(encrypterEPriv, @"init SMessage in Encrypt/Decrypt mode with empty private key should return nil");
  
    TSMessage *encrypterEPub = [[TSMessage alloc] initInEncryptModeWithPrivateKey:privateKeyData peerPublicKey:NULL];
    XCTAssertNil(encrypterEPub, @"init SMessage in Encrypt/Decrypt mode with empty public key should return nil");
  
    TSMessage *encrypterEPrivEPub = [[TSMessage alloc] initInEncryptModeWithPrivateKey:NULL peerPublicKey:NULL];
    XCTAssertNil(encrypterEPrivEPub, @"init SMessage in Encrypt/Decrypt mode with both empty keys should return nil");
#pragma GCC diagnostic pop

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
    NSData *emptyData = [@"" dataUsingEncoding:NSUTF8StringEncoding];

    NSError *themisError = nil;
    NSData *encryptedMessage = [encrypter wrapData:[message dataUsingEncoding:NSUTF8StringEncoding] error:&themisError];
    XCTAssertNil(themisError, @"signing data should be successful");
    XCTAssertNotNil(encryptedMessage, @"signing data should return encrypted data");

    NSData *decryptedMessage = [encrypter unwrapData:encryptedMessage error:&themisError];
    XCTAssertNil(themisError, @"verifying data should be successful");
    XCTAssertNotNil(decryptedMessage, @"verifying data should return original data");

    NSString *resultString = [[NSString alloc] initWithData:decryptedMessage encoding:NSUTF8StringEncoding];
    XCTAssertTrue([message isEqualToString:resultString], @"initial string and decrypted string should be the same");
  
    // NULL message
    NSError *nullErrorEn = nil;
    NSData *nullMessageEn = [encrypter wrapData:NULL error:&nullErrorEn];
    XCTAssertNotNil(nullErrorEn, @"signing data with NULL message is error");
    XCTAssertNil(nullMessageEn, @"signing data with NULL message should return nil message");
    
    NSError *nullErrorDe = nil;
    NSData *nullMessageDe = [encrypter unwrapData:NULL error:&nullErrorDe];
    XCTAssertNotNil(nullErrorDe, @"verifying data with NULL message is error");
    XCTAssertNil(nullMessageDe, @"verifying data with NULL message should return nil message");
    
    // empty message
    NSError *emptyErrorSig = nil;
    NSData *emptyMessageSig = [encrypter wrapData:emptyData error:&emptyErrorSig];
    XCTAssertNotNil(emptyErrorSig, @"signing data with empty message is error");
    XCTAssertNil(emptyMessageSig, @"signing data with empty message should return nil message");
  
    NSError *emptyErrorVer = nil;
    NSData *emptyMessageVer = [encrypter unwrapData:emptyData error:&emptyErrorVer];
    XCTAssertNotNil(emptyErrorVer, @"verifying data with empty message is error");
    XCTAssertNil(emptyMessageVer, @"verifying data with empty message should return nil message");
  
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
