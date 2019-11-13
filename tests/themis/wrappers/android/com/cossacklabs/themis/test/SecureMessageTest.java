/*
* Copyright (c) 2015 Cossack Labs Limited
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

package com.cossacklabs.themis.test;

import java.util.Arrays;
import java.util.Random;

import com.cossacklabs.themis.KeyGenerationException;
import com.cossacklabs.themis.Keypair;
import com.cossacklabs.themis.KeypairGenerator;
import com.cossacklabs.themis.NullArgumentException;
import com.cossacklabs.themis.SecureMessage;
import com.cossacklabs.themis.SecureMessageWrapException;

import static org.junit.Assert.*;
import org.junit.Test;

public class SecureMessageTest {
	
	@Test
	public void runTest() {
		
		Keypair aPair = null;
		Keypair bPair = null;
		
		try {
			aPair = KeypairGenerator.generateKeypair();
			bPair = KeypairGenerator.generateKeypair();
		} catch (KeyGenerationException e) {
			fail("Failed to generate keypairs");
		}
		
		assertNotNull(aPair);
		assertNotNull(bPair);

		Random rand = new Random();
		
		int messageLength = 0;
		
		do {
			messageLength = rand.nextInt(2048);
		} while (0 == messageLength);
		
		byte[] message = new byte[messageLength];
		rand.nextBytes(message);
		
		try {
			SecureMessage aWrapper = new SecureMessage(aPair.getPrivateKey(), bPair.getPublicKey());
			SecureMessage bWrapper = new SecureMessage(bPair.getPrivateKey(), aPair.getPublicKey());
			
			byte[] wrappedMessage = aWrapper.wrap(message);
			
			assertTrue(message.length < wrappedMessage.length);
			
			byte[] unwrappedMessage = bWrapper.unwrap(wrappedMessage);
			
			assertTrue(Arrays.equals(message, unwrappedMessage));


			SecureMessage signer = new SecureMessage(aPair.getPrivateKey());
			SecureMessage verifier = new SecureMessage(aPair.getPublicKey());

			byte[] signedMessage = signer.sign(message);

			assertTrue(message.length < signedMessage.length);

			byte[] verifiedMessage = verifier.verify(signedMessage);

			assertTrue(Arrays.equals(message, verifiedMessage));

		} catch (NullArgumentException e) {
			fail(e.getClass().getCanonicalName() + ": " + e.getMessage());
		} catch (SecureMessageWrapException e) {
			fail(e.getClass().getCanonicalName());
		}
	}
}
