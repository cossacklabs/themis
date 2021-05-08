package com.cossacklabs.themis.android.example

import android.os.Bundle
import android.util.Base64
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import com.cossacklabs.themis.InvalidArgumentException
import com.cossacklabs.themis.NullArgumentException
import com.cossacklabs.themis.SecureCell
import com.cossacklabs.themis.SecureCellException
import java.nio.charset.StandardCharsets

class MainActivitySecureCell : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        // Secure cell
        try {
            encryptDataForStoring()
        } catch (e: InvalidArgumentException) {
            e.printStackTrace()
        } catch (e: NullArgumentException) {
            e.printStackTrace()
        } catch (e: SecureCellException) {
            e.printStackTrace()
        }
    }

    @Throws(SecureCellException::class, NullArgumentException::class, InvalidArgumentException::class)
    fun encryptDataForStoring() {
        val charset = StandardCharsets.UTF_8
        val pass = "pass"
        val message = "hello message"
        val sc = SecureCell.SealWithPassphrase(pass, charset)
        val protectedData = sc.encrypt(message.toByteArray(charset))
        val encodedString = Base64.encodeToString(protectedData, Base64.NO_WRAP)
        Log.d("SMC", "encrypted string = $encodedString")
        val decodedString = Base64.decode(encodedString, Base64.NO_WRAP)
        val unprotected = sc.decrypt(decodedString)
        val decryptedData = String(unprotected, charset)
        Log.d("SMC", "decrypted data = $decryptedData")
    }
}
