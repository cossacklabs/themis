//
//  main.m
//  smessage
//
//  Created by Admin on 22.10.15.
//  Copyright © 2015 Cossacklabs. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "SMessageClient.h"

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        SMessageClient * client = [SMessageClient new];
        [client secureMessageCITest];
        sleep(10);
        
    }
    return 0;
}
