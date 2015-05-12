package com.cossacklabs.themis;

/**
 * Represents data protected by SecureCell
 */
public class SecureCellData {
	
	byte[] protectedData;
	byte[] additionalData;
	
	/**
	 * Creates new SecureCellData
	 * @param actual protected data
	 * @param auxiliary data
	 */
	public SecureCellData(byte[] protectedData, byte[] additionalData) {
		this.protectedData = protectedData;
		this.additionalData = additionalData;
	}
	
	/**
	 * Gets actual protected data
	 * @return protected data
	 */
	public byte[] getProtectedData() {
		return this.protectedData;
	}
	
	/**
	 * Gets auxiliary data (if present, must be provided for successful decryption)
	 * @return auxiliary data (may be null)
	 */
	public byte[] getAdditionalData() {
		return this.additionalData;
	}
	
	/**
	 * Checks whether this SecureCellData has auxiliary data
	 * @return true if auxiliary data is present
	 */
	public boolean hasAdditionalData() {
		return null != this.additionalData;
	}

}
