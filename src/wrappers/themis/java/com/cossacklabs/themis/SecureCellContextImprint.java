package com.cossacklabs.themis;

import org.jetbrains.annotations.Contract;
import org.jetbrains.annotations.NotNull;

class SecureCellContextImprint implements SecureCell.ContextImprint {

    @NotNull
    private final SymmetricKey key;

    @Contract(pure = true)
    SecureCellContextImprint(@NotNull SymmetricKey key) {
        this.key = key;
    }

    @NotNull
    @Override
    public byte[] encrypt(byte[] data, byte[] context) {
        if (data == null) {
            throw new NullArgumentException("data cannot be null");
        }
        if (data.length == 0) {
            throw new InvalidArgumentException("data cannot be empty");
        }
        if (context == null) {
            throw new NullArgumentException("context cannot be null");
        }
        if (context.length == 0) {
            throw new InvalidArgumentException("context cannot be empty");
        }
        byte[] keyBytes = this.key.key;
        byte[][] result = SecureCell.encrypt(keyBytes, context, data, SecureCell.MODE_CONTEXT_IMPRINT);
        // TODO(ilammy, 2020-05-05): teach SecureCell#encrypt to throw SecureCellException (T1605)
        if (result == null) {
            throw new RuntimeException(new SecureCellException());
        }
        return result[0];
    }

    @NotNull
    @Override
    public byte[] decrypt(byte[] data, byte[] context) {
        if (data == null) {
            throw new NullArgumentException("data cannot be null");
        }
        if (data.length == 0) {
            throw new InvalidArgumentException("data cannot be empty");
        }
        if (context == null) {
            throw new NullArgumentException("context cannot be null");
        }
        if (context.length == 0) {
            throw new InvalidArgumentException("context cannot be empty");
        }
        byte[] keyBytes = this.key.key;
        byte[][] encrypted = {data, null};
        byte[] result = SecureCell.decrypt(keyBytes, context, encrypted, SecureCell.MODE_CONTEXT_IMPRINT);
        // TODO(ilammy, 2020-05-05): teach SecureCell#decrypt to throw SecureCellException (T1605)
        if (result == null) {
            throw new RuntimeException(new SecureCellException());
        }
        return result;
    }
}
