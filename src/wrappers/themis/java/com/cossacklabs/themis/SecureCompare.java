package com.cossacklabs.themis;

import java.io.UnsupportedEncodingException;
import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;

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
	
	static final Charset CHARSET = StandardCharsets.UTF_16;
	
	private long nativeCtx = 0;
	
	native long create();
	native void destroy();

	public SecureCompare() {

		nativeCtx = create();
		
		if (0 == nativeCtx) {
			throw new RuntimeException("failed to create Secure Comparator", new SecureCompareException());
		}
	}

	public SecureCompare(byte[] secret) {

		this();
		appendSecret(secret);
		
	}

	public SecureCompare(String password) {
		this(password.getBytes(CHARSET));
	}
	
	public void close() {
		if (0 != nativeCtx) {
			destroy();
		}
	}
	
	@Override
	@SuppressWarnings("deprecation")
	protected void finalize() {
		close();
	}
	
	native int jniAppend(byte[] secret);

	public void appendSecret(byte[] secretData) {
		if (secretData == null) {
			throw new NullArgumentException("secret cannot be null");
		}
		if (secretData.length == 0) {
			throw new InvalidArgumentException("secret cannot be empty");
		}
		if (0 != jniAppend(secretData)) {
			throw new RuntimeException("failed to append secret data", new SecureCompareException());
		}
	}
	
	native byte[] jniBegin();

	CompareResult parseResult(int result) {
		if (result == scompareNotReady()) {
		    return CompareResult.NOT_READY;
		}else if (result == scompareNoMatch()){
		    return CompareResult.NO_MATCH;
		}else if (result == scompareMatch()){
		    return CompareResult.MATCH;
		}
		throw new RuntimeException("unexpected comparison result: " + result, new SecureCompareException());
	}

	public byte[] begin() {
		byte[] compareData = jniBegin();
		
		if (null == compareData) {
			throw new RuntimeException("failed to begin comparison", new SecureCompareException());
		}
		
		return compareData;
	}
	
	native byte[] jniProceed(byte[] compareData);

	public byte[] proceed(byte[] compareData) throws SecureCompareException {
		return jniProceed(compareData);
	}
	
	native int jniGetResult();

	public CompareResult getResult() {
		return parseResult(jniGetResult());
	}

	native int scompareMatch();
	native int scompareNoMatch();
	native int scompareNotReady();
}
