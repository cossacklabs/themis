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
