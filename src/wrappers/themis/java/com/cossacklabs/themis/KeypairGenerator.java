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

/**
 * Generates keypairs to be used in Themis
 */
public abstract class KeypairGenerator {
	
	static {
		System.loadLibrary("themis_jni");
	}
	
	private KeypairGenerator() {
		
	}
	
	static native byte[][] generateKeys(int keyType);
	
	/**
	 * Generates new EC keypair
	 * @return new EC Keypair
	 * @throws KeyGenerationException when cannot generate a keypair
	 */
	public static Keypair generateKeypair() throws KeyGenerationException {
		try {
			return generateKeypair(AsymmetricKey.KEYTYPE_EC);
		} catch (InvalidArgumentException e) {
			throw new KeyGenerationException();
		}
	}
	
	/**
	 * Generates new keypair
	 * @param key type (EC or RSA)
	 * @return new Keypair
	 * @throws KeyGenerationException when cannot generate a keypair
	 * @throws InvalidArgumentException when keyType is invalid
	 */
	public static Keypair generateKeypair(int keyType) throws KeyGenerationException, InvalidArgumentException {
		
		if ((keyType != AsymmetricKey.KEYTYPE_EC) && (keyType != AsymmetricKey.KEYTYPE_RSA)) {
			throw new InvalidArgumentException("keyType is invalid");
		}
		
		byte[][] keys = generateKeys(keyType);
		
		if (null == keys) {
			throw new KeyGenerationException();
		}
		
		return new Keypair(new PrivateKey(keys[0]), new PublicKey(keys[1]));
	}
}