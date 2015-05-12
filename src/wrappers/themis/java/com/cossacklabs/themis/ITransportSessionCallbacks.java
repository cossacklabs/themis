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
