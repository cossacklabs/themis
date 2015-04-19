package com.cossacklabs.themis;

public abstract class AsymmetricKey {
	
	byte[] key;
	
	public AsymmetricKey(byte[] key) {
		this.key = key;
	}
	
	public byte[] toByteArray() {
		return key.clone();
	}
}
