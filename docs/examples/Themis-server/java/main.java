import java.io.IOException;

import com.cossacklabs.themis.NullArgumentException;
import com.cossacklabs.themis.SecureMessageWrapException;
import com.cossacklabs.themis.SecureSessionException;

public class main {
	public static void main(final String[] args) throws NullArgumentException, SecureMessageWrapException, IOException, SecureSessionException {
		//SMessageClient.SMessageCIClientTest();
		SSessionClient.SSessionCIClientTest();
    }
}
