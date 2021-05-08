package com.cossacklabs.themis.android.example;

public interface HttpCallback {

    void onSuccess(byte[] response);

    void onFail(Exception ex);
}
