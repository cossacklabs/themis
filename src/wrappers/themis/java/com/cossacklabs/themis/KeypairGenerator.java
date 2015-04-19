package com.cossacklabs.themis;

public abstract class KeypairGenerator {
	
	static {
		System.loadLibrary("themis_jni");
	}
	
	private KeypairGenerator() {
		
	}
	
	static native byte[][] generateKeys();
	
	public static Keypair generateKeypair() throws KeyGenerationException {
		
		byte[][] keys = generateKeys();
		
		if (null == keys) {
			throw new KeyGenerationException();
		}
		
		return new Keypair(new PrivateKey(keys[0]), new PublicKey(keys[1]));
	}
}