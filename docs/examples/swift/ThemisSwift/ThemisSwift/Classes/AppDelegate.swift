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
        let masterKeyString: String = "UkVDMgAAAC13PCVZAKOczZXUpvkhsC+xvwWnv3CLmlG0Wzy8ZBMnT+2yx/dg"
        let masterKeyData: Data = Data(base64Encoded: masterKeyString, options: .ignoreUnknownCharacters)!
        return masterKeyData
    }
    
    
    // MARK:- Secure Cell
    // MARK:- cell seal mode
    func runExampleSecureCellSealMode() {
        print("----------------------------------", #function)
        let masterKeyData: Data = self.generateMasterKey()
        guard let cellSeal: TSCellSeal = TSCellSeal(key: masterKeyData) else {
            print("Error occurred while initializing object cellSeal", #function)
            return
        }
        let message: String = "All your base are belong to us!"
        let context: String = "For great justice"
        
        var encryptedMessage: Data = Data()
        do {
            // context is optional parameter and may be ignored
            encryptedMessage = try cellSeal.wrap(message.data(using: .utf8),
                                                     context: context.data(using: .utf8))
            print("decryptedMessagez = \(encryptedMessage)")
            
        } catch let error as NSError {
            print("Error occurred while encrypting \(error)", #function)
            return
        }
        
        
        do {
            let decryptedMessage: Data = try cellSeal.unwrapData(encryptedMessage,
                                                       context: context.data(using: .utf8))
            let resultString: String = String(data: decryptedMessage, encoding: .utf8)!
            print("decryptedMessage = \(resultString)")
            
        } catch let error as NSError {
            print("Error occurred while decrypting \(error)", #function)
            return
        }
    }
    
    // MARK:- cell token protect mode
    func runExampleSecureCellTokenProtectMode() {
        print("----------------------------------", #function)
        let masterKeyData: Data = self.generateMasterKey()
        guard let cellToken: TSCellToken = TSCellToken(key: masterKeyData) else {
            print("Error occurred while initializing object cellToken", #function)
            return
        }
        let message: String = "Roses are grey. Violets are grey."
        let context: String = "I'm a dog"
        
        var encryptedMessage: TSCellTokenEncryptedData = TSCellTokenEncryptedData()
        do {
            // context is optional parameter and may be ignored
            encryptedMessage = try cellToken.wrap(message.data(using: .utf8),
                                                      context: context.data(using: .utf8))
            print("encryptedMessage.cipher = \(encryptedMessage.cipherText)")
            print("encryptedMessage.token = \(encryptedMessage.token)")
            
        } catch let error as NSError {
            print("Error occurred while encrypting \(error)", #function)
            return
        }
        
    
        do {
            let decryptedMessage: Data = try cellToken.unwrapData(encryptedMessage,
                                                            context: context.data(using: .utf8))
            let resultString: String = String(data: decryptedMessage, encoding: .utf8)!
            print("decryptedMessage = \(resultString)")
            
        } catch let error as NSError {
            print("Error occurred while decrypting \(error)", #function)
            return
        }
    }
    
    
    // MARK:- cell imprint
    func runExampleSecureCellImprint() {
        print("----------------------------------", #function)
        let masterKeyData: Data = self.generateMasterKey()
        guard let contextImprint: TSCellContextImprint = TSCellContextImprint(key: masterKeyData) else {
            print("Error occurred while initializing object contextImprint", #function)
            return
        }
        let message: String = "Roses are red. My name is Dave. This poem have no sense"
        let context: String = "Microwave"
        
        var encryptedMessage: Data = Data()
        do {
            // context is NOT optional parameter here
            encryptedMessage = try contextImprint.wrap(message.data(using: .utf8),
                                                           context: context.data(using: .utf8))
            print("encryptedMessage = \(encryptedMessage)")
            
        } catch let error as NSError {
            print("Error occurred while encrypting \(error)", #function)
            return
        }
        
        
        do {
            // context is NOT optional parameter here
            let decryptedMessage: Data = try contextImprint.unwrapData(encryptedMessage,
                                                            context: context.data(using: .utf8))
            let resultString: String = String(data: decryptedMessage, encoding: .utf8)!
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
        guard let keyGeneratorRSA: TSKeyGen = TSKeyGen(algorithm: .RSA) else {
            print("Error occurred while initializing object keyGeneratorRSA", #function)
            return
        }
        let privateKeyRSA: Data = keyGeneratorRSA.privateKey as Data
        let publicKeyRSA: Data = keyGeneratorRSA.publicKey as Data
        print("RSA privateKey = \(privateKeyRSA)")
        print("RSA publicKey = \(publicKeyRSA)")
        
        // Generating EC keys
        guard let keyGeneratorEC: TSKeyGen = TSKeyGen(algorithm: .EC) else {
            print("Error occurred while initializing object keyGeneratorEC", #function)
            return
        }
        let privateKeyEC: Data = keyGeneratorEC.privateKey as Data
        let publicKeyEC: Data = keyGeneratorEC.publicKey as Data
        print("EC privateKey = \(privateKeyEC)")
        print("RSA publicKey = \(publicKeyEC)")
    }
    

    // MARK:- Keys from file
    // Sometimes you will need to read keys from files
    func readingKeysFromFile() {
        print("----------------------------------", #function)
        let fileManager: FileManager = FileManager.default
        let mainBundle: Bundle = Bundle.main
        
        // yes, app will crash if no keys. that's idea of our sample
        
        let serverPrivateKeyFromFile: Data = fileManager.contents(atPath: mainBundle.path(forResource: "server",
                                                                                             ofType: "priv")!)!
        let serverPublicKeyFromFile: Data = fileManager.contents(atPath: mainBundle.path(forResource: "server",
                                                                                            ofType: "pub")!)!
        let clientPrivateKeyOldFromFile: Data = fileManager.contents(atPath: mainBundle.path(forResource: "client",
                                                                                                ofType: "priv")!)!
        let clientPublicKeyOldFromFile: Data = fileManager.contents(atPath: mainBundle.path(forResource: "client",
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
        
        let serverPublicKeyString: String = "VUVDMgAAAC2ELbj5Aue5xjiJWW3P2KNrBX+HkaeJAb+Z4MrK0cWZlAfpBUql"
        let clientPrivateKeyString: String = "UkVDMgAAAC13PCVZAKOczZXUpvkhsC+xvwWnv3CLmlG0Wzy8ZBMnT+2yx/dg"
        
        guard let serverPublicKey: Data = Data(base64Encoded: serverPublicKeyString,
                                                   options: .ignoreUnknownCharacters),
            let clientPrivateKey: Data = Data(base64Encoded: clientPrivateKeyString,
                                                  options: .ignoreUnknownCharacters) else {
            print("Error occurred during base64 encoding", #function)
            return
        }
        
        let encrypter: TSMessage = TSMessage.init(inEncryptModeWithPrivateKey: clientPrivateKey,
                                                  peerPublicKey: serverPublicKey)
        
        let message: String = "- Knock, knock.\n- Who’s there?\n*very long pause...*\n- Java."
        
        var encryptedMessage: Data = Data()
        do {
            encryptedMessage = try encrypter.wrap(message.data(using: .utf8))
            print("encryptedMessage = \(encryptedMessage)")
            
        } catch let error as NSError {
            print("Error occurred while encrypting \(error)", #function)
            return
        }
        
        
        // ---------- decryption ----------------
        let serverPrivateKeyString: String = "UkVDMgAAAC1FsVa6AMGljYqtNWQ+7r4RjXTabLZxZ/14EXmi6ec2e1vrCmyR"
        let clientPublicKeyString: String = "VUVDMgAAAC1SsL32Axjosnf2XXUwm/4WxPlZauQ+v+0eOOjpwMN/EO+Huh5d"
        
        guard let serverPrivateKey: Data = Data(base64Encoded: serverPrivateKeyString,
                                                    options: .ignoreUnknownCharacters),
            let clientPublicKey: Data = Data(base64Encoded: clientPublicKeyString,
                                                 options: .ignoreUnknownCharacters) else {
            print("Error occurred during base64 encoding", #function)
            return
        }
        
        let decrypter: TSMessage = TSMessage.init(inEncryptModeWithPrivateKey: serverPrivateKey,
                                                  peerPublicKey: clientPublicKey)
        
        do {
            let decryptedMessage: Data = try decrypter.unwrapData(encryptedMessage)
            let resultString: String = String(data: decryptedMessage, encoding: .utf8)!
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
        
        let serverPublicKeyString: String = "VUVDMgAAAC2ELbj5Aue5xjiJWW3P2KNrBX+HkaeJAb+Z4MrK0cWZlAfpBUql"
        let clientPrivateKeyString: String = "UkVDMgAAAC13PCVZAKOczZXUpvkhsC+xvwWnv3CLmlG0Wzy8ZBMnT+2yx/dg"
        
        guard let serverPublicKey: Data = Data(base64Encoded: serverPublicKeyString,
                                                   options: .ignoreUnknownCharacters),
            let clientPrivateKey: Data = Data(base64Encoded: clientPrivateKeyString,
                                                  options: .ignoreUnknownCharacters) else {
                                                    print("Error occurred during base64 encoding", #function)
                                                    return
        }
        
        let encrypter: TSMessage = TSMessage.init(inSignVerifyModeWithPrivateKey: clientPrivateKey,
                                                  peerPublicKey: serverPublicKey)
        
        let message: String = "I had a problem so I though to use Java. Now I have a ProblemFactory."
        
        var encryptedMessage: Data = Data()
        do {
            encryptedMessage = try encrypter.wrap(message.data(using: .utf8))
            print("encryptedMessage = \(encryptedMessage)")
            
        } catch let error as NSError {
            print("Error occurred while encrypting \(error)", #function)
            return
        }
        
        
        // ---------- verification ----------------
        let serverPrivateKeyString: String = "UkVDMgAAAC1FsVa6AMGljYqtNWQ+7r4RjXTabLZxZ/14EXmi6ec2e1vrCmyR"
        let clientPublicKeyString: String = "VUVDMgAAAC1SsL32Axjosnf2XXUwm/4WxPlZauQ+v+0eOOjpwMN/EO+Huh5d"
        
        guard let serverPrivateKey: Data = Data(base64Encoded: serverPrivateKeyString,
                                                    options: .ignoreUnknownCharacters),
            let clientPublicKey: Data = Data(base64Encoded: clientPublicKeyString,
                                                 options: .ignoreUnknownCharacters) else {
                                                    print("Error occurred during base64 encoding", #function)
                                                    return
        }
        
        let decrypter: TSMessage = TSMessage.init(inSignVerifyModeWithPrivateKey: serverPrivateKey,
                                                  peerPublicKey: clientPublicKey)
        
        do {
            let decryptedMessage: Data = try decrypter.unwrapData(encryptedMessage)
            let resultString: String = String(data: decryptedMessage, encoding: .utf8)!
            print("decryptedMessage->\n\(resultString)")
            
        } catch let error as NSError {
            print("Error occurred while decrypting \(error)", #function)
            return
        }
    }
    
}
