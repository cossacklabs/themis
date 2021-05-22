import com.cossacklabs.themis.*;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;
import java.util.Base64;


public class main {

  static Charset charset = StandardCharsets.UTF_8;

  public static void main(final String[] args) throws NullArgumentException, SecureMessageWrapException, IOException, SecureSessionException, SecureCellException, InvalidArgumentException {
    encryptDataForStoring();
    encryptDataForMessaging();

    // tests with Themis Interactive simulator
    // setup Themis IS first:
    // https://themis.cossacklabs.com/interactive-simulator/setup/
//    SMessageClient.runSMessageWithThemisInteractiveSimulator();
//    SSessionClient.runSSessionWithThemisInteractiveSimulator();
  }

  static void encryptDataForStoring() throws SecureCellException, NullArgumentException, InvalidArgumentException {
    byte[] message = "data to encrypt".getBytes(charset);
    byte[] optionalContext = "some context".getBytes(charset);
    byte[] password = "pass".getBytes(charset);


    System.out.println("Running SecureCell example");
    SecureCell.Seal sc = SecureCell.SealWithPassphrase(password);

    byte[] encryptedData = sc.encrypt(message, optionalContext);
    String encryptedDataString = Base64.getEncoder().encodeToString(encryptedData);
    System.out.println("Encrypted encoded data = \n" + encryptedDataString);

    byte[] decodedEncryptedString = Base64.getDecoder().decode(encryptedDataString);

    byte[] unprotected = sc.decrypt(decodedEncryptedString, optionalContext);
    String decryptedString = new String(unprotected);
    System.out.println("Decrypted data = "+ decryptedString);
  }


  static void encryptDataForMessaging() throws UnsupportedEncodingException, NullArgumentException, SecureMessageWrapException {
    // keys can be generated using KeypairGenerator
    String clientPrivateKey = "UkVDMgAAAC1EvnquAPUxxwJsoJxoMAkEF7c06Fo7dVwnWPnmNX5afyjEEGmG";
    String serverPublicKey = "VUVDMgAAAC1FJv/DAmg8/L1Pl5l6ypyRqXUU9xQQaAgzfRZ+/gsjqgEdwXhc";

    String message = "message to send";

    System.out.println("Running SecureMessage example");

    PrivateKey privateKey = new PrivateKey(Base64.getDecoder().decode(clientPrivateKey.getBytes(charset.name())));
    PublicKey publicKey = new PublicKey(Base64.getDecoder().decode(serverPublicKey.getBytes(charset.name())));
    System.out.println("privateKey1 = " + Arrays.toString(privateKey.toByteArray()));
    System.out.println("publicKey1 = " + Arrays.toString(publicKey.toByteArray()));

    final SecureMessage sm = new SecureMessage(privateKey, publicKey);

    byte[] wrappedMessage = sm.wrap(message.getBytes(charset));
    String encodedMessage = Base64.getEncoder().encodeToString(wrappedMessage);
    System.out.println("EncodedMessage = " + encodedMessage);

    byte[] wrappedMessageFromB64 = Base64.getDecoder().decode(encodedMessage);
    String decodedMessage = new String(sm.unwrap(wrappedMessageFromB64), charset);
    System.out.println("DecodedMessage = " + decodedMessage);
  }
}
