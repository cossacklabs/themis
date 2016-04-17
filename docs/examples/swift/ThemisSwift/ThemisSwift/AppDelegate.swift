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
        
        
        // Secure Cell:
        runExampleSecureCellSealMode()
        
        return true
    }

    
    // MARK:- examples
    
    func generateMasterKey() -> NSData {
        let masterKeyString: String = "UkVDMgAAAC13PCVZAKOczZXUpvkhsC+xvwWnv3CLmlG0Wzy8ZBMnT+2yx/dg"
        let masterKeyData: NSData = NSData(base64EncodedString: masterKeyString, options: .IgnoreUnknownCharacters)!
        return masterKeyData
    }
    
    func runExampleSecureCellSealMode() {
        print("----------------------------------", #function)
        let masterKeyData: NSData = self.generateMasterKey()
        guard let cellSeal: TSCellSeal = TSCellSeal(key: masterKeyData) else {
            print("Error occured while initializing object cellSeal", #function)
            return
        }
        let message: String = "All your base are belong to us!"
        let context: String = "For great justice"
        
        // context is optional parameter and may be ignored
        
        var encryptedMessage: NSData = NSData()
        do {
            encryptedMessage = try cellSeal.wrapData(message.dataUsingEncoding(NSUTF8StringEncoding),
                                                     context: context.dataUsingEncoding(NSUTF8StringEncoding))
            print("encryptedMessage = \(encryptedMessage)")
            
        } catch let error as NSError {
            print("Error occured while enrypting \(error)", #function)
            return
        }
        
        
        var decryptedMessage: NSData = NSData()
        do {
            decryptedMessage = try cellSeal.unwrapData(encryptedMessage,
                                                       context: context.dataUsingEncoding(NSUTF8StringEncoding))
            let resultString: String = String(data: decryptedMessage, encoding: NSUTF8StringEncoding)!
            print("encryptedMessage = \(resultString)")
            
        } catch let error as NSError {
            print("Error occured while decrypting \(error)", #function)
            return
        }

    }
    
    
}

