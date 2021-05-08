package com.cossacklabs.themis.android.example;

import android.util.Base64;
import android.util.Log;

import com.cossacklabs.themis.NullArgumentException;
import com.cossacklabs.themis.PrivateKey;
import com.cossacklabs.themis.PublicKey;
import com.cossacklabs.themis.SecureMessage;
import com.cossacklabs.themis.SecureMessageWrapException;

import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;
import java.util.concurrent.Executors;

// ---------------------- IMPORTANT SETUP ---------------------------------------

// User id and Server Public Key should be copied from the Server Setup Page
// https://themis.cossacklabs.com/interactive-simulator/setup/
// Server public key ("server key"),

class SecMessageExampleClient {

    private final static String LOG_TAG = "SMC";

    private final static String CLIENT_PRIVATE_KEY = "UkVDMgAAAC1EvnquAPUxxwJsoJxoMAkEF7c06Fo7dVwnWPnmNX5afyjEEGmG";
    private final static String CLIENT_PUBLIC_KEY = "VUVDMgAAAC18urRTA1H1hts93vlLXX59OuyVnY1tGFxl/F3PkhDtzrdQETMi";
    private final static String SERVER_PUBLIC_KEY = "VUVDMgAAAC1FJv/DAmg8/L1Pl5l6ypyRqXUU9xQQaAgzfRZ+/gsjqgEdwXhc";

    private final static String MESSAGE = "meow";
    private final static String URL = "https://themis.cossacklabs.com/api/sjSwNQuZIaqsLJt/";

    private final static Charset CHARSET = StandardCharsets.UTF_8;

    private final HttpClient httpClient = new HttpClient(Executors.newSingleThreadExecutor());

    void testSMessageCIClient() throws NullArgumentException, SecureMessageWrapException {

        PrivateKey privateKey = new PrivateKey(Base64.decode(CLIENT_PRIVATE_KEY.getBytes(CHARSET), Base64.NO_WRAP));
        PublicKey publicKey = new PublicKey(Base64.decode(SERVER_PUBLIC_KEY.getBytes(CHARSET), Base64.NO_WRAP));
        Log.d(LOG_TAG, "privateKey1 = " + Arrays.toString(privateKey.toByteArray()));
        Log.d(LOG_TAG, "publicKey1 = " + Arrays.toString(publicKey.toByteArray()));

        final SecureMessage sm = new SecureMessage(privateKey, publicKey);

        byte[] wrappedMessage = sm.wrap(MESSAGE.getBytes(CHARSET));
        String encodedMessage = Base64.encodeToString(wrappedMessage, Base64.NO_WRAP);
        Log.d(LOG_TAG, "EncodedMessage = " + encodedMessage);

        // from iOS
        //EncodedMessage = "ICcEJjgAAAAAAQFADAAAABAAAAAEAAAAfMHW4JAVNfHuuzDCQyk9iVbS7wpcIqP7Uw4EGkBUTZ8=";

        httpClient.sendMessageAsync(URL, encodedMessage, CHARSET, new HttpCallback() {

            @Override
            public void onSuccess(byte[] response) {
                try {
                    String unwrappedResult = new String(sm.unwrap(response), CHARSET);
                    System.out.println(unwrappedResult);
                    Log.d(LOG_TAG, "unwrappedResult = " + unwrappedResult);

                } catch (Exception e) {
                    onFail(e);
                }
            }

            @Override
            public void onFail(Exception ex) {
                Log.e(LOG_TAG, "", ex);
            }
        });
    }
}
