import java.io.*;
import java.net.*;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;

import javax.net.ssl.*;

import java.util.Arrays;
import java.util.Base64;


import com.cossacklabs.themis.*;
import com.cossacklabs.themis.SecureSession.UnwrapResult;

public class SSessionClient {

	public static byte[] PostRequest(String url, String message) throws IOException {
		String httpsURL = url;

		String query = "message=" + URLEncoder.encode(message, "UTF-8");
		URL myurl = new URL(httpsURL);
		HttpsURLConnection con = (HttpsURLConnection) myurl.openConnection();
		con.setRequestMethod("POST");

		con.setRequestProperty("Content-length", String.valueOf(query.length()));
		con.setRequestProperty("Content-Type", "application/x-www-form-urlencoded");
		con.setDoOutput(true);
		con.setDoInput(true);

		DataOutputStream output = new DataOutputStream(con.getOutputStream());


		output.writeBytes(query);
		output.close();
		if (con.getResponseCode() != 200) {
			System.out.println("Please setup Themis Interactive simulator, copy URL, client ID and keys");
			System.out.println("https://themis.cossacklabs.com/interactive-simulator/setup/");
			throw new IOException();
		}

		DataInputStream input = new DataInputStream(con.getInputStream());
		int ResBytesCapacity = 5 * 1024;
		byte[] ResBytes = new byte[ResBytesCapacity];
		int CurrPos = 0;
		int resLength = 0;
		int ReadedBytes = -1;
		while ((ReadedBytes = input.read(ResBytes, CurrPos, ResBytesCapacity - resLength)) != -1) {
			if ((resLength + ReadedBytes) > ResBytesCapacity)
				throw new IOException();
			resLength += ReadedBytes;
			CurrPos += ReadedBytes;
		}
		input.close();

		byte[] res_bytes = new byte[resLength];
		for (int i = 0; i < resLength; ++i)
			res_bytes[i] = ResBytes[i];
		return res_bytes;

	}

	// ---------------------- IMPORTANT SETUP ---------------------------------------

	// User id, Server id and Server Public Key should be copied from the Server Setup Page
  // https://themis.cossacklabs.com/interactive-simulator/setup/
	// Server public key ("server key"),

	public static void runSSessionWithThemisInteractiveSimulator() throws NullArgumentException, SecureMessageWrapException, IOException, SecureSessionException {
		ISessionCallbacks callbacks = new ISessionCallbacks() {

			@Override
			public PublicKey getPublicKeyForId(SecureSession session, byte[] id) {

				try {
					byte[] serverId = "mxzrQihjVhKjSAP".getBytes("UTF-8");
					if (Arrays.equals(id, serverId)) {
						return new PublicKey(Base64.getDecoder().decode("VUVDMgAAAC3x8WGaAxu+rPQ7lRLQ97nDwt1Evooip1YGu5DQk8FFBnhYSI36"));
					}
				} catch (Exception e) {
					System.out.println("Exception " + e);
				}
				return null;
			}

			@Override
			public void stateChanged(SecureSession session) {
			}
		};

		String ClientPrivateKey = "UkVDMgAAAC1EvnquAPUxxwJsoJxoMAkEF7c06Fo7dVwnWPnmNX5afyjEEGmG";
		String ClientPublicKey = "VUVDMgAAAC18urRTA1H1hts93vlLXX59OuyVnY1tGFxl/F3PkhDtzrdQETMi";

		String ServerPublicKey = "VUVDMgAAAC3x8WGaAxu+rPQ7lRLQ97nDwt1Evooip1YGu5DQk8FFBnhYSI36";
		String ClientId = "HAjpkyjHoojKUXg";
		String Message = "This is test message";
		String URL = "https://themis.cossacklabs.com/api/" + ClientId + "/";

		SecureSession ss = new SecureSession(ClientId.getBytes("UTF-8"), new PrivateKey(Base64.getDecoder().decode(ClientPrivateKey)), callbacks);

		String message_to_send = Base64.getEncoder().encodeToString(ss.generateConnectRequest());
		do {
			message_to_send = Base64.getEncoder().encodeToString(ss.unwrap(PostRequest(URL, message_to_send)).getData());
		} while (!(ss.isEstablished()));
		System.out.println(new String(ss.unwrap(PostRequest(URL, Base64.getEncoder().encodeToString(ss.wrap(Message.getBytes("UTF-8"))))).getData(), StandardCharsets.UTF_8));
	}
}
