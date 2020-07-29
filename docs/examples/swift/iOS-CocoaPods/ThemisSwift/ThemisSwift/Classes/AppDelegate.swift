//
//  AppDelegate.swift
//  ThemisSwift
//
//  Created by Anastasi Voitova on 17.04.16.
//  Copyright © 2016 CossackLabs. All rights reserved.
//

import UIKit
import themis

@UIApplicationMain
class AppDelegate: UIResponder, UIApplicationDelegate {

    var window: UIWindow?

    func application(_ application: UIApplication, didFinishLaunchingWithOptions
        launchOptions: [UIApplication.LaunchOptionsKey: Any]?) -> Bool {
        
        // Please, look in debug console to see results
        
        // generate key from pre-defined string
        print("Using key from pre-defined string")
        let fixedKey = Data(base64Encoded: "UkVDMgAAAC13PCVZAKOczZXUpvkhsC+xvwWnv3CLmlG0Wzy8ZBMnT+2yx/dg", options: .ignoreUnknownCharacters)!
       
        // Secure Cell:
        runExampleSecureCellSealMode(masterKeyData: fixedKey)
        runExampleSecureCellTokenProtectMode(masterKeyData: fixedKey)
        runExampleSecureCellImprint(masterKeyData: fixedKey)
        
        // generate key from key generator
        print("Using key from TSGenerateSymmetricKey")
        let generatedKey = TSGenerateSymmetricKey()!
        
        // Secure Cell:
        runExampleSecureCellSealMode(masterKeyData: generatedKey)
        runExampleSecureCellTokenProtectMode(masterKeyData: generatedKey)
        runExampleSecureCellImprint(masterKeyData: generatedKey)
        
        // Secure Cell with passphrase
        runExampleSecureCellWithPassphrase()
        
        // Generating/reading keys:
        runExampleGeneratingKeys()
        readingKeysFromFile()
        
        // Secure Message:
        runExampleSecureMessageEncryptionDecryption()
        runExampleSecureMessageSignVerify()
        
        // Secure Comparator:
        runExampleSecureComparator()
        
        return true
    }
    
    
    // MARK: - Secure Cell
    // MARK: - cell seal mode
    func runExampleSecureCellSealMode(masterKeyData: Data) {
        print("----------------------------------", #function)
        guard let cellSeal: TSCellSeal = TSCellSeal(key: masterKeyData) else {
            print("Error occurred while initializing object cellSeal", #function)
            return
        }
        let message: String = "All your base are belong to us!"
        let context: String = "For great justice"
        
        var encryptedMessage: Data = Data()
        do {
            // context is optional parameter and may be omitted
            encryptedMessage = try cellSeal.encrypt(message.data(using: .utf8)!,
                                                    context: context.data(using: .utf8)!)
            print("encryptedMessage = \(encryptedMessage)")
            
        } catch let error as NSError {
            print("Error occurred while encrypting \(error)", #function)
            return
        }
        
        do {
            let decryptedMessage = try cellSeal.decrypt(encryptedMessage,
                                                        context: context.data(using: .utf8)!)
            let resultString: String = String(data: decryptedMessage, encoding: .utf8)!
            print("decryptedMessage = \(resultString)")
            
        } catch let error as NSError {
            print("Error occurred while decrypting \(error)", #function)
            return
        }
    }
    
    // MARK: - cell token protect mode
    func runExampleSecureCellTokenProtectMode(masterKeyData: Data) {
        print("----------------------------------", #function)
        guard let cellToken: TSCellToken = TSCellToken(key: masterKeyData) else {
            print("Error occurred while initializing object cellToken", #function)
            return
        }
        let message: String = "Roses are grey. Violets are grey."
        let context: String = "I'm a dog"
        
        var encryptedMessage = TSCellTokenEncryptedResult()
        do {
            // context is optional parameter and may be omitted
            encryptedMessage = try cellToken.encrypt(message.data(using: .utf8)!,
                                                     context: context.data(using: .utf8)!)
            print("encryptedMessage.encrypted = \(encryptedMessage.encrypted)")
            print("encryptedMessage.token = \(encryptedMessage.token)")
            
        } catch let error as NSError {
            print("Error occurred while encrypting \(error)", #function)
            return
        }
        
        do {
            let decryptedMessage = try cellToken.decrypt(encryptedMessage.encrypted,
                                                         token: encryptedMessage.token,
                                                         context: context.data(using: .utf8)!)
            let resultString: String = String(data: decryptedMessage, encoding: .utf8)!
            print("decryptedMessage = \(resultString)")
            
        } catch let error as NSError {
            print("Error occurred while decrypting \(error)", #function)
            return
        }
    }
    
    // MARK: - cell imprint
    func runExampleSecureCellImprint(masterKeyData: Data) {
        print("----------------------------------", #function)
        guard let contextImprint: TSCellContextImprint = TSCellContextImprint(key: masterKeyData) else {
            print("Error occurred while initializing object contextImprint", #function)
            return
        }
        let message: String = "Roses are red. My name is Dave. This poem have no sense"
        let context: String = "Microwave"
        
        var encryptedMessage: Data = Data()
        do {
            // context is NOT optional parameter here
            encryptedMessage = try contextImprint.encrypt(message.data(using: .utf8)!,
                                                          context: context.data(using: .utf8)!)
            print("encryptedMessage = \(encryptedMessage)")
            
        } catch let error as NSError {
            print("Error occurred while encrypting \(error)", #function)
            return
        }
        
        do {
            // context is NOT optional parameter here
            let decryptedMessage = try contextImprint.decrypt(encryptedMessage,
                                                              context: context.data(using: .utf8)!)
            let resultString: String = String(data: decryptedMessage, encoding: .utf8)!
            print("decryptedMessage = \(resultString)")
            
        } catch let error as NSError {
            print("Error occurred while decrypting \(error)", #function)
            return
        }
    }
    
    // MARK: - Secure Cell with Passphrase
    func runExampleSecureCellWithPassphrase() {
        print("----------------------------------", #function)
        let cellWithPassphrase = TSCellSeal(passphrase: "We are the champions")!
        let message = "Your secret is safe with us"
        let context = "Many secrets are safe"
        
        
        var encryptedMessage: Data = Data()
        do {
            // context is optional parameter and may be omitted
            encryptedMessage = try cellWithPassphrase.encrypt(message.data(using: .utf8)!,
                                                              context: context.data(using: .utf8)!)
            print("encryptedMessage = \(encryptedMessage)")
            
        } catch let error as NSError {
            print("Error occurred while encrypting \(error)", #function)
            return
        }
        
        do {
            let decryptedMessage = try cellWithPassphrase.decrypt(encryptedMessage,
                                                                  context: context.data(using: .utf8)!)
            let resultString: String = String(data: decryptedMessage, encoding: .utf8)!
            print("decryptedMessage = \(resultString)")
            
        } catch let error as NSError {
            print("Error occurred while decrypting \(error)", #function)
            return
        }
    }
    
    // MARK: - Key Generation
    func runExampleGeneratingKeys() {
        runExampleGeneratingAsymKeys()
        runExampleGeneratingSymKeys()
    }
    
    
    // MARK: - RSA/EC
    func runExampleGeneratingAsymKeys() {
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
    
    func runExampleGeneratingSymKeys() {
        print("----------------------------------", #function)
        
        let cell = TSCellSeal(key: TSGenerateSymmetricKey()!)!
        let message = "All your base are belong to us!"
        let context = "For great justice"

        let encrypted = try? cell.encrypt(message.data(using: .utf8)!,
                                          context: context.data(using: .utf8)!)
        print("Cell encrypted with symm key \(encrypted!)")

        let decrypted = try? cell.decrypt(encrypted!,
                                          context: context.data(using: .utf8)!)
        print("Cell decrypted with symm key \(decrypted!)")

        let decryptedMessage = String(data: decrypted!, encoding: .utf8)
        print("Cell decrypted content \(decryptedMessage!)")
    }
    
    // MARK: - Keys from file
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
    
    // MARK: - Secure Message
    // MARK: - encryption/decryption
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
                                                  peerPublicKey: serverPublicKey)!
        
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
                                                  peerPublicKey: clientPublicKey)!
        
        do {
            let decryptedMessage: Data = try decrypter.unwrapData(encryptedMessage)
            let resultString: String = String(data: decryptedMessage, encoding: .utf8)!
            print("decryptedMessage->\n\(resultString)")
            
        } catch let error as NSError {
            print("Error occurred while decrypting \(error)", #function)
            return
        }
    }
    
    // MARK: - sign/vefiry
    func runExampleSecureMessageSignVerify() {
        print("----------------------------------", #function)
        
        // base64 encoded keys:
        // private key
        // public key
        
        let publicKeyString: String = "VUVDMgAAAC2ELbj5Aue5xjiJWW3P2KNrBX+HkaeJAb+Z4MrK0cWZlAfpBUql"
        let privateKeyString: String = "UkVDMgAAAC1FsVa6AMGljYqtNWQ+7r4RjXTabLZxZ/14EXmi6ec2e1vrCmyR"
        
        guard let publicKey: Data = Data(base64Encoded: publicKeyString,
                                         options: .ignoreUnknownCharacters),
            let privateKey: Data = Data(base64Encoded: privateKeyString,
                                        options: .ignoreUnknownCharacters) else {
                                            print("Error occurred during base64 encoding", #function)
                                            return
        }
        
        // ---------- signing ----------------
        // use private key
        
        let signer: TSMessage = TSMessage.init(inSignVerifyModeWithPrivateKey: privateKey,
                                               peerPublicKey: nil)!
        
        let message: String = "I had a problem so I though to use Java. Now I have a ProblemFactory."
        
        var signedMessage: Data = Data()
        do {
            signedMessage = try signer.wrap(message.data(using: .utf8))
            print("signedMessage = \(signedMessage)")
            
        } catch let error as NSError {
            print("Error occurred while signing \(error)", #function)
            return
        }
        
        // ---------- verification ----------------
        // use public key
        let verifier = TSMessage.init(inSignVerifyModeWithPrivateKey: nil,
                                      peerPublicKey: publicKey)!
        
        do {
            let verifiedMessage: Data = try verifier.unwrapData(signedMessage)
            let resultString: String = String(data: verifiedMessage, encoding: .utf8)!
            print("verifiedMessage ->\n\(resultString)")
            
        } catch let error as NSError {
            print("Error occurred while verifing \(error)", #function)
            return
        }
    }

    // MARK: - Secure Comparator
    func runExampleSecureComparator() {
        print("----------------------------------", #function)
        
        let sharedMessage = "shared secret"
        let client: TSComparator = TSComparator.init(messageToCompare: sharedMessage.data(using: .utf8)!)!
        let server: TSComparator = TSComparator.init(messageToCompare: sharedMessage.data(using: .utf8)!)!
        
        // send this message to server
        var data = try? client.beginCompare()
        
        while (client.status() == TSComparatorStateType.notReady ||
            server.status() == TSComparatorStateType.notReady ) {
                
            // receive from server
            data = try? server.proceedCompare(data)
            
            // proceed and send again
            data = try? client.proceedCompare(data)
        }
        
        if (client.status() == TSComparatorStateType.match) {
            // secrets match
            print("SecureComparator secrets match")
        } else {
            // secrets don't match
            print("SecureComparator secrets do not match")
        }
    }
}
