//
//  SecureCellTestsSwift.swift
//  objthemis
//
//  Created by Anastasiia on 9/20/17.
//

import XCTest

// MARK: - Key generation

class SecureCellKeygenSwift: XCTestCase {

    let defaultKeyLength = 32

    func testKeyGeneration() {
        let masterKey = TSGenerateSymmetricKey()

        XCTAssertNotNil(masterKey, "TSGenerateSymmetricKey() should not fail")
        XCTAssertEqual(masterKey!.count, defaultKeyLength, "generated key must be not empty")
    }
}

// MARK: - Seal Mode

class SecureCellSealSwift: XCTestCase {

    func testInitWithGenerated() {
        let cell = TSCellSeal(key: TSGenerateSymmetricKey()!)
        XCTAssertNotNil(cell)
    }

    func testInitWithFixed() {
        let masterKeyString = "UkVDMgAAAC13PCVZAKOczZXUpvkhsC+xvwWnv3CLmlG0Wzy8ZBMnT+2yx/dg"
        let masterKeyData = Data(base64Encoded: masterKeyString)!

        let cell = TSCellSeal(key: masterKeyData)
        XCTAssertNotNil(cell)
    }

    func testInitWithEmpty() {
        XCTAssertNil(TSCellSeal(key: Data()))
    }

    func testRoundtrip() {
        let cell = TSCellSeal(key: TSGenerateSymmetricKey()!)!
        let message = "All your base are belong to us!"
        let context = "For great justice"

        let encrypted = try? cell.encrypt(message.data(using: .utf8)!,
                                          context: context.data(using: .utf8)!)
        XCTAssertNotNil(encrypted)

        let decrypted = try? cell.decrypt(encrypted!,
                                          context: context.data(using: .utf8)!)
        XCTAssertNotNil(decrypted)

        let decryptedMessage = String(data: decrypted!, encoding: .utf8)
        XCTAssertNotNil(decryptedMessage)

        XCTAssertEqual(decryptedMessage, message)
    }

    func testDataLengthExtension() {
        let cell = TSCellSeal(key: TSGenerateSymmetricKey()!)!
        let message = "All your base are belong to us!".data(using: .utf8)!

        let encrypted = try! cell.encrypt(message)

        XCTAssertGreaterThan(encrypted.count, message.count)
    }

    func testContextInclusion() {
        let cell = TSCellSeal(key: TSGenerateSymmetricKey()!)!
        let message = "All your base are belong to us!".data(using: .utf8)!
        let shortContext = ".".data(using: .utf8)!
        let longContext = "You have no chance to survive make your time. Ha ha ha ha ...".data(using: .utf8)!

        let encryptedShort = try! cell.encrypt(message, context: shortContext)
        let encryptedLong = try! cell.encrypt(message, context: longContext)

        // Context is not (directly) included into encrypted message.
        XCTAssertEqual(encryptedShort.count, encryptedLong.count)
    }

    func testWithoutContext() {
        let cell = TSCellSeal(key: TSGenerateSymmetricKey()!)!
        let message = "All your base are belong to us!".data(using: .utf8)!

        // Absent, empty, or nil context are all the same.
        let encrypted1 = try! cell.encrypt(message)
        let encrypted2 = try! cell.encrypt(message, context: nil)
        let encrypted3 = try! cell.encrypt(message, context: Data())

        XCTAssertEqual(message, try! cell.decrypt(encrypted1))
        XCTAssertEqual(message, try! cell.decrypt(encrypted2))
        XCTAssertEqual(message, try! cell.decrypt(encrypted3))

        XCTAssertEqual(message, try! cell.decrypt(encrypted1, context:nil))
        XCTAssertEqual(message, try! cell.decrypt(encrypted2, context:nil))
        XCTAssertEqual(message, try! cell.decrypt(encrypted3, context:nil))

        XCTAssertEqual(message, try! cell.decrypt(encrypted1, context:Data()))
        XCTAssertEqual(message, try! cell.decrypt(encrypted2, context:Data()))
        XCTAssertEqual(message, try! cell.decrypt(encrypted3, context:Data()))
    }

    func testContextSignificance() {
        let cell = TSCellSeal(key: TSGenerateSymmetricKey()!)!
        let message = "All your base are belong to us!".data(using: .utf8)!
        let correctContext = "We are CATS".data(using: .utf8)!
        let incorrectContext = "Captain !!".data(using: .utf8)!

        let encrypted = try! cell.encrypt(message, context: correctContext)

        // You cannot use a different context to decrypt data.
        XCTAssertThrowsError(try cell.decrypt(encrypted, context: incorrectContext))

        // Only the original context will work.
        let decrypted = try! cell.decrypt(encrypted, context: correctContext)
        XCTAssertEqual(decrypted, message)
    }

    func testDetectCorruptedData() {
        let cell = TSCellSeal(key: TSGenerateSymmetricKey()!)!
        let message = "All your base are belong to us!".data(using: .utf8)!

        let encrypted = try! cell.encrypt(message)

        var corrupted = Data(encrypted)
        // Invert every odd byte, this will surely break the message.
        for i in corrupted.indices where i % 2 == 1 {
            corrupted[i] = ~corrupted[i]
        }

        XCTAssertThrowsError(try cell.decrypt(corrupted))
    }

    func testDetectTruncatedData() {
        let cell = TSCellSeal(key: TSGenerateSymmetricKey()!)!
        let message = "All your base are belong to us!".data(using: .utf8)!

        let encrypted = try! cell.encrypt(message)

        let truncated = encrypted.dropLast(1)

        XCTAssertThrowsError(try cell.decrypt(truncated))
    }

    func testDetectExtendedData() {
        let cell = TSCellSeal(key: TSGenerateSymmetricKey()!)!
        let message = "All your base are belong to us!".data(using: .utf8)!

        let encrypted = try! cell.encrypt(message)

        var extended = Data(encrypted)
        extended.append(0)

        XCTAssertThrowsError(try cell.decrypt(extended))
    }

    func testEmptyMessage() {
        let cell = TSCellSeal(key: TSGenerateSymmetricKey()!)!

        XCTAssertThrowsError(try cell.encrypt(Data()))
        XCTAssertThrowsError(try cell.decrypt(Data()))
    }

    @available(*, deprecated)
    func testOldAPI() {
        let cell = TSCellSeal(key: TSGenerateSymmetricKey()!)!
        let message = "All your base are belong to us!".data(using: .utf8)!
        let context = "For great justice".data(using: .utf8)!
        var encrypted, decrypted: Data?

        encrypted = try? cell.wrap(message, context: context)
        XCTAssertNotNil(encrypted)

        decrypted = try? cell.decrypt(encrypted!, context: context)
        XCTAssertNotNil(decrypted)
        XCTAssertEqual(decrypted, message)

        encrypted = try? cell.encrypt(message, context: context)
        XCTAssertNotNil(encrypted)

        decrypted = try? cell.unwrapData(encrypted!, context: context)
        XCTAssertNotNil(decrypted)
        XCTAssertEqual(decrypted, message)
    }

    @available(*, deprecated)
    func testOldAPIWithoutContext() {
        let cell = TSCellSeal(key: TSGenerateSymmetricKey()!)!
        let message = "All your base are belong to us!".data(using: .utf8)!
        var encrypted, decrypted: Data?

        encrypted = try? cell.wrap(message)
        XCTAssertNotNil(encrypted)

        decrypted = try? cell.decrypt(encrypted!)
        XCTAssertNotNil(decrypted)
        XCTAssertEqual(decrypted, message)

        encrypted = try? cell.encrypt(message)
        XCTAssertNotNil(encrypted)

        decrypted = try? cell.unwrapData(encrypted!)
        XCTAssertNotNil(decrypted)
        XCTAssertEqual(decrypted, message)
    }

    func testKeyNotWipedOnDestruction() {
        let key = TSGenerateSymmetricKey()!
        do {
            let cellKey: Data
            do {
                let cell = TSCellSeal(key: key)!

                cellKey = cell.key

                XCTAssertNotEqual(cellKey.count, 0)
                XCTAssertEqual(cellKey, key)
            }
            // In Swift "Data" is a value type so "cellKey" is a copy of the key
            // which is not wiped by Secure Cell.
            XCTAssertEqual(cellKey, key)
        }
    }
}

// MARK: - Seal Mode (passphrase)

class SecureCellSealPassphraseSwift: XCTestCase {

    func testInitWithFixed() {
        let cell = TSCellSeal(passphrase: "secret")
        XCTAssertNotNil(cell)
    }

    func testInitWithRawData() {
        let cell = TSCellSeal(passphraseData: "secret".data(using: .ascii)!)
        XCTAssertNotNil(cell)
    }

    func testInitWithEmpty() {
        XCTAssertNil(TSCellSeal(passphrase: ""))
        XCTAssertNil(TSCellSeal(passphraseData: Data()))
    }

    func testRoundtrip() {
        let cell = TSCellSeal(passphrase: "secret")!
        let message = "All your base are belong to us!"
        let context = "For great justice"

        let encrypted = try? cell.encrypt(message.data(using: .utf8)!,
                                          context: context.data(using: .utf8)!)
        XCTAssertNotNil(encrypted)

        let decrypted = try? cell.decrypt(encrypted!,
                                          context: context.data(using: .utf8)!)
        XCTAssertNotNil(decrypted)

        let decryptedMessage = String(data: decrypted!, encoding: .utf8)
        XCTAssertNotNil(decryptedMessage)

        XCTAssertEqual(decryptedMessage, message)
    }

    func testDataLengthExtension() {
        let cell = TSCellSeal(passphrase: "secret")!
        let message = "All your base are belong to us!".data(using: .utf8)!

        let encrypted = try! cell.encrypt(message)

        XCTAssertGreaterThan(encrypted.count, message.count)
    }

    func testContextInclusion() {
        let cell = TSCellSeal(passphrase: "secret")!
        let message = "All your base are belong to us!".data(using: .utf8)!
        let shortContext = ".".data(using: .utf8)!
        let longContext = "You have no chance to survive make your time. Ha ha ha ha ...".data(using: .utf8)!

        let encryptedShort = try! cell.encrypt(message, context: shortContext)
        let encryptedLong = try! cell.encrypt(message, context: longContext)

        // Context is not (directly) included into encrypted message.
        XCTAssertEqual(encryptedShort.count, encryptedLong.count)
    }

    func testWithoutContext() {
        let cell = TSCellSeal(passphrase: "secret")!
        let message = "All your base are belong to us!".data(using: .utf8)!

        // Absent, empty, or nil context are all the same.
        let encrypted1 = try! cell.encrypt(message)
        let encrypted2 = try! cell.encrypt(message, context: nil)
        let encrypted3 = try! cell.encrypt(message, context: Data())

        XCTAssertEqual(message, try! cell.decrypt(encrypted1))
        XCTAssertEqual(message, try! cell.decrypt(encrypted2))
        XCTAssertEqual(message, try! cell.decrypt(encrypted3))

        XCTAssertEqual(message, try! cell.decrypt(encrypted1, context:nil))
        XCTAssertEqual(message, try! cell.decrypt(encrypted2, context:nil))
        XCTAssertEqual(message, try! cell.decrypt(encrypted3, context:nil))

        XCTAssertEqual(message, try! cell.decrypt(encrypted1, context:Data()))
        XCTAssertEqual(message, try! cell.decrypt(encrypted2, context:Data()))
        XCTAssertEqual(message, try! cell.decrypt(encrypted3, context:Data()))
    }

    func testContextSignificance() {
        let cell = TSCellSeal(passphrase: "secret")!
        let message = "All your base are belong to us!".data(using: .utf8)!
        let correctContext = "We are CATS".data(using: .utf8)!
        let incorrectContext = "Captain !!".data(using: .utf8)!

        let encrypted = try! cell.encrypt(message, context: correctContext)

        // You cannot use a different context to decrypt data.
        XCTAssertThrowsError(try cell.decrypt(encrypted, context: incorrectContext))

        // Only the original context will work.
        let decrypted = try! cell.decrypt(encrypted, context: correctContext)
        XCTAssertEqual(decrypted, message)
    }

    func testDetectCorruptedData() {
        let cell = TSCellSeal(passphrase: "secret")!
        let message = "All your base are belong to us!".data(using: .utf8)!

        let encrypted = try! cell.encrypt(message)

        var corrupted = Data(encrypted)
        // Invert every odd byte, this will surely break the message.
        for i in corrupted.indices where i % 2 == 1 {
            corrupted[i] = ~corrupted[i]
        }

        XCTAssertThrowsError(try cell.decrypt(corrupted))
    }

    func testDetectTruncatedData() {
        let cell = TSCellSeal(passphrase: "secret")!
        let message = "All your base are belong to us!".data(using: .utf8)!

        let encrypted = try! cell.encrypt(message)

        let truncated = encrypted.dropLast(1)

        XCTAssertThrowsError(try cell.decrypt(truncated))
    }

    func testDetectExtendedData() {
        let cell = TSCellSeal(passphrase: "secret")!
        let message = "All your base are belong to us!".data(using: .utf8)!

        let encrypted = try! cell.encrypt(message)

        var extended = Data(encrypted)
        extended.append(0)

        XCTAssertThrowsError(try cell.decrypt(extended))
    }

    func testEmptyMessage() {
        let cell = TSCellSeal(passphrase: "secret")!

        XCTAssertThrowsError(try cell.encrypt(Data()))
        XCTAssertThrowsError(try cell.decrypt(Data()))
    }

    func testKeyIncompatibility() {
        // Passphrases are not keys. Keys are not passphrases.
        let secret = TSGenerateSymmetricKey()!
        let cellMK = TSCellSeal(key: secret)!
        let cellPW = TSCellSeal(passphraseData: secret)!
        let message = "All your base are belong to us!".data(using: .utf8)!

        let encrypted = try! cellMK.encrypt(message)
        let decrypted = try? cellPW.decrypt(encrypted)

        XCTAssertNil(decrypted)
    }

    func testEncodingDefault() {
        // Passphrases are encoded in UTF-8 by default.
        let secret = "暗号"
        let cellA = TSCellSeal(passphrase: secret)!
        let cellB = TSCellSeal(passphraseData: secret.data(using: .utf8)!)!
        let message = "All your base are belong to us!".data(using: .utf8)!

        let encrypted = try! cellA.encrypt(message)
        let decrypted = try! cellB.decrypt(encrypted)

        XCTAssertEqual(decrypted, message)
    }

    func testEncodingSpecific() {
        let cell = TSCellSeal(passphraseData: "secret".data(using: .utf16BigEndian)!)!
        let message = "All your base are belong to us!".data(using: .utf8)!

        // Message encrypted by PyThemis
        let encrypted = Data(base64Encoded: "AAEBQQwAAAAQAAAAHwAAABYAAAB1n+2bNaaCXaILGSYz9+eYfr36pZ41ItCd3ShbQA0DABAAGEDx++sRQd2IQfIyiToDl7No5mPoKYigCL3HyQedivZ4tMYrFx3FyTZLExiqeJw=")!

        let decrypted = try! cell.decrypt(encrypted)

        XCTAssertEqual(decrypted, message)
    }
}

// MARK: - Token Protect

class SecureCellTokenProtectSwift: XCTestCase {

    func testInitWithGenerated() {
        let cell = TSCellToken(key: TSGenerateSymmetricKey()!)
        XCTAssertNotNil(cell)
    }

    func testInitWithFixed() {
        let masterKeyString = "UkVDMgAAAC13PCVZAKOczZXUpvkhsC+xvwWnv3CLmlG0Wzy8ZBMnT+2yx/dg"
        let masterKeyData = Data(base64Encoded: masterKeyString)!

        let cell = TSCellToken(key: masterKeyData)
        XCTAssertNotNil(cell)
    }

    func testInitWithEmpty() {
        XCTAssertNil(TSCellToken(key: Data()))
    }

    func testRoundtrip() {
        let cell = TSCellToken(key: TSGenerateSymmetricKey()!)!
        let message = "Roses are grey. Violets are grey."
        let context = "I'm a dog"

        let result = try? cell.encrypt(message.data(using: .utf8)!,
                                       context: context.data(using: .utf8)!)
        XCTAssertNotNil(result?.encrypted)
        XCTAssertNotNil(result?.token)

        let decrypted = try? cell.decrypt(result!.encrypted,
                                          token: result!.token,
                                          context: context.data(using: .utf8)!)
        XCTAssertNotNil(decrypted)

        let decryptedMessage = String(data: decrypted!, encoding: .utf8)
        XCTAssertNotNil(decryptedMessage)

        XCTAssertEqual(decryptedMessage, message)
    }

    func testDataLengthPreservation() {
        let cell = TSCellToken(key: TSGenerateSymmetricKey()!)!
        let message = "Roses are grey. Violets are grey.".data(using: .utf8)!

        let result = try! cell.encrypt(message)

        XCTAssertEqual(result.encrypted.count, message.count)
        XCTAssertFalse(result.token.isEmpty)
    }

    func testContextInclusion() {
        let cell = TSCellToken(key: TSGenerateSymmetricKey()!)!
        let message = "Roses are grey. Violets are grey.".data(using: .utf8)!
        let shortContext = "I'm a dog".data(using: .utf8)!
        let longContext = "This is why cats are ultimately superior creatures.".data(using: .utf8)!

        let resultShort = try! cell.encrypt(message, context: shortContext)
        let resultLong = try! cell.encrypt(message, context: longContext)

        // Context is not (directly) included into encrypted message.
        XCTAssertEqual(resultShort.encrypted.count, resultShort.encrypted.count)
        XCTAssertEqual(resultShort.token.count, resultLong.token.count)
    }

    func testWithoutContext() {
        let cell = TSCellToken(key: TSGenerateSymmetricKey()!)!
        let message = "Roses are grey. Violets are grey.".data(using: .utf8)!

        // Absent, empty, or nil context are all the same.
        let result1 = try! cell.encrypt(message)
        let result2 = try! cell.encrypt(message, context: nil)
        let result3 = try! cell.encrypt(message, context: Data())

        XCTAssertEqual(message, try! cell.decrypt(result1.encrypted, token: result1.token))
        XCTAssertEqual(message, try! cell.decrypt(result2.encrypted, token: result2.token))
        XCTAssertEqual(message, try! cell.decrypt(result3.encrypted, token: result3.token))

        XCTAssertEqual(message, try! cell.decrypt(result1.encrypted, token: result1.token, context:nil))
        XCTAssertEqual(message, try! cell.decrypt(result2.encrypted, token: result2.token, context:nil))
        XCTAssertEqual(message, try! cell.decrypt(result3.encrypted, token: result3.token, context:nil))

        XCTAssertEqual(message, try! cell.decrypt(result1.encrypted, token: result1.token, context:Data()))
        XCTAssertEqual(message, try! cell.decrypt(result2.encrypted, token: result2.token, context:Data()))
        XCTAssertEqual(message, try! cell.decrypt(result3.encrypted, token: result3.token, context:Data()))
    }

    func testContextSignificance() {
        let cell = TSCellToken(key: TSGenerateSymmetricKey()!)!
        let message = "Roses are grey. Violets are grey.".data(using: .utf8)!
        let correctContext = "I'm a dog".data(using: .utf8)!
        let incorrectContext = "Volcano eruption".data(using: .utf8)!

        let result = try! cell.encrypt(message, context: correctContext)

        // You cannot use a different context to decrypt data.
        XCTAssertThrowsError(try cell.decrypt(result.encrypted,
                                              token: result.token,
                                              context: incorrectContext))

        // Only the original context will work.
        let decrypted = try! cell.decrypt(result.encrypted,
                                          token: result.token,
                                          context: correctContext)
        XCTAssertEqual(decrypted, message)
    }

    func testTokenSignificance() {
        let cell = TSCellToken(key: TSGenerateSymmetricKey()!)!
        let message = "Roses are grey. Violets are grey.".data(using: .utf8)!

        let result1 = try! cell.encrypt(message)
        let result2 = try! cell.encrypt(message)

        // You cannot use a different token to decrypt data.
        XCTAssertThrowsError(try cell.decrypt(result1.encrypted, token: result2.token))
        XCTAssertThrowsError(try cell.decrypt(result2.encrypted, token: result1.token))

        // Only the original context will work.
        XCTAssertNoThrow(try cell.decrypt(result1.encrypted, token: result1.token))
        XCTAssertNoThrow(try cell.decrypt(result2.encrypted, token: result2.token))
    }

    func testDetectCorruptedData() {
        let cell = TSCellToken(key: TSGenerateSymmetricKey()!)!
        let message = "Roses are grey. Violets are grey.".data(using: .utf8)!

        let result = try! cell.encrypt(message)

        var corrupted = Data(result.encrypted)
        // Invert every odd byte, this will surely break the message.
        for i in corrupted.indices where i % 2 == 1 {
            corrupted[i] = ~corrupted[i]
        }

        XCTAssertThrowsError(try cell.decrypt(corrupted, token: result.token))
    }

    func testDetectTruncatedData() {
        let cell = TSCellToken(key: TSGenerateSymmetricKey()!)!
        let message = "Roses are grey. Violets are grey.".data(using: .utf8)!

        let result = try! cell.encrypt(message)

        let truncated = result.encrypted.dropLast(1)

        XCTAssertThrowsError(try cell.decrypt(truncated, token: result.token))
    }

    func testDetectExtendedData() {
        let cell = TSCellToken(key: TSGenerateSymmetricKey()!)!
        let message = "Roses are grey. Violets are grey.".data(using: .utf8)!

        let result = try! cell.encrypt(message)

        var extended = Data(result.encrypted)
        extended.append(0)

        XCTAssertThrowsError(try cell.decrypt(extended, token: result.token))
    }

    func testDetectCorruptedToken() {
        let cell = TSCellToken(key: TSGenerateSymmetricKey()!)!
        let message = "Roses are grey. Violets are grey.".data(using: .utf8)!

        let result = try! cell.encrypt(message)

        var corruptedToken = Data(result.token)
        // Invert every odd byte, this will surely break the token.
        for i in corruptedToken.indices where i % 2 == 1 {
            corruptedToken[i] = ~corruptedToken[i]
        }

        XCTAssertThrowsError(try cell.decrypt(result.encrypted, token: corruptedToken))
    }

    func testDetectTruncatedToken() {
        let cell = TSCellToken(key: TSGenerateSymmetricKey()!)!
        let message = "Roses are grey. Violets are grey.".data(using: .utf8)!

        let result = try! cell.encrypt(message)

        let truncatedToken = result.token.dropLast(1)

        XCTAssertThrowsError(try cell.decrypt(result.encrypted, token: truncatedToken))
    }

    func testDetectExtendedToken() {
        let cell = TSCellToken(key: TSGenerateSymmetricKey()!)!
        let message = "Roses are grey. Violets are grey.".data(using: .utf8)!

        let result = try! cell.encrypt(message)

        var extendedToken = Data(result.token)
        extendedToken.append(0)

        // Current implementation of Secure Cell allows the token to be overlong.
        // Extra data is simply ignored.
        let decrypted = try! cell.decrypt(result.encrypted, token: extendedToken)
        XCTAssertEqual(decrypted, message)
    }

    func testSwapTokenAndData() {
        let cell = TSCellToken(key: TSGenerateSymmetricKey()!)!
        let message = "Roses are grey. Violets are grey.".data(using: .utf8)!

        let result = try! cell.encrypt(message)

        XCTAssertThrowsError(try cell.decrypt(result.token, token: result.encrypted))
    }

    func testEmptyMessageOrToken() {
        let cell = TSCellToken(key: TSGenerateSymmetricKey()!)!

        XCTAssertThrowsError(try cell.encrypt(Data()))

        let result = try! cell.encrypt(TSGenerateSymmetricKey()!)

        XCTAssertThrowsError(try cell.decrypt(Data(), token: result.token))
        XCTAssertThrowsError(try cell.decrypt(result.encrypted, token: Data()))
    }

    @available(*, deprecated)
    func testOldAPI() {
        let cell = TSCellToken(key: TSGenerateSymmetricKey()!)!
        let message = "Roses are grey. Violets are grey.".data(using: .utf8)!
        let context = "I'm a dog".data(using: .utf8)!
        var decrypted: Data?

        let encryptedData: TSCellTokenEncryptedData? =
            try? cell.wrap(message, context: context)
        XCTAssertNotNil(encryptedData)
        XCTAssertNotNil(encryptedData?.cipherText)
        XCTAssertNotNil(encryptedData?.token)

        decrypted = try? cell.decrypt(encryptedData!.cipherText as Data,
                                      token: encryptedData!.token as Data,
                                      context: context)
        XCTAssertNotNil(decrypted)
        XCTAssertEqual(decrypted, message)

        let result = try? cell.encrypt(message, context: context)
        XCTAssertNotNil(result)
        XCTAssertNotNil(result?.cipherText)
        XCTAssertNotNil(result?.token)

        let data = TSCellTokenEncryptedData()
        data.cipherText = NSMutableData(data: result!.cipherText)
        data.token = NSMutableData(data: result!.token)
        decrypted = try? cell.unwrapData(data, context: context)
        XCTAssertNotNil(decrypted)
        XCTAssertEqual(decrypted, message)
    }

    @available(*, deprecated)
    func testOldAPIWithoutContext() {
        let cell = TSCellToken(key: TSGenerateSymmetricKey()!)!
        let message = "Roses are grey. Violets are grey.".data(using: .utf8)!
        var decrypted: Data?

        let encryptedData = try? cell.wrap(message)
        XCTAssertNotNil(encryptedData)
        XCTAssertNotNil(encryptedData?.cipherText)
        XCTAssertNotNil(encryptedData?.token)

        decrypted = try? cell.decrypt(encryptedData!.cipherText as Data,
                                      token: encryptedData!.token as Data)
        XCTAssertNotNil(decrypted)
        XCTAssertEqual(decrypted, message)

        let result = try? cell.encrypt(message)
        XCTAssertNotNil(result)
        XCTAssertNotNil(result?.cipherText)
        XCTAssertNotNil(result?.token)

        let data = TSCellTokenEncryptedData()
        data.cipherText = NSMutableData(data: result!.cipherText)
        data.token = NSMutableData(data: result!.token)
        decrypted = try? cell.unwrapData(data)
        XCTAssertNotNil(decrypted)
        XCTAssertEqual(decrypted, message)
    }

    func testKeyNotWipedOnDestruction() {
        let key = TSGenerateSymmetricKey()!
        do {
            let cellKey: Data
            do {
                let cell = TSCellToken(key: key)!

                cellKey = cell.key

                XCTAssertNotEqual(cellKey.count, 0)
                XCTAssertEqual(cellKey, key)
            }
            // In Swift "Data" is a value type so "cellKey" is a copy of the key
            // which is not wiped by Secure Cell.
            XCTAssertEqual(cellKey, key)
        }
    }
}

// MARK: - Context Imprint

class SecureCellContextImprintSwift: XCTestCase {

    func testInitWithGenerated() {
        let cell = TSCellContextImprint(key: TSGenerateSymmetricKey()!)
        XCTAssertNotNil(cell)
    }

    func testInitWithFixed() {
        let masterKeyString = "UkVDMgAAAC13PCVZAKOczZXUpvkhsC+xvwWnv3CLmlG0Wzy8ZBMnT+2yx/dg"
        let masterKeyData = Data(base64Encoded: masterKeyString)!

        let cell = TSCellContextImprint(key: masterKeyData)
        XCTAssertNotNil(cell)
    }

    func testInitWithEmpty() {
        XCTAssertNil(TSCellContextImprint(key: Data()))
    }

    func testRoundtrip() {
        let cell = TSCellContextImprint(key: TSGenerateSymmetricKey()!)!
        let message = "All your base are belong to us!"
        let context = "For great justice"

        let encrypted = try? cell.encrypt(message.data(using: .utf8)!,
                                          context: context.data(using: .utf8)!)
        XCTAssertNotNil(encrypted)

        let decrypted = try? cell.decrypt(encrypted!,
                                          context: context.data(using: .utf8)!)
        XCTAssertNotNil(decrypted)

        let decryptedMessage = String(data: decrypted!, encoding: .utf8)
        XCTAssertNotNil(decryptedMessage)

        XCTAssertEqual(decryptedMessage, message)
    }

    func testDataLengthPreservation() {
        let cell = TSCellContextImprint(key: TSGenerateSymmetricKey()!)!
        let message = "All your base are belong to us!".data(using: .utf8)!
        let context = "For great justice".data(using: .utf8)!

        let encrypted = try! cell.encrypt(message, context: context)

        XCTAssertEqual(encrypted.count, message.count)
    }

    func testContextInclusion() {
        let cell = TSCellContextImprint(key: TSGenerateSymmetricKey()!)!
        let message = "All your base are belong to us!".data(using: .utf8)!
        let shortContext = ".".data(using: .utf8)!
        let longContext = "You have no chance to survive make your time. Ha ha ha ha ...".data(using: .utf8)!

        let encryptedShort = try! cell.encrypt(message, context: shortContext)
        let encryptedLong = try! cell.encrypt(message, context: longContext)

        // Context is not (directly) included into encrypted message.
        XCTAssertEqual(encryptedShort.count, encryptedLong.count)
    }

    func testContextSignificance() {
        let cell = TSCellContextImprint(key: TSGenerateSymmetricKey()!)!
        let message = "All your base are belong to us!".data(using: .utf8)!
        let correctContext = "We are CATS".data(using: .utf8)!
        let incorrectContext = "Captain !!".data(using: .utf8)!

        let encrypted = try! cell.encrypt(message, context: correctContext)

        // You can use a different context to decrypt data, but you'll get garbage.
        let decryptedGarbage = try? cell.decrypt(encrypted, context: incorrectContext)
        XCTAssertNotNil(decryptedGarbage)
        XCTAssertNotEqual(decryptedGarbage!, message)
        XCTAssertEqual(decryptedGarbage!.count, message.count)

        // Only the original context will work.
        let decrypted = try? cell.decrypt(encrypted, context: correctContext)
        XCTAssertNotNil(decrypted)
        XCTAssertEqual(decrypted!, message)
    }

    func testNoDetectCorruptedData() {
        let cell = TSCellContextImprint(key: TSGenerateSymmetricKey()!)!
        let message = "All your base are belong to us!".data(using: .utf8)!
        let context = "For great justice".data(using: .utf8)!

        let encrypted = try! cell.encrypt(message, context: context)

        var corrupted = Data(encrypted)
        // Invert every odd byte, this will surely break the message.
        for i in corrupted.indices where i % 2 == 1 {
            corrupted[i] = ~corrupted[i]
        }

        // Decrypts successfully but the content is garbage.
        let decrypted = try? cell.decrypt(corrupted, context: context)
        XCTAssertNotNil(decrypted)
        XCTAssertNotEqual(decrypted!, message)
        XCTAssertEqual(decrypted!.count, message.count)
    }

    func testNoDetectTruncatedData() {
        let cell = TSCellContextImprint(key: TSGenerateSymmetricKey()!)!
        let message = "All your base are belong to us!".data(using: .utf8)!
        let context = "For great justice".data(using: .utf8)!

        let encrypted = try! cell.encrypt(message, context: context)

        let truncated = encrypted.dropLast(1)

        // Decrypts successfully but the content is garbage.
        let decrypted = try? cell.decrypt(truncated, context: context)
        XCTAssertNotNil(decrypted)
        XCTAssertNotEqual(decrypted!, message)
        XCTAssertNotEqual(decrypted!.count, message.count)
    }

    func testDetectExtendedData() {
        let cell = TSCellContextImprint(key: TSGenerateSymmetricKey()!)!
        let message = "All your base are belong to us!".data(using: .utf8)!
        let context = "For great justice".data(using: .utf8)!

        let encrypted = try! cell.encrypt(message, context: context)

        var extended = Data(encrypted)
        extended.append(0)

        // Decrypts successfully but the content is garbage.
        let decrypted = try? cell.decrypt(extended, context: context)
        XCTAssertNotNil(decrypted)
        XCTAssertNotEqual(decrypted!, message)
        XCTAssertNotEqual(decrypted!.count, message.count)
    }

    func testRequiredMessageAndContext() {
        let cell = TSCellContextImprint(key: TSGenerateSymmetricKey()!)!
        let message = "All your base are belong to us!".data(using: .utf8)!
        let context = "For great justice".data(using: .utf8)!

        XCTAssertThrowsError(try cell.encrypt(message, context: Data()))
        XCTAssertThrowsError(try cell.encrypt(Data(), context: context))

        XCTAssertThrowsError(try cell.decrypt(message, context: Data()))
        XCTAssertThrowsError(try cell.decrypt(Data(), context: context))
    }

    @available(*, deprecated)
    func testOldAPI() {
        let cell = TSCellContextImprint(key: TSGenerateSymmetricKey()!)!
        let message = "All your base are belong to us!".data(using: .utf8)!
        let context = "For great justice".data(using: .utf8)!
        var encrypted, decrypted: Data?

        encrypted = try? cell.wrap(message, context: context)
        XCTAssertNotNil(encrypted)

        decrypted = try? cell.decrypt(encrypted!, context: context)
        XCTAssertNotNil(decrypted)
        XCTAssertEqual(decrypted, message)

        encrypted = try? cell.encrypt(message, context: context)
        XCTAssertNotNil(encrypted)

        decrypted = try? cell.unwrapData(encrypted!, context: context)
        XCTAssertNotNil(decrypted)
        XCTAssertEqual(decrypted, message)
    }

    func testKeyNotWipedOnDestruction() {
        let key = TSGenerateSymmetricKey()!
        do {
            let cellKey: Data
            do {
                let cell = TSCellContextImprint(key: key)!

                cellKey = cell.key

                XCTAssertNotEqual(cellKey.count, 0)
                XCTAssertEqual(cellKey, key)
            }
            // In Swift "Data" is a value type so "cellKey" is a copy of the key
            // which is not wiped by Secure Cell.
            XCTAssertEqual(cellKey, key)
        }
    }
}
