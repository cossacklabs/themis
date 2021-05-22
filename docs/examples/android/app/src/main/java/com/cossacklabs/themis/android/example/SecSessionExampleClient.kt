package com.cossacklabs.themis.android.example

import android.util.Base64
import android.util.Log
import com.cossacklabs.themis.ISessionCallbacks
import com.cossacklabs.themis.PrivateKey
import com.cossacklabs.themis.PublicKey
import com.cossacklabs.themis.SecureSession
import java.nio.charset.StandardCharsets
import java.util.*
import java.util.concurrent.Executors

// ---------------------- IMPORTANT SETUP ---------------------------------------
// User id, Server id and Server Public Key should be copied from the Server Setup Page
// https://themis.cossacklabs.com/interactive-simulator/setup/
// Server public key ("server key"),
internal class SecSessionExampleClient {
    private val client = HttpClient(Executors.newSingleThreadExecutor())
    private fun startSession(ss: SecureSession, base64MessageToSend: String) {
        client.sendMessageAsync(URL, base64MessageToSend, CHARSET, object : HttpCallback {
            override fun onSuccess(response: ByteArray?) {
                try {
                    val unwrappedData = ss.unwrap(response).data
                    val unwrappedResult = String(unwrappedData, StandardCharsets.UTF_8)
                    println(unwrappedResult)
                    Log.d(LOG_TAG, "session establish in progress = $unwrappedResult")
                    val messageToSend2 = Base64.encodeToString(unwrappedData, Base64.NO_WRAP)
                    if (!ss.isEstablished) {
                        startSession(ss, messageToSend2)
                    } else {
                        sendRealMessage(ss)
                    }
                } catch (e: Exception) {
                    onFail(e)
                }
            }

            override fun onFail(ex: Exception?) {
                Log.e(LOG_TAG, "startSession", ex)
            }
        })
    }

    private fun sendRealMessage(ss: SecureSession) {
        val wrappedMessage = ss.wrap(MESSAGE.toByteArray(CHARSET))
        val base64Message = Base64.encodeToString(wrappedMessage, Base64.NO_WRAP)
        client.sendMessageAsync(URL, base64Message, CHARSET, object : HttpCallback {
            override fun onSuccess(response: ByteArray?) {
                try {
                    val unwrappedData = ss.unwrap(response).data
                    val unwrappedResult = String(unwrappedData, StandardCharsets.UTF_8)
                    println(unwrappedResult)
                    Log.d(LOG_TAG, "message received = $unwrappedResult")
                } catch (e: Exception) {
                    onFail(e)
                }
            }

            override fun onFail(ex: Exception?) {
                Log.e(LOG_TAG, "sendRealMessage", ex)
            }
        })
    }

    fun testSSessionCIClient() {
        val callbacks: ISessionCallbacks = object : ISessionCallbacks {
            override fun getPublicKeyForId(session: SecureSession, id: ByteArray): PublicKey? {
                val serverId = SERVER_ID.toByteArray(CHARSET)
                if (id.contentEquals(serverId)) {
                    val publicKey = PublicKey(Base64.decode(SERVER_PUBLIC_KEY.toByteArray(CHARSET), Base64.NO_WRAP))
                    Log.d(LOG_TAG, "publicKey1 = " + Arrays.toString(publicKey.toByteArray()))
                    return publicKey
                }
                return null
            }

            override fun stateChanged(session: SecureSession) {}
        }
        val privateKey = PrivateKey(Base64.decode(CLIENT_PRIVATE_KEY.toByteArray(CHARSET), Base64.NO_WRAP))
        Log.d(LOG_TAG, "privateKey1 = " + Arrays.toString(privateKey.toByteArray()))
        val ss = SecureSession(CLIENT_ID.toByteArray(CHARSET), privateKey, callbacks)
        val messageToSend = Base64.encodeToString(ss.generateConnectRequest(), Base64.NO_WRAP)
        startSession(ss, messageToSend)
    }

    companion object {
        private const val LOG_TAG = "SSC"
        private const val CLIENT_ID = "ULzsfYGzScUisNV"
        private const val SERVER_ID = "mLAResUZAQcQXXl"
        private const val CLIENT_PRIVATE_KEY = "UkVDMgAAAC1EvnquAPUxxwJsoJxoMAkEF7c06Fo7dVwnWPnmNX5afyjEEGmG"
        private const val CLIENT_PUBLIC_KEY = "VUVDMgAAAC18urRTA1H1hts93vlLXX59OuyVnY1tGFxl/F3PkhDtzrdQETMi"
        private const val SERVER_PUBLIC_KEY = "VUVDMgAAAC0boM1EAvAkoWsfqbMvugv/YzzMPC6AeKT/t5gtCb3xyPpEJJB/"
        private const val MESSAGE = "msg msg msg :)"
        private const val URL = "https://themis.cossacklabs.com/api/ULzsfYGzScUisNV/"
        private val CHARSET = StandardCharsets.UTF_8
    }
}
