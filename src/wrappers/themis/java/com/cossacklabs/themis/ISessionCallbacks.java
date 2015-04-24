package com.cossacklabs.themis;

public interface ISessionCallbacks {
	PublicKey getPublicKeyForId(SecureSession session, byte[] id);
	void stateChanged(SecureSession session);
}
