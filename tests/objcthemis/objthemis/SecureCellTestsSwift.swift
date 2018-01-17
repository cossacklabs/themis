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
        let cellSeal: TSCellSeal = TSCellSeal(key: masterKeyData)!
        XCTAssertNotNil(cellSeal, "secure cell encrypter (seal mode) should not be created without key")
        
        let cellSealWrong = TSCellSeal(key: Data())
        XCTAssertNil(cellSealWrong, "secure cell encrypter (seal mode) creation error")
    }
    
    func testSecureCellSealModeWithContext() {
        let cellSeal: TSCellSeal = TSCellSeal(key: masterKeyData)!
        let message: String = "All your base are belong to us!"
        let context: String = "For great justice"
        
        let encryptedMessage = try? cellSeal.wrap(message.data(using: .utf8)!,
                                                  context: context.data(using: .utf8)!)
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
        let cellSeal: TSCellSeal = TSCellSeal(key: masterKeyData)!
        let message: String = "All your base are belong to us!"
        let context: String = "For great justice"
        
        let encryptedMessageNoContext = try? cellSeal.wrap(message.data(using: .utf8)!,
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
        let cellSeal: TSCellSeal = TSCellSeal(key: masterKeyData)!
        let message: String = "All your base are belong to us!"
        let context: String = "For great justice"
        
        let encryptedMessage = try? cellSeal.wrap(message.data(using: .utf8)!,
                                                  context: context.data(using: .utf8)!)
        let encryptedMessageNoContext = try? cellSeal.wrap(message.data(using: .utf8)!,
                                                  context: nil)
        
        XCTAssertFalse((encryptedMessage == encryptedMessageNoContext), "secure cell encrypter (seal mode) encryption result with and without context can`t be equal")
    }

    func testSecureCellSealModeWrongContext() {
        let cellSeal: TSCellSeal = TSCellSeal(key: masterKeyData)!
        let message: String = "All your base are belong to us!"
        let context: String = "For great justice"
        let wrongContext: String = "Or not"
        
        let encryptedMessage = try? cellSeal.wrap(message.data(using: .utf8)!,
                                                  context: context.data(using: .utf8)!)
        XCTAssertNotNil(encryptedMessage, "encryption with data and without context should return data")
        
        
        let decryptedMessageWrongContext = try? cellSeal.unwrapData(encryptedMessage!,
                                                                    context: wrongContext.data(using: .utf8)!)
        XCTAssertNil(decryptedMessageWrongContext, "decrypt data with wrong context should return nil data")
    }
    
    // MARK: - Token Protect
    
    func testSecureCellTokenProtectModeInit() {
        let cellToken: TSCellToken = TSCellToken(key: masterKeyData)!
        XCTAssertNotNil(cellToken, "secure cell encrypter (token protect mode) creation error")
        
        let cellTokenWrong = TSCellToken(key: Data())
        XCTAssertNil(cellTokenWrong, "secure cell encrypter (token protect mode) should not be created without key")
    }
    
    func testSecureCellTokenProtectModeWithContext() {
        let cellToken: TSCellToken = TSCellToken(key: masterKeyData)!
        let message: String = "Roses are grey. Violets are grey."
        let context: String = "I'm a dog"
        let messageData: Data = message.data(using: .utf8)!
    
        let encryptedMessage = try? cellToken.wrap(messageData,
                                              context: context.data(using: .utf8)!)
        XCTAssertNotNil(encryptedMessage, "encryption with data and context should be successful")
        XCTAssertEqual(encryptedMessage?.cipherText.length, messageData.count)
        
        var decryptedMessage = try? cellToken.unwrapData(encryptedMessage!,
                                                        context: nil)
        XCTAssertNil(decryptedMessage, "decryption without context should return nil data")
        
        decryptedMessage = try? cellToken.unwrapData(encryptedMessage!,
                                                    context: context.data(using: .utf8)!)
        XCTAssertNotNil(decryptedMessage, "decryption with data and context should be successful")
        
        let resultString = String(data: decryptedMessage!, encoding: .utf8)
        XCTAssertTrue((message == resultString), "initial string and decrypted string should be the same")
    }
    
    func testSecureCellTokenProtectModeWithoutContext() {
        let cellToken: TSCellToken = TSCellToken(key: masterKeyData)!
        let message: String = "Roses are grey. Violets are grey."
        let context: String = "I'm a dog"
        let messageData: Data = message.data(using: .utf8)!
        
        let encryptedMessageNoContext = try? cellToken.wrap(messageData,
                                               context: nil)
        XCTAssertNotNil(encryptedMessageNoContext, "encryption without data-to-encrypt, without context should be successful")
        XCTAssertEqual(encryptedMessageNoContext?.cipherText.length, messageData.count)
        
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
        let cellToken: TSCellToken = TSCellToken(key: masterKeyData)!
        let message: String = "Roses are grey. Violets are grey."
        let context: String = "I'm a dog"
        let wrongContext: String = "I'm a cat"
        
        let encryptedMessage = try? cellToken.wrap(message.data(using: .utf8)!,
                                                            context: context.data(using: .utf8))
        
        let decryptedMessageWrongContext = try? cellToken.unwrapData(encryptedMessage!,
                                                                  context: wrongContext.data(using: .utf8))
        XCTAssertNil(decryptedMessageWrongContext, "decrypt data with wrong context should return nil data")
    }
    
    // MARK: - Token Imprint
    
    func testSecureCellContextImprintModeInit() {
        let contextImprint: TSCellContextImprint = TSCellContextImprint(key: masterKeyData)!
        XCTAssertNotNil(contextImprint, "secure cell encrypter (token imprint mode) creation error")
        
        let contextImprintWrong = TSCellContextImprint(key: Data())
        XCTAssertNil(contextImprintWrong, "secure cell encrypter (token imprint mode) should not be created without key")
    }
    
    func testSecureCellContextImprint() {
        let contextImprint: TSCellContextImprint = TSCellContextImprint(key: masterKeyData)!
        let message: String = "Roses are red. My name is Dave. This poem have no sense"
        let context: String = "Microwave"
        
        let encryptedMessageEmptyContext = try? contextImprint.wrap(message.data(using: .utf8)!,
                                                                    context: Data())
        XCTAssertNil(encryptedMessageEmptyContext, "encryption with data-to-encrypt, and empty context should return nil data")
        
        let encryptedMessageNoContext = try? contextImprint.wrap(message.data(using: .utf8)!,
                                                                 context: context.data(using: .utf8)!)
        XCTAssertNotNil(encryptedMessageNoContext, "encryption with data-to-encrypt, and context should be successful")

        let decryptedMessageNoContext = try? contextImprint.unwrapData(encryptedMessageNoContext!,
                                                                       context: context.data(using: .utf8)!)
        XCTAssertNotNil(decryptedMessageNoContext, "decrypt data with context that was encypted without context should return nil data")
        
        let decryptedMessageEmptyContext = try? contextImprint.unwrapData(encryptedMessageNoContext!,
                                                                          context: Data())
        XCTAssertNil(decryptedMessageEmptyContext, "decrypt data and empty context should return nil data")
        
        let resultString = String(data: decryptedMessageNoContext!, encoding: .utf8)
        XCTAssertTrue((message == resultString), "initial string and decrypted string should be the same")
    }
    
    func testSecureCellContextImprintIncorrectContext() {
        let contextImprint: TSCellContextImprint = TSCellContextImprint(key: masterKeyData)!
        let message: String = "Roses are red. My name is Dave. This poem have no sense"
        let context: String = "Microwave"
        let wrongContext: String = "Oven"
        
        let encryptedMessage = try? contextImprint.wrap(message.data(using: .utf8)!,
                                                        context: context.data(using: .utf8)!)

        let decryptedMessageWrongContext = try? contextImprint.unwrapData(encryptedMessage!,
                                                                          context: wrongContext.data(using: .utf8)!)
        XCTAssertNotNil(decryptedMessageWrongContext, "decrypt data with wrong context should ignore context")
        
        let decryptedMessageEmptyContext = try? contextImprint.unwrapData(encryptedMessage!,
                                                                          context: Data())
        XCTAssertNil(decryptedMessageEmptyContext, "decrypt data with empty context should return nil data")
    }
}
