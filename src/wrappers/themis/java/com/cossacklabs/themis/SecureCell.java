/*
* Copyright (c) 2015 Cossack Labs Limited
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

package com.cossacklabs.themis;

import java.io.UnsupportedEncodingException;

/**
 * Themis secure cell
 */
public class SecureCell {
	
	static {
		System.loadLibrary("themis_jni");
	}
	
	/**
	 * Creates new SecureCell in specified mode
	 * @param SecureCell mode
	 * @throws InvalidArgumentException when unsupported mode is specified
	 */
	public SecureCell(int mode) throws InvalidArgumentException {
		
		if (mode < MODE_SEAL || mode > MODE_CONTEXT_IMPRINT) {
			throw new InvalidArgumentException("invalid mode");
		}
		
		this.mode = mode;
	}
	
	/**
	 * Creates new SecureCell with default master key in SEAL mode
	 * @param default master key
	 */
	public SecureCell(byte[] key) {
		this.key = key;
	}
	
	/**
	 * Creates new SecureCell with default master key in specified mode
	 * @param default master key
	 * @param SecureCell mode
	 * @throws InvalidArgumentException when unsupported mode is specified
	 */
	public SecureCell(byte[] key, int mode) throws InvalidArgumentException {
		this(mode);
		this.key = key;
	}
	
	/**
	 * Creates new SecureCell with default master password in SEAL mode
	 * @param default master password
	 * @throws UnsupportedEncodingException when UTF-16 decoding is not supported
	 */
	public SecureCell(String password) throws UnsupportedEncodingException {
		this(password.getBytes(CHARSET));
	}
	
	/**
	 * Creates new SecureCell with default master password in specified mode
	 * @param default master password
	 * @param SecureCell mode
	 * @throws UnsupportedEncodingException when UTF-16 decoding is not supported
	 * @throws InvalidArgumentException when unsupported mode is specified
	 */
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
				throw new InvalidArgumentException("additionalData");
			}
		}
		
		byte[] data = decrypt(key, context, new byte[][]{protectedData.getProtectedData(), protectedData.getAdditionalData()}, mode);
		if (null == data) {
			throw new SecureCellException();
		}
		
		return data;
	}
	
	/**
	 * Protects data with specified master key
	 * @param master key to use for protecting data
	 * @param context to which protected data will be bound (may be null)
	 * @param data to protect
	 * @return SecureCellData with protected data
	 * @throws NullArgumentException when key or data is null
	 * @throws SecureCellException when cannot protect the data
	 */
	public SecureCellData protect(byte[] key, byte[] context, byte[] data) throws NullArgumentException, SecureCellException {
		return protect(key, context, data, this.mode);
	}
	
	/**
	 * Protects data with default master key
	 * @param context to which protected data will be bound (may be null)
	 * @param data to protect
	 * @return SecureCellData with protected data
	 * @throws NullArgumentException when default master key or data is null
	 * @throws SecureCellException when cannot protect the data
	 */
	public SecureCellData protect(byte[] context, byte[] data) throws NullArgumentException, SecureCellException {
		return this.protect(this.key, context, data);
	}
	
	/**
	 * Protects data with specified master password
	 * @param master password to use for protecting data
	 * @param context to which protected data will be bound (may be null)
	 * @param data to protect
	 * @return SecureCellData with protected data
	 * @throws NullArgumentException when key or data is null
	 * @throws SecureCellException when cannot protect the data
	 * @throws UnsupportedEncodingException when UTF-16 decoding is not supported
	 */
	public SecureCellData protect(String password, String context, byte[] data) throws UnsupportedEncodingException, NullArgumentException, SecureCellException {
		return this.protect(password.getBytes(CHARSET), context.getBytes(CHARSET), data);
	}
	
	/**
	 * Protects data with default master password
	 * @param context to which protected data will be bound (may be null)
	 * @param data to protect
	 * @return SecureCellData with protected data
	 * @throws NullArgumentException when key or data is null
	 * @throws SecureCellException when cannot protect the data
	 * @throws UnsupportedEncodingException when UTF-16 decoding is not supported
	 */
	public SecureCellData protect(String context, byte[] data) throws UnsupportedEncodingException, NullArgumentException, SecureCellException {
		return this.protect(this.key, context.getBytes(CHARSET), data);
	}
	
	/**
	 * Decrypts and verifies protected data
	 * @param master key
	 * @param context to which protected data will is bound (may be null, must be same as provided in protect call)
	 * @param protectedData to verify
	 * @return original data
	 * @throws NullArgumentException when key or protectedData is null
	 * @throws SecureCellException when cannot decrypt protectedData
	 * @throws InvalidArgumentException when protectedData is incorrect
	 */
	public byte[] unprotect(byte[] key, byte[] context, SecureCellData protectedData) throws NullArgumentException, SecureCellException, InvalidArgumentException {
		return unprotect(key, context, protectedData, this.mode);
	}
	
	/**
	 * Decrypts and verifies protected data with default master key
	 * @param context to which protected data will is bound (may be null, must be same as provided in protect call)
	 * @param protectedData to verify
	 * @return original data
	 * @throws NullArgumentException when key or protectedData is null
	 * @throws SecureCellException when cannot decrypt protectedData
	 * @throws InvalidArgumentException when protectedData is incorrect
	 */
	public byte[] unprotect(byte[] context, SecureCellData protectedData) throws NullArgumentException, SecureCellException, InvalidArgumentException {
		return this.unprotect(this.key, context, protectedData);
	}
	
	/**
	 * Decrypts and verifies protected data
	 * @param master password
	 * @param context to which protected data will is bound (may be null, must be same as provided in protect call)
	 * @param protectedData to verify
	 * @return original data
	 * @throws NullArgumentException when key or protectedData is null
	 * @throws SecureCellException when cannot decrypt protectedData
	 * @throws InvalidArgumentException when protectedData is incorrect
	 * @throws UnsupportedEncodingException when UTF-16 decoding is not supported
	 */
	public byte[] unprotect(String password, String context, SecureCellData protectedData) throws UnsupportedEncodingException, NullArgumentException, SecureCellException, InvalidArgumentException {
		return this.unprotect(password.getBytes(CHARSET), context.getBytes(CHARSET), protectedData);
	}
	
	/**
	 * Decrypts and verifies protected data with default master password
	 * @param context to which protected data will is bound (may be null, must be same as provided in protect call)
	 * @param protectedData to verify
	 * @return original data
	 * @throws NullArgumentException when key or protectedData is null
	 * @throws SecureCellException when cannot decrypt protectedData
	 * @throws InvalidArgumentException when protectedData is incorrect
	 * @throws UnsupportedEncodingException when UTF-16 decoding is not supported
	 */
	public byte[] unprotect(String context, SecureCellData protectedData) throws UnsupportedEncodingException, NullArgumentException, SecureCellException, InvalidArgumentException {
		return this.unprotect(this.key, context.getBytes(CHARSET), protectedData);
	}
}
