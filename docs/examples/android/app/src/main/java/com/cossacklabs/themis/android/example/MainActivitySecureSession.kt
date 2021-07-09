package com.cossacklabs.themis.android.example

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity

class MainActivitySecureSession : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        // Check secure session
        try {
            // tests with Themis Interactive simulator
            // setup Themis IS first:
            // https://themis.cossacklabs.com/interactive-simulator/setup/
            val client = SecSessionExampleClient()
            client.testSSessionCIClient()
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }
}
