package com.cossacklabs.themis.android.example

import android.os.Bundle
import android.util.Base64
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import com.cossacklabs.themis.PrivateKey
import com.cossacklabs.themis.PublicKey
import com.cossacklabs.themis.SecureMessage
import java.nio.charset.StandardCharsets
import java.util.*

class MainActivitySecureMessage : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        // Check secure message
        try {
            secureMessageLocal()
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }

    private fun secureMessageLocal() {
        val charset = StandardCharsets.UTF_8
        val clientPrivateKey = "UkVDMgAAAC1EvnquAPUxxwJsoJxoMAkEF7c06Fo7dVwnWPnmNX5afyjEEGmG"
        val serverPublicKey = "VUVDMgAAAC1FJv/DAmg8/L1Pl5l6ypyRqXUU9xQQaAgzfRZ+/gsjqgEdwXhc"
        val message = "some weird message here"
        val privateKey = PrivateKey(Base64.decode(clientPrivateKey.toByteArray(charset(charset.name())), Base64.NO_WRAP))
        val publicKey = PublicKey(Base64.decode(serverPublicKey.toByteArray(charset(charset.name())), Base64.NO_WRAP))
        Log.d("SMC", "privateKey1 = " + Arrays.toString(privateKey.toByteArray()))
        Log.d("SMC", "publicKey1 = " + Arrays.toString(publicKey.toByteArray()))
        val sm = SecureMessage(privateKey, publicKey)
        val wrappedMessage = sm.wrap(message.toByteArray(charset(charset.name())))
        val encodedMessage = Base64.encodeToString(wrappedMessage, Base64.NO_WRAP)
        Log.d("SMC", "EncodedMessage = $encodedMessage")
        val wrappedMessageFromB64 = Base64.decode(encodedMessage, Base64.NO_WRAP)
        val decodedMessage = String(sm.unwrap(wrappedMessageFromB64), charset)
        Log.d("SMC", "DecodedMessageFromOwnCode = $decodedMessage")
        val encodedMessageFromExternal = "ICcEJksAAAAAAQFADAAAABAAAAAXAAAAFi/vBAb2fiNBqf3a6wgyVoMAdPXpJ14ZYxk/oaUcwSmKnNgmRzaH7JkIQBvFChAVK9tF"
        val wrappedMessageFromB64External = Base64.decode(encodedMessageFromExternal, Base64.NO_WRAP)
        val decodedMessageExternal = String(sm.unwrap(wrappedMessageFromB64External), charset)
        Log.d("SMC", "DecodedMessageFromExternal = $decodedMessageExternal")
    }
}
