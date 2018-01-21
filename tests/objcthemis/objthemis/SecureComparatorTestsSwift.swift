//
//  SecureComparatorTestsSwift.swift
//  objthemis
//
//  Created by Anastasiia on 1/19/18.
//

import XCTest

class SecureComparatorTestsSwift: XCTestCase {
    
    func testSecureComparatorEqualMessage() {
        
        let sharedMessage = "shared secret"
        let alice: TSComparator = TSComparator.init(messageToCompare: sharedMessage.data(using: .utf8)!)!
        let bob: TSComparator = TSComparator.init(messageToCompare: sharedMessage.data(using: .utf8)!)!
        
        var data = try? alice.beginCompare()
        
        while (alice.status() == TSComparatorStateType.notReady || bob.status() == TSComparatorStateType.notReady ) {
            data = try? bob.proceedCompare(data)
            data = try? alice.proceedCompare(data)
        }
        
        XCTAssertEqual(alice.status(), TSComparatorStateType.match, "comparison should match")
        XCTAssertEqual(bob.status(), TSComparatorStateType.match, "comparison should match")
    }
    
    func testSecureComparatorDifferentMessage() {
        let alice: TSComparator = TSComparator.init(messageToCompare: "one secret".data(using: .utf8)!)!
        let bob: TSComparator = TSComparator.init(messageToCompare: "another secret".data(using: .utf8)!)!
        
        var data = try? alice.beginCompare()
        
        while (alice.status() == TSComparatorStateType.notReady || bob.status() == TSComparatorStateType.notReady ) {
            data = try? bob.proceedCompare(data)
            data = try? alice.proceedCompare(data)
        }
        
        XCTAssertEqual(alice.status(), TSComparatorStateType.notMatch, "comparison should not match")
        XCTAssertEqual(bob.status(), TSComparatorStateType.notMatch, "comparison should not match")
    }
    
}
