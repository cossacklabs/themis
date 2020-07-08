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
        encryptDecryptStrings(withPrivateKey: privateKeyEc256, publicKey: publicKeyEc256)
        signVerifyStrings(withPrivateKey: privateKeyEc256, publicKey: publicKeyEc256)
    }
    
    func testSecureMessageRSA1024() {
        encryptDecryptStrings(withPrivateKey: privateKeyRSA1024, publicKey: publicKeyRSA1024)
        signVerifyStrings(withPrivateKey: privateKeyRSA1024, publicKey: publicKeyRSA1024)
    }
    
    func testSecureMessageRSA2048() {
        encryptDecryptStrings(withPrivateKey: privateKeyRSA2048, publicKey: publicKeyRSA2048)
        signVerifyStrings(withPrivateKey: privateKeyRSA2048, publicKey: publicKeyRSA2048)
    }
    
    func testSecureMessageRSA4096() {
        encryptDecryptStrings(withPrivateKey: privateKeyRSA4096, publicKey: publicKeyRSA4096)
        signVerifyStrings(withPrivateKey: privateKeyRSA4096, publicKey: publicKeyRSA4096)
    }
  
    func testSecureMessageKeygenEC() {
      let keyGeneratorEC = TSKeyGen(algorithm: .EC)!
      encryptDecrypt(withPrivateKey: keyGeneratorEC.privateKey as Data, publicKeyData: keyGeneratorEC.publicKey as Data)
      signVerify(withPrivateKey: keyGeneratorEC.privateKey as Data, publicKeyData: keyGeneratorEC.publicKey as Data)
    }
  
    func testSecureMessageKeygenRSA() {
      let keyGeneratorRSA = TSKeyGen(algorithm: .RSA)!
      encryptDecrypt(withPrivateKey: keyGeneratorRSA.privateKey as Data, publicKeyData: keyGeneratorRSA.publicKey as Data)
      signVerify(withPrivateKey: keyGeneratorRSA.privateKey as Data, publicKeyData: keyGeneratorRSA.publicKey as Data)
    }
  
    func encryptDecryptStrings(withPrivateKey privateKey: String, publicKey: String) {
      let privateKeyData = Data(base64Encoded: privateKey, options: .ignoreUnknownCharacters)!
      let publicKeyData = Data(base64Encoded: publicKey, options: .ignoreUnknownCharacters)!
      return encryptDecrypt(withPrivateKey:privateKeyData, publicKeyData:publicKeyData)
    }
  
    func encryptDecrypt(withPrivateKey privateKeyData: Data, publicKeyData: Data) {
        let encrypter = TSMessage.init(inEncryptModeWithPrivateKey: privateKeyData,
                                       peerPublicKey: publicKeyData)!

        let message: String = "- Knock, knock.\n- Who’s there?\n*very long pause...*\n- Java."
        
        let encryptedMessage = try? encrypter.wrap(message.data(using: .utf8))
        XCTAssertNotNil(encryptedMessage, "encrypting data should return encrypted data")
        
        let decryptedMessage = try? encrypter.unwrapData(encryptedMessage!)
        XCTAssertNotNil(decryptedMessage, "decrypting data should return decrypted data")
        
        let resultString = String(data: decryptedMessage!, encoding: .utf8)
        XCTAssertTrue((message == resultString), "initial string and decrypted string should be the same")
      
        // nil message
        let encryptedNilMessage = try? encrypter.wrap(nil)
        XCTAssertNil(encryptedNilMessage, "encrypting nil data should return empty data")
      
        let decryptedNilMessage = try? encrypter.unwrapData(nil)
        XCTAssertNil(decryptedNilMessage, "decrypting nil data should return empty data")
        
        // empty message
        let encryptedEmptyMessage = try? encrypter.wrap("".data(using: .utf8))
        XCTAssertNil(encryptedEmptyMessage, "encrypting empty data should return empty data")
        
        let decryptedEmptyMessage = try? encrypter.unwrapData("".data(using: .utf8))
        XCTAssertNil(decryptedEmptyMessage, "decrypting empty data should return empty data")
      
        // another encryptor
        let keyGeneratorEC = TSKeyGen(algorithm: .EC)!
        let anotherEncryptor = TSMessage.init(inEncryptModeWithPrivateKey: keyGeneratorEC.privateKey as Data,
                                            peerPublicKey: keyGeneratorEC.publicKey as Data)!
        let badDecryptedMessage = try? anotherEncryptor.unwrapData(encryptedMessage!)
        XCTAssertNil(badDecryptedMessage, "another encryptor can't decrypt other encrypted message")
    }
  
    func signVerifyStrings(withPrivateKey privateKey: String, publicKey: String) {
      let privateKeyData = Data(base64Encoded: privateKey, options: .ignoreUnknownCharacters)!
      let publicKeyData = Data(base64Encoded: publicKey, options: .ignoreUnknownCharacters)!
      return signVerify(withPrivateKey:privateKeyData, publicKeyData:publicKeyData)
    }
  
    func signVerify(withPrivateKey privateKeyData: Data, publicKeyData: Data) {
        let encrypter = TSMessage.init(inSignVerifyModeWithPrivateKey: privateKeyData,
                                       peerPublicKey: publicKeyData)!

        let message: String = "- Knock, knock.\n- Who’s there?\n*very long pause...*\n- Java."
        
        let signedMessage = try? encrypter.wrap(message.data(using: .utf8))
        XCTAssertNotNil(signedMessage, "signing data should return signed data")
        
        let verifiedMessage = try? encrypter.unwrapData(signedMessage!)
        XCTAssertNotNil(verifiedMessage, "verifying data should return verified data")
        
        let resultString = String(data: verifiedMessage!, encoding: .utf8)
        XCTAssertTrue((message == resultString), "initial string and verified string should be the same")
    
        // nil message
        let signedNilMessage = try? encrypter.wrap(nil)
        XCTAssertNil(signedNilMessage, "signing nil data should return empty data")
        
        let verifiedNilMessage = try? encrypter.unwrapData(nil)
        XCTAssertNil(verifiedNilMessage, "verifying nil data should return empty data")
        
        // empty message
        let signedEmptyMessage = try? encrypter.wrap("".data(using: .utf8))
        XCTAssertNil(signedEmptyMessage, "signing empty data should return empty data")
        
        let verifiedEmptyMessage = try? encrypter.unwrapData("".data(using: .utf8))
        XCTAssertNil(verifiedEmptyMessage, "verifying empty data should return empty data")

        // empty both keys
        let wrongSM = TSMessage.init(inSignVerifyModeWithPrivateKey: nil,
                                    peerPublicKey: nil)
        XCTAssertNil(wrongSM, "can't to create secure mesage without both keys")
      
        // sign or verify only
        let anotherMessage = "sign message"
        let signer = TSMessage.init(inSignVerifyModeWithPrivateKey: privateKeyData,
                                     peerPublicKey: nil)!
      
        let anotherSignedMessage = try? signer.wrap(anotherMessage.data(using: .utf8))
        XCTAssertNotNil(anotherSignedMessage, "signing data should return signed data")
      
        let verifier = TSMessage.init(inSignVerifyModeWithPrivateKey: nil,
                                    peerPublicKey: publicKeyData)!

        let anotherVerifiedMessage = try? verifier.unwrapData(anotherSignedMessage!)
        XCTAssertNotNil(anotherVerifiedMessage, "verifying data should return verified data")
      
        let keyGeneratorEC = TSKeyGen(algorithm: .EC)!
        let yetAnotherVerifier = TSMessage.init(inSignVerifyModeWithPrivateKey: keyGeneratorEC.privateKey as Data,
                                      peerPublicKey: keyGeneratorEC.publicKey as Data)!
        let badVerifiedMessage = try? yetAnotherVerifier.unwrapData(anotherSignedMessage!)
        XCTAssertTrue((badVerifiedMessage == nil), "another verifier can't verify this signed message")
    }
    
}
