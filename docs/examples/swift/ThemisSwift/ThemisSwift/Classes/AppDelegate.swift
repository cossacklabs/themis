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


    func application(application: UIApplication, didFinishLaunchingWithOptions launchOptions: [NSObject: AnyObject]?) -> Bool {
        
        // Please, look in debug console to see results
        
        // Secure Cell:
        runExampleSecureCellSealMode()
        runExampleSecureCellTokenProtectMode()
        runExampleSecureCellImprint()
        
        // Generating/reading keys:
        runExampleGeneratingKeys()
        readingKeysFromFile()
        
        return true
    }

    
    
    func generateMasterKey() -> NSData {
        let masterKeyString: String = "UkVDMgAAAC13PCVZAKOczZXUpvkhsC+xvwWnv3CLmlG0Wzy8ZBMnT+2yx/dg"
        let masterKeyData: NSData = NSData(base64EncodedString: masterKeyString, options: .IgnoreUnknownCharacters)!
        return masterKeyData
    }
    
    // MARK:- Secure Cell
    // MARK:- cell seal mode
    func runExampleSecureCellSealMode() {
        print("----------------------------------", #function)
        let masterKeyData: NSData = self.generateMasterKey()
        guard let cellSeal: TSCellSeal = TSCellSeal(key: masterKeyData) else {
            print("Error occured while initializing object cellSeal", #function)
            return
        }
        let message: String = "All your base are belong to us!"
        let context: String = "For great justice"
        
        var encryptedMessage: NSData = NSData()
        do {
            // context is optional parameter and may be ignored
            encryptedMessage = try cellSeal.wrapData(message.dataUsingEncoding(NSUTF8StringEncoding),
                                                     context: context.dataUsingEncoding(NSUTF8StringEncoding))
            print("encryptedMessage = \(encryptedMessage)")
            
        } catch let error as NSError {
            print("Error occured while enrypting \(error)", #function)
            return
        }
        
        
        do {
            let decryptedMessage: NSData = try cellSeal.unwrapData(encryptedMessage,
                                                       context: context.dataUsingEncoding(NSUTF8StringEncoding))
            let resultString: String = String(data: decryptedMessage, encoding: NSUTF8StringEncoding)!
            print("encryptedMessage = \(resultString)")
            
        } catch let error as NSError {
            print("Error occured while decrypting \(error)", #function)
            return
        }
    }
    
    // MARK:- cell token protect mode
    func runExampleSecureCellTokenProtectMode() {
        print("----------------------------------", #function)
        let masterKeyData: NSData = self.generateMasterKey()
        guard let cellToken: TSCellToken = TSCellToken(key: masterKeyData) else {
            print("Error occured while initializing object cellToken", #function)
            return
        }
        let message: String = "Roses are grey. Violets are grey."
        let context: String = "I'm a dog"
        
        var encryptedMessage: TSCellTokenEncryptedData = TSCellTokenEncryptedData()
        do {
            // context is optional parameter and may be ignored
            encryptedMessage = try cellToken.wrapData(message.dataUsingEncoding(NSUTF8StringEncoding),
                                                      context: context.dataUsingEncoding(NSUTF8StringEncoding))
            print("encryptedMessage.cipher = \(encryptedMessage.cipherText)")
            print("encryptedMessage.token = \(encryptedMessage.token)")
            
        } catch let error as NSError {
            print("Error occured while enrypting \(error)", #function)
            return
        }
        
    
        do {
            let decryptedMessage: NSData = try cellToken.unwrapData(encryptedMessage,
                                                            context: context.dataUsingEncoding(NSUTF8StringEncoding))
            let resultString: String = String(data: decryptedMessage, encoding: NSUTF8StringEncoding)!
            print("encryptedMessage = \(resultString)")
            
        } catch let error as NSError {
            print("Error occured while decrypting \(error)", #function)
            return
        }
    }
    
    
    // MARK:- cell imprint
    func runExampleSecureCellImprint() {
        print("----------------------------------", #function)
        let masterKeyData: NSData = self.generateMasterKey()
        guard let contextImprint: TSCellContextImprint = TSCellContextImprint(key: masterKeyData) else {
            print("Error occured while initializing object contextImprint", #function)
            return
        }
        let message: String = "Roses are red. My name is Dave. This poem have no sense"
        let context: String = "Microwave"
        
        var encryptedMessage: NSData = NSData()
        do {
            // context is NOT optional parameter here
            encryptedMessage = try contextImprint.wrapData(message.dataUsingEncoding(NSUTF8StringEncoding),
                                                           context: context.dataUsingEncoding(NSUTF8StringEncoding))
            print("encryptedMessage = \(encryptedMessage)")
            
        } catch let error as NSError {
            print("Error occured while enrypting \(error)", #function)
            return
        }
        
        
        do {
            // context is NOT optional parameter here
            let decryptedMessage: NSData = try contextImprint.unwrapData(encryptedMessage,
                                                            context: context.dataUsingEncoding(NSUTF8StringEncoding))
            let resultString: String = String(data: decryptedMessage, encoding: NSUTF8StringEncoding)!
            print("encryptedMessage = \(resultString)")
            
        } catch let error as NSError {
            print("Error occured while decrypting \(error)", #function)
            return
        }
    }
    
    
    // MARK:- Key Generation
    
    // MARK:- RSA/EC
    func runExampleGeneratingKeys() {
        print("----------------------------------", #function)
        
        // Generating RSA keys
        guard let keyGeneratorRSA: TSKeyGen = TSKeyGen(algorithm: .RSA) else {
            print("Error occured while initializing object keyGeneratorRSA", #function)
            return
        }
        let privateKeyRSA: NSData = keyGeneratorRSA.privateKey
        let publicKeyRSA: NSData = keyGeneratorRSA.publicKey
        print("RSA privateKey = \(privateKeyRSA)")
        print("RSA publicKey = \(publicKeyRSA)")
        
        // Generating EC keys
        guard let keyGeneratorEC: TSKeyGen = TSKeyGen(algorithm: .EC) else {
            print("Error occured while initializing object keyGeneratorEC", #function)
            return
        }
        let privateKeyEC: NSData = keyGeneratorEC.privateKey
        let publicKeyEC: NSData = keyGeneratorEC.publicKey
        print("EC privateKey = \(privateKeyEC)")
        print("RSA publicKey = \(publicKeyEC)")
    }
    

    // MARK:- Keys from file
    // Sometimes you will need to read keys from files
    func readingKeysFromFile() {
        print("----------------------------------", #function)
        let fileManager: NSFileManager = NSFileManager.defaultManager()
        let mainBundle: NSBundle = NSBundle.mainBundle()
        
        // yes, app will crash if no keys. that's idea of our sample
        
        let serverPrivateKeyFromFile: NSData = fileManager.contentsAtPath(mainBundle.pathForResource("server",
                                                                                             ofType: "priv")!)!
        let serverPublicKeyFromFile: NSData = fileManager.contentsAtPath(mainBundle.pathForResource("server",
                                                                                            ofType: "pub")!)!
        let clientPrivateKeyOldFromFile: NSData = fileManager.contentsAtPath(mainBundle.pathForResource("client",
                                                                                                ofType: "priv")!)!
        let clientPublicKeyOldFromFile: NSData = fileManager.contentsAtPath(mainBundle.pathForResource("client",
                                                                                               ofType: "pub")!)!
        
        print("serverPrivateKeyFromFile = \(serverPrivateKeyFromFile)")
        print("serverPublicKeyFromFile = \(serverPublicKeyFromFile)")
        print("clientPrivateKeyOldFromFile = \(clientPrivateKeyOldFromFile)")
        print("clientPublicKeyOldFromFile = \(clientPublicKeyOldFromFile)")
    }
    
    
}

