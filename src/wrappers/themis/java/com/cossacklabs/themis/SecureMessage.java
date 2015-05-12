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
}
