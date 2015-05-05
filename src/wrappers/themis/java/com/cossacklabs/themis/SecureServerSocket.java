package com.cossacklabs.themis;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;

public class SecureServerSocket extends ServerSocket {
	
	byte[] id;
	PrivateKey signPrivateKey;
	ISessionCallbacks callbacks;

	public SecureServerSocket(int port, byte[] id, PrivateKey signPrivateKey, ISessionCallbacks callbacks) throws IOException {
		super(port);
		
		this.id = id;
		this.signPrivateKey = signPrivateKey;
		this.callbacks = callbacks;
	}
	
	@Override
	public Socket accept() throws IOException {
		if (isClosed()) {
			throw new SocketException("Socket is closed");
		}
		
		if (!isBound()) {
			throw new SocketException("Socket is not bound");
		}
		
		SecureSocket socket = new SecureSocket(this.id, this.signPrivateKey, this.callbacks);
		
		try {
			implAccept(socket);
		} catch (IOException e) {
			socket.close();
			throw e;
		}
		
		socket.runServerProtocol();
		
		return socket;
	}

}
