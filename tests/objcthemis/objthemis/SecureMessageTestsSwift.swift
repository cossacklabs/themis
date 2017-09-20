//
//  SecureCellTestsSwift.swift
//  objthemis
//
//  Created by Anastasiia on 9/19/17.
//

import XCTest

class SecureMessageTestsSwift: XCTestCase {
    
    func testExampleGeneratingKeys() {
        
        let keyGeneratorRSA: TSKeyGen = TSKeyGen(algorithm: .RSA)!
        XCTAssertNotNil(keyGeneratorRSA)
        XCTAssertNotNil(keyGeneratorRSA.privateKey)
        XCTAssertNotNil(keyGeneratorRSA.publicKey)
        
        let keyGeneratorEC: TSKeyGen = TSKeyGen(algorithm: .EC)!
        XCTAssertNotNil(keyGeneratorEC)
        XCTAssertNotNil(keyGeneratorEC.privateKey)
        XCTAssertNotNil(keyGeneratorEC.publicKey)
    }
    
    func testSecureMessageEC256() {
        wrapUnwrap(withPrivateKey: privateKeyEc256, publicKey: publicKeyEc256)
        signVerify(withPrivateKey: privateKeyEc256, publicKey: publicKeyEc256)
    }
    
    func testSecureMessageRSA1024() {
        wrapUnwrap(withPrivateKey: privateKeyRSA1024, publicKey: publicKeyRSA1024)
        signVerify(withPrivateKey: privateKeyRSA1024, publicKey: publicKeyRSA1024)
    }
    
    func testSecureMessageRSA2048() {
        wrapUnwrap(withPrivateKey: privateKeyRSA2048, publicKey: publicKeyRSA2048)
        signVerify(withPrivateKey: privateKeyRSA2048, publicKey: publicKeyRSA2048)
    }
    
    func testSecureMessageRSA4096() {
        wrapUnwrap(withPrivateKey: privateKeyRSA4096, publicKey: publicKeyRSA4096)
        signVerify(withPrivateKey: privateKeyRSA4096, publicKey: publicKeyRSA4096)
    }
    
    func wrapUnwrap(withPrivateKey privateKey: String, publicKey: String) {
        let publicKeyData = Data(base64Encoded: publicKey, options: .ignoreUnknownCharacters)
        let privateKeyData = Data(base64Encoded: privateKey, options: .ignoreUnknownCharacters)
        
        let encrypter = TSMessage.init(inEncryptModeWithPrivateKey: privateKeyData,
                                       peerPublicKey: publicKeyData)
        let message: String = "- Knock, knock.\n- Who’s there?\n*very long pause...*\n- Java."
        
        let encryptedMessage = try? encrypter?.wrap(message.data(using: .utf8))
        XCTAssertNotNil(encryptedMessage!, "wrapping data should return encrypted data")
        
        let decryptedMessage = try? encrypter?.unwrapData(encryptedMessage!)
        XCTAssertNotNil(decryptedMessage!, "unwrapping data should return decrypted data")
        
        let resultString = String(data: decryptedMessage!!, encoding: .utf8)
        XCTAssertTrue((message == resultString), "initial string and decrypted string should be the same")
    }
    
    func signVerify(withPrivateKey privateKey: String, publicKey: String) {
        let publicKeyData = Data(base64Encoded: publicKey, options: .ignoreUnknownCharacters)
        let privateKeyData = Data(base64Encoded: privateKey, options: .ignoreUnknownCharacters)
        
        let encrypter = TSMessage.init(inSignVerifyModeWithPrivateKey: privateKeyData,
                                       peerPublicKey: publicKeyData)
        let message: String = "- Knock, knock.\n- Who’s there?\n*very long pause...*\n- Java."
        
        let encryptedMessage = try? encrypter?.wrap(message.data(using: .utf8))
        XCTAssertNotNil(encryptedMessage!, "wrapping data should return encrypted data")
        
        let decryptedMessage = try? encrypter?.unwrapData(encryptedMessage!)
        XCTAssertNotNil(decryptedMessage!, "unwrapping data should return decrypted data")
        
        let resultString = String(data: decryptedMessage!!, encoding: .utf8)
        XCTAssertTrue((message == resultString), "initial string and decrypted string should be the same")
    }
    
}
