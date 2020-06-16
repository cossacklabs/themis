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

import com.cossacklabs.themis.ISessionCallbacks;
import com.cossacklabs.themis.KeyGenerationException;
import com.cossacklabs.themis.Keypair;
import com.cossacklabs.themis.KeypairGenerator;
import com.cossacklabs.themis.PublicKey;
import com.cossacklabs.themis.SecureSession;
import com.cossacklabs.themis.SecureSession.SessionDataType;
import com.cossacklabs.themis.SecureSession.UnwrapResult;

import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

public class SecureSessionTest {
	
	ISessionCallbacks callbacks = new ISessionCallbacks() {

		@Override
		public PublicKey getPublicKeyForId(SecureSession session, byte[] id) {
			
			try {
				byte[] serverId = "server".getBytes("UTF-16");
				byte[] clientId = "client".getBytes("UTF-16");
				
				if (Arrays.equals(id, serverId)) {
					return serverPair.getPublicKey();
				} else if (Arrays.equals(id, clientId)) {
					return clientPair.getPublicKey();
				}
			} catch (Exception e) {};
			
			fail("Could not parse requested id");
			return null;
		}

		@Override
		public void stateChanged(SecureSession session) {
			assertFalse(SecureSession.State.IDLE == session.getState());
			
			try {
				if (SecureSession.State.NEGOTIATING == session.getState()) {
					assertFalse(session.isEstablished());
				}
				
				if (SecureSession.State.ESTABLISHED == session.getState()) {
					assertTrue(session.isEstablished());
				}
			} catch (Exception e) {
				String failMessage = e.getClass().getCanonicalName();
				
				if (null != e.getMessage()) {
					failMessage += ": " + e.getMessage();
				}
				
				fail(failMessage);
			}
		}
	};
	
	Keypair clientPair;
	Keypair serverPair;
	
	
	@Before
	public void setUp() {
		try {
			clientPair = KeypairGenerator.generateKeypair();
			serverPair = KeypairGenerator.generateKeypair();
		} catch (KeyGenerationException e) {
			fail("Failed to generate keypairs");
		}
	}
	
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
	
	// Allow deprecated #save and #restore methods.
	@SuppressWarnings("deprecation")
	@Test
	public void runTest() {
		
		assertNotNull(clientPair);
		assertNotNull(serverPair);
		
		try {
			SecureSession clientSession = new SecureSession("client", clientPair.getPrivateKey(), callbacks);
			
			// Initiating secure session (client side)
			byte[] connectRequest = clientSession.generateConnectRequest();
			assertNotNull(connectRequest);
			
			// client --> connectRequest --> server
			
			// Accepting secure session request (server side)
			SecureSession serverSession = new SecureSession("server", serverPair.getPrivateKey(), callbacks);
			UnwrapResult result = serverSession.unwrap(connectRequest);
			
			assertTrue(SecureSession.SessionDataType.PROTOCOL_DATA == result.getDataType());
			assertTrue(result.getData().length > 0);
			
			// server --> result.getData() --> client
			
			result = clientSession.unwrap(result.getData());
			
			assertTrue(SecureSession.SessionDataType.PROTOCOL_DATA == result.getDataType());
			assertTrue(result.getData().length > 0);
			
			// client --> result.getData() --> server
			
			result = serverSession.unwrap(result.getData());
			
			assertTrue(SecureSession.SessionDataType.PROTOCOL_DATA == result.getDataType());
			assertTrue(result.getData().length > 0);
			
			// server --> result.getData() --> client
			
			result = clientSession.unwrap(result.getData());
			
			assertTrue(SecureSession.SessionDataType.NO_DATA == result.getDataType());
			assertTrue(result.getData().length == 0);
			
			// client got NO_DATA: it means secure session is established. Client may send user data now
		
			byte[] message = generateTestData();
			byte[] wrappedMessage = clientSession.wrap(message);
			
			assertNotNull(wrappedMessage);
			assertTrue(wrappedMessage.length > message.length);
			
			// client --> wrappedMessage --> server
			
			result = serverSession.unwrap(wrappedMessage);
			
			assertTrue(SecureSession.SessionDataType.USER_DATA == result.getDataType());
			assertTrue(result.getData().length > 0);
			assertTrue(Arrays.equals(result.getData(), message));
			
			wrappedMessage = serverSession.wrap(result.getData());
			
			assertNotNull(wrappedMessage);
			assertTrue(wrappedMessage.length > result.getData().length);
			
			// server --> wrappedMessage --> client
			
			byte[] clientState = clientSession.save();
			//clientSession.close();
			
			clientSession = SecureSession.restore(clientState, callbacks);
			
			result = clientSession.unwrap(wrappedMessage);
			
			assertTrue(SecureSession.SessionDataType.USER_DATA == result.getDataType());
			assertTrue(result.getData().length > 0);
			assertTrue(Arrays.equals(result.getData(), message));
			
			clientSession.close();
			serverSession.close();
		
		} catch (Exception e) {
			String failMessage = e.getClass().getCanonicalName();
			
			if (null != e.getMessage()) {
				failMessage += ": " + e.getMessage();
			}
			
			fail(failMessage);
		}
		
	}
}
