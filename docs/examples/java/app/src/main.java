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
    String message = "hello from alice to bob";

    System.out.println("Running SecureMessage example");

    Keypair alice = KeypairGenerator.generateKeypair(AsymmetricKey.KEYTYPE_EC);
    PrivateKey alicePrivateKey = alice.getPrivateKey();
    PublicKey alicePublicKey = alice.getPublicKey();

    Keypair bob = KeypairGenerator.generateKeypair(AsymmetricKey.KEYTYPE_EC);
    PrivateKey bobPrivateKey = bob.getPrivateKey();
    PublicKey bobPublicKey = bob.getPublicKey();

    System.out.println("alicePrivateKey = " + Arrays.toString(alicePrivateKey.toByteArray()));
    System.out.println("alicePublicKey = " + Arrays.toString(alicePublicKey.toByteArray()));
    System.out.println("bobPrivateKey = " + Arrays.toString(bobPrivateKey.toByteArray()));
    System.out.println("bobPublicKey = " + Arrays.toString(bobPublicKey.toByteArray()));

    final SecureMessage aliceSM = new SecureMessage(alicePrivateKey, bobPublicKey);

    byte[] wrappedMessage = aliceSM.wrap(message.getBytes(charset));
    String encodedMessage = Base64.getEncoder().encodeToString(wrappedMessage);
    System.out.println("EncodedMessage = " + encodedMessage);


    final SecureMessage bobSM = new SecureMessage(bobPrivateKey, alicePublicKey);
    byte[] wrappedMessageFromB64 = Base64.getDecoder().decode(encodedMessage);
    String decodedMessage = new String(bobSM.unwrap(wrappedMessageFromB64), charset);
    System.out.println("DecodedMessage = " + decodedMessage);
  }
}
