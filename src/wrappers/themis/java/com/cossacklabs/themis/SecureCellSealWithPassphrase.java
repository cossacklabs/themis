/*
 * Copyright (c) 2020 Cossack Labs Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.cossacklabs.themis;

import org.jetbrains.annotations.Contract;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

import java.nio.charset.Charset;

class SecureCellSealWithPassphrase implements SecureCell.Seal {

    @NotNull
    private final PassphraseBytes passphrase;

    @Contract(pure = true)
    SecureCellSealWithPassphrase(@NotNull String passphrase, @NotNull Charset charset) {
        this.passphrase = new PassphraseBytes(passphrase, charset);
    }

    @Contract(pure = true)
    SecureCellSealWithPassphrase(@NotNull byte[] encodedPassphrase) {
        this.passphrase = new PassphraseBytes(encodedPassphrase);
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
        byte[] passphraseBytes = this.passphrase.key;
        byte[][] result = SecureCell.encrypt(passphraseBytes, context, data, SecureCell.MODE_SEAL_PASSPHRASE);
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
        byte[] passphraseBytes = this.passphrase.key;
        byte[][] encrypted = {data, null};
        byte[] result = SecureCell.decrypt(passphraseBytes, context, encrypted, SecureCell.MODE_SEAL_PASSPHRASE);
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
