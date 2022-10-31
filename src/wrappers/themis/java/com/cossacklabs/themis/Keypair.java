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
 * Represents Themis asymmetric key pair
 */
public class Keypair {
	
	PrivateKey privateKey;
	PublicKey publicKey;
	
	/**
	 * Creates a new key pair
	 * @param privateKey of the key pair
	 * @param publicKey of the key pair
	 */
	public Keypair(PrivateKey privateKey, PublicKey publicKey) {
		this.privateKey = privateKey;
		this.publicKey = publicKey;
	}
	
	/**
	 * Returns private key of this key pair
	 * @return PrivateKey of the key pair
	 */
	public PrivateKey getPrivateKey() {
		return this.privateKey;
	}
	
	/**
	 * Returns public key of this key pair
	 * @return PublicKey of the key pair
	 */
	public PublicKey getPublicKey() {
		return this.publicKey;
	}

}
