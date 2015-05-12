package com.cossacklabs.themis;

/**
 * Represents Themis public key
 */
public class PublicKey extends AsymmetricKey {

	/**
	 * Creates new public key from byte array
	 * @param key byte array
	 */
	public PublicKey(byte[] key) {
		
		super(key);
		
	}

}
