package com.cossacklabs.themis;

public class SecureCellData {
	
	byte[] protectedData;
	byte[] additionalData;
	
	public SecureCellData(byte[] protectedData, byte[] additionalData) {
		this.protectedData = protectedData;
		this.additionalData = additionalData;
	}
	
	public byte[] getProtectedData() {
		return this.protectedData;
	}
	
	public byte[] getAdditionalData() {
		return this.additionalData;
	}
	
	public boolean hasAdditionalData() {
		return null != this.additionalData;
	}

}
