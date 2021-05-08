package com.cossacklabs.themis.android.example

import android.util.Log
import java.io.*
import java.net.HttpURLConnection
import java.net.URL
import java.net.URLEncoder
import java.nio.charset.Charset
import java.nio.charset.StandardCharsets
import java.util.concurrent.ExecutorService
import java.util.concurrent.Future
import javax.net.ssl.HttpsURLConnection

internal class HttpClient(private val executorService: ExecutorService) {
    fun sendMessage(endpoint: String?, message: String?, messageCharset: Charset): ByteArray {
        val query = "message=" + URLEncoder.encode(message, messageCharset.name())
        val url = URL(endpoint)
        val connection = url.openConnection() as HttpsURLConnection
        connection.requestMethod = "POST"
        connection.setRequestProperty("Content-length", query.length.toString())
        connection.setRequestProperty("Content-Type", "application/x-www-form-urlencoded")
        connection.doOutput = true
        connection.doInput = true
        val outputStream: OutputStream = BufferedOutputStream(connection.outputStream)
        val writer = BufferedWriter(OutputStreamWriter(outputStream, StandardCharsets.UTF_8))
        writer.write(query)
        writer.flush()
        writer.close()
        outputStream.close()
        connection.connect()
        Log.d("SMC", "getResponseMessage = " + connection.responseMessage)
        val responseCode = connection.responseCode
        if (responseCode >= HttpURLConnection.HTTP_BAD_REQUEST) {
            throw HttpException(responseCode, String(readResponse(connection.errorStream)))
        }
        return readResponse(connection.inputStream)
    }

    fun sendMessageAsync(endpoint: String?, message: String?, messageCharset: Charset, httpCallback: HttpCallback): Future<*> {
        return executorService.submit {
            try {
                val response = sendMessage(endpoint, message, messageCharset)
                httpCallback.onSuccess(response)
            } catch (e: Exception) {
                httpCallback.onFail(e)
            }
        }
    }

    private fun readResponse(inputStream: InputStream): ByteArray {
        val os = ByteArrayOutputStream()
        val buffer = ByteArray(1024)
        var readDataLen: Int
        while (inputStream.read(buffer).also { readDataLen = it } != -1) {
            os.write(buffer, 0, readDataLen)
        }
        return os.toByteArray()
    }

    class HttpException(code: Int, response: String?) : Exception(String.format("Request failed with code %s (%s)", code, response))

}
