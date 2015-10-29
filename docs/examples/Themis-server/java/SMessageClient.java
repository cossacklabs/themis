import java.io.*;
import java.net.*;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;

import javax.net.ssl.*;
import java.util.Base64;


import com.cossacklabs.themis.*;

public class SMessageClient {
	public static byte[] PostRequest(String url, String message) throws IOException{
		String httpsURL = url;

		String query = "message="+URLEncoder.encode(message,"UTF-8"); 
		URL myurl = new URL(httpsURL);
		HttpsURLConnection con = (HttpsURLConnection)myurl.openConnection();
		con.setRequestMethod("POST");

		con.setRequestProperty("Content-length", String.valueOf(query.length())); 
		con.setRequestProperty("Content-Type","application/x-www-form-urlencoded"); 
		con.setDoOutput(true); 
		con.setDoInput(true); 

		DataOutputStream output = new DataOutputStream(con.getOutputStream());  


		output.writeBytes(query);
		output.close();
		if (con .getResponseCode() != 200)
			throw new IOException();
		DataInputStream input = new DataInputStream( con.getInputStream() ); 		
		int ResBytesCapacity=5*1024;
		byte[] ResBytes= new byte[ResBytesCapacity];
		int CurrPos=0;
		int resLength=0;
		int ReadedBytes=-1;
		while( (ReadedBytes = input.read(ResBytes,CurrPos, ResBytesCapacity-resLength))!=-1){
			if((resLength+ReadedBytes)>ResBytesCapacity)
				throw new IOException();
			resLength+=ReadedBytes;
			CurrPos+=ReadedBytes;
		}
		input.close(); 

		byte[] res_bytes = new byte[resLength];
		for(int i=0;i<resLength;++i)
			res_bytes[i] = ResBytes[i];
		return res_bytes;
	}
	
	public static void SMessageCIClientTest() throws NullArgumentException, SecureMessageWrapException, IOException{
		String ClientPrivateKey="UkVDMgAAAC1whm6SAJ7vIP18Kq5QXgLd413DMjnb6Z5jAeiRgUeekMqMC0+x";
		String ServerPublicKey="VUVDMgAAAC1NY6NZAz62ZG25YU67a5zX1iDo0Aijz8xOu4nzE0yQhtJIQBbD";
		String ClientId="OiJLrqNZZvUcaCH";
		String Message="This is test message";
		String URL="https://themis.cossacklabs.com/api/"+ClientId+"/";
		
		SecureMessage sm=new SecureMessage(new PrivateKey(Base64.getDecoder().decode(ClientPrivateKey)), new PublicKey(Base64.getDecoder().decode(ServerPublicKey)));
		
		String EncodedMessage=Base64.getEncoder().encodeToString(sm.wrap(Message.getBytes("UTF-8")));
		byte[] a=PostRequest(URL,EncodedMessage);
		System.out.println(new String(sm.unwrap(a), StandardCharsets.UTF_8));
	}
}
