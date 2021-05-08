package com.cossacklabs.themis.android.example;

import android.util.Log;

import java.io.BufferedOutputStream;
import java.io.BufferedWriter;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.URLEncoder;
import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Future;

import javax.net.ssl.HttpsURLConnection;

class HttpClient {

    private final ExecutorService executorService;

    HttpClient(ExecutorService executorService) {
        this.executorService = executorService;
    }

    @SuppressWarnings("WeakerAccess")
    byte[] sendMessage(String endpoint, String message, Charset messageCharset) throws IOException, HttpException {
        String query = "message=" + URLEncoder.encode(message, messageCharset.name());

        java.net.URL url = new URL(endpoint);

        HttpsURLConnection connection = (HttpsURLConnection) url.openConnection();
        connection.setRequestMethod("POST");
        connection.setRequestProperty("Content-length", String.valueOf(query.length()));
        connection.setRequestProperty("Content-Type", "application/x-www-form-urlencoded");
        connection.setDoOutput(true);
        connection.setDoInput(true);

        OutputStream outputStream = new BufferedOutputStream(connection.getOutputStream());
        BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(outputStream, StandardCharsets.UTF_8));
        writer.write(query);
        writer.flush();
        writer.close();
        outputStream.close();

        connection.connect();

        Log.d("SMC", "getResponseMessage = " + connection.getResponseMessage());
        final int responseCode = connection.getResponseCode();
        if (responseCode >= HttpURLConnection.HTTP_BAD_REQUEST) {
            throw new HttpException(responseCode, new String(readResponse(connection.getErrorStream())));
        }
        return readResponse(connection.getInputStream());
    }

    @SuppressWarnings("UnusedReturnValue")
    Future<?> sendMessageAsync(final String endpoint, final String message, final Charset messageCharset, final HttpCallback httpCallback) {
        return executorService.submit(new Runnable() {
            @Override
            public void run() {
                try {
                    final byte[] response = sendMessage(endpoint, message, messageCharset);
                    httpCallback.onSuccess(response);
                } catch (Exception e) {
                    httpCallback.onFail(e);
                }
            }
        });
    }

    private byte[] readResponse(InputStream inputStream) throws IOException {
        final ByteArrayOutputStream os = new ByteArrayOutputStream();
        final byte[] buffer = new byte[1024];
        int readDataLen;
        while ((readDataLen = inputStream.read(buffer)) != -1) {
            os.write(buffer, 0, readDataLen);
        }
        return os.toByteArray();
    }

    @SuppressWarnings("WeakerAccess")
    public static class HttpException extends Exception {

        public final int code;
        public final String response;

        public HttpException(int code, String response) {
            super(String.format("Request failed with code %s (%s)", code, response));
            this.code = code;
            this.response = response;
        }
    }
}
