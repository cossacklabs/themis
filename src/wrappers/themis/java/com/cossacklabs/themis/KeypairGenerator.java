package com.cossacklabs.themis;

/**
 * Generates keypairs to be used in Themis
 */
public abstract class KeypairGenerator {
	
	static {
		System.loadLibrary("themis_jni");
	}
	
	private KeypairGenerator() {
		
	}
	
	static native byte[][] generateKeys();
	
	/**
	 * Generates new keypair
	 * @return new Keypair
	 * @throws KeyGenerationException when cannot generate a keypair
	 */
	public static Keypair generateKeypair() throws KeyGenerationException {
		
		byte[][] keys = generateKeys();
		
		if (null == keys) {
			throw new KeyGenerationException();
		}
		
		return new Keypair(new PrivateKey(keys[0]), new PublicKey(keys[1]));
	}
}