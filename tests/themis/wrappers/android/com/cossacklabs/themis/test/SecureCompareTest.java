package com.cossacklabs.themis.test;

import java.util.Random;

import com.cossacklabs.themis.SecureCompare;
import com.cossacklabs.themis.SecureCompareException;

import static org.junit.Assert.*;
import org.junit.Test;

public class SecureCompareTest {
	
	static final int MAX_TEST_DATA = 1024;
	Random rand = new Random();
	
	private byte[] generateTestData() {
		int dataLength = 0;
		
		do {
			dataLength = rand.nextInt(MAX_TEST_DATA);
		} while (0 == dataLength);
		
		byte[] data = new byte[dataLength];
		rand.nextBytes(data);
		
		return data;
	}
	
	private void performCompare(boolean shouldMatch) throws SecureCompareException {
		
		byte[] compareData = generateTestData();
		SecureCompare.CompareResult expectedResult;
		
		if (shouldMatch) {
			expectedResult = SecureCompare.CompareResult.MATCH;
		} else {
			expectedResult = SecureCompare.CompareResult.NO_MATCH;
		}
		
		SecureCompare alice = new SecureCompare(compareData);
		
		if (!shouldMatch) {
			compareData[0]++;
		}
		
		SecureCompare bob = new SecureCompare(compareData);

		// Initiating secure compare (alice, step1)
		byte[] peerData = alice.begin();

		assertNotNull(peerData);
		assertTrue(SecureCompare.CompareResult.NOT_READY == alice.getResult());
		assertTrue(SecureCompare.CompareResult.NOT_READY == bob.getResult());
		
		// alice --> peerData --> bob

		// Accepting secure compare (bob, step2)
		peerData = bob.proceed(peerData);
		assertNotNull(peerData);
		assertTrue(SecureCompare.CompareResult.NOT_READY == alice.getResult());
		assertTrue(SecureCompare.CompareResult.NOT_READY == bob.getResult());
		
		// bob --> peerData --> alice

		// ALice, step3
		peerData = alice.proceed(peerData);
		assertNotNull(peerData);
		assertTrue(SecureCompare.CompareResult.NOT_READY == alice.getResult());
		assertTrue(SecureCompare.CompareResult.NOT_READY == bob.getResult());
		
		// alice --> peerData --> bob

		// Bob, step4, should be ready after
		peerData = bob.proceed(peerData);
		assertNotNull(peerData);
		assertTrue(SecureCompare.CompareResult.NOT_READY == alice.getResult());
		assertTrue(expectedResult == bob.getResult());
		
		// bob --> peerData --> alice

		// Alice, step5, should be ready after
		peerData = alice.proceed(peerData);
		assertNull(peerData);
		assertTrue(expectedResult == alice.getResult());
		assertTrue(expectedResult == bob.getResult());
	}
	
	@Test
	public void runTest() {
				
		try {
			
			performCompare(true);
			performCompare(false);
			
		} catch (Exception e) {
			String failMessage = e.getClass().getCanonicalName();
			
			if (null != e.getMessage()) {
				failMessage += ": " + e.getMessage();
			}
			
			fail(failMessage);
		}
		
	}
}
