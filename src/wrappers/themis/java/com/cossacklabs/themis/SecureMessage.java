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

/**
 * Themis secure message
 */
public class SecureMessage {
	
	static {
		System.loadLibrary("themis_jni");
	}
	
	PrivateKey privateKey;
	PublicKey peerPublicKey;
	
	/**
	 * Creates new SecureMessage with specified PrivateKey
	 * @param your own PrivateKey
	 * @throws NullArgumentException when privateKey is null
	 */
	public SecureMessage(PrivateKey privateKey) throws NullArgumentException {
		
		if (null == privateKey) {
			throw new NullArgumentException("Private key was not provided");
		}
		
		this.privateKey = privateKey;
	}

	/**
	 * Creates new SecureMessage with default peer PublicKey (can be used only for signature verification)
	 * @param default peer PublicKey
	 * @throws NullArgumentException when peerPublicKey is null
	 */
	public SecureMessage(PublicKey peerPublicKey) throws NullArgumentException {
		
		if (null == peerPublicKey) {
			throw new NullArgumentException("Peer public key was not provided");
		}
		
		this.peerPublicKey = peerPublicKey;
	}
	
	/**
	 * Creates new SecureMessage with specified PrivateKey and default peer PublicKey
	 * @param your own PrivateKey
	 * @param default peer PublicKey
	 * @throws NullArgumentException when privateKey or peerPublicKey is null
	 */
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
	
	/**
	 * Wraps message for peer
	 * @param message to wrap
	 * @param receiver's PublicKey
	 * @return wrapped message
	 * @throws NullArgumentException when message or peerPublicKey is null
	 * @throws SecureMessageWrapException when cannot wrap message
	 */
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
	
	/**
	 * Wraps message for default peer
	 * @param message to wrap
	 * @return wrapped message
	 * @throws NullArgumentException when message or default peer PublicKey is null
	 * @throws SecureMessageWrapException when cannot wrap message
	 */
	public byte[] wrap(byte[] message) throws NullArgumentException, SecureMessageWrapException {
		return wrap(message, this.peerPublicKey);
	}
	
	/**
	 * Unwraps message from peer
	 * @param wrapped message
	 * @param sender's PublicKey
	 * @return unwrapped message
	 * @throws NullArgumentException when message or peerPublicKey is null
	 * @throws SecureMessageWrapException when cannot unwrap message
	 */
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
	
	/**
	 * Unwraps message from default peer
	 * @param wrapped message
	 * @return unwrapped message
	 * @throws NullArgumentException when message or default peer PublicKey is null
	 * @throws SecureMessageWrapException when cannot unwrap message
	 */
	public byte[] unwrap(byte[] message) throws NullArgumentException, SecureMessageWrapException {
		return unwrap(message, this.peerPublicKey);
	}

	/**
	 * Signs message
	 * @param message to sign
	 * @return signed message
	 * @throws NullArgumentException when message or default peer PublicKey is null
	 * @throws SecureMessageWrapException when cannot wrap message
	 */
	public byte[] sign(byte[] message) throws NullArgumentException, SecureMessageWrapException {

		if (null == privateKey) {
			throw new NullArgumentException("Private key was not provided");
		}

		if (null == message) {
			throw new NullArgumentException("No message was provided");
		}

		byte[] signedMessage = process(this.privateKey.toByteArray(), null, message, true);
		
		if (null == signedMessage) {
			throw new SecureMessageWrapException();
		}
		
		return signedMessage;
	}

	/**
	 * Verifies signed message from peer
	 * @param signed message
	 * @param sender's PublicKey
	 * @return verified message
	 * @throws NullArgumentException when message or peerPublicKey is null
	 * @throws SecureMessageWrapException when cannot verify message
	 */
	public byte[] verify(byte[] message, PublicKey peerPublicKey) throws NullArgumentException, SecureMessageWrapException {

		if (null == peerPublicKey) {
			throw new NullArgumentException("Peer public key was not provided");
		}

		if (null == message) {
			throw new NullArgumentException("No message was provided");
		}

		byte[] verifiedMessage = process(null, peerPublicKey.toByteArray(), message, false);

		if (null == verifiedMessage) {
			throw new SecureMessageWrapException();
		}

		return verifiedMessage;
	}

	/**
	 * Verifies message from default peer
	 * @param signed message
	 * @return verified message
	 * @throws NullArgumentException when message or default peer PublicKey is null
	 * @throws SecureMessageWrapException when cannot verify message
	 */
	public byte[] verify(byte[] message) throws NullArgumentException, SecureMessageWrapException {
		return verify(message, this.peerPublicKey);
	}
}
