//
//  SecureComparatorTests.m
//  objthemis
//
//  Created by Anastasiia on 1/19/18.
//

#import <XCTest/XCTest.h>
#import <objcthemis/objcthemis.h>


#define SECURE_COMPARATOR_ENABLED
#import <objcthemis/scomparator.h>


@interface SecureComparatorTests : XCTestCase

@end

@implementation SecureComparatorTests

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

/*
 alice = scomparator.SComparator(self.message)
 bob = scomparator.SComparator(self.message)
 data = alice.begin_compare()
 while not (alice.is_compared() and bob.is_compared()):
 data = alice.proceed_compare(bob.proceed_compare(data))
 self.assertTrue(alice.is_equal())
 self.assertTrue(bob.is_equal())
 */

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
    XCTAssertEqual([alice status], TSComparatorNotMatch, @"comparison should match");
    XCTAssertEqual([bob status], TSComparatorNotMatch, @"comparison should match");
}


@end
