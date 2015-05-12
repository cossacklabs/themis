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

package com.cossacklabs.themis;

public class SecureMessage {
	
	static {
		System.loadLibrary("themis_jni");
	}
	
	PrivateKey privateKey;
	PublicKey peerPublicKey;
	
	public SecureMessage(PrivateKey privateKey) throws NullArgumentException {
		
		if (null == privateKey) {
			throw new NullArgumentException("Private key was not provided");
		}
		
		this.privateKey = privateKey;
	}
	
	public SecureMessage(PrivateKey privateKey, PublicKey peerPublicKey) throws NullArgumentException {
		
		if (null == privateKey) {
			throw new NullArgumentException("Private key was not provided");
		}
		
		if (null == peerPublicKey) {
			throw new NullArgumentException("Peer public key was not provided");
		}
		
		this.privateKey = privateKey;
		this.peerPublicKey = peerPublicKey;
	}
	
	static native byte[] process(byte[] privateKey, byte[] publicKey, byte[] message, boolean isWrap);
	
	public byte[] wrap(byte[] message, PublicKey peerPublicKey) throws NullArgumentException, SecureMessageWrapException {
		
		if (null == peerPublicKey) {
			throw new NullArgumentException("Peer public key was not provided");
		}
		
		if (null == message) {
			throw new NullArgumentException("No message was provided");
		}
		
		byte[] wrappedMessage = process(this.privateKey.toByteArray(), peerPublicKey.toByteArray(), message, true);
		
		if (null == wrappedMessage) {
			throw new SecureMessageWrapException();
		}
		
		return wrappedMessage;
	}
	
	public byte[] wrap(byte[] message) throws NullArgumentException, SecureMessageWrapException {
		return wrap(message, this.peerPublicKey);
	}
	
	public byte[] unwrap(byte[] message, PublicKey peerPublicKey) throws NullArgumentException, SecureMessageWrapException {
		
		if (null == peerPublicKey) {
			throw new NullArgumentException("Peer public key was not provided");
		}
		
		if (null == message) {
			throw new NullArgumentException("No message was provided");
		}
		
		byte[] unwrappedMessage = process(this.privateKey.toByteArray(), peerPublicKey.toByteArray(), message, false);
		
		if (null == unwrappedMessage) {
			throw new SecureMessageWrapException();
		}
		
		return unwrappedMessage;
	}
	
	public byte[] unwrap(byte[] message) throws NullArgumentException, SecureMessageWrapException {
		return unwrap(message, this.peerPublicKey);
	}
}
