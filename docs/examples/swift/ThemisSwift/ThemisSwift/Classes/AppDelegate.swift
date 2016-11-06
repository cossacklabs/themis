//
//  AppDelegate.swift
//  ThemisSwift
//
//  Created by Anastasi Voitova on 17.04.16.
//  Copyright © 2016 CossackLabs. All rights reserved.
//

import UIKit

@UIApplicationMain
class AppDelegate: UIResponder, UIApplicationDelegate {

    var window: UIWindow?


    func application(_ application: UIApplication, didFinishLaunchingWithOptions
        launchOptions: [UIApplicationLaunchOptionsKey: Any]?) -> Bool {
        
        // Please, look in debug console to see results
        
        // Secure Cell:
        runExampleSecureCellSealMode()
        runExampleSecureCellTokenProtectMode()
        runExampleSecureCellImprint()
        
        // Generating/reading keys:
        runExampleGeneratingKeys()
        readingKeysFromFile()
        
        // Secure Message:
        runExampleSecureMessageEncryptionDecryption()
        runExampleSecureMessageSignVerify()
        
        return true
    }
    
    
    func generateMasterKey() -> Data {
        let masterKeyString = "UkVDMgAAAC13PCVZAKOczZXUpvkhsC+xvwWnv3CLmlG0Wzy8ZBMnT+2yx/dg"
        let masterKeyData = Data(base64Encoded: masterKeyString, options: .ignoreUnknownCharacters)!
        return masterKeyData
    }
    
    
    // MARK:- Secure Cell
    // MARK:- cell seal mode
    func runExampleSecureCellSealMode() {
        print("----------------------------------", #function)
        let masterKeyData = self.generateMasterKey()
        guard let cellSeal = TSCellSeal(key: masterKeyData) else {
            print("Error occurred while initializing object cellSeal", #function)
            return
        }
        let message = "All your base are belong to us!"
        let context = "For great justice"
        
        var encryptedMessage = Data()
        do {
            // context is optional parameter and may be ignored
            encryptedMessage = try cellSeal.wrap(message.data(using: String.Encoding.utf8),
                                                     context: context.data(using: String.Encoding.utf8))
            print("decryptedMessagez = \(encryptedMessage)")
            
        } catch let error as NSError {
            print("Error occurred while encrypting \(error)", #function)
            return
        }
        
        
        do {
            let decryptedMessage = try cellSeal.unwrapData(encryptedMessage,
                                                       context: context.data(using: String.Encoding.utf8))
            let resultString = String(data: decryptedMessage, encoding: String.Encoding.utf8)!
            print("decryptedMessage = \(resultString)")
            
        } catch let error as NSError {
            print("Error occurred while decrypting \(error)", #function)
            return
        }
    }
    
    // MARK:- cell token protect mode
    func runExampleSecureCellTokenProtectMode() {
        print("----------------------------------", #function)
        let masterKeyData = self.generateMasterKey()
        guard let cellToken = TSCellToken(key: masterKeyData) else {
            print("Error occurred while initializing object cellToken", #function)
            return
        }
        let message = "Roses are grey. Violets are grey."
        let context = "I'm a dog"
        
        var encryptedMessage = TSCellTokenEncryptedData()
        do {
            // context is optional parameter and may be ignored
            encryptedMessage = try cellToken.wrap(message.data(using: String.Encoding.utf8),
                                                      context: context.data(using: String.Encoding.utf8))
            print("encryptedMessage.cipher = \(encryptedMessage.cipherText)")
            print("encryptedMessage.token = \(encryptedMessage.token)")
            
        } catch let error as NSError {
            print("Error occurred while encrypting \(error)", #function)
            return
        }
        
    
        do {
            let decryptedMessage = try cellToken.unwrapData(encryptedMessage,
                                                            context: context.data(using: String.Encoding.utf8))
            let resultString = String(data: decryptedMessage, encoding: String.Encoding.utf8)!
            print("decryptedMessage = \(resultString)")
            
        } catch let error as NSError {
            print("Error occurred while decrypting \(error)", #function)
            return
        }
    }
    
    
    // MARK:- cell imprint
    func runExampleSecureCellImprint() {
        print("----------------------------------", #function)
        let masterKeyData = self.generateMasterKey()
        guard let contextImprint = TSCellContextImprint(key: masterKeyData) else {
            print("Error occurred while initializing object contextImprint", #function)
            return
        }
        let message = "Roses are red. My name is Dave. This poem have no sense"
        let context = "Microwave"
        
        var encryptedMessage = Data()
        do {
            // context is NOT optional parameter here
            encryptedMessage = try contextImprint.wrap(message.data(using: String.Encoding.utf8),
                                                           context: context.data(using: String.Encoding.utf8))
            print("encryptedMessage = \(encryptedMessage)")
            
        } catch let error as NSError {
            print("Error occurred while encrypting \(error)", #function)
            return
        }
        
        
        do {
            // context is NOT optional parameter here
            let decryptedMessage = try contextImprint.unwrapData(encryptedMessage,
                                                            context: context.data(using: String.Encoding.utf8))
            let resultString = String(data: decryptedMessage, encoding: String.Encoding.utf8)!
            print("decryptedMessage = \(resultString)")
            
        } catch let error as NSError {
            print("Error occurred while decrypting \(error)", #function)
            return
        }
    }
    
    
    // MARK:- Key Generation
    
    // MARK:- RSA/EC
    func runExampleGeneratingKeys() {
        print("----------------------------------", #function)
        
        // Generating RSA keys
        guard let keyGeneratorRSA = TSKeyGen(algorithm: .RSA) else {
            print("Error occurred while initializing object keyGeneratorRSA", #function)
            return
        }
        let privateKeyRSA = keyGeneratorRSA.privateKey as Data
        let publicKeyRSA = keyGeneratorRSA.publicKey as Data
        print("RSA privateKey = \(privateKeyRSA)")
        print("RSA publicKey = \(publicKeyRSA)")
        
        // Generating EC keys
        guard let keyGeneratorEC = TSKeyGen(algorithm: .EC) else {
            print("Error occurred while initializing object keyGeneratorEC", #function)
            return
        }
        let privateKeyEC = keyGeneratorEC.privateKey as Data
        let publicKeyEC = keyGeneratorEC.publicKey as Data
        print("EC privateKey = \(privateKeyEC)")
        print("RSA publicKey = \(publicKeyEC)")
    }
    

    // MARK:- Keys from file
    // Sometimes you will need to read keys from files
    func readingKeysFromFile() {
        print("----------------------------------", #function)
        let fileManager = FileManager.default
        let mainBundle = Bundle.main
        
        // yes, app will crash if no keys. that's idea of our sample
        
        let serverPrivateKeyFromFile = fileManager.contents(atPath: mainBundle.path(forResource: "server",
                                                                                             ofType: "priv")!)!
        let serverPublicKeyFromFile = fileManager.contents(atPath: mainBundle.path(forResource: "server",
                                                                                            ofType: "pub")!)!
        let clientPrivateKeyOldFromFile = fileManager.contents(atPath: mainBundle.path(forResource: "client",
                                                                                                ofType: "priv")!)!
        let clientPublicKeyOldFromFile = fileManager.contents(atPath: mainBundle.path(forResource: "client",
                                                                                               ofType: "pub")!)!
        
        print("serverPrivateKeyFromFile = \(serverPrivateKeyFromFile)")
        print("serverPublicKeyFromFile = \(serverPublicKeyFromFile)")
        print("clientPrivateKeyOldFromFile = \(clientPrivateKeyOldFromFile)")
        print("clientPublicKeyOldFromFile = \(clientPublicKeyOldFromFile)")
    }
    
    
    // MARK:- Secure Message
    
    // MARK:- encryption/decryption
    func runExampleSecureMessageEncryptionDecryption() {
        print("----------------------------------", #function)
        
        // ---------- encryption ----------------
        
        // base64 encoded keys:
        // client private key
        // server public key
        
        let serverPublicKeyString = "VUVDMgAAAC2ELbj5Aue5xjiJWW3P2KNrBX+HkaeJAb+Z4MrK0cWZlAfpBUql"
        let clientPrivateKeyString = "UkVDMgAAAC13PCVZAKOczZXUpvkhsC+xvwWnv3CLmlG0Wzy8ZBMnT+2yx/dg"
        
        guard let serverPublicKey = Data(base64Encoded: serverPublicKeyString,
                                                   options: .ignoreUnknownCharacters),
            let clientPrivateKey = Data(base64Encoded: clientPrivateKeyString,
                                                  options: .ignoreUnknownCharacters) else {
            print("Error occurred during base64 encoding", #function)
            return
        }
        
        let encrypter = TSMessage(inEncryptModeWithPrivateKey: clientPrivateKey,
                                                  peerPublicKey: serverPublicKey)!
        
        let message = "- Knock, knock.\n- Who’s there?\n*very long pause...*\n- Java."
        
        var encryptedMessage = Data()
        do {
            encryptedMessage = try encrypter.wrap(message.data(using: String.Encoding.utf8))
            print("encryptedMessage = \(encryptedMessage)")
            
        } catch let error as NSError {
            print("Error occurred while encrypting \(error)", #function)
            return
        }
        
        
        // ---------- decryption ----------------
        let serverPrivateKeyString = "UkVDMgAAAC1FsVa6AMGljYqtNWQ+7r4RjXTabLZxZ/14EXmi6ec2e1vrCmyR"
        let clientPublicKeyString = "VUVDMgAAAC1SsL32Axjosnf2XXUwm/4WxPlZauQ+v+0eOOjpwMN/EO+Huh5d"
        
        guard let serverPrivateKey = Data(base64Encoded: serverPrivateKeyString,
                                                    options: .ignoreUnknownCharacters),
            let clientPublicKey = Data(base64Encoded: clientPublicKeyString,
                                                 options: .ignoreUnknownCharacters) else {
            print("Error occurred during base64 encoding", #function)
            return
        }
        
        let decrypter = TSMessage(inEncryptModeWithPrivateKey: serverPrivateKey,
                                                  peerPublicKey: clientPublicKey)!
        
        do {
            let decryptedMessage = try decrypter.unwrapData(encryptedMessage)
            let resultString = String(data: decryptedMessage, encoding: String.Encoding.utf8)!
            print("decryptedMessage->\n\(resultString)")
            
        } catch let error as NSError {
            print("Error occurred while decrypting \(error)", #function)
            return
        }
    }
    
    // MARK:- sign/vefiry
    func runExampleSecureMessageSignVerify() {
        print("----------------------------------", #function)
        
        // ---------- signing ----------------
        
        // base64 encoded keys:
        // client private key
        // server public key
        
        let serverPublicKeyString = "VUVDMgAAAC2ELbj5Aue5xjiJWW3P2KNrBX+HkaeJAb+Z4MrK0cWZlAfpBUql"
        let clientPrivateKeyString = "UkVDMgAAAC13PCVZAKOczZXUpvkhsC+xvwWnv3CLmlG0Wzy8ZBMnT+2yx/dg"
        
        guard let serverPublicKey = Data(base64Encoded: serverPublicKeyString,
                                                   options: .ignoreUnknownCharacters),
            let clientPrivateKey = Data(base64Encoded: clientPrivateKeyString,
                                                  options: .ignoreUnknownCharacters) else {
                                                    print("Error occurred during base64 encoding", #function)
                                                    return
        }
        
        let encrypter = TSMessage(inSignVerifyModeWithPrivateKey: clientPrivateKey,
                                                  peerPublicKey: serverPublicKey)!
        
        let message = "I had a problem so I though to use Java. Now I have a ProblemFactory."
        
        var encryptedMessage = Data()
        do {
            encryptedMessage = try encrypter.wrap(message.data(using: String.Encoding.utf8))
            print("encryptedMessage = \(encryptedMessage)")
            
        } catch let error as NSError {
            print("Error occurred while encrypting \(error)", #function)
            return
        }
        
        
        // ---------- verification ----------------
        let serverPrivateKeyString = "UkVDMgAAAC1FsVa6AMGljYqtNWQ+7r4RjXTabLZxZ/14EXmi6ec2e1vrCmyR"
        let clientPublicKeyString = "VUVDMgAAAC1SsL32Axjosnf2XXUwm/4WxPlZauQ+v+0eOOjpwMN/EO+Huh5d"
        
        guard let serverPrivateKey = Data(base64Encoded: serverPrivateKeyString,
                                                    options: .ignoreUnknownCharacters),
            let clientPublicKey = Data(base64Encoded: clientPublicKeyString,
                                                 options: .ignoreUnknownCharacters) else {
                                                    print("Error occurred during base64 encoding", #function)
                                                    return
        }
        
        let decrypter = TSMessage(inSignVerifyModeWithPrivateKey: serverPrivateKey,
                                                  peerPublicKey: clientPublicKey)!
        
        do {
            let decryptedMessage = try decrypter.unwrapData(encryptedMessage)
            let resultString = String(data: decryptedMessage, encoding: String.Encoding.utf8)!
            print("decryptedMessage->\n\(resultString)")
            
        } catch let error as NSError {
            print("Error occurred while decrypting \(error)", #function)
            return
        }
    }
    
}
