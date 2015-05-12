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

import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.FilterInputStream;
import java.io.FilterOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.Socket;
import java.net.SocketAddress;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Arrays;

public class SecureSocket extends Socket {
	
	SecureSession session;
	
	InputStream inStream;
	OutputStream outStream;
	
	byte[] id;
	PrivateKey signPrivateKey;
	ISessionCallbacks callbacks;
	
	public SecureSocket(byte[] id, PrivateKey signPrivateKey, ISessionCallbacks callbacks) {
		super();
		
		this.id = id;
		this.signPrivateKey = signPrivateKey;
		this.callbacks = callbacks;
	}
	
	public SecureSocket(InetAddress dstAddress, int dstPort, byte[] id, PrivateKey signPrivateKey, ISessionCallbacks callbacks) throws IOException {
		super(dstAddress, dstPort);
		
		this.id = id;
		this.signPrivateKey = signPrivateKey;
		this.callbacks = callbacks;
		
		runClientProtocol();
	}
	
	static final byte[] HDR_TAG = {0x54, 0x53, 0x50, 0x4d}; // TSPM
	
	private void runClientProtocol() throws IOException {
		inStream = super.getInputStream();
		outStream = super.getOutputStream();
		
		try {
			session = new SecureSession(this.id, this.signPrivateKey, this.callbacks);
			byte[] connectRequest = session.generateConnectRequest();
			
			DataInputStream dataInStream = new DataInputStream(inStream);
			DataOutputStream dataOutStream = new DataOutputStream(outStream);
			
			dataOutStream.writeInt(0xffff0000 | connectRequest.length);
			dataOutStream.write(connectRequest);
			
			SecureSession.UnwrapResult result;
			
			do {

				int size = dataInStream.readInt();
				
				if (0xffff0000 != (size & 0xffff0000)) {
					throw new IOException("wrong protocol data");
				}
				
				size &= 0xffff;
				if (size < 12) {
					throw new IOException("wrong protocol data4");
				}
				
				byte[] protocolData = new byte[size];
				dataInStream.readFully(protocolData, 0, size);
				
				for (int i = 0; i < HDR_TAG.length; i++) {
					if (protocolData[i] != HDR_TAG[i]) {
						throw new IOException("wrong protocol data3");
					}
				}

				result = session.unwrap(protocolData);
				
				if (SecureSession.SessionDataType.PROTOCOL_DATA == result.getDataType()) {
					dataOutStream.writeInt(0xffff0000 | result.getData().length);
					dataOutStream.write(result.getData());
				}
				
			} while (!session.isEstablished());
			
		} catch (SecureSessionException | NullArgumentException e) {
			throw new IOException(e);
		}
	}
	
	void runServerProtocol() throws IOException {
		inStream = super.getInputStream();
		outStream = super.getOutputStream();
		
		try {
			session = new SecureSession(this.id, this.signPrivateKey, this.callbacks);
			
			DataInputStream dataInStream = new DataInputStream(inStream);
			DataOutputStream dataOutStream = new DataOutputStream(outStream);
			
			SecureSession.UnwrapResult result;
			
			do {
				
				int size = dataInStream.readInt();
				
				if (0xffff0000 != (size & 0xffff0000)) {
					throw new IOException("wrong protocol data");
				}
				
				size &= 0xffff;
				if (size < 12) {
					throw new IOException("wrong protocol data4");
				}
				
				byte[] protocolData = new byte[size];
				dataInStream.readFully(protocolData, 0, size);
				
				for (int i = 0; i < HDR_TAG.length; i++) {
					if (protocolData[i] != HDR_TAG[i]) {
						throw new IOException("wrong protocol data3");
					}
				}

				result = session.unwrap(protocolData);
				
				if (SecureSession.SessionDataType.PROTOCOL_DATA == result.getDataType()) {
					dataOutStream.writeInt(0xffff0000 | result.getData().length);
					dataOutStream.write(result.getData());
				}
			} while (!session.isEstablished());
			
		} catch (SecureSessionException | NullArgumentException e) {
			throw new IOException(e);
		}
	}
	
	@Override
	public void connect(SocketAddress remoteAddr, int timeout) throws IOException {
		super.connect(remoteAddr, timeout);
		
		runClientProtocol();
	}
	
	@Override
	public void connect(SocketAddress remoteAddr) throws IOException {
		this.connect(remoteAddr, 0);
	}
	
	@Override
	public void close() throws IOException {
		if (null != session) {
			session.close();
		}
		super.close();
	}
	
	@Override
	public OutputStream getOutputStream() throws IOException {
		if (null == secureOutputStream) {
			synchronized(this) {
				if (null == secureOutputStream) {
					secureOutputStream = new SecureOutputStream(outStream, session);
				}
			}
		}
		
		return secureOutputStream;
	}
	
	private SecureOutputStream secureOutputStream;
	
	private class SecureOutputStream extends FilterOutputStream {

		public SecureOutputStream(OutputStream out, SecureSession session) {
			super(new DataOutputStream(out));
		}
		
		@Override
		public void write(byte[] buffer, int offset, int length) throws IOException {
			byte[] data = new byte[length];
			System.arraycopy(buffer, offset, data, 0, length);
			
			try {
				byte[] wrappedData = session.wrap(data);
				((DataOutputStream)out).writeInt(0xffff0000 | wrappedData.length);
				out.write(wrappedData);
			} catch (SecureSessionException | NullArgumentException e) {
				throw new IOException(e);
			}
		}
		
		@Override
		public void write(int oneByte) throws IOException {
			byte[] bytes = new byte[1];
			bytes[0] = (byte)(0xff & oneByte);

			write(bytes);
		}
	}
	
	@Override
	public InputStream getInputStream() {
		if (null == secureInputStream) {
			synchronized(this) {
				if (null == secureInputStream) {
					secureInputStream = new SecureInputStream(inStream, session);
				}
			}
		}
		
		return secureInputStream;
	}
	
	private SecureInputStream secureInputStream;
	
	private class SecureInputStream extends FilterInputStream {
		
		ByteArrayInputStream bufferInputStream = new ByteArrayInputStream(new byte[0]);

		protected SecureInputStream(InputStream in, SecureSession session) {
			super(new DataInputStream(in));
		}
		
		@Override
		public int read() throws IOException {
			byte[] bytes = new byte[1];
			
			int r = this.read(bytes);
			if (1 != r) {
				throw new IOException();
			}

			return bytes[0];
		}
		
		@Override
		public int read(byte[] buffer, int byteOffset, int byteCount) throws IOException {
			
			synchronized (in) {
				if (bufferInputStream.available() > 0) {
					return bufferInputStream.read(buffer, byteOffset, byteCount);
				}
				
				DataInputStream dataInStream = (DataInputStream)in;
				int length = dataInStream.readInt();
				
				if (0xffff0000 != (length & 0xffff0000)) {
					throw new IOException("wrong protocol data");
				}
				
				length &= 0xffff;
				
				byte[] wrappedData = new byte[length];
				dataInStream.readFully(wrappedData);
				
				try {
					SecureSession.UnwrapResult result = session.unwrap(wrappedData);
					if (SecureSession.SessionDataType.USER_DATA != result.getDataType()) {
						throw new IOException("wrong protocol data");
					}
					
					bufferInputStream = new ByteArrayInputStream(result.getData());
					
					return bufferInputStream.read(buffer, byteOffset, byteCount);
				} catch (SecureSessionException | NullArgumentException e) {
					throw new IOException(e);
				}
			}
		}
	}
}
