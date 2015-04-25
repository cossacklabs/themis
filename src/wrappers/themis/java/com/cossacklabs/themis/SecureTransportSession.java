package com.cossacklabs.themis;

import java.io.UnsupportedEncodingException;

public class SecureTransportSession extends SecureSession {

	public SecureTransportSession(byte[] id, PrivateKey signPrivateKey, ITransportSessionCallbacks callbacks) throws SecureSessionException {
		super(id, signPrivateKey, callbacks);
	}

	public SecureTransportSession(String id, PrivateKey signPrivateKey, ITransportSessionCallbacks callbacks) throws SecureSessionException, UnsupportedEncodingException {
		super(id, signPrivateKey, callbacks);
	}
	
	public void connect() throws SecureSessionException {
		((ITransportSessionCallbacks)this.callbacks).write(this.generateConnectRequest());
	}
	
	public void write(byte[] data) throws SecureSessionException, NullArgumentException {
		((ITransportSessionCallbacks)this.callbacks).write(this.wrap(data));
	}
	
	public byte[] read() throws SecureSessionException, NullArgumentException {
		byte[] wrappedData = ((ITransportSessionCallbacks)this.callbacks).read();
		
		UnwrapResult result = this.unwrap(wrappedData);
		if (SecureSession.SessionDataType.USER_DATA == result.getDataType()) {
			return result.getData();
		} else {
			return new byte[0];
		}
	}
}
