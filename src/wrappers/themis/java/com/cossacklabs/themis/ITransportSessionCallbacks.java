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
 * Callback interface for Themis secure session with decoupled transport operations
 */
public interface ITransportSessionCallbacks extends ISessionCallbacks {
	
	/**
	 * Called, when session needs to send some data to its peer
	 * @param buffer with data to send
	 */
	void write(byte[] buffer);
	
	/**
	 * Called, when user issued a read request on the session. Implementers should pass this request to underlying transport
	 * @return data which was read from underlying transport
	 */
	byte[] read();
}
