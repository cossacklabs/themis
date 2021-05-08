package com.cossacklabs.themis.android.example;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Base64;
import android.util.Log;

import com.cossacklabs.themis.InvalidArgumentException;
import com.cossacklabs.themis.NullArgumentException;
import com.cossacklabs.themis.SecureCell;
import com.cossacklabs.themis.SecureCellException;

import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;

public class MainActivitySecureCell extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Secure cell
        try {
            encryptDataForStoring();

        } catch (InvalidArgumentException | NullArgumentException | SecureCellException e) {
            e.printStackTrace();
        }
    }

    void encryptDataForStoring() throws SecureCellException, NullArgumentException, InvalidArgumentException {
        Charset charset = StandardCharsets.UTF_8;
        String pass = "pass";
        String message = "hello message";

        SecureCell.Seal sc = SecureCell.SealWithPassphrase(pass, charset);

        byte[] protectedData = sc.encrypt(message.getBytes(charset));
        String encodedString = Base64.encodeToString(protectedData, Base64.NO_WRAP);
        Log.d("SMC", "encrypted string = " + encodedString);

        byte[] decodedString = Base64.decode(encodedString, Base64.NO_WRAP);

        byte[] unprotected = sc.decrypt(decodedString);
        String decryptedData = new String(unprotected, charset);
        Log.d("SMC", "decrypted data = " + decryptedData);
    }
}
