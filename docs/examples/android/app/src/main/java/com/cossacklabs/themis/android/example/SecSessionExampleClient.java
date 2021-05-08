package com.cossacklabs.themis.android.example;

import android.util.Base64;
import android.util.Log;

import com.cossacklabs.themis.ISessionCallbacks;
import com.cossacklabs.themis.NullArgumentException;
import com.cossacklabs.themis.PrivateKey;
import com.cossacklabs.themis.PublicKey;
import com.cossacklabs.themis.SecureSession;
import com.cossacklabs.themis.SecureSessionException;

import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;
import java.util.concurrent.Executors;

// ---------------------- IMPORTANT SETUP ---------------------------------------

// User id, Server id and Server Public Key should be copied from the Server Setup Page
// https://themis.cossacklabs.com/interactive-simulator/setup/
// Server public key ("server key"),

class SecSessionExampleClient {

    private final static String LOG_TAG = "SSC";

    private final static String CLIENT_ID = "ULzsfYGzScUisNV";
    private final static String SERVER_ID = "mLAResUZAQcQXXl";
    private final static String CLIENT_PRIVATE_KEY = "UkVDMgAAAC1EvnquAPUxxwJsoJxoMAkEF7c06Fo7dVwnWPnmNX5afyjEEGmG";
    private final static String CLIENT_PUBLIC_KEY = "VUVDMgAAAC18urRTA1H1hts93vlLXX59OuyVnY1tGFxl/F3PkhDtzrdQETMi";
    private final static String SERVER_PUBLIC_KEY = "VUVDMgAAAC0boM1EAvAkoWsfqbMvugv/YzzMPC6AeKT/t5gtCb3xyPpEJJB/";

    private final static String MESSAGE = "msg msg msg :)";
    private final static String URL = "https://themis.cossacklabs.com/api/ULzsfYGzScUisNV/";

    private final static Charset CHARSET = StandardCharsets.UTF_8;

    private final HttpClient client = new HttpClient(Executors.newSingleThreadExecutor());

    private void startSession(final SecureSession ss, String base64MessageToSend) {
        client.sendMessageAsync(URL, base64MessageToSend, CHARSET, new HttpCallback() {
            @Override
            public void onSuccess(byte[] response) {
                try {
                    byte[] unwrappedData = ss.unwrap(response).getData();
                    String unwrappedResult = new String(unwrappedData, StandardCharsets.UTF_8);
                    System.out.println(unwrappedResult);

                    Log.d(LOG_TAG, "session establish in progress = " + unwrappedResult);
                    String messageToSend2 = Base64.encodeToString(unwrappedData, Base64.NO_WRAP);

                    if (!ss.isEstablished()) {
                        startSession(ss, messageToSend2);
                    } else {
                        sendRealMessage(ss);
                    }

                } catch (Exception e) {
                    onFail(e);
                }
            }

            @Override
            public void onFail(Exception ex) {
                Log.e(LOG_TAG, "startSession", ex);
            }
        });
    }

    private void sendRealMessage(final SecureSession ss) throws SecureSessionException, NullArgumentException {
        byte[] wrappedMessage = ss.wrap(MESSAGE.getBytes(CHARSET));
        String base64Message = Base64.encodeToString(wrappedMessage, Base64.NO_WRAP);

        client.sendMessageAsync(URL, base64Message, CHARSET, new HttpCallback() {
            @Override
            public void onSuccess(byte[] response) {
                try {
                    byte[] unwrappedData = ss.unwrap(response).getData();
                    String unwrappedResult = new String(unwrappedData, StandardCharsets.UTF_8);
                    System.out.println(unwrappedResult);

                    Log.d(LOG_TAG, "message received = " + unwrappedResult);
                } catch (Exception e) {
                    onFail(e);
                }
            }

            @Override
            public void onFail(Exception ex) {
                Log.e(LOG_TAG, "sendRealMessage", ex);
            }
        });
    }

    void testSSessionCIClient() throws SecureSessionException {
        ISessionCallbacks callbacks = new ISessionCallbacks() {

            @Override
            public PublicKey getPublicKeyForId(SecureSession session, byte[] id) {
                byte[] serverId = SERVER_ID.getBytes(CHARSET);
                if (Arrays.equals(id, serverId)) {
                    PublicKey publicKey = new PublicKey(Base64.decode(SERVER_PUBLIC_KEY.getBytes(CHARSET), Base64.NO_WRAP));
                    Log.d(LOG_TAG, "publicKey1 = " + Arrays.toString(publicKey.toByteArray()));
                    return publicKey;
                }
                return null;
            }

            @Override
            public void stateChanged(SecureSession session) {
            }
        };

        PrivateKey privateKey = new PrivateKey(Base64.decode(CLIENT_PRIVATE_KEY.getBytes(CHARSET), Base64.NO_WRAP));
        Log.d(LOG_TAG, "privateKey1 = " + Arrays.toString(privateKey.toByteArray()));

        final SecureSession ss = new SecureSession(CLIENT_ID.getBytes(CHARSET), privateKey, callbacks);

        final String messageToSend = Base64.encodeToString(ss.generateConnectRequest(), Base64.NO_WRAP);
        startSession(ss, messageToSend);
    }
}
