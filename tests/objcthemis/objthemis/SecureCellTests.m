//
//  SecureCellTests.m
//  objthemis
//
//  Created by Anastasiia on 9/19/17.
//

#import <XCTest/XCTest.h>

// TODO: use a unified import here
// CocoaPods tests do not work with canonical import of Themis for some reason.
// Please fix this if you have any idea how.
#if COCOAPODS
#import <objcthemis/objcthemis.h>
#else
@import themis;
#endif

@interface SecureCellKeygen : XCTestCase
@end

@interface SecureCellSeal : XCTestCase
@end

@interface SecureCellSealPassphrase : XCTestCase
@end

@interface SecureCellTokenProtect : XCTestCase
@end

@interface SecureCellContextImprint : XCTestCase
@end

#pragma mark - Key generation

@implementation SecureCellKeygen

static const size_t defaultLength = 32;

- (void)testKeyGeneration
{
    NSData *masterKey = TSGenerateSymmetricKey();

    XCTAssertNotNil(masterKey, "TSGenerateSymmetricKey() should not fail");
    XCTAssertEqual(masterKey.length, defaultLength, "generated key must be not empty");
}

@end

#pragma mark - Seal Mode

@implementation SecureCellSeal

- (void)testInitWithGenerated
{
    TSCellSeal *cell = [[TSCellSeal alloc] initWithKey:TSGenerateSymmetricKey()];
    XCTAssertNotNil(cell);
}

- (void)testInitWithFixed
{
    NSString *masterKeyString = @"UkVDMgAAAC13PCVZAKOczZXUpvkhsC+xvwWnv3CLmlG0Wzy8ZBMnT+2yx/dg";
    NSData *masterKeyData = [[NSData alloc] initWithBase64EncodedString:masterKeyString
                                                                options:NSDataBase64DecodingIgnoreUnknownCharacters];

    TSCellSeal *cell = [[TSCellSeal alloc] initWithKey:masterKeyData];
    XCTAssertNotNil(cell);
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnonnull"
- (void)testInitWithEmpty
{
    XCTAssertNil([[TSCellSeal alloc] initWithKey:nil]);
    XCTAssertNil([[TSCellSeal alloc] initWithKey:[NSData new]]);
}
#pragma clang diagnostic pop

- (void)testRoundtrip
{
    TSCellSeal *cell = [[TSCellSeal alloc] initWithKey:TSGenerateSymmetricKey()];
    NSString *message = @"All your base are belong to us!";
    NSString *context = @"For great justice";
    NSError *error;

    NSData *encrypted = [cell encrypt:[message dataUsingEncoding:NSUTF8StringEncoding]
                              context:[context dataUsingEncoding:NSUTF8StringEncoding]
                                error:&error];
    XCTAssertNotNil(encrypted);
    XCTAssertNil(error);

    NSData *decrypted = [cell decrypt:encrypted
                              context:[context dataUsingEncoding:NSUTF8StringEncoding]
                                error:&error];
    XCTAssertNotNil(decrypted);
    XCTAssertNil(error);

    NSString *decryptedMessage = [[NSString alloc] initWithData:decrypted
                                                       encoding:NSUTF8StringEncoding];

    XCTAssert([decryptedMessage isEqualToString:message]);
}

- (void)testDataLengthExtension
{
    TSCellSeal *cell = [[TSCellSeal alloc] initWithKey:TSGenerateSymmetricKey()];
    NSString *message = @"All your base are belong to us!";
    NSData *data = [message dataUsingEncoding:NSUTF8StringEncoding];

    NSData *encrypted = [cell encrypt:data];

    XCTAssertGreaterThan(encrypted.length, data.length);
}

- (void)testContextInclusion
{
    TSCellSeal *cell = [[TSCellSeal alloc] initWithKey:TSGenerateSymmetricKey()];
    NSString *message = @"All your base are belong to us!";
    NSString *shortContext = @".";
    NSString *longContext = @"You have no chance to survive make your time. Ha ha ha ha ...";

    NSData *encryptedShort = [cell encrypt:[message dataUsingEncoding:NSUTF8StringEncoding]
                                   context:[shortContext dataUsingEncoding:NSUTF8StringEncoding]];

    NSData *encryptedLong = [cell encrypt:[message dataUsingEncoding:NSUTF8StringEncoding]
                                  context:[longContext dataUsingEncoding:NSUTF8StringEncoding]];

    // Context is not (directly) included into encrypted message.
    XCTAssertEqual(encryptedShort.length, encryptedLong.length);
}

- (void)testWithoutContext
{
    TSCellSeal *cell = [[TSCellSeal alloc] initWithKey:TSGenerateSymmetricKey()];
    NSData *message = [@"All your base are belong to us!" dataUsingEncoding:NSUTF8StringEncoding];

    // Absent, empty, or nil context are all the same.
    NSData *encrypted1 = [cell encrypt:message];
    NSData *encrypted2 = [cell encrypt:message context:nil];
    NSData *encrypted3 = [cell encrypt:message context:[NSData new]];

    XCTAssert([message isEqualToData:[cell decrypt:encrypted1]]);
    XCTAssert([message isEqualToData:[cell decrypt:encrypted2]]);
    XCTAssert([message isEqualToData:[cell decrypt:encrypted3]]);

    XCTAssert([message isEqualToData:[cell decrypt:encrypted1 context:nil]]);
    XCTAssert([message isEqualToData:[cell decrypt:encrypted2 context:nil]]);
    XCTAssert([message isEqualToData:[cell decrypt:encrypted3 context:nil]]);

    XCTAssert([message isEqualToData:[cell decrypt:encrypted1 context:[NSData new]]]);
    XCTAssert([message isEqualToData:[cell decrypt:encrypted2 context:[NSData new]]]);
    XCTAssert([message isEqualToData:[cell decrypt:encrypted3 context:[NSData new]]]);
}

- (void)testContextSignificance
{
    TSCellSeal *cell = [[TSCellSeal alloc] initWithKey:TSGenerateSymmetricKey()];
    NSData *message = [@"All your base are belong to us!" dataUsingEncoding:NSUTF8StringEncoding];
    NSData *correctContext = [@"We are CATS" dataUsingEncoding:NSUTF8StringEncoding];
    NSData *incorrectContext = [@"Captain !!" dataUsingEncoding:NSUTF8StringEncoding];
    NSError *error;

    NSData *encrypted = [cell encrypt:message context:correctContext];

    // You cannot use a different context to decrypt data.
    NSData *decrypted = [cell decrypt:encrypted context:incorrectContext error:&error];
    XCTAssertNil(decrypted);
    XCTAssertNotNil(error);
    XCTAssertEqual(error.code, TSErrorTypeFail);

    // Only the original context will work.
    error = nil;
    decrypted = [cell decrypt:encrypted context:correctContext error:&error];
    XCTAssertNotNil(decrypted);
    XCTAssertNil(error);
    XCTAssert([decrypted isEqualToData:message]);
}

- (void)testDetectCorruptedData
{
    TSCellSeal *cell = [[TSCellSeal alloc] initWithKey:TSGenerateSymmetricKey()];
    NSData *message = [@"All your base are belong to us!" dataUsingEncoding:NSUTF8StringEncoding];
    NSError *error;

    NSData *encrypted = [cell encrypt:message];

    NSMutableData *corrupted = [NSMutableData dataWithData:encrypted];
    // Invert every odd byte, this will surely break the message.
    uint8_t *bytes = corrupted.mutableBytes;
    for (NSUInteger i = 1; i < corrupted.length; i += 2) {
        bytes[i] = ~bytes[i];
    }

    NSData *decrypted = [cell decrypt:corrupted error:&error];
    XCTAssertNil(decrypted);
    XCTAssertNotNil(error);
}

- (void)testDetectTruncatedData
{
    TSCellSeal *cell = [[TSCellSeal alloc] initWithKey:TSGenerateSymmetricKey()];
    NSData *message = [@"All your base are belong to us!" dataUsingEncoding:NSUTF8StringEncoding];
    NSError *error;

    NSData *encrypted = [cell encrypt:message];

    NSMutableData *truncated = [NSMutableData dataWithData:encrypted];
    [truncated setLength:truncated.length - 1];

    NSData *decrypted = [cell decrypt:truncated error:&error];
    XCTAssertNil(decrypted);
    XCTAssertNotNil(error);
}

- (void)testDetectExtendedData
{
    TSCellSeal *cell = [[TSCellSeal alloc] initWithKey:TSGenerateSymmetricKey()];
    NSData *message = [@"All your base are belong to us!" dataUsingEncoding:NSUTF8StringEncoding];
    NSError *error;

    NSData *encrypted = [cell encrypt:message];

    NSMutableData *extended = [NSMutableData dataWithData:encrypted];
    [extended setLength:extended.length + 1];

    NSData *decrypted = [cell decrypt:extended error:&error];
    XCTAssertNil(decrypted);
    XCTAssertNotNil(error);
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnonnull"
- (void)testEmptyMessage
{
    TSCellSeal *cell = [[TSCellSeal alloc] initWithKey:TSGenerateSymmetricKey()];
    NSError *error;

    error = nil;
    XCTAssertNil([cell encrypt:nil error:&error]);
    XCTAssertNotNil(error);

    error = nil;
    XCTAssertNil([cell encrypt:[NSData new] error:&error]);
    XCTAssertNotNil(error);

    error = nil;
    XCTAssertNil([cell decrypt:nil error:&error]);
    XCTAssertNotNil(error);

    error = nil;
    XCTAssertNil([cell decrypt:[NSData new] error:&error]);
    XCTAssertNotNil(error);
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated"
- (void)testOldAPI
{
    TSCellSeal *cell = [[TSCellSeal alloc] initWithKey:TSGenerateSymmetricKey()];
    NSData *message = [@"All your base are belong to us!" dataUsingEncoding:NSUTF8StringEncoding];
    NSData *context = [@"For great justice" dataUsingEncoding:NSUTF8StringEncoding];
    NSError *error;

    NSData *encrypted = [cell wrapData:message context:context error:&error];
    XCTAssertNotNil(encrypted);
    XCTAssertNil(error);

    NSData *decrypted = [cell decrypt:encrypted context:context error:&error];
    XCTAssertNotNil(decrypted);
    XCTAssertNil(error);
    XCTAssert([decrypted isEqualToData:message]);

    encrypted = [cell encrypt:message context:context error:&error];
    XCTAssertNotNil(encrypted);
    XCTAssertNil(error);

    decrypted = [cell unwrapData:encrypted context:context error:&error];
    XCTAssertNotNil(decrypted);
    XCTAssertNil(error);
    XCTAssert([decrypted isEqualToData:message]);
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated"
- (void)testOldAPIWithoutContext
{
    TSCellSeal *cell = [[TSCellSeal alloc] initWithKey:TSGenerateSymmetricKey()];
    NSData *message = [@"All your base are belong to us!" dataUsingEncoding:NSUTF8StringEncoding];
    NSError *error;

    NSData *encrypted = [cell wrapData:message error:&error];
    XCTAssertNotNil(encrypted);
    XCTAssertNil(error);

    NSData *decrypted = [cell decrypt:encrypted error:&error];
    XCTAssertNotNil(decrypted);
    XCTAssertNil(error);
    XCTAssert([decrypted isEqualToData:message]);

    encrypted = [cell encrypt:message error:&error];
    XCTAssertNotNil(encrypted);
    XCTAssertNil(error);

    decrypted = [cell unwrapData:encrypted error:&error];
    XCTAssertNotNil(decrypted);
    XCTAssertNil(error);
    XCTAssert([decrypted isEqualToData:message]);
}
#pragma clang diagnostic pop

- (void)testKeyWipedOnDealloc
{
    NSData *key = TSGenerateSymmetricKey();
    {
        NSData *cellKey;
        {
            TSCellSeal *cell = [[TSCellSeal alloc] initWithKey:key];

            cellKey = cell.key;

            XCTAssertNotNil(cellKey);
            XCTAssertNotEqual(cellKey.length, 0);
            XCTAssertNotEqual(cellKey, key, @"Secure Cell makes a copy of the key");
            XCTAssert([cellKey isEqualToData:key]);
        }
        // The key is wiped after the cell has left the scope and has been deallocated.
        XCTAssertNotNil(cellKey);
        XCTAssertEqual(cellKey.length, 0);
        XCTAssertFalse([cellKey isEqualToData:key]);
    }
}

@end

#pragma mark - Seal Mode (passphrase)

@implementation SecureCellSealPassphrase

- (void)testInitWithFixed
{
    TSCellSeal *cell = [[TSCellSeal alloc] initWithPassphrase:@"secret"];
    XCTAssertNotNil(cell);
}

- (void)testInitWithRawData
{
    NSData *data = [@"secret" dataUsingEncoding:NSASCIIStringEncoding];
    TSCellSeal *cell = [[TSCellSeal alloc] initWithPassphraseData:data];
    XCTAssertNotNil(cell);
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnonnull"
- (void)testInitWithEmpty
{
    XCTAssertNil([[TSCellSeal alloc] initWithPassphrase:nil]);
    XCTAssertNil([[TSCellSeal alloc] initWithPassphrase:@""]);
    XCTAssertNil([[TSCellSeal alloc] initWithPassphraseData:nil]);
    XCTAssertNil([[TSCellSeal alloc] initWithPassphraseData:[NSData new]]);
}
#pragma clang diagnostic pop

- (void)testRoundtrip
{
    TSCellSeal *cell = [[TSCellSeal alloc] initWithPassphrase:@"secret"];
    NSString *message = @"All your base are belong to us!";
    NSString *context = @"For great justice";
    NSError *error;

    NSData *encrypted = [cell encrypt:[message dataUsingEncoding:NSUTF8StringEncoding]
                              context:[context dataUsingEncoding:NSUTF8StringEncoding]
                                error:&error];
    XCTAssertNotNil(encrypted);
    XCTAssertNil(error);

    NSData *decrypted = [cell decrypt:encrypted
                              context:[context dataUsingEncoding:NSUTF8StringEncoding]
                                error:&error];
    XCTAssertNotNil(decrypted);
    XCTAssertNil(error);

    NSString *decryptedMessage = [[NSString alloc] initWithData:decrypted
                                                       encoding:NSUTF8StringEncoding];

    XCTAssert([decryptedMessage isEqualToString:message]);
}

- (void)testDataLengthExtension
{
    TSCellSeal *cell = [[TSCellSeal alloc] initWithPassphrase:@"secret"];
    NSString *message = @"All your base are belong to us!";
    NSData *data = [message dataUsingEncoding:NSUTF8StringEncoding];

    NSData *encrypted = [cell encrypt:data];

    XCTAssertGreaterThan(encrypted.length, data.length);
}

- (void)testContextInclusion
{
    TSCellSeal *cell = [[TSCellSeal alloc] initWithPassphrase:@"secret"];
    NSString *message = @"All your base are belong to us!";
    NSString *shortContext = @".";
    NSString *longContext = @"You have no chance to survive make your time. Ha ha ha ha ...";

    NSData *encryptedShort = [cell encrypt:[message dataUsingEncoding:NSUTF8StringEncoding]
                                   context:[shortContext dataUsingEncoding:NSUTF8StringEncoding]];

    NSData *encryptedLong = [cell encrypt:[message dataUsingEncoding:NSUTF8StringEncoding]
                                  context:[longContext dataUsingEncoding:NSUTF8StringEncoding]];

    // Context is not (directly) included into encrypted message.
    XCTAssertEqual(encryptedShort.length, encryptedLong.length);
}

- (void)testWithoutContext
{
    TSCellSeal *cell = [[TSCellSeal alloc] initWithPassphrase:@"secret"];
    NSData *message = [@"All your base are belong to us!" dataUsingEncoding:NSUTF8StringEncoding];

    // Absent, empty, or nil context are all the same.
    NSData *encrypted1 = [cell encrypt:message];
    NSData *encrypted2 = [cell encrypt:message context:nil];
    NSData *encrypted3 = [cell encrypt:message context:[NSData new]];

    XCTAssert([message isEqualToData:[cell decrypt:encrypted1]]);
    XCTAssert([message isEqualToData:[cell decrypt:encrypted2]]);
    XCTAssert([message isEqualToData:[cell decrypt:encrypted3]]);

    XCTAssert([message isEqualToData:[cell decrypt:encrypted1 context:nil]]);
    XCTAssert([message isEqualToData:[cell decrypt:encrypted2 context:nil]]);
    XCTAssert([message isEqualToData:[cell decrypt:encrypted3 context:nil]]);

    XCTAssert([message isEqualToData:[cell decrypt:encrypted1 context:[NSData new]]]);
    XCTAssert([message isEqualToData:[cell decrypt:encrypted2 context:[NSData new]]]);
    XCTAssert([message isEqualToData:[cell decrypt:encrypted3 context:[NSData new]]]);
}

- (void)testContextSignificance
{
    TSCellSeal *cell = [[TSCellSeal alloc] initWithPassphrase:@"secret"];
    NSData *message = [@"All your base are belong to us!" dataUsingEncoding:NSUTF8StringEncoding];
    NSData *correctContext = [@"We are CATS" dataUsingEncoding:NSUTF8StringEncoding];
    NSData *incorrectContext = [@"Captain !!" dataUsingEncoding:NSUTF8StringEncoding];
    NSError *error;

    NSData *encrypted = [cell encrypt:message context:correctContext];

    // You cannot use a different context to decrypt data.
    NSData *decrypted = [cell decrypt:encrypted context:incorrectContext error:&error];
    XCTAssertNil(decrypted);
    XCTAssertNotNil(error);
    XCTAssertEqual(error.code, TSErrorTypeFail);

    // Only the original context will work.
    error = nil;
    decrypted = [cell decrypt:encrypted context:correctContext error:&error];
    XCTAssertNotNil(decrypted);
    XCTAssertNil(error);
    XCTAssert([decrypted isEqualToData:message]);
}

- (void)testDetectCorruptedData
{
    TSCellSeal *cell = [[TSCellSeal alloc] initWithPassphrase:@"secret"];
    NSData *message = [@"All your base are belong to us!" dataUsingEncoding:NSUTF8StringEncoding];
    NSError *error;

    NSData *encrypted = [cell encrypt:message];

    NSMutableData *corrupted = [NSMutableData dataWithData:encrypted];
    // Invert every odd byte, this will surely break the message.
    uint8_t *bytes = corrupted.mutableBytes;
    for (NSUInteger i = 1; i < corrupted.length; i += 2) {
        bytes[i] = ~bytes[i];
    }

    NSData *decrypted = [cell decrypt:corrupted error:&error];
    XCTAssertNil(decrypted);
    XCTAssertNotNil(error);
}

- (void)testDetectTruncatedData
{
    TSCellSeal *cell = [[TSCellSeal alloc] initWithPassphrase:@"secret"];
    NSData *message = [@"All your base are belong to us!" dataUsingEncoding:NSUTF8StringEncoding];
    NSError *error;

    NSData *encrypted = [cell encrypt:message];

    NSMutableData *truncated = [NSMutableData dataWithData:encrypted];
    [truncated setLength:truncated.length - 1];

    NSData *decrypted = [cell decrypt:truncated error:&error];
    XCTAssertNil(decrypted);
    XCTAssertNotNil(error);
}

- (void)testDetectExtendedData
{
    TSCellSeal *cell = [[TSCellSeal alloc] initWithPassphrase:@"secret"];
    NSData *message = [@"All your base are belong to us!" dataUsingEncoding:NSUTF8StringEncoding];
    NSError *error;

    NSData *encrypted = [cell encrypt:message];

    NSMutableData *extended = [NSMutableData dataWithData:encrypted];
    [extended setLength:extended.length + 1];

    NSData *decrypted = [cell decrypt:extended error:&error];
    XCTAssertNil(decrypted);
    XCTAssertNotNil(error);
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnonnull"
- (void)testEmptyMessage
{
    TSCellSeal *cell = [[TSCellSeal alloc] initWithPassphrase:@"secret"];
    NSError *error;

    error = nil;
    XCTAssertNil([cell encrypt:nil error:&error]);
    XCTAssertNotNil(error);

    error = nil;
    XCTAssertNil([cell encrypt:[NSData new] error:&error]);
    XCTAssertNotNil(error);

    error = nil;
    XCTAssertNil([cell decrypt:nil error:&error]);
    XCTAssertNotNil(error);

    error = nil;
    XCTAssertNil([cell decrypt:[NSData new] error:&error]);
    XCTAssertNotNil(error);
}
#pragma clang diagnostic pop

- (void)testKeyIncompatibility
{
    // Passphrases are not keys. Keys are not passphrases.
    NSData *secret = TSGenerateSymmetricKey();
    TSCellSeal *cellMK = [[TSCellSeal alloc] initWithKey:secret];
    TSCellSeal *cellPW = [[TSCellSeal alloc] initWithPassphraseData:secret];
    NSData *message = [@"All your base are belong to us!" dataUsingEncoding:NSUTF8StringEncoding];

    NSData *encrypted = [cellMK encrypt:message];
    NSData *decrypted = [cellPW decrypt:encrypted];

    XCTAssertNil(decrypted);
}

- (void)testEncodingDefault
{
    // Passphrases are encoded in UTF-8 by default.
    NSString *secret = @"暗号";
    NSData *secretUTF8 = [secret dataUsingEncoding:NSUTF8StringEncoding];
    TSCellSeal *cellA = [[TSCellSeal alloc] initWithPassphrase:secret];
    TSCellSeal *cellB = [[TSCellSeal alloc] initWithPassphraseData:secretUTF8];
    NSData *message = [@"All your base are belong to us!" dataUsingEncoding:NSUTF8StringEncoding];

    NSData *encrypted = [cellA encrypt:message];
    NSData *decrypted = [cellB decrypt:encrypted];

    XCTAssert([decrypted isEqualToData:message]);
}

- (void)testEncodingSpecific
{
    TSCellSeal *cell = [[TSCellSeal alloc] initWithPassphraseData:[@"secret" dataUsingEncoding:NSUTF16BigEndianStringEncoding]];
    NSData *message = [@"All your base are belong to us!" dataUsingEncoding:NSUTF8StringEncoding];

    // Message encrypted by PyThemis
    NSString *encryptedString = @"AAEBQQwAAAAQAAAAHwAAABYAAADd7pTPG8qgEZEl+P44uql6d3bIIrVV8+OCTZOVQA0DABAAu+6OAvFZuj6nfd40hsLMyJiu+yObBmb5De7ZOsHL3j3SRZqKIoEGX0e1iAApBFQ=";
    NSData *encrypted = [[NSData alloc] initWithBase64EncodedString:encryptedString
                                                            options:NSDataBase64DecodingIgnoreUnknownCharacters];

    NSData *decrypted = [cell decrypt:encrypted];

    XCTAssert([decrypted isEqualToData:message]);
}

@end

#pragma mark - Token Protect

@implementation SecureCellTokenProtect

- (void)testInitWithGenerated
{
    TSCellToken *cell = [[TSCellToken alloc] initWithKey:TSGenerateSymmetricKey()];
    XCTAssertNotNil(cell);
}

- (void)testInitWithFixed
{
    NSString *masterKeyString = @"UkVDMgAAAC13PCVZAKOczZXUpvkhsC+xvwWnv3CLmlG0Wzy8ZBMnT+2yx/dg";
    NSData *masterKeyData = [[NSData alloc] initWithBase64EncodedString:masterKeyString
                                                                options:NSDataBase64DecodingIgnoreUnknownCharacters];

    TSCellToken *cell = [[TSCellToken alloc] initWithKey:masterKeyData];
    XCTAssertNotNil(cell);
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnonnull"
- (void)testInitWithEmpty
{
    XCTAssertNil([[TSCellToken alloc] initWithKey:nil]);
    XCTAssertNil([[TSCellToken alloc] initWithKey:[NSData new]]);
}
#pragma clang diagnostic pop

- (void)testRoundtrip
{
    TSCellToken *cell = [[TSCellToken alloc] initWithKey:TSGenerateSymmetricKey()];
    NSString *message = @"Roses are grey. Violets are grey.";
    NSString *context = @"I'm a dog";
    NSError *error;

    TSCellTokenEncryptedResult *result =
        [cell encrypt:[message dataUsingEncoding:NSUTF8StringEncoding]
              context:[context dataUsingEncoding:NSUTF8StringEncoding]
                error:&error];
    XCTAssertNotNil(result);
    XCTAssertNotNil(result.encrypted);
    XCTAssertNotNil(result.token);
    XCTAssertNil(error);

    NSData *decrypted = [cell decrypt:result.encrypted
                                token:result.token
                              context:[context dataUsingEncoding:NSUTF8StringEncoding]
                                error:&error];
    XCTAssertNotNil(decrypted);
    XCTAssertNil(error);

    NSString *decryptedMessage = [[NSString alloc] initWithData:decrypted
                                                       encoding:NSUTF8StringEncoding];

    XCTAssert([decryptedMessage isEqualToString:message]);
}

- (void)testDataLengthPreservation
{
    TSCellToken *cell = [[TSCellToken alloc] initWithKey:TSGenerateSymmetricKey()];
    NSString *message = @"Roses are grey. Violets are grey.";
    NSData *data = [message dataUsingEncoding:NSUTF8StringEncoding];

    TSCellTokenEncryptedResult *result = [cell encrypt:data];

    XCTAssertEqual(result.encrypted.length, data.length);
    XCTAssert(result.token.length > 0);
}

- (void)testContextInclusion
{
    TSCellToken *cell = [[TSCellToken alloc] initWithKey:TSGenerateSymmetricKey()];
    NSString *message = @"Roses are grey. Violets are grey.";
    NSString *shortContext = @"I'm a dog";
    NSString *longContext = @"This is why cats are ultimately superior creatures.";

    TSCellTokenEncryptedResult *resultShort =
        [cell encrypt:[message dataUsingEncoding:NSUTF8StringEncoding]
              context:[shortContext dataUsingEncoding:NSUTF8StringEncoding]];

    TSCellTokenEncryptedResult *resultLong =
        [cell encrypt:[message dataUsingEncoding:NSUTF8StringEncoding]
              context:[longContext dataUsingEncoding:NSUTF8StringEncoding]];

    // Context is not (directly) included into encrypted message.
    XCTAssertEqual(resultShort.encrypted.length, resultLong.encrypted.length);
    XCTAssertEqual(resultShort.token.length, resultLong.token.length);
}

- (void)testWithoutContext
{
    TSCellToken *cell = [[TSCellToken alloc] initWithKey:TSGenerateSymmetricKey()];
    NSData *message = [@"Roses are grey. Violets are grey." dataUsingEncoding:NSUTF8StringEncoding];

    // Absent, empty, or nil context are all the same.
    TSCellTokenEncryptedResult *result1 = [cell encrypt:message];
    TSCellTokenEncryptedResult *result2 = [cell encrypt:message context:nil];
    TSCellTokenEncryptedResult *result3 = [cell encrypt:message context:[NSData new]];

    XCTAssert([message isEqualToData:[cell decrypt:result1.encrypted token:result1.token]]);
    XCTAssert([message isEqualToData:[cell decrypt:result2.encrypted token:result2.token]]);
    XCTAssert([message isEqualToData:[cell decrypt:result3.encrypted token:result3.token]]);

    XCTAssert([message isEqualToData:[cell decrypt:result1.encrypted token:result1.token context:nil]]);
    XCTAssert([message isEqualToData:[cell decrypt:result2.encrypted token:result2.token context:nil]]);
    XCTAssert([message isEqualToData:[cell decrypt:result3.encrypted token:result3.token context:nil]]);

    XCTAssert([message isEqualToData:[cell decrypt:result1.encrypted token:result1.token context:[NSData new]]]);
    XCTAssert([message isEqualToData:[cell decrypt:result2.encrypted token:result2.token context:[NSData new]]]);
    XCTAssert([message isEqualToData:[cell decrypt:result3.encrypted token:result3.token context:[NSData new]]]);
}

- (void)testContextSignificance
{
    TSCellToken *cell = [[TSCellToken alloc] initWithKey:TSGenerateSymmetricKey()];
    NSData *message = [@"Roses are grey. Violets are grey." dataUsingEncoding:NSUTF8StringEncoding];
    NSData *correctContext = [@"I'm a dog" dataUsingEncoding:NSUTF8StringEncoding];
    NSData *incorrectContext = [@"Volcano eruption" dataUsingEncoding:NSUTF8StringEncoding];
    NSError *error;

    TSCellTokenEncryptedResult *result = [cell encrypt:message context:correctContext];

    // You cannot use a different context to decrypt data.
    NSData *decrypted = [cell decrypt:result.encrypted
                                token:result.token
                              context:incorrectContext
                                error:&error];
    XCTAssertNil(decrypted);
    XCTAssertNotNil(error);
    XCTAssertEqual(error.code, TSErrorTypeFail);

    // Only the original context will work.
    error = nil;
    decrypted = [cell decrypt:result.encrypted
                        token:result.token
                      context:correctContext
                        error:&error];
    XCTAssertNotNil(decrypted);
    XCTAssertNil(error);
    XCTAssert([decrypted isEqualToData:message]);
}

- (void)testTokenSignificance
{
    TSCellToken *cell = [[TSCellToken alloc] initWithKey:TSGenerateSymmetricKey()];
    NSData *message = [@"Roses are grey. Violets are grey." dataUsingEncoding:NSUTF8StringEncoding];
    NSData *decrypted;
    NSError *error;

    TSCellTokenEncryptedResult *result1 = [cell encrypt:message];
    TSCellTokenEncryptedResult *result2 = [cell encrypt:message];

    // You cannot use a different token to decrypt data.
    error = nil;
    decrypted = [cell decrypt:result1.encrypted
                        token:result2.token
                        error:&error];
    XCTAssertNil(decrypted);
    XCTAssertNotNil(error);
    XCTAssertEqual(error.code, TSErrorTypeFail);

    error = nil;
    decrypted = [cell decrypt:result2.encrypted
                        token:result1.token
                        error:&error];
    XCTAssertNil(decrypted);
    XCTAssertNotNil(error);
    XCTAssertEqual(error.code, TSErrorTypeFail);

    // Only the corresponding token will work.
    error = nil;
    decrypted = [cell decrypt:result1.encrypted
                        token:result1.token
                        error:&error];
    XCTAssertNotNil(decrypted);
    XCTAssertNil(error);
    XCTAssert([decrypted isEqualToData:message]);

    error = nil;
    decrypted = [cell decrypt:result2.encrypted
                        token:result2.token
                        error:&error];
    XCTAssertNotNil(decrypted);
    XCTAssertNil(error);
    XCTAssert([decrypted isEqualToData:message]);
}

- (void)testDetectCorruptedData
{
    TSCellToken *cell = [[TSCellToken alloc] initWithKey:TSGenerateSymmetricKey()];
    NSData *message = [@"Roses are grey. Violets are grey." dataUsingEncoding:NSUTF8StringEncoding];
    NSError *error;

    TSCellTokenEncryptedResult *encryptedData = [cell encrypt:message];

    NSMutableData *corrupted = [NSMutableData dataWithData:encryptedData.encrypted];
    // Invert every odd byte, this will surely break the message.
    uint8_t *bytes = corrupted.mutableBytes;
    for (NSUInteger i = 1; i < corrupted.length; i += 2) {
        bytes[i] = ~bytes[i];
    }

    NSData *decrypted = [cell decrypt:corrupted token:encryptedData.token error:&error];
    XCTAssertNil(decrypted);
    XCTAssertNotNil(error);
}

- (void)testDetectTruncatedData
{
    TSCellToken *cell = [[TSCellToken alloc] initWithKey:TSGenerateSymmetricKey()];
    NSData *message = [@"Roses are grey. Violets are grey." dataUsingEncoding:NSUTF8StringEncoding];
    NSError *error;

    TSCellTokenEncryptedResult *encryptedData = [cell encrypt:message];

    NSMutableData *truncated = [NSMutableData dataWithData:encryptedData.encrypted];
    [truncated setLength:truncated.length - 1];

    NSData *decrypted = [cell decrypt:truncated token:encryptedData.token error:&error];
    XCTAssertNil(decrypted);
    XCTAssertNotNil(error);
}

- (void)testDetectExtendedData
{
    TSCellToken *cell = [[TSCellToken alloc] initWithKey:TSGenerateSymmetricKey()];
    NSData *message = [@"Roses are grey. Violets are grey." dataUsingEncoding:NSUTF8StringEncoding];
    NSError *error;

    TSCellTokenEncryptedResult *encryptedData = [cell encrypt:message];

    NSMutableData *extended = [NSMutableData dataWithData:encryptedData.encrypted];
    [extended setLength:extended.length + 1];

    NSData *decrypted = [cell decrypt:extended token:encryptedData.token error:&error];
    XCTAssertNil(decrypted);
    XCTAssertNotNil(error);
}

- (void)testDetectCorruptedToken
{
    TSCellToken *cell = [[TSCellToken alloc] initWithKey:TSGenerateSymmetricKey()];
    NSData *message = [@"Roses are grey. Violets are grey." dataUsingEncoding:NSUTF8StringEncoding];
    NSError *error;

    TSCellTokenEncryptedResult *encryptedData = [cell encrypt:message];

    NSMutableData *corruptedToken = [NSMutableData dataWithData:encryptedData.token];
    // Invert every odd byte, this will surely break the token.
    uint8_t *bytes = corruptedToken.mutableBytes;
    for (NSUInteger i = 1; i < corruptedToken.length; i += 2) {
        bytes[i] = ~bytes[i];
    }

    NSData *decrypted = [cell decrypt:encryptedData.encrypted token:corruptedToken error:&error];
    XCTAssertNil(decrypted);
    XCTAssertNotNil(error);
}

- (void)testDetectTruncatedToken
{
    TSCellToken *cell = [[TSCellToken alloc] initWithKey:TSGenerateSymmetricKey()];
    NSData *message = [@"Roses are grey. Violets are grey." dataUsingEncoding:NSUTF8StringEncoding];
    NSError *error;

    TSCellTokenEncryptedResult *encryptedData = [cell encrypt:message];

    NSMutableData *truncatedToken = [NSMutableData dataWithData:encryptedData.token];
    [truncatedToken setLength:truncatedToken.length - 1];

    NSData *decrypted = [cell decrypt:encryptedData.encrypted token:truncatedToken error:&error];
    XCTAssertNil(decrypted);
    XCTAssertNotNil(error);
}

- (void)testDetectExtendedToken
{
    TSCellToken *cell = [[TSCellToken alloc] initWithKey:TSGenerateSymmetricKey()];
    NSData *message = [@"Roses are grey. Violets are grey." dataUsingEncoding:NSUTF8StringEncoding];
    NSError *error;

    TSCellTokenEncryptedResult *encryptedData = [cell encrypt:message];

    NSMutableData *extendedToken = [NSMutableData dataWithData:encryptedData.token];
    [extendedToken setLength:extendedToken.length + 1];

    // Current implementation of Secure Cell allows the token to be overlong.
    // Extra data is simply ignored.
    NSData *decrypted = [cell decrypt:encryptedData.encrypted token:extendedToken error:&error];
    XCTAssertNotNil(decrypted);
    XCTAssertNil(error);
    XCTAssert([decrypted isEqualToData:message]);
}

- (void)testSwapTokenAndData
{
    TSCellToken *cell = [[TSCellToken alloc] initWithKey:TSGenerateSymmetricKey()];
    NSData *message = [@"Roses are grey. Violets are grey." dataUsingEncoding:NSUTF8StringEncoding];
    NSError *error;

    TSCellTokenEncryptedResult *encryptedData = [cell encrypt:message];

    NSData *decrypted = [cell decrypt:encryptedData.token
                                token:encryptedData.encrypted
                                error:&error];
    XCTAssertNil(decrypted);
    XCTAssertNotNil(error);
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnonnull"
- (void)testEmptyMessageOrToken
{
    TSCellToken *cell = [[TSCellToken alloc] initWithKey:TSGenerateSymmetricKey()];
    NSError *error;

    error = nil;
    XCTAssertNil([cell encrypt:nil error:&error]);
    XCTAssertNotNil(error);

    error = nil;
    XCTAssertNil([cell encrypt:[NSData new] error:&error]);
    XCTAssertNotNil(error);

    TSCellTokenEncryptedResult *encryptedData = [cell encrypt:TSGenerateSymmetricKey()];

    error = nil;
    XCTAssertNil([cell decrypt:nil token:encryptedData.token error:&error]);
    XCTAssertNotNil(error);

    error = nil;
    XCTAssertNil([cell decrypt:[NSData new] token:encryptedData.token error:&error]);
    XCTAssertNotNil(error);

    error = nil;
    XCTAssertNil([cell decrypt:encryptedData.encrypted token:nil error:&error]);
    XCTAssertNotNil(error);

    error = nil;
    XCTAssertNil([cell decrypt:encryptedData.encrypted token:[NSData new] error:&error]);
    XCTAssertNotNil(error);
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated"
- (void)testOldAPI
{
    TSCellToken *cell = [[TSCellToken alloc] initWithKey:TSGenerateSymmetricKey()];
    NSData *message = [@"Roses are grey. Violets are grey." dataUsingEncoding:NSUTF8StringEncoding];
    NSData *context = [@"I'm a dog" dataUsingEncoding:NSUTF8StringEncoding];
    NSError *error;

    TSCellTokenEncryptedData *encrypted = [cell wrapData:message
                                                 context:context
                                                   error:&error];
    XCTAssertNotNil(encrypted);
    XCTAssertNotNil(encrypted.cipherText);
    XCTAssertNotNil(encrypted.token);
    XCTAssertNil(error);

    NSData *decrypted = [cell decrypt:encrypted.cipherText
                                token:encrypted.token
                              context:context
                                error:&error];
    XCTAssertNotNil(decrypted);
    XCTAssertNil(error);
    XCTAssert([decrypted isEqualToData:message]);

    TSCellTokenEncryptedResult *result = [cell encrypt:message context:context error:&error];
    XCTAssertNotNil(result);
    XCTAssertNotNil(result.cipherText);
    XCTAssertNotNil(result.encrypted);
    XCTAssertNotNil(result.token);
    XCTAssertNil(error);
    XCTAssert([result.cipherText isEqualToData:result.encrypted]);

    encrypted.cipherText = [NSMutableData dataWithData:result.cipherText];
    encrypted.token = [NSMutableData dataWithData:result.token];
    decrypted = [cell unwrapData:encrypted context:context error:&error];
    XCTAssertNotNil(decrypted);
    XCTAssertNil(error);
    XCTAssert([decrypted isEqualToData:message]);
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated"
- (void)testOldAPIWithoutContext
{
    TSCellToken *cell = [[TSCellToken alloc] initWithKey:TSGenerateSymmetricKey()];
    NSData *message = [@"Roses are grey. Violets are grey." dataUsingEncoding:NSUTF8StringEncoding];
    NSError *error;

    TSCellTokenEncryptedData *encrypted = [cell wrapData:message error:&error];
    XCTAssertNotNil(encrypted);
    XCTAssertNotNil(encrypted.cipherText);
    XCTAssertNotNil(encrypted.token);
    XCTAssertNil(error);

    NSData *decrypted = [cell decrypt:encrypted.cipherText token:encrypted.token error:&error];
    XCTAssertNotNil(decrypted);
    XCTAssertNil(error);
    XCTAssert([decrypted isEqualToData:message]);

    TSCellTokenEncryptedResult *result = [cell encrypt:message error:&error];
    XCTAssertNotNil(result);
    XCTAssertNotNil(result.cipherText);
    XCTAssertNotNil(result.encrypted);
    XCTAssertNotNil(result.token);
    XCTAssertNil(error);
    XCTAssert([result.cipherText isEqualToData:result.encrypted]);

    encrypted.cipherText = [NSMutableData dataWithData:result.cipherText];
    encrypted.token = [NSMutableData dataWithData:result.token];
    decrypted = [cell unwrapData:encrypted error:&error];
    XCTAssertNotNil(decrypted);
    XCTAssertNil(error);
    XCTAssert([decrypted isEqualToData:message]);
}
#pragma clang diagnostic pop

- (void)testKeyWipedOnDealloc
{
    NSData *key = TSGenerateSymmetricKey();
    {
        NSData *cellKey;
        {
            TSCellToken *cell = [[TSCellToken alloc] initWithKey:key];

            cellKey = cell.key;

            XCTAssertNotNil(cellKey);
            XCTAssertNotEqual(cellKey.length, 0);
            XCTAssertNotEqual(cellKey, key, @"Secure Cell makes a copy of the key");
            XCTAssert([cellKey isEqualToData:key]);
        }
        // The key is wiped after the cell has left the scope and has been deallocated.
        XCTAssertNotNil(cellKey);
        XCTAssertEqual(cellKey.length, 0);
        XCTAssertFalse([cellKey isEqualToData:key]);
    }
}

@end

#pragma mark - Context Imprint

@implementation SecureCellContextImprint

- (void)testInitWithGenerated
{
    TSCellContextImprint *cell = [[TSCellContextImprint alloc] initWithKey:TSGenerateSymmetricKey()];
    XCTAssertNotNil(cell);
}

- (void)testInitWithFixed
{
    NSString *masterKeyString = @"UkVDMgAAAC13PCVZAKOczZXUpvkhsC+xvwWnv3CLmlG0Wzy8ZBMnT+2yx/dg";
    NSData *masterKeyData = [[NSData alloc] initWithBase64EncodedString:masterKeyString
                                                                options:NSDataBase64DecodingIgnoreUnknownCharacters];

    TSCellContextImprint *cell = [[TSCellContextImprint alloc] initWithKey:masterKeyData];
    XCTAssertNotNil(cell);
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnonnull"
- (void)testInitWithEmpty
{
    XCTAssertNil([[TSCellContextImprint alloc] initWithKey:nil]);
    XCTAssertNil([[TSCellContextImprint alloc] initWithKey:[NSData new]]);
}
#pragma clang diagnostic pop

- (void)testRoundtrip
{
    TSCellContextImprint *cell = [[TSCellContextImprint alloc] initWithKey:TSGenerateSymmetricKey()];
    NSString *message = @"All your base are belong to us!";
    NSString *context = @"For great justice";
    NSError *error;

    NSData *encrypted = [cell encrypt:[message dataUsingEncoding:NSUTF8StringEncoding]
                              context:[context dataUsingEncoding:NSUTF8StringEncoding]
                                error:&error];
    XCTAssertNotNil(encrypted);
    XCTAssertNil(error);

    NSData *decrypted = [cell decrypt:encrypted
                              context:[context dataUsingEncoding:NSUTF8StringEncoding]
                                error:&error];
    XCTAssertNotNil(decrypted);
    XCTAssertNil(error);

    NSString *decryptedMessage = [[NSString alloc] initWithData:decrypted
                                                       encoding:NSUTF8StringEncoding];

    XCTAssert([decryptedMessage isEqualToString:message]);
}

- (void)testDataLengthPreservation
{
    TSCellContextImprint *cell = [[TSCellContextImprint alloc] initWithKey:TSGenerateSymmetricKey()];
    NSString *message = @"All your base are belong to us!";
    NSData *data = [message dataUsingEncoding:NSUTF8StringEncoding];
    NSData *context = [@"For great justice" dataUsingEncoding:NSUTF8StringEncoding];

    NSData *encrypted = [cell encrypt:data context:context];

    XCTAssertEqual(encrypted.length, data.length);
}

- (void)testContextInclusion
{
    TSCellContextImprint *cell = [[TSCellContextImprint alloc] initWithKey:TSGenerateSymmetricKey()];
    NSString *message = @"All your base are belong to us!";
    NSString *shortContext = @".";
    NSString *longContext = @"You have no chance to survive make your time. Ha ha ha ha ...";

    NSData *encryptedShort = [cell encrypt:[message dataUsingEncoding:NSUTF8StringEncoding]
                                   context:[shortContext dataUsingEncoding:NSUTF8StringEncoding]];

    NSData *encryptedLong = [cell encrypt:[message dataUsingEncoding:NSUTF8StringEncoding]
                                  context:[longContext dataUsingEncoding:NSUTF8StringEncoding]];

    // Context is not (directly) included into encrypted message.
    XCTAssertEqual(encryptedShort.length, encryptedLong.length);
}

- (void)testContextSignificance
{
    TSCellContextImprint *cell = [[TSCellContextImprint alloc] initWithKey:TSGenerateSymmetricKey()];
    NSData *message = [@"All your base are belong to us!" dataUsingEncoding:NSUTF8StringEncoding];
    NSData *correctContext = [@"We are CATS" dataUsingEncoding:NSUTF8StringEncoding];
    NSData *incorrectContext = [@"Captain !!" dataUsingEncoding:NSUTF8StringEncoding];
    NSError *error;

    NSData *encrypted = [cell encrypt:message context:correctContext];

    // You can use a different context to decrypt data, but you'll get garbage.
    NSData *decrypted = [cell decrypt:encrypted context:incorrectContext error:&error];
    XCTAssertNotNil(decrypted);
    XCTAssertNil(error);
    XCTAssertFalse([decrypted isEqualToData:message]);
    XCTAssertEqual(decrypted.length, message.length);

    // Only the original context will work.
    error = nil;
    decrypted = [cell decrypt:encrypted context:correctContext error:&error];
    XCTAssertNotNil(decrypted);
    XCTAssertNil(error);
    XCTAssert([decrypted isEqualToData:message]);
}

- (void)testNoDetectCorruptedData
{
    TSCellContextImprint *cell = [[TSCellContextImprint alloc] initWithKey:TSGenerateSymmetricKey()];
    NSData *message = [@"All your base are belong to us!" dataUsingEncoding:NSUTF8StringEncoding];
    NSData *context = [@"For great justice" dataUsingEncoding:NSUTF8StringEncoding];
    NSError *error;

    NSData *encrypted = [cell encrypt:message context:context];

    NSMutableData *corrupted = [NSMutableData dataWithData:encrypted];
    // Invert every odd byte, this will surely break the message.
    uint8_t *bytes = corrupted.mutableBytes;
    for (NSUInteger i = 1; i < corrupted.length; i += 2) {
        bytes[i] = ~bytes[i];
    }

    NSData *decrypted = [cell decrypt:corrupted context:context error:&error];
    XCTAssertNotNil(decrypted);
    XCTAssertNil(error);

    // Decrypts successfully but the content is garbage.
    XCTAssertFalse([decrypted isEqualToData:message]);
}

- (void)testNoDetectTruncatedData
{
    TSCellContextImprint *cell = [[TSCellContextImprint alloc] initWithKey:TSGenerateSymmetricKey()];
    NSData *message = [@"All your base are belong to us!" dataUsingEncoding:NSUTF8StringEncoding];
    NSData *context = [@"For great justice" dataUsingEncoding:NSUTF8StringEncoding];
    NSError *error;

    NSData *encrypted = [cell encrypt:message context:context];

    NSMutableData *truncated = [NSMutableData dataWithData:encrypted];
    [truncated setLength:truncated.length - 1];

    NSData *decrypted = [cell decrypt:truncated context:context error:&error];
    XCTAssertNotNil(decrypted);
    XCTAssertNil(error);

    // Decrypts successfully but the content is garbage.
    XCTAssertFalse([decrypted isEqualToData:message]);
    XCTAssertNotEqual(decrypted.length, message.length);
}

- (void)testDetectExtendedData
{
    TSCellContextImprint *cell = [[TSCellContextImprint alloc] initWithKey:TSGenerateSymmetricKey()];
    NSData *message = [@"All your base are belong to us!" dataUsingEncoding:NSUTF8StringEncoding];
    NSData *context = [@"For great justice" dataUsingEncoding:NSUTF8StringEncoding];
    NSError *error;

    NSData *encrypted = [cell encrypt:message context:context];

    NSMutableData *extended = [NSMutableData dataWithData:encrypted];
    [extended setLength:extended.length + 1];

    NSData *decrypted = [cell decrypt:extended context:context error:&error];
    XCTAssertNotNil(decrypted);
    XCTAssertNil(error);

    // Decrypts successfully but the content is garbage.
    XCTAssertFalse([decrypted isEqualToData:message]);
    XCTAssertNotEqual(decrypted.length, message.length);
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnonnull"
- (void)testRequiredMessageAndContext
{
    TSCellContextImprint *cell = [[TSCellContextImprint alloc] initWithKey:TSGenerateSymmetricKey()];
    NSData *message = [@"All your base are belong to us!" dataUsingEncoding:NSUTF8StringEncoding];
    NSData *context = [@"For great justice" dataUsingEncoding:NSUTF8StringEncoding];
    NSError *error;

    error = nil;
    XCTAssertNil([cell encrypt:nil context:context error:&error]);
    XCTAssertNotNil(error);

    error = nil;
    XCTAssertNil([cell encrypt:[NSData new] context:context error:&error]);
    XCTAssertNotNil(error);

    error = nil;
    XCTAssertNil([cell encrypt:message context:nil error:&error]);
    XCTAssertNotNil(error);

    error = nil;
    XCTAssertNil([cell encrypt:message context:[NSData new] error:&error]);
    XCTAssertNotNil(error);

    error = nil;
    XCTAssertNil([cell decrypt:nil context:context error:&error]);
    XCTAssertNotNil(error);

    error = nil;
    XCTAssertNil([cell decrypt:[NSData new] context:context error:&error]);
    XCTAssertNotNil(error);

    error = nil;
    XCTAssertNil([cell decrypt:message context:nil error:&error]);
    XCTAssertNotNil(error);

    error = nil;
    XCTAssertNil([cell decrypt:message context:[NSData new] error:&error]);
    XCTAssertNotNil(error);
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated"
- (void)testOldAPI
{
    TSCellContextImprint *cell = [[TSCellContextImprint alloc] initWithKey:TSGenerateSymmetricKey()];
    NSData *message = [@"All your base are belong to us!" dataUsingEncoding:NSUTF8StringEncoding];
    NSData *context = [@"For great justice" dataUsingEncoding:NSUTF8StringEncoding];
    NSError *error;

    NSData *encrypted = [cell wrapData:message context:context error:&error];
    XCTAssertNotNil(encrypted);
    XCTAssertNil(error);

    NSData *decrypted = [cell decrypt:encrypted context:context error:&error];
    XCTAssertNotNil(decrypted);
    XCTAssertNil(error);
    XCTAssert([decrypted isEqualToData:message]);

    encrypted = [cell encrypt:message context:context error:&error];
    XCTAssertNotNil(encrypted);
    XCTAssertNil(error);

    decrypted = [cell unwrapData:encrypted context:context error:&error];
    XCTAssertNotNil(decrypted);
    XCTAssertNil(error);
    XCTAssert([decrypted isEqualToData:message]);
}
#pragma clang diagnostic pop

- (void)testKeyWipedOnDealloc
{
    NSData *key = TSGenerateSymmetricKey();
    {
        NSData *cellKey;
        {
            TSCellContextImprint *cell = [[TSCellContextImprint alloc] initWithKey:key];

            cellKey = cell.key;

            XCTAssertNotNil(cellKey);
            XCTAssertNotEqual(cellKey.length, 0);
            XCTAssertNotEqual(cellKey, key, @"Secure Cell makes a copy of the key");
            XCTAssert([cellKey isEqualToData:key]);
        }
        // The key is wiped after the cell has left the scope and has been deallocated.
        XCTAssertNotNil(cellKey);
        XCTAssertEqual(cellKey.length, 0);
        XCTAssertFalse([cellKey isEqualToData:key]);
    }
}

@end
