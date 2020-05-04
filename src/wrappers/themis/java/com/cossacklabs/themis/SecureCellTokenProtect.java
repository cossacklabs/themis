package com.cossacklabs.themis;

import org.jetbrains.annotations.Contract;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

class SecureCellTokenProtect implements SecureCell.TokenProtect {

    @NotNull
    private final SymmetricKey key;

    @Contract(pure = true)
    SecureCellTokenProtect(@NotNull SymmetricKey key) {
        this.key = key;
    }

    @NotNull
    @Override
    public SecureCellData encrypt(byte[] data, @Nullable byte[] context) {
        if (data == null) {
            throw new NullArgumentException("data cannot be null");
        }
        if (data.length == 0) {
            throw new InvalidArgumentException("data cannot be empty");
        }
        byte[] keyBytes = this.key.key;
        byte[][] result = SecureCell.encrypt(keyBytes, context, data, SecureCell.MODE_TOKEN_PROTECT);
        // TODO(ilammy, 2020-05-05): teach SecureCell#encrypt to throw SecureCellException (T1605)
        if (result == null) {
            throw new RuntimeException(new SecureCellException());
        }
        return new SecureCellData(result[0], result[1]);
    }

    @NotNull
    @Override
    public SecureCellData encrypt(byte[] data) {
        return encrypt(data, null);
    }

    @NotNull
    @Override
    public byte[] decrypt(byte[] data, byte[] token, @Nullable byte[] context) throws SecureCellException {
        if (data == null) {
            throw new NullArgumentException("data cannot be null");
        }
        if (data.length == 0) {
            throw new InvalidArgumentException("data cannot be empty");
        }
        if (token == null) {
            throw new NullArgumentException("token cannot be null");
        }
        if (token.length == 0) {
            throw new InvalidArgumentException("token cannot be empty");
        }
        byte[] keyBytes = this.key.key;
        byte[][] encrypted = {data, token};
        byte[] result = SecureCell.decrypt(keyBytes, context, encrypted, SecureCell.MODE_TOKEN_PROTECT);
        // TODO(ilammy, 2020-05-05): teach SecureCell#encrypt to throw SecureCellException (T1605)
        if (result == null) {
            throw new SecureCellException();
        }
        return result;
    }

    @NotNull
    @Override
    public byte[] decrypt(byte[] data, byte[] token) throws SecureCellException {
        return decrypt(data, token, null);
    }
}
