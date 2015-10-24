//
//  AppDelegate.m
//  WorkingWithThemisServer
//
//  Created by Anastasi Voitova on 24.10.15.
//  Copyright © 2015 Stanfy. All rights reserved.
//

#import "AppDelegate.h"
#import "SMessageClient.h"
#import "SSessionClient.h"


@interface AppDelegate ()

@end

@implementation AppDelegate



- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {

    // See https://themis.cossacklabs.com/interactive-simulator/setup/

    NSLog(@" ------------ running SMessage Client example ");
    [[SMessageClient new] runSecureMessageCITest];
    
    
    NSLog(@" ------------ running SSession Client example ");
    [[SSessionClient new] runSecureSessionCITest];
    
    
    NSLog(@" ------------ ");
    
    return YES;
}

@end
