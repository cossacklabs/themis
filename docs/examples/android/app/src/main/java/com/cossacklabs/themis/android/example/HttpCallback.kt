package com.cossacklabs.themis.android.example

interface HttpCallback {
    fun onSuccess(response: ByteArray?)
    fun onFail(ex: Exception?)
}
