//
//  SecureCellTestsSwift.swift
//  objthemis
//
//  Created by Anastasiia on 9/20/17.
//

import XCTest

class SecureCellTestsSwift: XCTestCase {
    
    var masterKeyData: Data!
    
    override func setUp() {
        super.setUp()
        
        self.masterKeyData = generateMasterKey()
    }
    
    func generateMasterKey() -> Data {
        let masterKeyString: String = "UkVDMgAAAC13PCVZAKOczZXUpvkhsC+xvwWnv3CLmlG0Wzy8ZBMnT+2yx/dg"
        let masterKeyData: Data = Data(base64Encoded: masterKeyString, options: .ignoreUnknownCharacters)!
        return masterKeyData
    }
    
    func testSecureCellSealModeInit() {
        let cellSeal: TSCellSeal = TSCellSeal(key: masterKeyData)
        XCTAssertNotNil(cellSeal, "secure cell encrypter (seal mode) should not be created without key")
        
        let cellSealWrong = TSCellSeal(key: nil)
        XCTAssertNil(cellSealWrong, "secure cell encrypter (seal mode) creation error")
    }
    
    func testSecureCellSealModeWithContext() {
        let cellSeal: TSCellSeal = TSCellSeal(key: masterKeyData)
        let message: String = "All your base are belong to us!"
        let context: String = "For great justice"
        
        var encryptedMessage = try? cellSeal.wrap(nil,
                                                  context: context.data(using: .utf8))
        XCTAssertNil(encryptedMessage, "encryption without data-to-encrypt should return nil data")
        
        encryptedMessage = try? cellSeal.wrap(message.data(using: .utf8),
                                              context: context.data(using: .utf8))
        XCTAssertNotNil(encryptedMessage, "encryption with data and context should be successful")
        
        
        var decryptedMessage = try? cellSeal.unwrapData(encryptedMessage!,
                                                  context: nil)
        XCTAssertNil(decryptedMessage, "decryption without context should return nil data")
        
        decryptedMessage = try? cellSeal.unwrapData(encryptedMessage!,
                                                    context: context.data(using: .utf8))
        XCTAssertNotNil(decryptedMessage, "decryption with data and context should be successful")
        
        let resultString = String(data: decryptedMessage!, encoding: .utf8)
        XCTAssertTrue((message == resultString), "initial string and decrypted string should be the same")
    }
    
    func testSecureCellSealModeWithoutContext() {
        let cellSeal: TSCellSeal = TSCellSeal(key: masterKeyData)
        let message: String = "All your base are belong to us!"
        let context: String = "For great justice"
        
        let encryptedMessageNoContext = try? cellSeal.wrap(message.data(using: .utf8),
                                                  context: nil)
        XCTAssertNotNil(encryptedMessageNoContext, "encryption with data and without context should return data")
        
        
        var decryptedMessageNoContext = try? cellSeal.unwrapData(encryptedMessageNoContext!,
                                                        context: context.data(using: .utf8))
        XCTAssertNil(decryptedMessageNoContext, "decrypt data with context that was encypted without context should return nil data")
        
        decryptedMessageNoContext = try? cellSeal.unwrapData(encryptedMessageNoContext!,
                                                    context: nil)
        XCTAssertNotNil(decryptedMessageNoContext, "decryption without context should be successful")
        
        let resultString = String(data: decryptedMessageNoContext!, encoding: .utf8)
        XCTAssertTrue((message == resultString), "initial string and decrypted string should be the same")
    }
    
    func testSecureCellSealModeEncryptionDecryptionMixed() {
        let cellSeal: TSCellSeal = TSCellSeal(key: masterKeyData)
        let message: String = "All your base are belong to us!"
        let context: String = "For great justice"
        
        let encryptedMessage = try? cellSeal.wrap(message.data(using: .utf8),
                                                           context: context.data(using: .utf8))
        let encryptedMessageNoContext = try? cellSeal.wrap(message.data(using: .utf8),
                                                  context: nil)
        
        XCTAssertFalse((encryptedMessage == encryptedMessageNoContext), "secure cell encrypter (seal mode) encryption result with and without context can`t be equal")
    }

    func testSecureCellSealModeWrongContext() {
        let cellSeal: TSCellSeal = TSCellSeal(key: masterKeyData)
        let message: String = "All your base are belong to us!"
        let context: String = "For great justice"
        let wrongContext: String = "Or not"
        
        let encryptedMessage = try? cellSeal.wrap(message.data(using: .utf8),
                                                  context: context.data(using: .utf8))
        XCTAssertNotNil(encryptedMessage, "encryption with data and without context should return data")
        
        
        let decryptedMessageWrongContext = try? cellSeal.unwrapData(encryptedMessage!,
                                                        context: wrongContext.data(using: .utf8))
        XCTAssertNil(decryptedMessageWrongContext, "decrypt data with wrong context should return nil data")
    }
    
    // MARK: - Token Protect
    
    func testSecureCellTokenProtectModeInit() {
        let cellToken: TSCellToken = TSCellToken(key: masterKeyData)
        XCTAssertNotNil(cellToken, "secure cell encrypter (token protect mode) creation error")
        
        let cellTokenWrong = TSCellToken(key: nil)
        XCTAssertNil(cellTokenWrong, "secure cell encrypter (token protect mode) should not be created without key")
    }
    
    func testSecureCellTokenProtectModeWithContext() {
        let cellToken: TSCellToken = TSCellToken(key: masterKeyData)
        let message: String = "Roses are grey. Violets are grey."
        let context: String = "I'm a dog"
        
        var encryptedMessage = try? cellToken.wrap(nil,
                                                  context: context.data(using: .utf8))
        XCTAssertNil(encryptedMessage, "encryption without data-to-encrypt should return nil data")
        
        encryptedMessage = try? cellToken.wrap(nil, context: nil)
        XCTAssertNil(encryptedMessage, "encryption without data-to-encrypt, without context should return nil data")
        
        encryptedMessage = try? cellToken.wrap(message.data(using: .utf8),
                                              context: context.data(using: .utf8))
        XCTAssertNotNil(encryptedMessage, "encryption with data and context should be successful")
        
        
        var decryptedMessage = try? cellToken.unwrapData(encryptedMessage!,
                                                        context: nil)
        XCTAssertNil(decryptedMessage, "decryption without context should return nil data")
        
        decryptedMessage = try? cellToken.unwrapData(encryptedMessage!,
                                                    context: context.data(using: .utf8))
        XCTAssertNotNil(decryptedMessage, "decryption with data and context should be successful")
        
        let resultString = String(data: decryptedMessage!, encoding: .utf8)
        XCTAssertTrue((message == resultString), "initial string and decrypted string should be the same")
    }
    
    func testSecureCellTokenProtectModeWithoutContext() {
        let cellToken: TSCellToken = TSCellToken(key: masterKeyData)
        let message: String = "Roses are grey. Violets are grey."
        let context: String = "I'm a dog"
        
        let encryptedMessageNoContext = try? cellToken.wrap(message.data(using: .utf8),
                                               context: nil)
        XCTAssertNotNil(encryptedMessageNoContext, "encryption without data-to-encrypt, without context should be successful")
        
        
        var decryptedMessageNoContext = try? cellToken.unwrapData(encryptedMessageNoContext!,
                                                         context: context.data(using: .utf8))
        XCTAssertNil(decryptedMessageNoContext, "decrypt data with context that was encypted without context should return nil data")
        
        decryptedMessageNoContext = try? cellToken.unwrapData(encryptedMessageNoContext!,
                                                     context: nil)
        XCTAssertNotNil(decryptedMessageNoContext, "decryption without context should be successful")
        
        let resultString = String(data: decryptedMessageNoContext!, encoding: .utf8)
        XCTAssertTrue((message == resultString), "initial string and decrypted string should be the same")
    }
    
    func testSecureCellTokenProtectModeIncorrectContext() {
        let cellToken: TSCellToken = TSCellToken(key: masterKeyData)
        let message: String = "Roses are grey. Violets are grey."
        let context: String = "I'm a dog"
        let wrongContext: String = "I'm a cat"
        
        let encryptedMessage = try? cellToken.wrap(message.data(using: .utf8),
                                                            context: context.data(using: .utf8))
        
        let decryptedMessageWrongContext = try? cellToken.unwrapData(encryptedMessage!,
                                                                  context: wrongContext.data(using: .utf8))
        XCTAssertNil(decryptedMessageWrongContext, "decrypt data with wrong context should return nil data")
    }
    
    // MARK: - Token Imprint
    
    func testSecureCellTokenImprintModeInit() {
        let contextImprint: TSCellContextImprint = TSCellContextImprint(key: masterKeyData)
        XCTAssertNotNil(contextImprint, "secure cell encrypter (token imprint mode) creation error")
        
        let contextImprintWrong = TSCellContextImprint(key: nil)
        XCTAssertNil(contextImprintWrong, "secure cell encrypter (token imprint mode) should not be created without key")
    }
    
    func testSecureCellTokenImprintContext() {
        let contextImprint: TSCellContextImprint = TSCellContextImprint(key: masterKeyData)
        let message: String = "Roses are red. My name is Dave. This poem have no sense"
        let context: String = "Microwave"
        
        var encryptedMessage = try? contextImprint.wrap(nil,
                                                   context: context.data(using: .utf8))
        XCTAssertNil(encryptedMessage, "encryption without data-to-encrypt should return nil data")
        
        encryptedMessage = try? contextImprint.wrap(message.data(using: .utf8),
                                               context: context.data(using: .utf8))
        XCTAssertNotNil(encryptedMessage, "encryption with data and context should be successful")
        
        
        var decryptedMessage = try? contextImprint.unwrapData(encryptedMessage!,
                                                         context: nil)
        XCTAssertNil(decryptedMessage, "decryption without context should return nil data")
        
        decryptedMessage = try? contextImprint.unwrapData(encryptedMessage!,
                                                     context: context.data(using: .utf8))
        XCTAssertNotNil(decryptedMessage, "decryption with context should be successful")
        
        let resultString = String(data: decryptedMessage!, encoding: .utf8)
        XCTAssertTrue((message == resultString), "initial string and decrypted string should be the same")
    }
    
    func testSecureCellTokenImprintWithoutContext() {
        let contextImprint: TSCellContextImprint = TSCellContextImprint(key: masterKeyData)
        let message: String = "Roses are red. My name is Dave. This poem have no sense"
        let context: String = "Microwave"
        
        var encryptedMessageNoContext = try? contextImprint.wrap(message.data(using: .utf8),
                                                        context: nil)
        XCTAssertNil(encryptedMessageNoContext, "encryption without data-to-encrypt, without context should return nil")
        
        encryptedMessageNoContext = try? contextImprint.wrap(nil,
                                                    context: nil)
        XCTAssertNil(encryptedMessageNoContext, "encryption without data-to-encrypt, without context should return nil data")
        
        encryptedMessageNoContext = try? contextImprint.wrap(message.data(using: .utf8),
                                               context: context.data(using: .utf8))
        
        
        var decryptedMessageNoContext = try? contextImprint.unwrapData(encryptedMessageNoContext!,
                                                              context: nil)
        XCTAssertNil(decryptedMessageNoContext, "decryption without context should be successful")
        
        decryptedMessageNoContext = try? contextImprint.unwrapData(encryptedMessageNoContext!,
                                                          context: context.data(using: .utf8))
        XCTAssertNotNil(decryptedMessageNoContext, "decrypt data with context that was encypted without context should return nil data")
        
        let resultString = String(data: decryptedMessageNoContext!, encoding: .utf8)
        XCTAssertTrue((message == resultString), "initial string and decrypted string should be the same")
    }
    
    func testSecureCellTokenImprintIncorrectContext() {
        let contextImprint: TSCellContextImprint = TSCellContextImprint(key: masterKeyData)
        let message: String = "Roses are red. My name is Dave. This poem have no sense"
        let context: String = "Microwave"
        let wrongContext: String = "Oven"
        
        let encryptedMessage = try? contextImprint.wrap(message.data(using: .utf8),
                                                   context: context.data(using: .utf8))
        
        
        let decryptedMessageWrongContext = try? contextImprint.unwrapData(encryptedMessage!,
                                                                          context: wrongContext.data(using: .utf8))
        XCTAssertNotNil(decryptedMessageWrongContext, "decrypt data with wrong context should ignore context")
    }
}
