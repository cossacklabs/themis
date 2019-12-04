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

import java.io.InputStream;
import java.io.InterruptedIOException;
import java.io.OutputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Arrays;
import java.util.Random;

import com.cossacklabs.themis.ISessionCallbacks;
import com.cossacklabs.themis.KeyGenerationException;
import com.cossacklabs.themis.Keypair;
import com.cossacklabs.themis.KeypairGenerator;
import com.cossacklabs.themis.PublicKey;
import com.cossacklabs.themis.SecureServerSocket;
import com.cossacklabs.themis.SecureSession;
import com.cossacklabs.themis.SecureSocket;

import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

public class SecureSocketTest {
	
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
	
	static final int clientPort = 10110;
	static final int serverPort = 10120;
	
	byte[] data;
	byte[] response;
	
	static final byte[] finishMessage = {(byte) 0xDE, (byte) 0xAD, (byte) 0xC0, (byte) 0xDE};
	
	static boolean isFinished(byte[] message) {
		if (message.length >= finishMessage.length) {
			return Arrays.equals(finishMessage, Arrays.copyOf(message, finishMessage.length));
		}
		
		return false;
	}
	
	@Test
	public void runTest() {
		
		try {
			
			serverThread.start();
			
			// Give some time for server thread to start before trying to connect
			Thread.sleep(500);
			
			clientThread.start();
			
			clientThread.join();
			serverThread.join();
			
			assertTrue(Arrays.equals(data, response));
			
		} catch (Exception e) {
			String failMessage = e.getClass().getCanonicalName();
			
			if (null != e.getMessage()) {
				failMessage += ": " + e.getMessage();
			}
			
			fail(failMessage);
		}

	}
	
	Thread clientThread = new Thread() {
		
		@Override
		public void run() {
			
			try {
			
				//Socket client = new Socket(InetAddress.getLocalHost(), serverPort);
				Socket client = new SecureSocket(InetAddress.getLocalHost(), serverPort, "client".getBytes("UTF-16"), clientPair.getPrivateKey(), callbacks);
				
				data = generateTestData();
				InputStream inStream = client.getInputStream();
				OutputStream outStream = client.getOutputStream();
				
				outStream.write(data);
				
				for (int i = 0; i < 10; i++) {
					response = new byte[MAX_TEST_DATA];
					
					int bytesReceived = inStream.read(response);					
					outStream.write(response, 0, bytesReceived);
				}
				
				response = new byte[MAX_TEST_DATA];
				int bytesReceived = inStream.read(response);
				
				response = Arrays.copyOf(response, bytesReceived);
				
				byte[] finishMessage = {(byte) 0xDE, (byte) 0xAD, (byte) 0xC0, (byte) 0xDE};
				
				outStream.write(finishMessage);
				client.close();
			} catch (Exception e) {
				String failMessage = e.getClass().getCanonicalName();
				
				if (null != e.getMessage()) {
					failMessage += ": " + e.getMessage();
				}

				fail(failMessage);
			}
		}
		
	};
	
	Thread serverThread = new Thread() {
		
		@Override
		public void run() {
			
			try {
				//ServerSocket server = new ServerSocket(serverPort);
				ServerSocket server = new SecureServerSocket(serverPort, "server".getBytes("UTF-16"), serverPair.getPrivateKey(), callbacks);
				
				Socket s = server.accept();
				InputStream inStream = s.getInputStream();
				OutputStream outStream = s.getOutputStream();
				
				byte[] buffer = new byte[MAX_TEST_DATA];
				int bytesRead = inStream.read(buffer);
				
				while (!isFinished(buffer)) {
					outStream.write(buffer, 0, bytesRead);
					
					bytesRead = inStream.read(buffer);
				}
				
				s.close();
				server.close();
			} catch (InterruptedIOException e) {
				
			} catch (Exception e) {
				String failMessage = e.getClass().getCanonicalName();
				
				if (null != e.getMessage()) {
					failMessage += ": " + e.getMessage();
				}

				fail(failMessage);
			}
		}

	};
}
