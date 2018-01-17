//
//  SecureCellTests.m
//  objthemis
//
//  Created by Anastasiia on 9/19/17.
//

#import <XCTest/XCTest.h>
#import <objcthemis/objcthemis.h>

@interface SecureCellTests : XCTestCase

@property(nonatomic, strong) NSData *masterKeyData;

@end


@implementation SecureCellTests

- (void)setUp {
    [super setUp];

    self.masterKeyData = [self generateMasterKey];
}

- (NSData *)generateMasterKey {
    NSString *masterKeyString = @"UkVDMgAAAC13PCVZAKOczZXUpvkhsC+xvwWnv3CLmlG0Wzy8ZBMnT+2yx/dg";
    NSData *masterKeyData = [[NSData alloc] initWithBase64EncodedString:masterKeyString
                                                                options:NSDataBase64DecodingIgnoreUnknownCharacters];
    return masterKeyData;
}


#pragma MARK - Seal Mode -

- (void)testSecureCellSealModeInit {
    TSCellSeal *cellSeal = [[TSCellSeal alloc] initWithKey:[NSData new]];
    XCTAssertNil(cellSeal, "secure cell encrypter (seal mode) should not be created without key");

    cellSeal = [[TSCellSeal alloc] initWithKey:self.masterKeyData];
    XCTAssertNotNil(cellSeal, @"secure cell encrypter (seal mode) creation error");
}


- (void)testSecureCellSealModeWithContext {
    TSCellSeal *cellSeal = [[TSCellSeal alloc] initWithKey:self.masterKeyData];
    NSString *message = @"All your base are belong to us!";
    NSString *context = @"For great justice";
    NSError *themisError;

    NSData *encryptedMessage = [cellSeal wrapData:nil
                                          context:[context dataUsingEncoding:NSUTF8StringEncoding]
                                            error:&themisError];
    XCTAssertNotNil(themisError, @"encryption without data-to-encrypt should populate error");
    XCTAssertNil(encryptedMessage, @"encryption without data-to-encrypt should return nil data");

    encryptedMessage = [cellSeal wrapData:[NSData new]
                                  context:[context dataUsingEncoding:NSUTF8StringEncoding]
                                    error:&themisError];
    XCTAssertNotNil(themisError, @"encryption with empty data-to-encrypt should populate error");
    XCTAssertNil(encryptedMessage, @"encryption with empty data-to-encrypt should return nil data");

    
    themisError = nil;
    encryptedMessage = [cellSeal wrapData:[message dataUsingEncoding:NSUTF8StringEncoding]
                                  context:[context dataUsingEncoding:NSUTF8StringEncoding]
                                    error:&themisError];
    XCTAssertNil(themisError, @"encryption with data and context should be successful");
    XCTAssertNotNil(encryptedMessage, @"encryption with data and context should be successful");

    NSData *decryptedMessage = [cellSeal unwrapData:encryptedMessage
                                            context:nil
                                              error:&themisError];
    XCTAssertNotNil(themisError, @"decryption without context should populate error");
    XCTAssertNil(decryptedMessage, @"decryption without context should return nil data");

    themisError = nil;
    decryptedMessage = [cellSeal unwrapData:encryptedMessage
                                    context:[context dataUsingEncoding:NSUTF8StringEncoding]
                                      error:&themisError];
    XCTAssertNil(themisError, @"decryption with data and context should be successful");
    XCTAssertNotNil(decryptedMessage, @"decryption with data and context should be successful");

    NSString *resultString = [[NSString alloc] initWithData:decryptedMessage
                                                   encoding:NSUTF8StringEncoding];
    XCTAssertTrue([message isEqualToString:resultString], @"initial string and decrypted string should be the same");
}

- (void)testSecureCellSealModeWithoutContext {
    TSCellSeal *cellSeal = [[TSCellSeal alloc] initWithKey:self.masterKeyData];
    NSString *message = @"All your base are belong to us!";
    NSString *context = @"For great justice";
    NSError *themisError;

    NSData *encryptedMessageNoContext = [cellSeal wrapData:[message dataUsingEncoding:NSUTF8StringEncoding]
                                                   context:nil
                                                     error:&themisError];
    XCTAssertNil(themisError, @"encryption with data and without context should be successful");
    XCTAssertNotNil(encryptedMessageNoContext, @"encryption with data and without context should return data");

    NSData *decryptedMessageNoContext = [cellSeal unwrapData:encryptedMessageNoContext
                                                     context:[context dataUsingEncoding:NSUTF8StringEncoding]
                                                       error:&themisError];
    XCTAssertNotNil(themisError, @"decrypt data with context that was encypted without context should populate error");
    XCTAssertNil(decryptedMessageNoContext, @"decrypt data with context that was encypted without context should return nil data");

    themisError = nil;
    decryptedMessageNoContext = [cellSeal unwrapData:encryptedMessageNoContext
                                             context:nil
                                               error:&themisError];
    XCTAssertNil(themisError, @"decryption without context should be successful");
    XCTAssertNotNil(decryptedMessageNoContext, @"decryption without context should be successful");

    NSString *resultString = [[NSString alloc] initWithData:decryptedMessageNoContext encoding:NSUTF8StringEncoding];
    XCTAssertTrue([message isEqualToString:resultString], @"initial string and decrypted string should be the same");
}

- (void)testSecureCellSealModeEncryptionDecryptionMixed {
    TSCellSeal *cellSeal = [[TSCellSeal alloc] initWithKey:self.masterKeyData];
    NSString *message = @"All your base are belong to us!";
    NSString *context = @"For great justice";
    NSError *themisError;

    NSData *encryptedMessage = [cellSeal wrapData:[message dataUsingEncoding:NSUTF8StringEncoding]
                                          context:[context dataUsingEncoding:NSUTF8StringEncoding]
                                            error:&themisError];

    NSData *encryptedMessageNoContext = [cellSeal wrapData:[message dataUsingEncoding:NSUTF8StringEncoding]
                                                   context:nil
                                                     error:&themisError];

    XCTAssertFalse([encryptedMessage isEqualToData:encryptedMessageNoContext],
            @"secure cell encrypter (seal mode) encryption result with and without context can`t be equal");
}

- (void)testSecureCellSealModeWrongContext {
    TSCellSeal *cellSeal = [[TSCellSeal alloc] initWithKey:self.masterKeyData];
    NSString *message = @"All your base are belong to us!";
    NSString *context = @"For great justice";
    NSString *wrongContext = @"Or not";
    NSError *themisError;

    NSData *encryptedMessage = [cellSeal wrapData:[message dataUsingEncoding:NSUTF8StringEncoding]
                                          context:[context dataUsingEncoding:NSUTF8StringEncoding]
                                            error:&themisError];

    themisError = nil;
    NSData *decryptedMessageWrongContext = [cellSeal unwrapData:encryptedMessage
                                                        context:[wrongContext dataUsingEncoding:NSUTF8StringEncoding]
                                                          error:&themisError];
    XCTAssertNotNil(themisError, @"decrypt data with wrong context should populate error");
    XCTAssertNil(decryptedMessageWrongContext, @"decrypt data with wrong context should return nil data");
}


#pragma MARK - Token Protect -

- (void)testSecureCellTokenProtectModeInit {
    TSCellToken *cellToken = [[TSCellToken alloc] initWithKey:[NSData new]];
    XCTAssertNil(cellToken, "secure cell encrypter (token protect mode) should not be created without key");

    cellToken = [[TSCellToken alloc] initWithKey:self.masterKeyData];
    XCTAssertNotNil(cellToken, @"secure cell encrypter (token protect mode) creation error");
}


- (void)testSecureCellTokenProtectModeWithContext {
    TSCellToken *cellToken = [[TSCellToken alloc] initWithKey:self.masterKeyData];
    NSString *message = @"Roses are grey. Violets are grey.";
    NSString *context = @"I'm a dog";
    NSError *themisError;

    TSCellTokenEncryptedData *encryptedMessage = [cellToken wrapData:nil
                                                             context:[context dataUsingEncoding:NSUTF8StringEncoding]
                                                               error:&themisError];
    XCTAssertNotNil(themisError, @"encryption without data-to-encrypt should populate error");
    XCTAssertNil(encryptedMessage, @"encryption without data-to-encrypt should return nil data");

    themisError = nil;
    encryptedMessage = [cellToken wrapData:nil
                                   context:nil
                                     error:&themisError];
    XCTAssertNotNil(themisError, @"encryption without data-to-encrypt, without context should populate error");
    XCTAssertNil(encryptedMessage, @"encryption without data-to-encrypt, without context should return nil data");

    themisError = nil;
    NSData * messageData = [message dataUsingEncoding:NSUTF8StringEncoding];
    encryptedMessage = [cellToken wrapData:messageData
                                   context:[context dataUsingEncoding:NSUTF8StringEncoding]
                                     error:&themisError];
    XCTAssertNil(themisError, @"encryption with data and context should be successful");
    XCTAssertNotNil(encryptedMessage, @"encryption with data and context should be successful");
    XCTAssertEqual([messageData length], [encryptedMessage.cipherText length], @"encrypted data length should be the same as message length");

    themisError = nil;
    NSData *decryptedMessage = [cellToken unwrapData:encryptedMessage
                                             context:nil error:&themisError];
    XCTAssertNotNil(themisError, @"decryption without context should populate error");
    XCTAssertNil(decryptedMessage, @"decryption without context should return nil data");

    themisError = nil;
    decryptedMessage = [cellToken unwrapData:encryptedMessage
                                     context:[context dataUsingEncoding:NSUTF8StringEncoding]
                                       error:&themisError];
    XCTAssertNil(themisError, @"decryption with context should be successful");
    XCTAssertNotNil(decryptedMessage, @"decryption with context should be successful");


    NSString *resultString = [[NSString alloc] initWithData:decryptedMessage
                                                   encoding:NSUTF8StringEncoding];
    XCTAssertTrue([message isEqualToString:resultString], @"initial string and decrypted string should be the same");
}


- (void)testSecureCellTokenProtectModeWithoutContext {
    TSCellToken *cellToken = [[TSCellToken alloc] initWithKey:self.masterKeyData];
    NSString *message = @"Roses are grey. Violets are grey.";
    NSString *context = @"I'm a dog";
    NSError *themisError;
    NSData *messageData = [message dataUsingEncoding:NSUTF8StringEncoding];

    TSCellTokenEncryptedData *encryptedMessageNoContext = [cellToken
            wrapData:[message dataUsingEncoding:NSUTF8StringEncoding]
             context:nil
               error:&themisError];
    XCTAssertNil(themisError, @"encryption without data-to-encrypt, without context should be successful");
    XCTAssertNotNil(encryptedMessageNoContext, @"encryption without data-to-encrypt, without context should be successful");
    XCTAssertEqual([messageData length], [encryptedMessageNoContext.cipherText length], @"encrypted data length should be the same as message length");

    themisError = nil;
    NSData *decryptedMessageNoContext = [cellToken unwrapData:encryptedMessageNoContext
                                                      context:[context dataUsingEncoding:NSUTF8StringEncoding]
                                                        error:&themisError];
    XCTAssertNotNil(themisError, @"decrypt data with context that was encypted without context should populate error");
    XCTAssertNil(decryptedMessageNoContext, @"decrypt data with context that was encypted without context should return nil data");

    themisError = nil;
    decryptedMessageNoContext = [cellToken unwrapData:encryptedMessageNoContext
                                              context:nil
                                                error:&themisError];
    XCTAssertNil(themisError, @"decryption without context should be successful");
    XCTAssertNotNil(decryptedMessageNoContext, @"decryption without context should be successful");

    NSString *resultString = [[NSString alloc] initWithData:decryptedMessageNoContext encoding:NSUTF8StringEncoding];
    XCTAssertTrue([message isEqualToString:resultString], @"initial string and decrypted string should be the same");
}

- (void)testSecureCellTokenProtectModeIncorrectContext {
    TSCellToken *cellToken = [[TSCellToken alloc] initWithKey:self.masterKeyData];
    NSString *message = @"Roses are grey. Violets are grey.";
    NSString *context = @"I'm a dog";
    NSString *wrongContext = @"I'm a cat";
    NSError *themisError;

    TSCellTokenEncryptedData *encryptedMessage = [cellToken
            wrapData:[message dataUsingEncoding:NSUTF8StringEncoding]
             context:[context dataUsingEncoding:NSUTF8StringEncoding]
               error:&themisError];

    themisError = nil;
    NSData *decryptedMessageWrongContext = [cellToken
            unwrapData:encryptedMessage
               context:[wrongContext dataUsingEncoding:NSUTF8StringEncoding]
                 error:&themisError];
    XCTAssertNotNil(themisError, @"decrypt data with wrong context should populate error");
    XCTAssertNil(decryptedMessageWrongContext, @"decrypt data with wrong context should return nil data");
}


#pragma MARK - Imprint -

- (void)testSecureCellTokenImprintInit {
    TSCellContextImprint *contextImprint = [[TSCellContextImprint alloc] initWithKey:nil];
    XCTAssertNil(contextImprint, "secure cell encrypter (token imprint mode) should not be created without key");

    contextImprint = [[TSCellContextImprint alloc] initWithKey:self.masterKeyData];
    XCTAssertNotNil(contextImprint, @"secure cell encrypter (token imprint mode) creation error");
}

- (void)testSecureCellTokenImprintContext {
    TSCellContextImprint *contextImprint = [[TSCellContextImprint alloc] initWithKey:self.masterKeyData];
    NSString *message = @"Roses are red. My name is Dave. This poem have no sense";
    NSString *context = @"Microwave";
    NSError *themisError;

    NSData *encryptedMessage = [contextImprint wrapData:nil
                                                context:[context dataUsingEncoding:NSUTF8StringEncoding]
                                                  error:&themisError];
    XCTAssertNotNil(themisError, @"encryption without data-to-encrypt should populate error");
    XCTAssertNil(encryptedMessage, @"encryption without data-to-encrypt should return nil data");

    themisError = nil;
    encryptedMessage = [contextImprint wrapData:[message dataUsingEncoding:NSUTF8StringEncoding]
                                        context:[context dataUsingEncoding:NSUTF8StringEncoding]
                                          error:&themisError];
    XCTAssertNil(themisError, @"encryption with data and context should be successful");
    XCTAssertNotNil(encryptedMessage, @"encryption with data and context should be successful");

    themisError = nil;
    NSData *decryptedMessage = [contextImprint unwrapData:encryptedMessage
                                                  context:nil
                                                    error:&themisError];
    XCTAssertNotNil(themisError, @"decryption without context should populate error");
    XCTAssertNil(decryptedMessage, @"decryption without context should return nil data");

    themisError = nil;
    decryptedMessage = [contextImprint unwrapData:encryptedMessage
                                          context:[context dataUsingEncoding:NSUTF8StringEncoding]
                                            error:&themisError];
    XCTAssertNil(themisError, @"decryption with context should be successful");
    XCTAssertNotNil(decryptedMessage, @"decryption with context should be successful");


    NSString *resultString = [[NSString alloc] initWithData:decryptedMessage
                                                   encoding:NSUTF8StringEncoding];
    XCTAssertTrue([message isEqualToString:resultString], @"initial string and decrypted string should be the same");
}


- (void)testSecureCellTokenImprintWithoutContext {
    TSCellContextImprint *contextImprint = [[TSCellContextImprint alloc] initWithKey:self.masterKeyData];
    NSString *message = @"Roses are red. My name is Dave. This poem have no sense";
    NSString *context = @"Microwave";
    NSError *themisError;

    NSData *encryptedMessageNoContext = [contextImprint
                                         wrapData:[message dataUsingEncoding:NSUTF8StringEncoding]
                                         context:nil
                                         error:&themisError];
    XCTAssertNotNil(themisError, @"encryption without data-to-encrypt, without context should populate error");
    XCTAssertNil(encryptedMessageNoContext, @"encryption without data-to-encrypt, without context should return nil");

    encryptedMessageNoContext = [contextImprint
                                     wrapData:[message dataUsingEncoding:NSUTF8StringEncoding]
                                     context:[NSData new]
                                     error:&themisError];
    XCTAssertNotNil(themisError, @"encryption without data-to-encrypt, with empty context should populate error");
    XCTAssertNil(encryptedMessageNoContext, @"encryption without data-to-encrypt, with empty context should return nil");
    
    themisError = nil;
    encryptedMessageNoContext = [contextImprint wrapData:nil
                                                 context:nil
                                                   error:&themisError];
    XCTAssertNotNil(themisError, @"encryption without data-to-encrypt, without context should populate error");
    XCTAssertNil(encryptedMessageNoContext, @"encryption without data-to-encrypt, without context should return nil data");

    encryptedMessageNoContext = [contextImprint wrapData:[message dataUsingEncoding:NSUTF8StringEncoding]
                                                 context:[context dataUsingEncoding:NSUTF8StringEncoding]
                                                   error:&themisError];

    themisError = nil;
    NSData *decryptedMessageNoContext = [contextImprint unwrapData:encryptedMessageNoContext
                                                           context:nil
                                                             error:&themisError];
    XCTAssertNotNil(themisError, @"decryption without context should be successful");
    XCTAssertNil(decryptedMessageNoContext, @"decryption without context should be successful");

    themisError = nil;
    decryptedMessageNoContext = [contextImprint unwrapData:encryptedMessageNoContext
                                                   context:[context dataUsingEncoding:NSUTF8StringEncoding]
                                                     error:&themisError];
    XCTAssertNil(themisError, @"decrypt data with context that was encypted without context should populate error");
    XCTAssertNotNil(decryptedMessageNoContext, @"decrypt data with context that was encypted without context should return nil data");

    NSString *resultString = [[NSString alloc] initWithData:decryptedMessageNoContext encoding:NSUTF8StringEncoding];
    XCTAssertTrue([message isEqualToString:resultString], @"initial string and decrypted string should be the same");
}


- (void)testSecureCellTokenImprintIncorrectContext {
    TSCellContextImprint *contextImprint = [[TSCellContextImprint alloc] initWithKey:self.masterKeyData];
    NSString *message = @"Roses are red. My name is Dave. This poem have no sense";
    NSString *context = @"Microwave";
    NSString *wrongContext = @"Oven";
    NSError *themisError;

    NSData *encryptedMessage = [contextImprint
            wrapData:[message dataUsingEncoding:NSUTF8StringEncoding]
             context:[context dataUsingEncoding:NSUTF8StringEncoding]
               error:&themisError];

    themisError = nil;
    NSData *decryptedMessageWrongContext = [contextImprint
            unwrapData:encryptedMessage
               context:[wrongContext dataUsingEncoding:NSUTF8StringEncoding]
                 error:&themisError];
    XCTAssertNil(themisError, @"decrypt data with wrong context should ignore context");
    XCTAssertNotNil(decryptedMessageWrongContext, @"decrypt data with wrong context should ignore context");
    
    themisError = nil;
    NSData *decryptedMessageEmptyContext = [contextImprint
                                            unwrapData:encryptedMessage
                                            context:[NSData new]
                                            error:&themisError];
    XCTAssertNotNil(themisError, @"decrypt data with empty context should populate error");
    XCTAssertNil(decryptedMessageEmptyContext, @"decrypt data with empty context should return nil");
}

@end
