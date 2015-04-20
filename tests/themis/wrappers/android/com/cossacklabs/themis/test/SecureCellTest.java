package com.cossacklabs.themis.test;

import java.util.Arrays;
import java.util.Random;

import com.cossacklabs.themis.SecureCell;
import com.cossacklabs.themis.SecureCellData;

import android.test.AndroidTestCase;

public class SecureCellTest extends AndroidTestCase {
	
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

	@Override
	public void runTest() {
		try {
			testSeal();
			testTokenProtect();
			testContextImprint();
		} catch (Exception e) {
			String failMessage = e.getClass().getCanonicalName();
			
			if (null != e.getMessage()) {
				failMessage += ": " + e.getMessage();
			}
			
			fail(failMessage);
		}
	}
	
	void testSeal() throws Exception {
		String key = "seal key";
		String context = "seal context";
		byte[] data = generateTestData();
		
		SecureCell cell = new SecureCell(key);
		SecureCellData protectedData = cell.protect(context, data);
		
		assertNotNull(protectedData);
		assertNotNull(protectedData.getProtectedData());
		assertNull(protectedData.getAdditionalData());
		
		assertTrue(protectedData.getProtectedData().length > data.length);
		
		cell = new SecureCell(key);
		byte[] unprotectedData = cell.unprotect(context, protectedData);
		
		assertTrue(Arrays.equals(data, unprotectedData));
	}
	
	void testTokenProtect() throws Exception {
		String key = "token protect key";
		String context = "token protect context";
		byte[] data = generateTestData();
		
		SecureCell cell = new SecureCell(key, SecureCell.MODE_TOKEN_PROTECT);
		SecureCellData protectedData = cell.protect(context, data);
		
		assertNotNull(protectedData);
		assertNotNull(protectedData.getProtectedData());
		assertNotNull(protectedData.getAdditionalData());
		
		assertTrue(protectedData.getProtectedData().length == data.length);
		
		cell = new SecureCell(key);
		byte[] unprotectedData = cell.unprotect(context, protectedData);
		
		assertTrue(Arrays.equals(data, unprotectedData));
	}
	
	void testContextImprint() throws Exception {
		String key = "context imprint key";
		String context = "context imprint context";
		byte[] data = generateTestData();
		
		SecureCell cell = new SecureCell(key, SecureCell.MODE_CONTEXT_IMPRINT);
		SecureCellData protectedData = cell.protect(context, data);
		
		assertNotNull(protectedData);
		assertNotNull(protectedData.getProtectedData());
		assertNull(protectedData.getAdditionalData());
		
		assertTrue(protectedData.getProtectedData().length == data.length);
		
		cell = new SecureCell(key);
		byte[] unprotectedData = cell.unprotect(context, protectedData);
		
		assertTrue(Arrays.equals(data, unprotectedData));
	}
}
