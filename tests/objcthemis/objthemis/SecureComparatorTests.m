//
//  SecureComparatorTests.m
//  objthemis
//
//  Created by Anastasiia on 1/19/18.
//

#import <XCTest/XCTest.h>

// TODO: use a unified import here
// CocoaPods tests do not work with canonical import of Themis for some reason.
// Please fix this if you have any idea how.
#if COCOAPODS
#import <objcthemis/objcthemis.h>
#else
@import themis;
#endif

@interface SecureComparatorTests : XCTestCase

@end

@implementation SecureComparatorTests

- (void)testSecureComparatorEqualMessage {
    NSString * sharedSecret = @"shared secret";
    NSData * sharedSecretData = [sharedSecret dataUsingEncoding:NSUTF8StringEncoding];
    TSComparator * alice = [[TSComparator alloc] initWithMessageToCompare:sharedSecretData];
    TSComparator * bob = [[TSComparator alloc] initWithMessageToCompare:sharedSecretData];
    NSError * error = nil;
    
    NSData * data = [alice beginCompare:&error];
    while ([alice status] == TSComparatorNotReady || [bob status] == TSComparatorNotReady) {
        data = [bob proceedCompare:data error:&error];
        data = [alice proceedCompare:data error:&error];
    }
    
    XCTAssertNil(error, @"result of comparison should be successful");
    XCTAssertEqual([alice status], TSComparatorMatch, @"comparison should match");
    XCTAssertEqual([bob status], TSComparatorMatch, @"comparison should match");
}

- (void)testSecureComparatorDifferentMessage {
    TSComparator * alice = [[TSComparator alloc] initWithMessageToCompare:[@"some secret" dataUsingEncoding:NSUTF8StringEncoding]];
    TSComparator * bob = [[TSComparator alloc] initWithMessageToCompare:[@"another secret" dataUsingEncoding:NSUTF8StringEncoding]];
    NSError * error = nil;
    
    NSData * data = [alice beginCompare:&error];
    while ([alice status] == TSComparatorNotReady || [bob status] == TSComparatorNotReady) {
        data = [bob proceedCompare:data error:&error];
        data = [alice proceedCompare:data error:&error];
    }
    
    XCTAssertNil(error, @"result of comparison should be successful");
    XCTAssertEqual([alice status], TSComparatorNotMatch, @"comparison should not match");
    XCTAssertEqual([bob status], TSComparatorNotMatch, @"comparison should not match");
}


@end
