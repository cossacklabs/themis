//
//  objthemis.m
//  objthemis
//
//  Created by Admin on 21.07.15.
//
//

#import <UIKit/UIKit.h>
#import <XCTest/XCTest.h>
#import <objcthemis/objcthemis.h>

@interface objthemis : XCTestCase

@property (nonatomic, strong) NSData * masterKeyData;

@end


@implementation objthemis

- (void)setUp {
    [super setUp];
    
    self.masterKeyData = [self generateMasterKey];
}

- (NSData *)generateMasterKey {
    NSString * masterKeyString = @"UkVDMgAAAC13PCVZAKOczZXUpvkhsC+xvwWnv3CLmlG0Wzy8ZBMnT+2yx/dg";
    NSData * masterKeyData = [[NSData alloc] initWithBase64EncodedString:masterKeyString
                                                                 options:NSDataBase64DecodingIgnoreUnknownCharacters];
    return masterKeyData;
}


#pragma MARK - Seal Mode -

- (void)testSecureCellSealModeInit{
    TSCellSeal * cellSeal = [[TSCellSeal alloc] initWithKey:nil];
    XCTAssertNil(cellSeal,"secure cell encrypter (seal mode) should not be created without key");
    
    cellSeal = [[TSCellSeal alloc] initWithKey:self.masterKeyData];
    XCTAssertNotNil(cellSeal,@"secure cell encrypter (seal mode) creation error");
}


- (void)testSecureCellSealModeWithContext {
    TSCellSeal * cellSeal = [[TSCellSeal alloc] initWithKey:self.masterKeyData];
    NSString * message = @"All your base are belong to us!";
    NSString * context = @"For great justice";
    NSError * themisError;
    
    NSData * encryptedMessage = [cellSeal wrapData:nil
                                           context:[context dataUsingEncoding:NSUTF8StringEncoding]
                                             error:&themisError];
    XCTAssertNotNil(themisError, @"encryption without data-to-encrypt should populate error");
    XCTAssertNil(encryptedMessage, @"encryption without data-to-encrypt should return nil data");
    
    themisError = nil;
    encryptedMessage = [cellSeal wrapData:[message dataUsingEncoding:NSUTF8StringEncoding]
                                  context:[context dataUsingEncoding:NSUTF8StringEncoding]
                                    error:&themisError];
    XCTAssertNil(themisError, @"encryption with data and context should be successful");
    XCTAssertNotNil(encryptedMessage, @"encryption with data and context should be successful");
    
    NSData * decryptedMessage = [cellSeal unwrapData:encryptedMessage
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
    
    NSString * resultString = [[NSString alloc] initWithData:decryptedMessage
                                                    encoding:NSUTF8StringEncoding];
    XCTAssertTrue([message isEqualToString:resultString], @"initial string and decrypted string should be the same");
}
    
- (void)testSecureCellSealModeWithoutContext {
    TSCellSeal * cellSeal = [[TSCellSeal alloc] initWithKey:self.masterKeyData];
    NSString * message = @"All your base are belong to us!";
    NSString * context = @"For great justice";
    NSError * themisError;
    
    NSData * encryptedMessageNoContext = [cellSeal wrapData:[message dataUsingEncoding:NSUTF8StringEncoding]
                                                    context:nil
                                                      error:&themisError];
    XCTAssertNil(themisError, @"encryption with data and without context should be successful");
    XCTAssertNotNil(encryptedMessageNoContext, @"encryption without data-to-encrypt should return data");

    NSData * decryptedMessageNoContext = [cellSeal unwrapData:encryptedMessageNoContext
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
    
    NSString * resultString = [[NSString alloc] initWithData:decryptedMessageNoContext encoding:NSUTF8StringEncoding];
    XCTAssertTrue([message isEqualToString:resultString], @"initial string and decrypted string should be the same");
}

- (void)testSecureCellSealModeEncryptionDecryptionMixed {
    TSCellSeal * cellSeal = [[TSCellSeal alloc] initWithKey:self.masterKeyData];
    NSString * message = @"All your base are belong to us!";
    NSString * context = @"For great justice";
    NSError * themisError;
    
    NSData * encryptedMessage = [cellSeal wrapData:[message dataUsingEncoding:NSUTF8StringEncoding]
                                           context:[context dataUsingEncoding:NSUTF8StringEncoding]
                                             error:&themisError];
    
    NSData * encryptedMessageNoContext = [cellSeal wrapData:[message dataUsingEncoding:NSUTF8StringEncoding]
                                                    context:nil
                                                      error:&themisError];
    
    XCTAssertFalse([encryptedMessage isEqualToData:encryptedMessageNoContext],
                   @"secure cell encrypter (seal mode) encryption result with and without context can`t be equal");
}

- (void)testSecureCellSealModeWrongContext {
    TSCellSeal * cellSeal = [[TSCellSeal alloc] initWithKey:self.masterKeyData];
    NSString * message = @"All your base are belong to us!";
    NSString * context = @"For great justice";
    NSString * wrongContext = @"Or not";
    NSError * themisError;
    
    NSData * encryptedMessage = [cellSeal wrapData:[message dataUsingEncoding:NSUTF8StringEncoding]
                                           context:[context dataUsingEncoding:NSUTF8StringEncoding]
                                             error:&themisError];
    
    themisError = nil;
    NSData * decryptedMessageWrongContext = [cellSeal unwrapData:encryptedMessage
                                                    context:[wrongContext dataUsingEncoding:NSUTF8StringEncoding]
                                                      error:&themisError];
    XCTAssertNotNil(themisError, @"decrypt data with wrong context should populate error");
    XCTAssertNil(decryptedMessageWrongContext, @"decrypt data with wrong context should return nil data");
}


#pragma MARK - Token Protect -

- (void)testSecureCellTokenProtectModeInit {
    TSCellToken * cellToken = [[TSCellToken alloc] initWithKey:nil];
    XCTAssertNil(cellToken,"secure cell encrypter (token protect mode) should not be created without key");
    
    cellToken = [[TSCellToken alloc] initWithKey:self.masterKeyData];
    XCTAssertNotNil(cellToken, @"secure cell encrypter (token protect mode) creation error");
}


- (void)testSecureCellTokenProtectModeWithContext {
    TSCellToken * cellToken = [[TSCellToken alloc] initWithKey:self.masterKeyData];
    NSString * message = @"Roses are grey. Violets are grey.";
    NSString * context = @"I'm a dog";
    NSError * themisError;

    TSCellTokenEncryptedData * encryptedMessage = [cellToken wrapData:nil
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
    encryptedMessage = [cellToken wrapData:[message dataUsingEncoding:NSUTF8StringEncoding]
                                   context:[context dataUsingEncoding:NSUTF8StringEncoding]
                                     error:&themisError];
    XCTAssertNil(themisError, @"encryption with data and context should be successful");
    XCTAssertNotNil(encryptedMessage, @"encryption with data and context should be successful");
    
    themisError = nil;
    NSData * decryptedMessage = [cellToken unwrapData:encryptedMessage
                                              context:nil error:&themisError];
    XCTAssertNotNil(themisError, @"decryption without context should populate error");
    XCTAssertNil(decryptedMessage, @"decryption without context should return nil data");
    
    themisError = nil;
    decryptedMessage = [cellToken unwrapData:encryptedMessage
                                              context:[context dataUsingEncoding:NSUTF8StringEncoding]
                                                error:&themisError];
    XCTAssertNil(themisError, @"decryption with context should be successful");
    XCTAssertNotNil(decryptedMessage, @"decryption with context should be successful");
    
    
    NSString * resultString = [[NSString alloc] initWithData:decryptedMessage
                                                    encoding:NSUTF8StringEncoding];
    XCTAssertTrue([message isEqualToString:resultString], @"initial string and decrypted string should be the same");
}


- (void)testSecureCellTokenProtectModeWithoutContext {
    TSCellToken * cellToken = [[TSCellToken alloc] initWithKey:self.masterKeyData];
    NSString * message = @"Roses are grey. Violets are grey.";
    NSString * context = @"I'm a dog";
    NSError * themisError;
    
    
    TSCellTokenEncryptedData * encryptedMessageNoContext = [cellToken
                                                            wrapData:[message dataUsingEncoding:NSUTF8StringEncoding]
                                                            context:nil
                                                            error:&themisError];
    XCTAssertNil(themisError, @"encryption without data-to-encrypt, without context should be successful");
    XCTAssertNotNil(encryptedMessageNoContext, @"encryption without data-to-encrypt, without context should be successful");

    themisError = nil;
    NSData * decryptedMessageNoContext = [cellToken unwrapData:encryptedMessageNoContext
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
    
    NSString * resultString = [[NSString alloc] initWithData:decryptedMessageNoContext encoding:NSUTF8StringEncoding];
    XCTAssertTrue([message isEqualToString:resultString], @"initial string and decrypted string should be the same");
}

- (void)testSecureCellTokenProtectModeIncorrectContext {
    TSCellToken * cellToken = [[TSCellToken alloc] initWithKey:self.masterKeyData];
    NSString * message = @"Roses are grey. Violets are grey.";
    NSString * context = @"I'm a dog";
    NSString * wrongContext = @"I'm a cat";
    NSError * themisError;
    
    TSCellTokenEncryptedData * encryptedMessage = [cellToken
                                                   wrapData:[message dataUsingEncoding:NSUTF8StringEncoding]
                                                   context:[context dataUsingEncoding:NSUTF8StringEncoding]
                                                   error:&themisError];

    themisError = nil;
    NSData * decryptedMessageWrongContext = [cellToken
                                             unwrapData:encryptedMessage
                                             context:[wrongContext dataUsingEncoding:NSUTF8StringEncoding]
                                             error:&themisError];
    XCTAssertNotNil(themisError, @"decrypt data with wrong context should populate error");
    XCTAssertNil(decryptedMessageWrongContext, @"decrypt data with wrong context should return nil data");
}


#pragma MARK - Imprint -

- (void)testSecureCellTokenImprintInit {
    TSCellContextImprint * contextImprint = [[TSCellContextImprint alloc] initWithKey:nil];
    XCTAssertNil(contextImprint,"secure cell encrypter (token imprint mode) should not be created without key");
    
    contextImprint = [[TSCellContextImprint alloc] initWithKey:self.masterKeyData];
    XCTAssertNotNil(contextImprint, @"secure cell encrypter (token imprint mode) creation error");
}

- (void)testSecureCellTokenImprintContext {
    TSCellContextImprint * contextImprint = [[TSCellContextImprint alloc] initWithKey:self.masterKeyData];
    NSString * message = @"Roses are red. My name is Dave. This poem have no sense";
    NSString * context = @"Microwave";
    NSError * themisError;
    
    NSData * encryptedMessage = [contextImprint wrapData:nil
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
    NSData * decryptedMessage = [contextImprint unwrapData:encryptedMessage
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
    
    
    NSString * resultString = [[NSString alloc] initWithData:decryptedMessage
                                                    encoding:NSUTF8StringEncoding];
    XCTAssertTrue([message isEqualToString:resultString], @"initial string and decrypted string should be the same");
}


- (void)testSecureCellTokenImprintWithoutContext {
    TSCellContextImprint * contextImprint = [[TSCellContextImprint alloc] initWithKey:self.masterKeyData];
    NSString * message = @"Roses are red. My name is Dave. This poem have no sense";
    NSString * context = @"Microwave";
    NSError * themisError;
    
    NSData * encryptedMessageNoContext = [contextImprint
                                          wrapData:[message dataUsingEncoding:NSUTF8StringEncoding]
                                          context:nil
                                          error:&themisError];
    XCTAssertNotNil(themisError, @"encryption without data-to-encrypt, without context should populate error");
    XCTAssertNil(encryptedMessageNoContext, @"encryption without data-to-encrypt, without context should return nil");
    
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
    NSData * decryptedMessageNoContext = [contextImprint unwrapData:encryptedMessageNoContext
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

    NSString * resultString = [[NSString alloc] initWithData:decryptedMessageNoContext encoding:NSUTF8StringEncoding];
    XCTAssertTrue([message isEqualToString:resultString], @"initial string and decrypted string should be the same");
}


- (void)testSecureCellTokenImprintIncorrectContext {
    TSCellContextImprint * contextImprint = [[TSCellContextImprint alloc] initWithKey:self.masterKeyData];
    NSString * message = @"Roses are red. My name is Dave. This poem have no sense";
    NSString * context = @"Microwave";
    NSString * wrongContext = @"Oven";
    NSError * themisError;
    
    NSData * encryptedMessage = [contextImprint
                                 wrapData:[message dataUsingEncoding:NSUTF8StringEncoding]
                                 context:[context dataUsingEncoding:NSUTF8StringEncoding]
                                 error:&themisError];
    
    themisError = nil;
    NSData * decryptedMessageWrongContext = [contextImprint
                                             unwrapData:encryptedMessage
                                             context:[wrongContext dataUsingEncoding:NSUTF8StringEncoding]
                                             error:&themisError];
    XCTAssertNil(themisError, @"decrypt data with wrong context should ignore context");
    XCTAssertNotNil(decryptedMessageWrongContext, @"decrypt data with wrong context should ignore context");
}


#pragma MARK -

- (void)testGeneratingKeys {
    // Generating RSA keys
    TSKeyGen * keygenRSA = [[TSKeyGen alloc] initWithAlgorithm:TSKeyGenAsymmetricAlgorithmRSA];
    XCTAssertNotNil(keygenRSA);
    
//    NSLog(@"client private key %@", [keygenRSA.privateKey base64EncodedStringWithOptions:NSDataBase64Encoding64CharacterLineLength]);
//    NSLog(@"client public key %@", [keygenRSA.publicKey base64EncodedStringWithOptions:NSDataBase64Encoding64CharacterLineLength]);
    
    TSKeyGen * keygenEC = [[TSKeyGen alloc] initWithAlgorithm:TSKeyGenAsymmetricAlgorithmEC];
    XCTAssertNotNil(keygenEC);
    
    TSKeyGen * keygenInvalid = [[TSKeyGen alloc] initWithAlgorithm:200];
    XCTAssertNil(keygenInvalid);
}

- (void)testSecureSessionEC256 {
    NSString * private = @"UkVDMgAAAC1JhwRrAPIGB33HHFmhjzn8lIE/nsW6cG+TCI3jhYJb+D/Gnwvf";
    NSString * public = @"VUVDMgAAAC11WDPUAhLfH+nqSBHh+XGOJBHL/cCjbtasiLZEwpokhO5QTD6g";

    [self wrapUnwrapWithPrivateKey:private publicKey:public];
    [self signVerifyWithPrivateKey:private publicKey:public];
}

- (void)testSecureSessionRSA1024 {
    NSString * private =
    @"UlJBMQAAAlChmrBzbrVeMLFsu3cRhwcFc/9zPjHSifVY2XzpQA8nUA7XXbtpNAvM\r\n"
    "vMEpETeQvy2nSI/tmaOaTGYWVlP3fouNO8WjrnDX0WN9nK1HCjR0+4VAwOGbfo/p\r\n"
    "yV2JGfz7VGGyz29qCApe3uW/fMz+M5eZqaWL1DqRMsHahijexD9mEw84F4HacN1y\r\n"
    "cJmIXThZyOT/wpfH2elPrFQjieR9Yaej808/NBrHYwRvIM2ZoH4m0Z8BYoIWl5VY\r\n"
    "6nJzlolCS7GaEENh1sx9eW8TXGW2DW3gV4lGvMClH9sF0D55BEkPySd2W4uAHod2\r\n"
    "+t722PdiqcsijYuJubV5MBW3j+ww1+0ko7tM24y2keBLaouuSUOFJwceajnCRQoe\r\n"
    "36B5Dmg6RQxXCuCTBwET5uJTRj5FZZ5KAFybAWPeR8WcWRfejBLZwdGV0mFEXnph\r\n"
    "uJU1B7rWs26A7lTxp2/nn9NN4VSrpIW9xS3LHa/+B6g7WdTAYp2o5dttqkiAL98P\r\n"
    "CxqOWOODFxxecTOZF7d9DYPfW96yGjOx9InO18vH1zVv0D44pTbl/QiEvI8x+LXg\r\n"
    "sn9kIcFlwouLv7I0Gmft6cuBMH3HK9Z4z7smC7dI1h7bkxOlL1ANHjhYg7QIioUx\r\n"
    "NKVvfqRJ38ohpnMafp544o2cuz76pShHVVAsA9ar0CULirFJVXhlKtHU7T12j3X5\r\n"
    "fJPBg8w3OVuEg9JtWeAOtUMOETtItvKk4hPAmjkBGIw7ae6debuuNBPTVWzR+owa\r\n"
    "mv7Zd/kkpheaxW/7AAEAAQ==";
    
    NSString * public =
    @"VVJBMQAAAJCscXUnt0jWHtuTE6UvUA0eOFiDtAiKhTE0pW9+pEnfyiGmcxp+nnji\r\n"
    "jZy7PvqlKEdVUCwD1qvQJQuKsUlVeGUq0dTtPXaPdfl8k8GDzDc5W4SD0m1Z4A61\r\n"
    "Qw4RO0i28qTiE8CaOQEYjDtp7p15u640E9NVbNH6jBqa/tl3+SSmF5rFb/sAAQAB";
    
    [self wrapUnwrapWithPrivateKey:private publicKey:public];
    [self signVerifyWithPrivateKey:private publicKey:public];
}

- (void)testSecureSessionRSA2048 {
    NSString * private =
    @"UlJBMgAABJB1cSeRoRLyt+DO4s6ZopWCqlQC2w8D8AkEx6HO1AKxHqfk3yfUik3N\r"
    "gd3BNr8NkukviE1dOmU18dJYkh0RevS2FK8cQs9A0Xk0w1qti42WJs6vylHU3DYn\r"
    "gLsS56hfwKi9/5Oiwu1Gz0viTn6N22ClKICh0nhVfsLHqRlaE2xAX16O4gB5GBtn\r"
    "rsUVySaNkHtVILPowUrv91Sg/73Tdph/lzhZjgGug/tgie2jQNpfuzqPryfwLnhJ\r"
    "d2q32uuD91wAu9I+Ar4SVj5DdeDqDKgmzMP1p87J6zUN+g2BOQ3giph220YI5jtK\r"
    "jI2SKx/RIB7gcnVKOSTLIaRVLE2umuet5JTz+fo5Ct2URA4xD3JhzQHl3sWFlM4O\r"
    "C3TWTa/GBdPg4MVXnUxyFSaDdSRl7t0t/J+7V1Rw5coYDQOzK/KdaaXM16Ci9qhC\r"
    "IsBlf7F+gAYeDllWkbsAq1vmuDdy1/G7/ACVXYecuzDPTFnBA0i4hWCrlKoSRdAy\r"
    "AjNA/3hsBmbys/RXzwMfOgFIjWR07nUIzLCaG+ANBH40JuaYoFnCwORp3Ppx2N1h\r"
    "0pBswH9Gsn67XFMsLSuMycr4nA9J7Mxr2UsHVD+2oi6FmX4ICRP5PQsfuBYc9ANA\r"
    "3RJuhjgmZWAoiqL3Kog1PduOS6CUIMalXo9EBPEwIonUjvhVVCl3a6/4vV2ogle9\r"
    "5wZ55DI53oE4QPzoWlBe//hZI6VHtXzhNnVIkPteqwlxWWA/NeJJ05loXLqwJCOs\r"
    "GMxS7IuxJK8sOCoShQzFPVy34uK+vqkq9XzGT1Vu8UF1b4gp105z/hv0rKJDHJq3\r"
    "gTMy0pgKljFEa+h/LDUVw0fWCnTBPUaGd/YWm5w/pQApG9Ps8P1TRNMKsSc9E1ZC\r"
    "06ZRAv51AbSKMwVh2zBu2HkQHaWl63yqf0yOUpe9eUSOeNuCBrA/E2Ata1jBxl3f\r"
    "c2OJZxNDCipzDPvrZOsg0YKLePTByOlXjQInQxvNNzJ7V4npqCSdCxC2XgbE7SoE\r"
    "knVfJzihG8wiRELZp1TPB7GhYRB+DxPzns5aWQF4eX9w6QVQigQjysp+v3+lVGnU\r"
    "oWymMmI6y/WBXHBMIumc6AF2h0XSeW9CSWYxfM0LAJ5RnmLfgbrbmGbBHCq0oFd5\r"
    "W9K2VUJP0hh4OcXJP3YCjqxHtwQ74/9cu83WQNHvSRkE5oEbGuNSR4V7UhfKVyz5\r"
    "5foytzKpACsF9JeesdofooRX5+O5rmIsxB5H7evc0aSpG/NInn7ptiSVaTKVn+Ty\r"
    "vOtMxG7CdAPVkhRQeLB1B9jsgwUM5WXlAZF4oB6H/HIrUkDVhuJNqOAmjpPl4CmY\r"
    "Ewv6zYipsbynKKfVJL924b2v3+eRNnOJdnMrLhObgjwGI0r1knb2+uRIEv119f4S\r"
    "35EA2aEqErxZQpIjosEFTPte5ov6pVRZCsTn/MmzdB+ApgYCN9fwhP+hM4voAtvA\r"
    "N3/2Ym5IJ3YC1gAXktrkppMH5ig3EAhgH5/XtnAlsh91c9MaQ5uRHdgysdf5UDIO\r"
    "F2CVd9lXCE40C/6bAAEAAQ==";
    
    NSString * public =
    @"VVJBMgAAARDJNF2Yylcs+eX6MrcyqQArBfSXnrHaH6KEV+fjua5iLMQeR+3r3NGk\r"
    "qRvzSJ5+6bYklWkylZ/k8rzrTMRuwnQD1ZIUUHiwdQfY7IMFDOVl5QGReKAeh/xy\r"
    "K1JA1YbiTajgJo6T5eApmBML+s2IqbG8pyin1SS/duG9r9/nkTZziXZzKy4Tm4I8\r"
    "BiNK9ZJ29vrkSBL9dfX+Et+RANmhKhK8WUKSI6LBBUz7XuaL+qVUWQrE5/zJs3Qf\r"
    "gKYGAjfX8IT/oTOL6ALbwDd/9mJuSCd2AtYAF5La5KaTB+YoNxAIYB+f17ZwJbIf\r"
    "dXPTGkObkR3YMrHX+VAyDhdglXfZVwhONAv+mwABAAE=";
    
    [self wrapUnwrapWithPrivateKey:private publicKey:public];
    [self signVerifyWithPrivateKey:private publicKey:public];
}

- (void)testSecureSessionRSA4096 {
    NSString * private =
    @"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\r\n"
    "uIrExiW+wsjlME6ZAAEAAQ==";
    
    NSString * public =
    @"VVJBNAAAAhCO+6CWl9IxU37wILsrxMTXIx1bDQRMNwxfi7zBSHjiJWCmjBnPi8uH\r\n"
    "OttoG2eNuwOPJItDy+kyk81F+xsYBWqe/h6EOY3Smo3Z8ePmeANu5U1yR6TSzFKT\r\n"
    "tn6TeE9YWZZbHmEF3f158IEyKF9JotDu95WYB+q9PcpW8BvpSzGnOn1LFxwUsJGj\r\n"
    "MDeV698p9W9SNoWSDjzdBPQ2o3cTaCykSy7UIyzeH4PqtdbfFt3NHvfQHQwlSeb2\r\n"
    "6S5qL2KDDPms1E6vlzm1fIVWOZDB2Ehr5DIxxP6Jc3gIxpyMabca0OMUMPqKo8fm\r\n"
    "krAmlj5wtXT7+1gevZE2DsexwKKyMsqCnASnmF04ibAMUCkHbnzS9zumodIuMbR7\r\n"
    "DYup7uV+DO0D3PbzUHpePXIP1XB4d9nrGqXuXXilhZqBDroMohK+Th86wG9OmN0a\r\n"
    "nKcXloMrz54Rh6XquKLqGx6nNjfneRlIhhwPIl2vHLf78g9LOqr8WyTMhHWaHzuN\r\n"
    "XpDtdOTHlwN80w2dx6LrViMHjkW0J+IZOosGmw8Qmq6cl9LJ5k+941jhv5ObabhQ\r\n"
    "lBl7jyfGXwTv2W4VX05OWnBkcUdNicjKoBTXeebCPqvu+wjzcDd8uS+gbCEVqh7W\r\n"
    "Sub5ZIxvGGWTdg5atMVjrpOiRdP+cxii08KeBRPmv0y4isTGJb7CyOUwTpkAAQAB";
    
    [self wrapUnwrapWithPrivateKey:private publicKey:public];
    [self signVerifyWithPrivateKey:private publicKey:public];
}


- (void)wrapUnwrapWithPrivateKey:(NSString *)privateKey publicKey:(NSString *)publicKey {
    NSData * publicKeyData = [[NSData alloc] initWithBase64EncodedString:publicKey
                                                                 options:NSDataBase64DecodingIgnoreUnknownCharacters];
    NSData * privateKeyData = [[NSData alloc] initWithBase64EncodedString:privateKey
                                                                  options:NSDataBase64DecodingIgnoreUnknownCharacters];

    TSMessage * encrypter = [[TSMessage alloc] initInEncryptModeWithPrivateKey:privateKeyData peerPublicKey:publicKeyData];
    
    NSString * message = @"- Knock, knock.\n- Who’s there?\n*very long pause...*\n- Java.";
    
    
    NSError * themisError = nil;
    NSData * encryptedMessage = [encrypter wrapData:[message dataUsingEncoding:NSUTF8StringEncoding] error:&themisError];
    XCTAssertNil(themisError, @"wrapping data should be successful");
    XCTAssertNotNil(encryptedMessage, @"wrapping data should return encrypted data");
    
    NSData * decryptedMessage = [encrypter unwrapData:encryptedMessage error:&themisError];
    XCTAssertNil(themisError, @"unwrapping data should be successful");
    XCTAssertNotNil(decryptedMessage, @"unwrapping data should return decrypted data");
    
    NSString * resultString = [[NSString alloc] initWithData:decryptedMessage encoding:NSUTF8StringEncoding];
    XCTAssertTrue([message isEqualToString:resultString], @"initial string and decrypted string should be the same");
}

- (void)signVerifyWithPrivateKey:(NSString *)privateKey publicKey:(NSString *)publicKey {
    NSData * publicKeyData = [[NSData alloc] initWithBase64EncodedString:publicKey
                                                                 options:NSDataBase64DecodingIgnoreUnknownCharacters];
    NSData * privateKeyData = [[NSData alloc] initWithBase64EncodedString:privateKey
                                                                  options:NSDataBase64DecodingIgnoreUnknownCharacters];
    
    TSMessage * encrypter = [[TSMessage alloc] initInSignVerifyModeWithPrivateKey:privateKeyData peerPublicKey:publicKeyData];
    
    NSString * message = @"- Knock, knock.\n- Who’s there?\n*very long pause...*\n- Java.";
    
    
    NSError * themisError = nil;
    NSData * encryptedMessage = [encrypter wrapData:[message dataUsingEncoding:NSUTF8StringEncoding] error:&themisError];
    XCTAssertNil(themisError, @"wrapping data should be successful");
    XCTAssertNotNil(encryptedMessage, @"wrapping data should return encrypted data");
    
    NSData * decryptedMessage = [encrypter unwrapData:encryptedMessage error:&themisError];
    XCTAssertNil(themisError, @"unwrapping data should be successful");
    XCTAssertNotNil(decryptedMessage, @"unwrapping data should return decrypted data");
    
    NSString * resultString = [[NSString alloc] initWithData:decryptedMessage encoding:NSUTF8StringEncoding];
    XCTAssertTrue([message isEqualToString:resultString], @"initial string and decrypted string should be the same");
}

@end
