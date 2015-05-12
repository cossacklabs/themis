package com.cossacklabs.themis;

/**
 * Base class for Themis asymmetric keys
 */
public abstract class AsymmetricKey {
	
	byte[] key;

	/**
	 * Creates asymmetric key from byte array
	 * @param [in] key
	 */
	public AsymmetricKey(byte[] key) {
		this.key = key;
	}
	
	/**
	 * Serializes this key to a byte array
	 * @return key as byte array
	 */
	public byte[] toByteArray() {
		return key.clone();
	}
}
