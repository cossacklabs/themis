package com.cossacklabs.themis.android.example

import android.util.Base64
import android.util.Log
import com.cossacklabs.themis.PrivateKey
import com.cossacklabs.themis.PublicKey
import com.cossacklabs.themis.SecureMessage
import java.nio.charset.StandardCharsets
import java.util.*
import java.util.concurrent.Executors

// ---------------------- IMPORTANT SETUP ---------------------------------------
// User id and Server Public Key should be copied from the Server Setup Page
// https://themis.cossacklabs.com/interactive-simulator/setup/
// Server public key ("server key"),
internal class SecMessageExampleClient {
    private val httpClient = HttpClient(Executors.newSingleThreadExecutor())

    fun testSMessageCIClient() {
        val privateKey = PrivateKey(Base64.decode(CLIENT_PRIVATE_KEY.toByteArray(CHARSET), Base64.NO_WRAP))
        val publicKey = PublicKey(Base64.decode(SERVER_PUBLIC_KEY.toByteArray(CHARSET), Base64.NO_WRAP))
        Log.d(LOG_TAG, "privateKey1 = " + Arrays.toString(privateKey.toByteArray()))
        Log.d(LOG_TAG, "publicKey1 = " + Arrays.toString(publicKey.toByteArray()))
        val sm = SecureMessage(privateKey, publicKey)
        val wrappedMessage = sm.wrap(MESSAGE.toByteArray(CHARSET))
        val encodedMessage = Base64.encodeToString(wrappedMessage, Base64.NO_WRAP)
        Log.d(LOG_TAG, "EncodedMessage = $encodedMessage")

        // from iOS
        //EncodedMessage = "ICcEJjgAAAAAAQFADAAAABAAAAAEAAAAfMHW4JAVNfHuuzDCQyk9iVbS7wpcIqP7Uw4EGkBUTZ8=";
        httpClient.sendMessageAsync(URL, encodedMessage, CHARSET, object : HttpCallback {
            override fun onSuccess(response: ByteArray?) {
                try {
                    val unwrappedResult = String(sm.unwrap(response), CHARSET)
                    println(unwrappedResult)
                    Log.d(LOG_TAG, "unwrappedResult = $unwrappedResult")
                } catch (e: Exception) {
                    onFail(e)
                }
            }

            override fun onFail(ex: Exception?) {
                Log.e(LOG_TAG, "", ex)
            }
        })
    }

    companion object {
        private const val LOG_TAG = "SMC"
        private const val CLIENT_PRIVATE_KEY = "UkVDMgAAAC1EvnquAPUxxwJsoJxoMAkEF7c06Fo7dVwnWPnmNX5afyjEEGmG"
        private const val CLIENT_PUBLIC_KEY = "VUVDMgAAAC18urRTA1H1hts93vlLXX59OuyVnY1tGFxl/F3PkhDtzrdQETMi"
        private const val SERVER_PUBLIC_KEY = "VUVDMgAAAC1FJv/DAmg8/L1Pl5l6ypyRqXUU9xQQaAgzfRZ+/gsjqgEdwXhc"
        private const val MESSAGE = "meow"
        private const val URL = "https://themis.cossacklabs.com/api/sjSwNQuZIaqsLJt/"
        private val CHARSET = StandardCharsets.UTF_8
    }
}
