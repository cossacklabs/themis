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

import org.jetbrains.annotations.NotNull;

/**
 * Represents data protected by SecureCell
 */
public class SecureCellData {
	
	byte[] protectedData;
	byte[] additionalData;
	
	/**
	 * Creates new SecureCellData
	 * @param protectedData actual protected data
	 * @param additionalData auxiliary data
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

	/**
	 * Returns encrypted data for Token Protect mode.
	 * <p>
	 * This method is equivalent to {@link #getProtectedData()}.
	 * You are not expected to use it directly, it exists for improved Kotlin API.
	 * @return protected data.
	 */
	public @NotNull byte[] component1() {
		return this.protectedData;
	}

	/**
	 * Returns authentication token for Token Protect mode.
	 * <p>
	 * This method is equivalent to {@link #getAdditionalData()}.
	 * You are not expected to use it directly, it exists for improved Kotlin API.
	 * @return auxiliary data (may be null).
	 */
	public @NotNull byte[] component2() {
		return this.additionalData;
	}
}
