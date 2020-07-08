/*
 * Copyright (c) 2019 Cossack Labs Limited
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

/**
 * Symmetric encryption key.
 *
 * These keys are used with Secure Cell cryptosystem.
 */
public class SymmetricKey extends KeyBytes {

    static {
        System.loadLibrary("themis_jni");
    }

    /**
     * Generates a new symmetric key.
     */
    public SymmetricKey() {
        super(newSymmetricKey());
    }

    /**
     * Creates a symmetric key from byte array.
     *
     * @param key byte array
     *
     * @throws NullArgumentException if `key` is null.
     * @throws InvalidArgumentException if `key` is empty.
     */
    public SymmetricKey(byte[] key) {
        super(key);
    }

    private static native byte[] generateSymmetricKey();

    private static byte[] newSymmetricKey() {
        byte[] key = generateSymmetricKey();
        if (key == null || key.length == 0) {
            throw new KeyGenerationException("failed to generate symmetric key");
        }
        return key;
    }
}
