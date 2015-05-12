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
 * Callback interface for Themis secure session
 */
public interface ISessionCallbacks {
	
	/**
	 * Called, when a secure session needs some peer public key
	 * @param session object which requests the key
	 * @param id of the requested peer
	 * @return PublicKey of the requested peer or null, if no key found
	 */
	PublicKey getPublicKeyForId(SecureSession session, byte[] id);
	
	/**
	 * Notifies when SecureSession state changes
	 * @param session object of changed state
	 */
	void stateChanged(SecureSession session);
}
