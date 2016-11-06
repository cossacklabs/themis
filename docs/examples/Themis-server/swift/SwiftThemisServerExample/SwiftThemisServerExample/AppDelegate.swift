//
//  AppDelegate.swift
//  SwiftThemisServerExample
//
//  Created by Anastasi Voitova on 19.04.16.
//  Copyright © 2016 CossackLabs. All rights reserved.
//

import UIKit

@UIApplicationMain
class AppDelegate: UIResponder, UIApplicationDelegate {

    var window: UIWindow?


    func application(_ application: UIApplication, didFinishLaunchingWithOptions launchOptions: [UIApplicationLaunchOptionsKey: Any]?) -> Bool {
        
        // Check ReadME!
        // Start server simulator 
        // https://themis.cossacklabs.com/interactive-simulator/setup/
        // 
        // Read more how server simulator works
        // https://github.com/cossacklabs/themis/wiki/Using-Themis-Server
        
        print(" ------------ running SMessage Client example ")
        SMessageClient().runSecureMessageCITest()
        
        //print(" ------------ running SSession Client example ")
        //SSessionClient().runSecureSessionCITest()
        
        return true
    }
}

