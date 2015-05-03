package com.cossacklabs.themis;

public interface ITransportSessionCallbacks extends ISessionCallbacks {
	void write(byte[] buffer);
	byte[] read();
}
