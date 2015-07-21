//
//  objthemis.m
//  objthemis
//
//  Created by Admin on 21.07.15.
//
//

#import <UIKit/UIKit.h>
#import <XCTest/XCTest.h>
#import "skeygen.h"
#import "ssession.h"
#import "smessage.h"
#import "scell_seal.h"
#import "scell_token.h"
#import "scell_context_imprint.h"

@interface objthemis : XCTestCase

@end

@implementation objthemis

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (NSData *)generateMasterKey {
    NSString * masterKeyString = @"UkVDMgAAAC13PCVZAKOczZXUpvkhsC+xvwWnv3CLmlG0Wzy8ZBMnT+2yx/dg";
    NSData * masterKeyData = [[NSData alloc] initWithBase64EncodedString:masterKeyString
                                                                 options:NSDataBase64DecodingIgnoreUnknownCharacters];
    return masterKeyData;
}

- (void)testSecureCellSealMode {
    
    NSData * masterKeyData = [self generateMasterKey];
    TSCellSeal * cellSeal = [[TSCellSeal alloc] initWithKey:NULL];
//    XCTAssertTrue(!cellSeal,"secure cell encrypter (Seal mode) creation error");
    cellSeal = [[TSCellSeal alloc] initWithKey:masterKeyData];
    XCTAssertTrue(cellSeal,@"secure cell encrypter (Seal mode) creation error");
    NSString * message = @"All your base are belong to us!";
    NSString * context = @"For great justice";
    NSError * themisError;
    
    
    NSData * encryptedMessage = [cellSeal wrapData:nil context:[context dataUsingEncoding:NSUTF8StringEncoding] error:&themisError];
    XCTAssertTrue(themisError, @"secure cell encrypter (Seal mode) encryption error");
    themisError=NULL;
    encryptedMessage = [cellSeal wrapData:[message dataUsingEncoding:NSUTF8StringEncoding] context:[context dataUsingEncoding:NSUTF8StringEncoding] error:&themisError];
    XCTAssertTrue(!themisError, @"secure cell encrypter (Seal mode) encryption error");
    NSData * decryptedMessage = [cellSeal unwrapData:encryptedMessage context:nil error:&themisError];
    XCTAssertTrue(themisError, @"secure cell encrypter (Seal mode) decryption error");
    themisError=NULL;
    decryptedMessage = [cellSeal unwrapData:encryptedMessage context:[context dataUsingEncoding:NSUTF8StringEncoding] error:&themisError];
    XCTAssertTrue(!themisError, @"secure cell encrypter (Seal mode) decryption error");
    NSString * resultString = [[NSString alloc] initWithData:decryptedMessage encoding:NSUTF8StringEncoding];
    XCTAssertTrue([message isEqualToString:resultString], @"secure cell encrypter (Seal mode) comparation error");
    NSData * encryptedMessageNoContext = [cellSeal wrapData:[message dataUsingEncoding:NSUTF8StringEncoding] context:nil error:&themisError];
    XCTAssertTrue(!themisError, @"secure cell encrypter (Seal mode) encryption error");
    XCTAssertTrue(![encryptedMessage isEqualToData:encryptedMessageNoContext], @"secure cell encrypter (Seal mode) encryption result with and without context can`t be equal");
    NSData * decryptedMessageNoContext = [cellSeal unwrapData:encryptedMessageNoContext context:[context dataUsingEncoding:NSUTF8StringEncoding] error:&themisError];
    XCTAssertTrue(themisError, @"secure cell encrypter (Seal mode) decryption error");
    themisError=NULL;
    decryptedMessageNoContext = [cellSeal unwrapData:encryptedMessageNoContext context:nil error:&themisError];
    XCTAssertTrue(!themisError, @"secure cell encrypter (Seal mode) decryption error");
    resultString = [[NSString alloc] initWithData:decryptedMessageNoContext encoding:NSUTF8StringEncoding];
    XCTAssertTrue([message isEqualToString:resultString], @"secure cell encrypter (Seal mode) comparation error");
}


- (void)testSecureCellTokenProtectMode {
    
    NSData * masterKeyData = [self generateMasterKey];
    TSCellToken * cellToken = [[TSCellToken alloc] initWithKey:masterKeyData];
    XCTAssertTrue(cellToken);
    
    NSString * message = @"Roses are grey. Violets are grey.";
    NSString * context = @"I'm a dog";
    NSString * incorrect_context = @"I'm a cat";
    NSError * themisError;

    TSCellTokenEncryptedData * encryptedMessage = [cellToken wrapData:nil context:[context dataUsingEncoding:NSUTF8StringEncoding] error:&themisError];
    XCTAssertTrue(themisError);
    themisError=NULL;
    
    encryptedMessage = [cellToken wrapData:nil context:nil error:&themisError];
    XCTAssertTrue(themisError);
    themisError=NULL;
    
    encryptedMessage = [cellToken wrapData:[message dataUsingEncoding:NSUTF8StringEncoding] context:[context dataUsingEncoding:NSUTF8StringEncoding] error:&themisError];
    XCTAssertTrue(!themisError);
    
    TSCellTokenEncryptedData * encryptedMessageNoContext = [cellToken wrapData:[message dataUsingEncoding:NSUTF8StringEncoding] context:nil error:&themisError];
    XCTAssertTrue(!themisError);
    
    NSData * decryptedMessage = [cellToken unwrapData:encryptedMessage context:nil error:&themisError];
    XCTAssertTrue(themisError);
    themisError=NULL;

    decryptedMessage = [cellToken unwrapData:encryptedMessage context:[incorrect_context dataUsingEncoding:NSUTF8StringEncoding] error:&themisError];
    XCTAssertTrue(themisError);
    themisError=NULL;
    
    decryptedMessage = [cellToken unwrapData:encryptedMessage context:[context dataUsingEncoding:NSUTF8StringEncoding] error:&themisError];
    XCTAssertTrue(!themisError);
    NSString * resultString = [[NSString alloc] initWithData:decryptedMessage encoding:NSUTF8StringEncoding];
    XCTAssertTrue([message isEqualToString:resultString]);

    NSData * decryptedMessageNoContext = [cellToken unwrapData:encryptedMessageNoContext context:[context dataUsingEncoding:NSUTF8StringEncoding] error:&themisError];
    XCTAssertTrue(themisError);
    themisError=NULL;
    
    decryptedMessageNoContext = [cellToken unwrapData:encryptedMessageNoContext context:nil error:&themisError];
    XCTAssertTrue(!themisError);
    resultString = [[NSString alloc] initWithData:decryptedMessageNoContext encoding:NSUTF8StringEncoding];
    XCTAssertTrue([message isEqualToString:resultString]);
    
}


- (void)testSecureCellImprint {
    NSData * masterKeyData = [self generateMasterKey];
    TSCellContextImprint * contextImprint = [[TSCellContextImprint alloc] initWithKey:masterKeyData];
    XCTAssertTrue(contextImprint);
    
    NSString * message = @"Roses are red. My name is Dave. This poem have no sense";
    NSString * context = @"Microwave";
    NSString * incorrect_context = @"Macrowave";
    NSError * themisError;
    
    NSData * encryptedMessage = [contextImprint wrapData:nil context:[context dataUsingEncoding:NSUTF8StringEncoding] error:&themisError];
    XCTAssertTrue(themisError);
    themisError=NULL;
    encryptedMessage = [contextImprint wrapData:nil context:nil error:&themisError];
    XCTAssertTrue(themisError);
    themisError=NULL;
    encryptedMessage = [contextImprint wrapData:[message dataUsingEncoding:NSUTF8StringEncoding] context:nil error:&themisError];
    XCTAssertTrue(themisError);
    themisError=NULL;
    encryptedMessage = [contextImprint wrapData:[message dataUsingEncoding:NSUTF8StringEncoding] context:[context dataUsingEncoding:NSUTF8StringEncoding] error:&themisError];
    XCTAssertTrue(!themisError);

    NSData * decryptedMessage = [contextImprint unwrapData:encryptedMessage context:nil error:&themisError];
    XCTAssertTrue(themisError);
    themisError=NULL;
    decryptedMessage = [contextImprint unwrapData:encryptedMessage context:[incorrect_context dataUsingEncoding:NSUTF8StringEncoding] error:&themisError];
    XCTAssertTrue(!themisError);
    NSString * resultString = [[NSString alloc] initWithData:decryptedMessage encoding:NSUTF8StringEncoding];
    XCTAssertTrue(![message isEqualToString:resultString]);
    decryptedMessage = [contextImprint unwrapData:encryptedMessage context:[context dataUsingEncoding:NSUTF8StringEncoding] error:&themisError];
    XCTAssertTrue(!themisError);
    resultString = [[NSString alloc] initWithData:decryptedMessage encoding:NSUTF8StringEncoding];
    XCTAssertTrue([message isEqualToString:resultString]);
}


- (void)testGeneratingKeys {
    // Generating RSA keys
    TSKeyGen * keygenRSA = [[TSKeyGen alloc] initWithAlgorithm:TSKeyGenAsymmetricAlgorithmRSA];
    XCTAssertTrue(keygenRSA);
    TSKeyGen * keygenEC = [[TSKeyGen alloc] initWithAlgorithm:TSKeyGenAsymmetricAlgorithmEC];
    XCTAssertTrue(keygenEC);
}


- (void)testSecureMessageEncryptionDecryption {
    NSLog(@"----------------- %s -----------------", sel_getName(_cmd));
    
    // ---------- encryption
    
    // base64 encoded keys:
    // client private key
    // server public key
    NSString * serverPublicKeyString = @"VUVDMgAAAC2ELbj5Aue5xjiJWW3P2KNrBX+HkaeJAb+Z4MrK0cWZlAfpBUql";
    NSString * clientPrivateKeyString = @"UkVDMgAAAC13PCVZAKOczZXUpvkhsC+xvwWnv3CLmlG0Wzy8ZBMnT+2yx/dg";
    
    NSData * serverPublicKey = [[NSData alloc] initWithBase64EncodedString:serverPublicKeyString
                                                                   options:NSDataBase64DecodingIgnoreUnknownCharacters];
    NSData * clientPrivateKey = [[NSData alloc] initWithBase64EncodedString:clientPrivateKeyString
                                                                    options:NSDataBase64DecodingIgnoreUnknownCharacters];
    
    // initialize encrypter
    TSMessage * encrypter = [[TSMessage alloc] initInEncryptModeWithPrivateKey:clientPrivateKey peerPublicKey:serverPublicKey];
    
    NSString * message = @"- Knock, knock.\n- Who’s there?\n*very long pause...*\n- Java.";
    
    NSError * themisError;
    NSData * encryptedMessage = [encrypter wrapData:[message dataUsingEncoding:NSUTF8StringEncoding]
                                              error:&themisError];
    if (themisError) {
        NSLog(@"%s Error occured while encrypting %@", sel_getName(_cmd), themisError);
        return;
    }
    NSLog(@"%@", encryptedMessage);
    
    
    // -------- decryption
    
    // base64 encoded keys:
    // server private key
    // client public key
    NSString * serverPrivateKeyString = @"UkVDMgAAAC1FsVa6AMGljYqtNWQ+7r4RjXTabLZxZ/14EXmi6ec2e1vrCmyR";
    NSString * clientPublicKeyString = @"VUVDMgAAAC1SsL32Axjosnf2XXUwm/4WxPlZauQ+v+0eOOjpwMN/EO+Huh5d";
    
    NSData * serverPrivateKey = [[NSData alloc] initWithBase64EncodedString:serverPrivateKeyString
                                                                    options:NSDataBase64DecodingIgnoreUnknownCharacters];
    NSData * clientPublicKey = [[NSData alloc] initWithBase64EncodedString:clientPublicKeyString
                                                                   options:NSDataBase64DecodingIgnoreUnknownCharacters];
    
    // initialize decrypter
    TSMessage * decrypter = [[TSMessage alloc] initInEncryptModeWithPrivateKey:serverPrivateKey peerPublicKey:clientPublicKey];
    
    NSData * decryptedMessage = [decrypter unwrapData:encryptedMessage error:&themisError];
    if (themisError) {
        NSLog(@"%s Error occured while decrypting %@", sel_getName(_cmd), themisError);
        return;
    }
    
    NSString * resultString = [[NSString alloc] initWithData:decryptedMessage encoding:NSUTF8StringEncoding];
    NSLog(@"%@", resultString);
}

- (void)testSecureMessageSignVerify {
    NSLog(@"----------------- %s -----------------", sel_getName(_cmd));
    
    // ---------- signing
    
    // base64 encoded keys:
    // client private key
    // server public key
    NSString * serverPublicKeyString = @"VUVDMgAAAC2ELbj5Aue5xjiJWW3P2KNrBX+HkaeJAb+Z4MrK0cWZlAfpBUql";
    NSString * clientPrivateKeyString = @"UkVDMgAAAC13PCVZAKOczZXUpvkhsC+xvwWnv3CLmlG0Wzy8ZBMnT+2yx/dg";
    
    NSData * serverPublicKey = [[NSData alloc] initWithBase64EncodedString:serverPublicKeyString
                                                                   options:NSDataBase64DecodingIgnoreUnknownCharacters];
    NSData * clientPrivateKey = [[NSData alloc] initWithBase64EncodedString:clientPrivateKeyString
                                                                    options:NSDataBase64DecodingIgnoreUnknownCharacters];
    
    // initialize encrypter
    TSMessage * encrypter = [[TSMessage alloc] initInSignVerifyModeWithPrivateKey:clientPrivateKey peerPublicKey:serverPublicKey];
    
    NSString * message = @"- Knock, knock.\n- Who’s there?\n*very long pause...*\n- Java.";
    
    NSError * themisError;
    NSData * encryptedMessage = [encrypter wrapData:[message dataUsingEncoding:NSUTF8StringEncoding]
                                              error:&themisError];
    if (themisError) {
        NSLog(@"%s Error occured while encrypting %@", sel_getName(_cmd), themisError);
        return;
    }
    NSLog(@"%@", encryptedMessage);
    
    
    // -------- verification
    
    // base64 encoded keys:
    // server private key
    // client public key
    NSString * serverPrivateKeyString = @"UkVDMgAAAC1FsVa6AMGljYqtNWQ+7r4RjXTabLZxZ/14EXmi6ec2e1vrCmyR";
    NSString * clientPublicKeyString = @"VUVDMgAAAC1SsL32Axjosnf2XXUwm/4WxPlZauQ+v+0eOOjpwMN/EO+Huh5d";
    
    NSData * serverPrivateKey = [[NSData alloc] initWithBase64EncodedString:serverPrivateKeyString
                                                                    options:NSDataBase64DecodingIgnoreUnknownCharacters];
    NSData * clientPublicKey = [[NSData alloc] initWithBase64EncodedString:clientPublicKeyString
                                                                   options:NSDataBase64DecodingIgnoreUnknownCharacters];
    
    // initialize decrypter
    TSMessage * decrypter = [[TSMessage alloc] initInSignVerifyModeWithPrivateKey:serverPrivateKey peerPublicKey:clientPublicKey];
    
    NSData * decryptedMessage = [decrypter unwrapData:encryptedMessage error:&themisError];
    if (themisError) {
        NSLog(@"%s Error occured while decrypting %@", sel_getName(_cmd), themisError);
        return;
    }
    
    NSString * resultString = [[NSString alloc] initWithData:decryptedMessage encoding:NSUTF8StringEncoding];
    NSLog(@"%@", resultString);
}

@end
