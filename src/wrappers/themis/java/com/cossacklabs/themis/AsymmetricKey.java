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
 * Base class for Themis asymmetric keys
 */
public abstract class AsymmetricKey extends KeyBytes {

	public static final int KEYTYPE_EC = 0;
	public static final int KEYTYPE_RSA = 1;

	/**
	 * Creates asymmetric key from byte array.
	 *
	 * @param key byte array
	 *
	 * @throws NullArgumentException if `key` is null.
	 * @throws InvalidArgumentException if `key` is empty.
	 */
	public AsymmetricKey(byte[] key) {
		super(key);
	}
}
