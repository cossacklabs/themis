package com.reactnativethemis;

import androidx.annotation.NonNull;

import com.cossacklabs.themis.AsymmetricKey;
import com.cossacklabs.themis.Keypair;
import com.cossacklabs.themis.KeypairGenerator;
import com.cossacklabs.themis.NullArgumentException;
import com.cossacklabs.themis.PrivateKey;
import com.cossacklabs.themis.PublicKey;
import com.cossacklabs.themis.SecureCompareException;
import com.cossacklabs.themis.SecureMessageWrapException;
import com.cossacklabs.themis.SymmetricKey;
import com.cossacklabs.themis.SecureCell;
import com.cossacklabs.themis.SecureCellException;
import com.cossacklabs.themis.SecureCellData;
import com.cossacklabs.themis.SecureMessage;
import com.cossacklabs.themis.SecureCompare;

import com.facebook.react.bridge.Callback;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.bridge.ReadableArray;
import com.facebook.react.bridge.WritableArray;
import com.facebook.react.bridge.WritableMap;
import com.facebook.react.bridge.WritableNativeArray;
import com.facebook.react.bridge.WritableNativeMap;

import java.nio.charset.StandardCharsets;
import java.util.HashMap;
import java.util.Map;
import java.util.UUID;

public class ThemisModule extends ReactContextBaseJavaModule {
  public static class ByteOverflowException extends Exception {
    public ByteOverflowException(String errorMessage) {
      super(errorMessage);
    }
  }

  Map<String, SecureCompare> cmprs = new HashMap<>();

  private final static int comparator_not_ready = 0;
  private final static int comparator_not_match = 22;
  private final static int comparator_match = 21;
  private final static int comparator_error = -1;

  private final static String contextRequired = "Context required";
  private final static String privateKeyRequired = "Private key can not be null or blank";
  private final static String publicKeyRequired = "Public key can not be null or blank";

  ThemisModule(ReactApplicationContext context)
  {
    super(context);
  }

  @NonNull
  @Override
  public String getName()
  {
    return "Themis";
  }

  /* Replace JAVA enum version to equal with iOS version. */

  @Override
  public Map<String, Object> getConstants()
  {
    final Map<String, Object> constants = new HashMap<>();
    constants.put("COMPARATOR_NOT_READY", comparator_not_ready);
    constants.put("COMPARATOR_NOT_MATCH", comparator_not_match);
    constants.put("COMPARATOR_MATCH", comparator_match);
    constants.put("COMPARATOR_ERROR", comparator_error);
    constants.put("KEYTYPE_RSA", AsymmetricKey.KEYTYPE_RSA);
    constants.put("KEYTYPE_EC", AsymmetricKey.KEYTYPE_EC);
    return constants;
  }

  private static int compareResultSerialize(SecureCompare.CompareResult result)
  {
    switch (result) {
      case NOT_READY: return comparator_not_ready;
      case NO_MATCH: return comparator_not_match;
      case MATCH: return comparator_match;
      default: return comparator_error;
    }
  }

  private static WritableArray dataSerialize(byte[] data)
  {
    if (data == null) {
      return null;
    }
    WritableArray result = new WritableNativeArray();
    for (byte datum : data) result.pushInt(datum);

    return result;
  }

  private static byte[] dataDeserialize(ReadableArray serializedData) throws ByteOverflowException {
    if (serializedData == null || serializedData.size() == 0) {
      return null;
    }
    byte[] data = new byte[serializedData.size()];
    for (int i = 0; i < serializedData.size(); i++) {
      if (serializedData.getInt(i) >= 0 && serializedData.getInt(i) <= 255) {
        byte j = (byte) serializedData.getInt(i);
        data[i] = j;
      } else {
        throw new ByteOverflowException("Value " + serializedData.getInt(i) + " is out of range");
      }
    }
    return data;
  }

  @ReactMethod
  public void stringSerialize(String text,
                              Callback callback)
  {
    byte[] data = text.getBytes(StandardCharsets.UTF_8);
    WritableArray response = dataSerialize(data);
    callback.invoke(response);
  }

  @ReactMethod
  public void keyPair(int algorithm, Callback callback)
  {
    Keypair pair;
    switch (algorithm) {
      case AsymmetricKey.KEYTYPE_RSA:
        pair = KeypairGenerator.generateKeypair(AsymmetricKey.KEYTYPE_RSA);
        break;
      default:
        pair = KeypairGenerator.generateKeypair(AsymmetricKey.KEYTYPE_EC);
    }

    KeypairGenerator.generateKeypair();
    PrivateKey privateKey = pair.getPrivateKey();
    PublicKey publicKey = pair.getPublicKey();

    WritableArray privateSerialized = dataSerialize(privateKey.toByteArray());
    WritableArray publicSerialized = dataSerialize(publicKey.toByteArray());

    WritableMap response = new WritableNativeMap();
    response.putArray("private", privateSerialized);
    response.putArray("public", publicSerialized);

    callback.invoke(response);
  }

  @ReactMethod
  public void symmetricKey(Callback callback)
  {
    SymmetricKey masterKey = new SymmetricKey();
    WritableArray response = dataSerialize(masterKey.toByteArray());
    callback.invoke(response);
  }

  @ReactMethod
  public void secureCellSealWithSymmetricKeyEncrypt(ReadableArray symmetricKey,
                                                           String plaintext,
                                                           String context,
                                                         Callback successCallback,
                                                         Callback errorCallback)
  {
    try {
      byte[] symmetricKeyBinary = dataDeserialize(symmetricKey);
      SecureCell.Seal cell = SecureCell.SealWithKey(symmetricKeyBinary);
      byte[] plaintextBinary = plaintext.getBytes(StandardCharsets.UTF_8);
      byte[] contextBinary = context.getBytes(StandardCharsets.UTF_8);
      byte[] encrypted = cell.encrypt(plaintextBinary, contextBinary);
      WritableArray response = dataSerialize(encrypted);
      successCallback.invoke(response);
    } catch (Exception e) {
      WritableMap error = new WritableNativeMap();
      error.putString("message", e.toString());
      errorCallback.invoke(error);
    }
  }

  @ReactMethod
  public void secureCellSealWithSymmetricKeyDecrypt(ReadableArray symmetricKey,
                                                    ReadableArray encrypted,
                                                           String context,
                                                         Callback successCallback,
                                                         Callback errorCallback)
  {
    try {
      byte[] symmetricKeyBinary = dataDeserialize(symmetricKey);
      SecureCell.Seal cell = SecureCell.SealWithKey(symmetricKeyBinary);
      byte[] encryptedBinary = dataDeserialize(encrypted);
      byte[] contextBinary = context.getBytes(StandardCharsets.UTF_8);
      byte[] decrypted = cell.decrypt(encryptedBinary, contextBinary);
      WritableArray response = dataSerialize(decrypted);
      successCallback.invoke(response);
    } catch (Exception e) {
      WritableMap error = new WritableNativeMap();
      error.putString("message", e.toString());
      errorCallback.invoke(error);
    }
  }

  @ReactMethod
  public void secureCellSealWithPassphraseEncrypt(String passphrase,
                                                  String plaintext,
                                                  String context,
                                                Callback successCallback,
                                                Callback errorCallback)
  {
    try {
      SecureCell.Seal cell = SecureCell.SealWithPassphrase(passphrase);
      byte[] plaintextBinary = plaintext.getBytes(StandardCharsets.UTF_8);
      byte[] contextBinary = context.getBytes(StandardCharsets.UTF_8);
      byte[] encrypted = cell.encrypt(plaintextBinary, contextBinary);
      WritableArray response = dataSerialize(encrypted);
      successCallback.invoke(response);
    } catch (Exception e) {
      WritableMap error = new WritableNativeMap();
      error.putString("message", e.toString());
      errorCallback.invoke(error);
    }
  }

  @ReactMethod
  public void secureCellSealWithPassphraseDecrypt(String passphrase,
                                           ReadableArray encrypted,
                                                  String context,
                                                Callback successCallback,
                                                Callback errorCallback)
  {
    try {
      SecureCell.Seal cell = SecureCell.SealWithPassphrase(passphrase);
      byte[] encryptedBinary = dataDeserialize(encrypted);
      byte[] contextBinary = context.getBytes(StandardCharsets.UTF_8);
      byte[] decrypted = cell.decrypt(encryptedBinary, contextBinary);
      WritableArray response = dataSerialize(decrypted);
      successCallback.invoke(response);
    } catch (Exception e) {
      WritableMap error = new WritableNativeMap();
      error.putString("message", e.toString());
      errorCallback.invoke(error);
    }
  }

  /* MARK: Token protect mode */
  @ReactMethod
  public void secureCellTokenProtectEncrypt(ReadableArray symmetricKey,
                                                   String plaintext,
                                                   String context,
                                                 Callback successCallback,
                                                 Callback errorCallback)
  {
    try {
      byte[] symmetricKeyBinary = dataDeserialize(symmetricKey);
      SecureCell.TokenProtect cell = SecureCell.TokenProtectWithKey(symmetricKeyBinary);
      byte[] plaintextBinary = plaintext.getBytes(StandardCharsets.UTF_8);
      byte[] contextBinary = context.getBytes(StandardCharsets.UTF_8);
      SecureCellData result = cell.encrypt(plaintextBinary, contextBinary);
      byte[] encrypted = result.getProtectedData();
      byte[] authToken = result.getAdditionalData();
      WritableMap response = new WritableNativeMap();
      response.putArray("encrypted", dataSerialize(encrypted));
      response.putArray("token", dataSerialize(authToken));
      successCallback.invoke(response);
    } catch (Exception e) {
      WritableMap error = new WritableNativeMap();
      error.putString("message", e.toString());
      errorCallback.invoke(error);
    }
  }

  @ReactMethod
  public void secureCellTokenProtectDecrypt(ReadableArray symmetricKey,
                                            ReadableArray encrypted,
                                            ReadableArray token,
                                                   String context,
                                                 Callback successCallback,
                                                 Callback errorCallback)
  {
    try {
      byte[] symmetricKeyBinary = dataDeserialize(symmetricKey);
      byte[] encryptedBinary = dataDeserialize(encrypted);
      byte[] tokenBinary = dataDeserialize(token);
      SecureCell.TokenProtect cell = SecureCell.TokenProtectWithKey(symmetricKeyBinary);
      byte[] contextBinary = context.getBytes(StandardCharsets.UTF_8);
      byte[] decrypted = cell.decrypt(encryptedBinary, tokenBinary, contextBinary);
      WritableArray response = dataSerialize(decrypted);
      successCallback.invoke(response);
    } catch (Exception e) {
      WritableMap error = new WritableNativeMap();
      error.putString("message", e.toString());
      errorCallback.invoke(error);
    }
  }

  /* Context imprint mode */
  @ReactMethod
  public void secureCellContextImprintEncrypt(ReadableArray symmetricKey,
                                                     String plaintext,
                                                     String context,
                                                   Callback successCallback,
                                                   Callback errorCallback)
  {
    if (context == null || context.length() == 0) {
      WritableMap error = new WritableNativeMap();
      error.putString("message", contextRequired);
      errorCallback.invoke(error);
      return;
    }
    try {
      byte[] symmetricKeyBinary = dataDeserialize(symmetricKey);
      SecureCell.ContextImprint cell = SecureCell.ContextImprintWithKey(symmetricKeyBinary);
      byte[] plaintextBinary = plaintext.getBytes(StandardCharsets.UTF_8);
      byte[] contextBinary = context.getBytes(StandardCharsets.UTF_8);
      byte[] encrypted = cell.encrypt(plaintextBinary, contextBinary);
      WritableArray response = dataSerialize(encrypted);
      successCallback.invoke(response);
    } catch (Exception e) {
      WritableMap error = new WritableNativeMap();
      error.putString("message", e.toString());
      errorCallback.invoke(error);
    }
  }

  @ReactMethod
  public void secureCellContextImprintDecrypt(ReadableArray symmetricKey,
                                              ReadableArray encrypted,
                                                     String context,
                                                   Callback successCallback,
                                                   Callback errorCallback)
  {
    if (context == null || context.length() == 0) {
      WritableMap error = new WritableNativeMap();
      error.putString("message", contextRequired);
      errorCallback.invoke(error);
      return;
    }
    try {
      byte[] symmetricKeyBinary = dataDeserialize(symmetricKey);
      byte[] encryptedBinary = dataDeserialize(encrypted);
      SecureCell.ContextImprint cell = SecureCell.ContextImprintWithKey(symmetricKeyBinary);
      byte[] contextBinary = context.getBytes(StandardCharsets.UTF_8);
      byte[] decrypted = cell.decrypt(encryptedBinary, contextBinary);
      WritableArray response = dataSerialize(decrypted);
      successCallback.invoke(response);
    } catch (Exception e) {
      WritableMap error = new WritableNativeMap();
      error.putString("message", e.toString());
      errorCallback.invoke(error);
    }
  }

  /* Secure Message */
  @ReactMethod
  public void secureMessageSign(String message,
                         ReadableArray privateKey,
                              Callback successCallback,
                              Callback errorCallback)
  {
    if (privateKey == null || privateKey.size() == 0) {
      WritableMap error = new WritableNativeMap();
      error.putString("message", privateKeyRequired);
      errorCallback.invoke(error);
      return;
    }
    try {
      byte[] privateKeyBinary = dataDeserialize(privateKey);
      PrivateKey keyPrivateKey = new PrivateKey(privateKeyBinary);
      byte[] messageBinary = message.getBytes(StandardCharsets.UTF_8);
      SecureMessage secureMessage = new SecureMessage(keyPrivateKey);
      byte[] signedMessage = secureMessage.sign(messageBinary);
      WritableArray response = dataSerialize(signedMessage);
      successCallback.invoke(response);
    } catch (Exception e) {
      WritableMap error = new WritableNativeMap();
      error.putString("message", e.toString());
      errorCallback.invoke(error);
    }
  }

  @ReactMethod
  public void secureMessageVerify(ReadableArray message,
                                  ReadableArray publicKey,
                                       Callback successCallback,
                                       Callback errorCallback)
  {
    if (publicKey == null || publicKey.size() == 0) {
      WritableMap error = new WritableNativeMap();
      error.putString("message", publicKeyRequired);
      errorCallback.invoke(error);
      return;
    }
    try {
      byte[] publicKeyBinary = dataDeserialize(publicKey);
      PublicKey keyPublicKey = new PublicKey(publicKeyBinary);
      byte[] messageBinary = dataDeserialize(message);
      SecureMessage secureMessage = new SecureMessage(keyPublicKey);
      byte[] verifiedMessage = secureMessage.verify(messageBinary, keyPublicKey);
      WritableArray response = dataSerialize(verifiedMessage);
      successCallback.invoke(response);
    } catch (Exception e) {
      WritableMap error = new WritableNativeMap();
      error.putString("message", e.toString());
      errorCallback.invoke(error);
    }
  }

  @ReactMethod
  public void secureMessageEncrypt(String message,
                            ReadableArray privateKey,
                            ReadableArray publicKey,
                                 Callback successCallback,
                                 Callback errorCallback)
  {
    if (privateKey == null || privateKey.size() == 0) {
      WritableMap error = new WritableNativeMap();
      error.putString("message", privateKeyRequired);
      errorCallback.invoke(error);
      return;
    }
    if (publicKey == null || publicKey.size() == 0) {
      WritableMap error = new WritableNativeMap();
      error.putString("message", publicKeyRequired);
      errorCallback.invoke(error);
      return;
    }
    try {
      byte[] privateKeyBinary = dataDeserialize(privateKey);
      PrivateKey keyPrivateKey = new PrivateKey(privateKeyBinary);
      byte[] publicKeyBinary = dataDeserialize(publicKey);
      PublicKey keyPublicKey = new PublicKey(publicKeyBinary);
      byte[] messageBinary = message.getBytes(StandardCharsets.UTF_8);
      SecureMessage secureMessage = new SecureMessage(keyPrivateKey, keyPublicKey);
      byte[] encryptedMessage = secureMessage.wrap(messageBinary);
      WritableArray response = dataSerialize(encryptedMessage);
      successCallback.invoke(response);
    } catch (Exception e) {
      WritableMap error = new WritableNativeMap();
      error.putString("message", e.toString());
      errorCallback.invoke(error);
    }
  }

  @ReactMethod
  public void secureMessageDecrypt(ReadableArray message,
                                   ReadableArray privateKey,
                                   ReadableArray publicKey,
                                        Callback successCallback,
                                        Callback errorCallback)
  {
    if (privateKey == null || privateKey.size() == 0) {
      WritableMap error = new WritableNativeMap();
      error.putString("message", privateKeyRequired);
      errorCallback.invoke(error);
      return;
    }
    if (publicKey == null || publicKey.size() == 0) {
      WritableMap error = new WritableNativeMap();
      error.putString("message", publicKeyRequired);
      errorCallback.invoke(error);
      return;
    }
    try {
      byte[] privateKeyBinary = dataDeserialize(privateKey);
      PrivateKey keyPrivateKey = new PrivateKey(privateKeyBinary);
      byte[] publicKeyBinary = dataDeserialize(publicKey);
      PublicKey keyPublicKey = new PublicKey(publicKeyBinary);
      byte[] messageBinary = dataDeserialize(message);
      SecureMessage secureMessage = new SecureMessage(keyPrivateKey, keyPublicKey);
      byte[] decrypted = secureMessage.unwrap(messageBinary, keyPublicKey);
      WritableArray response = dataSerialize(decrypted);
      successCallback.invoke(response);
    } catch (Exception e) {
      WritableMap error = new WritableNativeMap();
      error.putString("message", e.toString());
      errorCallback.invoke(error);
    }
  }

  @ReactMethod
  public void initComparator(ReadableArray sharedSecret,
                                  Callback successCallback,
                                  Callback errorCallback)
  {
    try {
      byte[] sharedSecretData = dataDeserialize(sharedSecret);
      SecureCompare cmp = new SecureCompare(sharedSecretData);
      final String uuid = UUID.randomUUID().toString();
      cmprs.put(uuid, cmp);
      successCallback.invoke(uuid);
    } catch (Exception e) {
      WritableMap error = new WritableNativeMap();
      error.putString("message", e.toString());
      errorCallback.invoke(error);
    }
  }

  @ReactMethod
  public void statusOfComparator(String uuid,
                               Callback callback)
  {
    SecureCompare cmp = cmprs.get(uuid);
    if (cmp == null) {
      callback.invoke(comparator_error);
    } else {
      SecureCompare.CompareResult result = cmp.getResult();
      int status = compareResultSerialize(result);
      callback.invoke(status);
    }
  }

  @ReactMethod
  public void beginCompare(String uuid,
                         Callback successCallback,
                         Callback errorCallback)
  {
    SecureCompare cmp = cmprs.get(uuid);
    if (cmp == null) {
      errorCallback.invoke(comparator_error);
    } else {
      byte[] data = cmp.begin();
      WritableArray response = dataSerialize(data);
      successCallback.invoke(response);
    }
  }

  @ReactMethod
  public void proceedCompare(String uuid,
                      ReadableArray previous,
                           Callback successCallback,
                           Callback errorCallback)
  {
    SecureCompare cmp = cmprs.get(uuid);
    if (cmp == null) {
        errorCallback.invoke(comparator_error);
        return;
    }
    try {
      byte[] data = dataDeserialize(previous);
      data = cmp.proceed(data);
      WritableArray response = dataSerialize(data);
      SecureCompare.CompareResult result = cmp.getResult();
      int status = compareResultSerialize(result);
      if (result != SecureCompare.CompareResult.NOT_READY) {
        cmprs.remove(uuid);
      }
      successCallback.invoke(response, status);
    } catch (Exception e) {
      WritableMap error = new WritableNativeMap();
      error.putString("message", e.toString());
      errorCallback.invoke(error);
    }
  }
}
