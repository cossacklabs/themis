package com.cossacklabs.themis;

import java.io.UnsupportedEncodingException;

public class SecureCell {
	
	static {
		System.loadLibrary("themis_jni");
	}
	
	public SecureCell(int mode) throws InvalidArgumentException {
		
		if (mode < MODE_SEAL || mode > MODE_CONTEXT_IMPRINT) {
			throw new InvalidArgumentException("invalid mode");
		}
		
		this.mode = mode;
	}
	
	public SecureCell(byte[] key) {
		this.key = key;
	}
	
	public SecureCell(byte[] key, int mode) throws InvalidArgumentException {
		this(mode);
		this.key = key;
	}
	
	public SecureCell(String password) throws UnsupportedEncodingException {
		this(password.getBytes(CHARSET));
	}
	
	public SecureCell(String password, int mode) throws UnsupportedEncodingException, InvalidArgumentException {
		this(mode);
		this.key = password.getBytes(CHARSET);
	}
	
	int mode = MODE_SEAL;
	byte[] key;
	
	static final String CHARSET = "UTF-16";
	
	public static final int MODE_SEAL = 0;
	public static final int MODE_TOKEN_PROTECT = 1;
	public static final int MODE_CONTEXT_IMPRINT = 2;
	
	static native byte[][] encrypt(byte[] key, byte[] context, byte[] data, int mode);
	static native byte[] decrypt(byte[] key, byte[] context, byte[][] protectedData, int mode);
	
	static SecureCellData protect(byte[] key, byte[] context, byte[] data, int mode) throws NullArgumentException, SecureCellException {
		
		if (null == key) {
			throw new NullArgumentException("Master key was not provided");
		}
		
		if (null == data) {
			throw new NullArgumentException("Data was not provided");
		}
		
		if (MODE_CONTEXT_IMPRINT == mode) {
			// Context is mandatory for this mode
			if (null == context) {
				throw new NullArgumentException("Context is mandatory for context imprint mode");
			}
		}
		
		byte[][] protectedData = encrypt(key, context, data, mode);
		
		if (null == protectedData) {
			throw new SecureCellException();
		}
		
		return new SecureCellData(protectedData[0], protectedData[1]);
	}
	
	static byte[] unprotect(byte[] key, byte[] context, SecureCellData protectedData, int mode) throws NullArgumentException, SecureCellException, InvalidArgumentException {
		
		if (null == key) {
			throw new NullArgumentException("Master key was not provided");
		}
		
		if (null == protectedData) {
			throw new NullArgumentException("Protected data was not provided");
		}
		
		if (MODE_CONTEXT_IMPRINT == mode) {
			// Context is mandatory for this mode
			if (null == context) {
				throw new NullArgumentException("Context is mandatory for context imprint mode");
			}
		}
		
		if (null == protectedData.getProtectedData()) {
			throw new InvalidArgumentException("protectedData");
		}
		
		if (MODE_TOKEN_PROTECT == mode) {
			if (null == protectedData.getAdditionalData()) {
				throw new InvalidArgumentException("protectedData");
			}
		}
		
		byte[] data = decrypt(key, context, new byte[][]{protectedData.getProtectedData(), protectedData.getAdditionalData()}, mode);
		if (null == data) {
			throw new SecureCellException();
		}
		
		return data;
	}
	
	public SecureCellData protect(byte[] key, byte[] context, byte[] data) throws NullArgumentException, SecureCellException {
		return protect(key, context, data, this.mode);
	}
	
	public SecureCellData protect(byte[] context, byte[] data) throws NullArgumentException, SecureCellException {
		return this.protect(this.key, context, data);
	}
	
	public SecureCellData protect(String password, String context, byte[] data) throws UnsupportedEncodingException, NullArgumentException, SecureCellException {
		return this.protect(password.getBytes(CHARSET), context.getBytes(CHARSET), data);
	}
	
	public SecureCellData protect(String context, byte[] data) throws UnsupportedEncodingException, NullArgumentException, SecureCellException {
		return this.protect(this.key, context.getBytes(CHARSET), data);
	}
	
	public byte[] unprotect(byte[] key, byte[] context, SecureCellData protectedData) throws NullArgumentException, SecureCellException, InvalidArgumentException {
		return unprotect(key, context, protectedData, this.mode);
	}
	
	public byte[] unprotect(byte[] context, SecureCellData protectedData) throws NullArgumentException, SecureCellException, InvalidArgumentException {
		return this.unprotect(this.key, context, protectedData);
	}
	
	public byte[] unprotect(String password, String context, SecureCellData protectedData) throws UnsupportedEncodingException, NullArgumentException, SecureCellException, InvalidArgumentException {
		return this.unprotect(password.getBytes(CHARSET), context.getBytes(CHARSET), protectedData);
	}
	
	public byte[] unprotect(String context, SecureCellData protectedData) throws UnsupportedEncodingException, NullArgumentException, SecureCellException, InvalidArgumentException {
		return this.unprotect(this.key, context.getBytes(CHARSET), protectedData);
	}
}