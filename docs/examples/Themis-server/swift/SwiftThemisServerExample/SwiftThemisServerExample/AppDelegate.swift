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


    func application(_ application: UIApplication, didFinishLaunchingWithOptions launchOptions: [UIApplication.LaunchOptionsKey: Any]?) -> Bool {
        
        // Check ReadME!
        // Start server simulator 
        // https://docs.cossacklabs.com/simulator/interactive/
        // 
        // Read more how server simulator works
        // https://docs.cossacklabs.com/pages/documentation-themis/#interactive-simulator-themis-server
        
        print(" ------------ running SMessage Client example ")
        SMessageClient().runSecureMessageCITest()
        
        //print(" ------------ running SSession Client example ")
        //SSessionClient().runSecureSessionCITest()
        
        return true
    }
}

