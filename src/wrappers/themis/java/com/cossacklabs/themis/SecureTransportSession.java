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

public class SecureTransportSession extends SecureSession {

	public SecureTransportSession(byte[] id, PrivateKey signPrivateKey, ITransportSessionCallbacks callbacks) {
		super(id, signPrivateKey, callbacks);
	}

	public SecureTransportSession(String id, PrivateKey signPrivateKey, ITransportSessionCallbacks callbacks) {
		super(id, signPrivateKey, callbacks);
	}

	public void connect() {
		((ITransportSessionCallbacks)this.callbacks).write(this.generateConnectRequest());
	}

	public void write(byte[] data) throws SecureSessionException {
		((ITransportSessionCallbacks)this.callbacks).write(this.wrap(data));
	}

	public byte[] read() throws SecureSessionException {
		byte[] wrappedData = ((ITransportSessionCallbacks)this.callbacks).read();
		
		UnwrapResult result = this.unwrap(wrappedData);
		if (SecureSession.SessionDataType.USER_DATA == result.getDataType()) {
			return result.getData();
		} else {
			return new byte[0];
		}
	}
}
