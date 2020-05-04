package com.cossacklabs.themis;

class SecureCellSeal implements SecureCell.Seal {

    private final SymmetricKey key;

    SecureCellSeal(SymmetricKey key) {
        this.key = key;
    }

    @Override
    public byte[] encrypt(byte[] data, byte[] context) {
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

    @Override
    public byte[] encrypt(byte[] data) {
        return encrypt(data, null);
    }

    @Override
    public byte[] decrypt(byte[] data, byte[] context) throws SecureCellException {
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

    @Override
    public byte[] decrypt(byte[] data) throws SecureCellException {
        return decrypt(data, null);
    }
}
