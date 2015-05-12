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
