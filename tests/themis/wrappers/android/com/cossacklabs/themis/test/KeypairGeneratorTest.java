package com.cossacklabs.themis.test;

import com.cossacklabs.themis.Keypair;
import com.cossacklabs.themis.KeypairGenerator;

import android.test.AndroidTestCase;

public class KeypairGeneratorTest extends AndroidTestCase {
	
	@Override
	public void runTest() {
		Keypair pair = KeypairGenerator.generateKeypair();
		
		assertNotNull(pair);
		assertNotNull(pair.getPrivateKey());
		assertNotNull(pair.getPublicKey());
	}
	
}