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
import java.nio.ByteBuffer;

public class SecureSession {
	
	static {
		System.loadLibrary("themis_jni");
	}
	
	public enum State {
		IDLE,
		NEGOTIATING,
		ESTABLISHED
	}
	
	public enum SessionDataType {
		NO_DATA,
		PROTOCOL_DATA,
		USER_DATA;
		
		public static SessionDataType fromByte(byte src) throws SecureSessionException {
			switch (src) {
			case 0:
				return NO_DATA;
			case 1:
				return PROTOCOL_DATA;
			case 2:
				return USER_DATA;
			}
			
			throw new SecureSessionException();
		}
	}
	
	public class UnwrapResult {
		private SessionDataType dataType;
		private byte[] data;
		
		private UnwrapResult(SessionDataType dataType, byte[] data) {
			this.dataType = dataType;
			this.data = data;
		}
		
		public SessionDataType getDataType() {
			return dataType;
		}
		
		public byte[] getData() {
			return data;
		}
	}
	
	static final String CHARSET = "UTF-16";
	
	protected ISessionCallbacks callbacks;
	private long sessionPtr;
	private State sessionState = State.IDLE;
	
	native long create(byte[] id, byte[] signKey);
	native void destroy();
	
	native byte[] jniGenerateConntect();
	native byte[] jniWrap(byte[] data);
	native byte[][] jniUnwrap(byte[] wrappedData);
	native boolean jniIsEstablished();
	native byte[] jniSave();
	static native long jniLoad(byte[] state);
	
	public SecureSession(byte[] id, PrivateKey signPrivateKey, ISessionCallbacks callbacks) throws SecureSessionException {
		
		sessionPtr = create(id, signPrivateKey.toByteArray());
		
		if (0 == sessionPtr) {
			throw new SecureSessionException();
		}
		
		this.callbacks = callbacks;
	}
	
	public SecureSession(String id, PrivateKey signPrivateKey, ISessionCallbacks callbacks) throws UnsupportedEncodingException, SecureSessionException {
		this(id.getBytes(CHARSET), signPrivateKey, callbacks);
	}
	
	private SecureSession() {
		
	};
	
	public synchronized byte[] generateConnectRequest() throws SecureSessionException {
		
		if (0 == sessionPtr) {
			throw new SecureSessionException("session is closed");
		}
		
		byte[] request = jniGenerateConntect();
		
		if (null == request) {
			throw new SecureSessionException();
		}
		
		return request;
	}
	
	public synchronized byte[] wrap(byte[] data) throws SecureSessionException, NullArgumentException {
		
		if (0 == sessionPtr) {
			throw new SecureSessionException("session is closed");
		}
		
		if (null == data) {
			throw new NullArgumentException();
		}
		
		byte[] wrappedData = jniWrap(data);
		if (null == wrappedData) {
			throw new SecureSessionException();
		}
		
		return wrappedData;
	}
	
	public synchronized UnwrapResult unwrap(byte[] wrappedData) throws SecureSessionException, NullArgumentException {
		
		if (0 == sessionPtr) {
			throw new SecureSessionException("session is closed");
		}
		
		if (null == wrappedData) {
			throw new NullArgumentException();
		}
		
		byte[][] result = jniUnwrap(wrappedData);
		if (null == result) {
			throw new SecureSessionException();
		}
		
		SessionDataType dataType = SessionDataType.fromByte(result[0][0]);
		if (SessionDataType.NO_DATA == dataType) {
			return new UnwrapResult(dataType, new byte[0]);
		} else {
			return new UnwrapResult(dataType, result[1]);
		}
	}
	
	public void close() {
		if (0 != sessionPtr) {
			destroy();
		}
	}
	
	@Override
	protected void finalize() {
		close();
	}
	
	private byte[] getPublicKeyForId(byte[] id) {
		PublicKey publicKey = callbacks.getPublicKeyForId(this, id);
		
		if (null == publicKey) {
			return null;
		} else {
			return publicKey.toByteArray();
		}
	}
	
	private void stateChanged(int state) {
		switch (state) {
		case 0:
			this.sessionState = State.IDLE;
			break;
		case 1:
			this.sessionState = State.NEGOTIATING;
			break;
		case 2:
			this.sessionState = State.ESTABLISHED;
		}
		
		this.callbacks.stateChanged(this);
	}
	
	public synchronized boolean isEstablished() throws SecureSessionException {
		if (0 == sessionPtr) {
			throw new SecureSessionException("session is closed");
		}
		
		return jniIsEstablished();
	}
	
	public synchronized byte[] save() throws SecureSessionException {
		if (0 == sessionPtr) {
			throw new SecureSessionException("session is closed");
		}
		
		byte[] state = jniSave();
		if (null == state) {
			throw new SecureSessionException();
		}
		
		return state;
	}
	
	public static SecureSession restore(byte[] state, ISessionCallbacks callbacks) throws SecureSessionException {
		SecureSession session = new SecureSession();
		
		session.sessionPtr = jniLoad(state);
		if (0 == session.sessionPtr) {
			throw new SecureSessionException();
		}
		
		session.callbacks = callbacks;
		return session;
	}
	
	public State getState() {
		return sessionState;
	}
}