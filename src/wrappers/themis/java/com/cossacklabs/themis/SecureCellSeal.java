package com.cossacklabs.themis;

import org.jetbrains.annotations.Contract;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

class SecureCellSeal implements SecureCell.Seal {

    @NotNull
    private final SymmetricKey key;

    @Contract(pure = true)
    SecureCellSeal(@NotNull SymmetricKey key) {
        this.key = key;
    }

    @NotNull
    @Override
    public byte[] encrypt(byte[] data, @Nullable byte[] context) {
        if (data == null) {
            throw new NullArgumentException("data cannot be null");
        }
        if (data.length == 0) {
            throw new InvalidArgumentException("data cannot be empty");
        }
        byte[] keyBytes = this.key.key;
        byte[][] result = SecureCell.encrypt(keyBytes, context, data, SecureCell.MODE_SEAL);
        // TODO(ilammy, 2020-05-05): teach SecureCell#encrypt to throw SecureCellException (T1605)
        if (result == null) {
            throw new RuntimeException(new SecureCellException());
        }
        return result[0];
    }

    @NotNull
    @Override
    public byte[] encrypt(byte[] data) {
        return encrypt(data, null);
    }

    @NotNull
    @Override
    public byte[] decrypt(byte[] data, @Nullable byte[] context) throws SecureCellException {
        if (data == null) {
            throw new NullArgumentException("data cannot be null");
        }
        if (data.length == 0) {
            throw new InvalidArgumentException("data cannot be empty");
        }
        byte[] keyBytes = this.key.key;
        byte[][] encrypted = {data, null};
        byte[] result = SecureCell.decrypt(keyBytes, context, encrypted, SecureCell.MODE_SEAL);
        // TODO(ilammy, 2020-05-05): teach SecureCell#decrypt to throw SecureCellException (T1605)
        if (result == null) {
            throw new SecureCellException();
        }
        return result;
    }

    @NotNull
    @Override
    public byte[] decrypt(byte[] data) throws SecureCellException {
        return decrypt(data, null);
    }
}
