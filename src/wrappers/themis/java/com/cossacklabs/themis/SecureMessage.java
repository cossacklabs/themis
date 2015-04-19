package com.cossacklabs.themis;

public class SecureMessage {
	
	byte[] privateKey;
	byte[] peerPublicKey;
	
	public SecureMessage(byte[] privateKey) {
		this.privateKey = privateKey;
	}
	
	public SecureMessage(byte[] privateKey, byte[] peerPublicKey) {
		this.privateKey = privateKey;
		this.peerPublicKey = peerPublicKey;
	}
	
	public byte[] wrap(byte[] message, byte[] peerPublicKey) {
		return null;
	}
	
	public byte[] wrap(byte[] message) {
		return null;
	}
	
	public byte[] unwrap(byte[] message, byte[] peerPublicKey) {
		return null;
	}
	
	public byte[] unwrap(byte[] message) {
		return null;
	}
}
