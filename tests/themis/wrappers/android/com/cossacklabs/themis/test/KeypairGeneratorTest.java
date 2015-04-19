package com.cossacklabs.themis.test;

import com.cossacklabs.themis.Keypair;
import com.cossacklabs.themis.KeypairGenerator;
import com.cossacklabs.themis.KeyGenerationException;

import android.test.AndroidTestCase;

public class KeypairGeneratorTest extends AndroidTestCase {
	
	@Override
	public void runTest() {
		
		Keypair pair = null;
		
		try {
			pair = KeypairGenerator.generateKeypair();
		} catch (KeyGenerationException e) {
			fail("Failed to generate keypair");
		}
		
		assertNotNull(pair);
		assertNotNull(pair.getPrivateKey());
		assertNotNull(pair.getPublicKey());
	}
	
}