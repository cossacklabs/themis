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

package com.cossacklabs.themis.test;

import java.nio.charset.StandardCharsets;
import java.util.Arrays;

import com.cossacklabs.themis.InvalidArgumentException;
import com.cossacklabs.themis.NullArgumentException;
import com.cossacklabs.themis.SecureCell;
import com.cossacklabs.themis.SecureCellData;
import com.cossacklabs.themis.SecureCellException;
import com.cossacklabs.themis.SymmetricKey;

import static org.junit.Assert.*;
import org.junit.Test;

public class SecureCellContextImprintTest {

    @Test
    public void initWithGenerated() {
        SecureCell.ContextImprint cell = SecureCell.ContextImprintWithKey(new SymmetricKey());

        assertNotNull(cell);
    }

    @Test
    public void initWithFixed() {
        String keyBase64 = "UkVDMgAAAC13PCVZAKOczZXUpvkhsC+xvwWnv3CLmlG0Wzy8ZBMnT+2yx/dg";
        byte[] keyBytes = Base64.getDecoder().decode(keyBase64);

        SecureCell.ContextImprint cell = SecureCell.ContextImprintWithKey(keyBytes);

        assertNotNull(cell);
    }

    @Test
    @SuppressWarnings("ResultOfMethodCallIgnored")
    public void initWithEmpty() {
        try {
            SecureCell.ContextImprintWithKey((SymmetricKey)null);
            fail("expected NullArgumentException");
        }
        catch (NullArgumentException ignored) {}
        try {
            SecureCell.ContextImprintWithKey((byte[])null);
            fail("expected NullArgumentException");
        }
        catch (NullArgumentException ignored) {}
        try {
            SecureCell.ContextImprintWithKey(new byte[]{});
            fail("expected InvalidArgumentException");
        }
        catch (InvalidArgumentException ignored) {}
    }

    @Test
    public void roundtrip() {
        SecureCell.ContextImprint cell = SecureCell.ContextImprintWithKey(new SymmetricKey());
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);
        byte[] context = "For great justice".getBytes(StandardCharsets.UTF_8);

        byte[] encrypted = cell.encrypt(message, context);
        assertNotNull(encrypted);

        byte[] decrypted = cell.decrypt(encrypted, context);
        assertNotNull(decrypted);

        assertArrayEquals(message, decrypted);
    }

    @Test
    public void dataLengthPreservation() {
        SecureCell.ContextImprint cell = SecureCell.ContextImprintWithKey(new SymmetricKey());
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);
        byte[] context = "For great justice".getBytes(StandardCharsets.UTF_8);

        byte[] encrypted = cell.encrypt(message, context);

        assertEquals(message.length, encrypted.length);
    }

    @Test
    public void contextInclusion() {
        SecureCell.ContextImprint cell = SecureCell.ContextImprintWithKey(new SymmetricKey());
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);
        byte[] shortContext = ".".getBytes(StandardCharsets.UTF_8);
        byte[] longContext = "You have no chance to survive make your time. Ha ha ha ha ...".getBytes(StandardCharsets.UTF_8);

        byte[] encryptedShort = cell.encrypt(message, shortContext);
        byte[] encryptedLong = cell.encrypt(message, longContext);

        // Context is not (directly) included into encrypted message.
        assertEquals(encryptedShort.length, encryptedLong.length);
    }

    @Test
    public void contextSignificance() {
        SecureCell.ContextImprint cell = SecureCell.ContextImprintWithKey(new SymmetricKey());
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);
        byte[] correctContext = "We are CATS".getBytes(StandardCharsets.UTF_8);
        byte[] incorrectContext = "Captain !!".getBytes(StandardCharsets.UTF_8);

        byte[] encrypted = cell.encrypt(message, correctContext);

        // You can use a different context to decrypt data, but you'll get garbage.
        byte[] decrypted = cell.decrypt(encrypted, incorrectContext);
        assertNotNull(decrypted);
        assertEquals(message.length, decrypted.length);
        assertFalse(Arrays.equals(message, decrypted));

        // Only the original context will work.
        decrypted = cell.decrypt(encrypted, correctContext);
        assertArrayEquals(message, decrypted);
    }

    @Test
    public void noDetectCorruptedData() {
        SecureCell.ContextImprint cell = SecureCell.ContextImprintWithKey(new SymmetricKey());
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);
        byte[] context = "We are CATS".getBytes(StandardCharsets.UTF_8);

        byte[] encrypted = cell.encrypt(message, context);

        // Invert every odd byte, this will surely break the message.
        byte[] corrupted = Arrays.copyOf(encrypted, encrypted.length);
        for (int i = 0; i < corrupted.length; i++) {
            if (i % 2 == 1) {
                corrupted[i] = (byte)~corrupted[i];
            }
        }

        // Decrypts successfully but the content is garbage.
        byte[] decrypted = cell.decrypt(corrupted, context);
        assertNotNull(decrypted);
        assertEquals(message.length, decrypted.length);
        assertFalse(Arrays.equals(message, decrypted));
    }

    @Test
    public void noDetectTruncatedData() {
        SecureCell.ContextImprint cell = SecureCell.ContextImprintWithKey(new SymmetricKey());
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);
        byte[] context = "We are CATS".getBytes(StandardCharsets.UTF_8);

        byte[] encrypted = cell.encrypt(message, context);

        byte[] truncated = Arrays.copyOf(encrypted, encrypted.length - 1);

        // Decrypts successfully but the content is garbage.
        byte[] decrypted = cell.decrypt(truncated, context);
        assertNotNull(decrypted);
        assertEquals(truncated.length, decrypted.length);
        assertFalse(Arrays.equals(message, decrypted));
    }

    @Test
    public void noDetectExtendedData() {
        SecureCell.ContextImprint cell = SecureCell.ContextImprintWithKey(new SymmetricKey());
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);
        byte[] context = "We are CATS".getBytes(StandardCharsets.UTF_8);

        byte[] encrypted = cell.encrypt(message, context);

        byte[] extended = Arrays.copyOf(encrypted, encrypted.length + 1);

        // Decrypts successfully but the content is garbage.
        byte[] decrypted = cell.decrypt(extended, context);
        assertNotNull(decrypted);
        assertEquals(extended.length, decrypted.length);
        assertFalse(Arrays.equals(message, decrypted));
    }

    @Test
    @SuppressWarnings("ResultOfMethodCallIgnored")
    public void requiredMessageAndContext() {
        SecureCell.ContextImprint cell = SecureCell.ContextImprintWithKey(new SymmetricKey());
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);
        byte[] context = "We are CATS".getBytes(StandardCharsets.UTF_8);

        try {
            cell.encrypt(message, null);
            fail("expected NullArgumentException");
        }
        catch (NullArgumentException ignored) {}
        try {
            cell.decrypt(message, null);
            fail("expected NullArgumentException");
        }
        catch (NullArgumentException ignored) {}
        try {
            cell.encrypt(null, context);
            fail("expected NullArgumentException");
        }
        catch (NullArgumentException ignored) {}
        try {
            cell.decrypt(null, context);
            fail("expected NullArgumentException");
        }
        catch (NullArgumentException ignored) {}

        try {
            cell.encrypt(message, new byte[]{});
            fail("expected InvalidArgumentException");
        }
        catch (InvalidArgumentException ignored) {}
        try {
            cell.decrypt(message, new byte[]{});
            fail("expected InvalidArgumentException");
        }
        catch (InvalidArgumentException ignored) {}
        try {
            cell.encrypt(new byte[]{}, context);
            fail("expected InvalidArgumentException");
        }
        catch (InvalidArgumentException ignored) {}
        try {
            cell.decrypt(new byte[]{}, context);
            fail("expected InvalidArgumentException");
        }
        catch (InvalidArgumentException ignored) {}
    }

    @Test
    @SuppressWarnings("deprecation")
    public void oldAPI() throws SecureCellException {
        SymmetricKey key = new SymmetricKey();
        SecureCell.ContextImprint newCell = SecureCell.ContextImprintWithKey(key);
        SecureCell oldCell = new SecureCell(key.toByteArray(), SecureCell.MODE_CONTEXT_IMPRINT);
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);
        byte[] context = "We are CATS".getBytes(StandardCharsets.UTF_8);
        byte[] encrypted, decrypted;

        SecureCellData result = oldCell.protect(context, message);
        encrypted = result.getProtectedData();
        assertNotNull(encrypted);

        decrypted = newCell.decrypt(encrypted, context);
        assertArrayEquals(message, decrypted);

        encrypted = newCell.encrypt(message, context);
        assertNotNull(encrypted);

        decrypted = oldCell.unprotect(context, new SecureCellData(encrypted, null));
        assertArrayEquals(message, decrypted);
    }
}
