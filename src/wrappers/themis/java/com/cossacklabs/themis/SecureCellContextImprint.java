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
        @SuppressWarnings("deprecation")
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
        @SuppressWarnings("deprecation")
        byte[] result = SecureCell.decrypt(keyBytes, context, encrypted, SecureCell.MODE_CONTEXT_IMPRINT);
        // TODO(ilammy, 2020-05-05): teach SecureCell#decrypt to throw SecureCellException (T1605)
        if (result == null) {
            throw new RuntimeException(new SecureCellException());
        }
        return result;
    }
}
