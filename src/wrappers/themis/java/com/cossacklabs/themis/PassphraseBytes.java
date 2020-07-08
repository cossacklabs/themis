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

import org.jetbrains.annotations.NotNull;

import java.nio.ByteBuffer;
import java.nio.CharBuffer;
import java.nio.charset.CharacterCodingException;
import java.nio.charset.Charset;
import java.nio.charset.CharsetEncoder;
import java.nio.charset.CodingErrorAction;

class PassphraseBytes extends KeyBytes {

    PassphraseBytes(@NotNull byte[] encodedPassphrase) {
        super(encodedPassphrase);
    }

    PassphraseBytes(@NotNull String passphrase, @NotNull Charset charset) {
        super(encodePassphrase(passphrase, charset));
    }

    @NotNull
    private static byte[] encodePassphrase(String passphrase, Charset charset) {
        if (passphrase == null) {
            throw new NullArgumentException("passphrase cannot be null");
        }
        if (charset == null) {
            throw new NullArgumentException("charset cannot be null");
        }
        // Use strict encoding. Report errors instead of doing silly conversions
        // that String#getBytes(Charset) does by default.
        final CharsetEncoder encoder = charset.newEncoder()
                .onMalformedInput(CodingErrorAction.REPORT)
                .onUnmappableCharacter(CodingErrorAction.REPORT);
        try {
            CharBuffer inputBuffer = CharBuffer.wrap(passphrase);
            ByteBuffer outputBuffer = encoder.encode(inputBuffer);
            // Avoid allocations if the backing storage array is okay.
            if (outputBuffer.hasArray() && outputBuffer.arrayOffset() == 0) {
                byte[] backingStorage = outputBuffer.array();
                if (backingStorage.length == outputBuffer.remaining()) {
                    return backingStorage;
                }
            }
            byte[] result = new byte[outputBuffer.remaining()];
            outputBuffer.get(result);
            return result;
        }
        catch (CharacterCodingException e) {
            throw new RuntimeException("failed to encode passphrase in " + charset, e);
        }
    }
}
