package com.cossacklabs.themis;

/**
 * Represents Themis private key
 */
public class PrivateKey extends AsymmetricKey {

	/**
	 * Creates new private key from byte array
	 * @param key byte array
	 */
	public PrivateKey(byte[] key) {
		
		super(key);
		
	}

}
