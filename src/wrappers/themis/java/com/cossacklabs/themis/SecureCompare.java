package com.cossacklabs.themis;

import java.io.UnsupportedEncodingException;

public class SecureCompare {
	
	static {
		System.loadLibrary("themis_jni");
	}
	
	public enum CompareResult {
		NOT_READY,
		NO_MATCH,
		MATCH
	}
	
	public enum ProtocolResult {
		NO_OUTPUT,
		SEND_TO_PEER
	}
	
	static final String CHARSET = "UTF-16";
	
	private long nativeCtx = 0;
	
	native long create();
	native void destroy();
	
	public SecureCompare() throws SecureCompareException {
		
		nativeCtx = create();
		
		if (0 == nativeCtx) {
			throw new SecureCompareException();
		}
	}
	
	public SecureCompare(byte[] secret) throws SecureCompareException {
		
		this();
		appendSecret(secret);
		
	}
	
	public SecureCompare(String password) throws UnsupportedEncodingException, SecureCompareException {
		this(password.getBytes(CHARSET));
	}
	
	public void close() {
		if (0 != nativeCtx) {
			destroy();
		}
	}
	
	@Override
	protected void finalize() {
		close();
	}
	
	native int jniAppend(byte[] secret);
	
	public void appendSecret(byte[] secretData) throws SecureCompareException {
		if (0 != jniAppend(secretData)) {
			throw new SecureCompareException();
		}
	}
	
	native byte[] jniBegin();
	
	CompareResult parseResult(int result) throws SecureCompareException {
		if (result == scompareNotReady()) {
		    return CompareResult.NOT_READY;
		}else if (result == scompareNoMatch()){
		    return CompareResult.NO_MATCH;
		}else if (result == scompareMatch()){
		    return CompareResult.MATCH;
		}
		throw new SecureCompareException();
	}
	
	public byte[] begin() throws SecureCompareException {
		byte[] compareData = jniBegin();
		
		if (null == compareData) {
			throw new SecureCompareException();
		}
		
		return compareData;
	}
	
	native byte[] jniProceed(byte[] compareData);
	
	public byte[] proceed(byte[] compareData) throws SecureCompareException {
		return jniProceed(compareData);
	}
	
	native int jniGetResult();
	
	public CompareResult getResult() throws SecureCompareException {
		return parseResult(jniGetResult());
	}

	native int scompareMatch();
	native int scompareNoMatch();
	native int scompareNotReady();
}