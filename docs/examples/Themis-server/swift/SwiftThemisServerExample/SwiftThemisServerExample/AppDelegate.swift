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


    func application(application: UIApplication, didFinishLaunchingWithOptions launchOptions: [NSObject: AnyObject]?) -> Bool {
        
        // See https://themis.cossacklabs.com/interactive-simulator/setup/
        print(" ------------ running SMessage Client example ")
        SMessageClient().runSecureMessageCITest()
        
        print(" ------------ running SSession Client example ")
        SSessionClient().runSecureMessageCITest()
        
        return true
    }
}

