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
import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;

/**
 * Themis secure session
 */
public class SecureSession {
	
	static {
		System.loadLibrary("themis_jni");
	}
	
	/**
	 * SecureSession state
	 */
	public enum State {
		/**
		 * Initial state for newly created Secure Session.
		 */
		IDLE,
		/**
		 * Key agreement is in progress. Data exchange is not possible yet.
		 */
		NEGOTIATING,
		/**
		 * Secure Session is established. You may exchange data securely now.
		 */
		ESTABLISHED
	}
	
	public enum SessionDataType {
		/**
		 * No output data.
		 */
		NO_DATA,
		/**
		 * Output is a Secure Session protocol message.
		 * Send this data to your peer as is.
		 */
		PROTOCOL_DATA,
		/**
		 * Output is decrypted user data.
		 * Pass it to the application for processing.
		 */
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
	
	/**
	 * Represents result of unwrap operation
	 */
	public class UnwrapResult {
		private SessionDataType dataType;
		private byte[] data;
		
		private UnwrapResult(SessionDataType dataType, byte[] data) {
			this.dataType = dataType;
			this.data = data;
		}
		
		/**
		 * Returns output data type in this result
		 * @return output data type
		 */
		public SessionDataType getDataType() {
			return dataType;
		}
		
		/**
		 * Returns actual output data if any
		 * @return output data
		 */
		public byte[] getData() {
			return data;
		}
	}
	
	static final Charset CHARSET = StandardCharsets.UTF_16;
	
	protected ISessionCallbacks callbacks;
	private long sessionPtr;
	private State sessionState = State.IDLE;
	
	native long create(byte[] id, byte[] signKey);
	native void destroy();
	
	native byte[] jniGenerateConnect();
	native byte[] jniWrap(byte[] data);
	native byte[][] jniUnwrap(byte[] wrappedData);
	native boolean jniIsEstablished();
	native byte[] jniSave();
	static native long jniLoad(byte[] state);
	
	/**
	 * Creates new SecureSession
	 * @param id your id
	 * @param signPrivateKey your sign PrivateKey
	 * @param callbacks callbacks implementation
	 * @throws NullArgumentException if `id` is null
	 * @throws InvalidArgumentException if `id` is empty
	 * @throws RuntimeException when cannot create session
	 */
	public SecureSession(byte[] id, PrivateKey signPrivateKey, ISessionCallbacks callbacks) {
		if (id == null) {
			throw new NullArgumentException("peer ID cannot be null");
		}
		if (id.length == 0) {
			throw new InvalidArgumentException("peer ID cannot be empty");
		}

		sessionPtr = create(id, signPrivateKey.toByteArray());
		
		if (0 == sessionPtr) {
			throw new RuntimeException("failed to create Secure Session", new SecureSessionException());
		}
		
		this.callbacks = callbacks;
	}
	
	/**
	 * Creates new SecureSession
	 * @param id your id
	 * @param signPrivateKey your sign PrivateKey
	 * @param callbacks callbacks implementation
	 * @throws NullArgumentException if `id` is null
	 * @throws InvalidArgumentException if `id` is empty
	 * @throws RuntimeException when cannot create session
	 */
	public SecureSession(String id, PrivateKey signPrivateKey, ISessionCallbacks callbacks) {
		this(id.getBytes(CHARSET), signPrivateKey, callbacks);
	}
	
	private SecureSession() {
		
	};
	
	/**
	 * Starts secure session negotiation in client mode
	 * @return opaque connect request. Should be sent to your peer
	 * @throws IllegalStateException is the session is already closed
	 * @throws RuntimeException when cannot generate request
	 */
	public synchronized byte[] generateConnectRequest() {

		if (0 == sessionPtr) {
			throw new IllegalStateException("Secure Session is closed");
		}
		
		byte[] request = jniGenerateConnect();
		
		if (null == request) {
			throw new RuntimeException("Secure Session cannot generate connection request", new SecureSessionException());
		}
		
		return request;
	}
	
	/**
	 * Wraps outgoing data
	 * @param data data to wrap
	 * @return wrapped data
	 * @throws IllegalStateException is the session is already closed
	 * @throws SecureSessionException when cannot wrap data
	 * @throws NullArgumentException when data is null
	 */
	public synchronized byte[] wrap(byte[] data) throws SecureSessionException {

		if (0 == sessionPtr) {
			throw new IllegalStateException("Secure Session is closed");
		}
		
		if (null == data) {
			throw new NullArgumentException();
		}
		
		byte[] wrappedData = jniWrap(data);
		if (null == wrappedData) {
			throw new SecureSessionException("Secure Session failed to wrap data");
		}
		
		return wrappedData;
	}
	
	/**
	 * Unwraps incoming data
	 * @param wrappedData wrapped data
	 * @return unwrapped data
	 * @throws IllegalStateException is the session is already closed
	 * @throws SecureSessionException when cannot unwrap data
	 * @throws NullArgumentException when wrappedData is null
	 */
	public synchronized UnwrapResult unwrap(byte[] wrappedData) throws SecureSessionException {

		if (0 == sessionPtr) {
			throw new IllegalStateException("Secure Session is closed");
		}
		
		if (null == wrappedData) {
			throw new NullArgumentException();
		}
		
		byte[][] result = jniUnwrap(wrappedData);
		if (null == result) {
			throw new SecureSessionException("Secure Session failed to unwrap data");
		}
		
		SessionDataType dataType = SessionDataType.fromByte(result[0][0]);
		if (SessionDataType.NO_DATA == dataType) {
			return new UnwrapResult(dataType, new byte[0]);
		} else {
			return new UnwrapResult(dataType, result[1]);
		}
	}
	
	/**
	 * Closes the session and releases all native resources
	 */
	public void close() {
		if (0 != sessionPtr) {
			destroy();
		}
		sessionPtr = 0;
	}
	
	@Override
	@SuppressWarnings("deprecation")
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
	
	/**
	 * Checks whether session is already established (can wrap/unwrap data)
	 * @return true if session is already established
	 * @throws IllegalStateException is the session is already closed
	 */
	public synchronized boolean isEstablished() {
		if (0 == sessionPtr) {
			throw new IllegalStateException("Secure Session is closed");
		}
		
		return jniIsEstablished();
	}
	
	/**
	 * Saves (serializes) this session state (session must be established)
	 * @return session state
	 * @throws IllegalStateException is the session is already closed
	 * @throws RuntimeException when cannot serialize session, e.g. if session is not established
	 * @deprecated since JavaThemis 0.13
	 * <p>
	 * This method might be replaced in a next release, please do not use.
	 */
	@Deprecated
	public synchronized byte[] save() {
		if (0 == sessionPtr) {
			throw new IllegalStateException("Secure Session is closed");
		}
		
		byte[] state = jniSave();
		if (null == state) {
			throw new RuntimeException(new SecureSessionException("failed to serialize Secure Session"));
		}
		
		return state;
	}
	
	/**
	 * Restores previously saved session
	 * @param state saved session state
	 * @param callbacks callbacks implementation
	 * @return restored SecureSession
	 * @throws SecureSessionException when cannot restore session
	 * @deprecated since JavaThemis 0.13
	 * <p>
	 * This method might be replaced in a next release, please do not use.
	 */
	@Deprecated
	public static SecureSession restore(byte[] state, ISessionCallbacks callbacks) throws SecureSessionException {
		SecureSession session = new SecureSession();
		
		session.sessionPtr = jniLoad(state);
		if (0 == session.sessionPtr) {
			throw new SecureSessionException("failed to restore Secure Session");
		}
		
		session.callbacks = callbacks;
		return session;
	}
	
	/**
	 * Gets session current state
	 * @return session state
	 */
	public State getState() {
		return sessionState;
	}
}
