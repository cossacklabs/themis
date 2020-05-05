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
import java.util.Base64;

import com.cossacklabs.themis.InvalidArgumentException;
import com.cossacklabs.themis.NullArgumentException;
import com.cossacklabs.themis.SecureCell;
import com.cossacklabs.themis.SecureCellData;
import com.cossacklabs.themis.SecureCellException;
import com.cossacklabs.themis.SymmetricKey;

import static org.junit.Assert.*;
import org.junit.Test;

public class SecureCellSealTest {

    @Test
    public void initWithGenerated() {
        SecureCell.Seal cell = SecureCell.SealWithKey(new SymmetricKey());

        assertNotNull(cell);
    }

    @Test
    public void initWithFixed() {
        String keyBase64 = "UkVDMgAAAC13PCVZAKOczZXUpvkhsC+xvwWnv3CLmlG0Wzy8ZBMnT+2yx/dg";
        byte[] keyBytes = Base64.getDecoder().decode(keyBase64);

        SecureCell.Seal cell = SecureCell.SealWithKey(keyBytes);

        assertNotNull(cell);
    }

    @Test
    @SuppressWarnings("ResultOfMethodCallIgnored")
    public void initWithEmpty() {
        assertThrows(NullArgumentException.class, () -> SecureCell.SealWithKey((SymmetricKey)null));
        assertThrows(NullArgumentException.class, () -> SecureCell.SealWithKey((byte[])null));
        assertThrows(InvalidArgumentException.class, () -> SecureCell.SealWithKey(new byte[]{}));
    }

    @Test
    public void roundtrip() throws SecureCellException {
        SecureCell.Seal cell = SecureCell.SealWithKey(new SymmetricKey());
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);
        byte[] context = "For great justice".getBytes(StandardCharsets.UTF_8);

        byte[] encrypted = cell.encrypt(message, context);
        assertNotNull(encrypted);

        byte[] decrypted = cell.decrypt(encrypted, context);
        assertNotNull(decrypted);

        assertArrayEquals(message, decrypted);
    }

    @Test
    public void dataLengthExtension() {
        SecureCell.Seal cell = SecureCell.SealWithKey(new SymmetricKey());
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);

        byte[] encrypted = cell.encrypt(message);

        assertTrue(encrypted.length > message.length);
    }

    @Test
    public void contextInclusion() {
        SecureCell.Seal cell = SecureCell.SealWithKey(new SymmetricKey());
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);
        byte[] shortContext = ".".getBytes(StandardCharsets.UTF_8);
        byte[] longContext = "You have no chance to survive make your time. Ha ha ha ha ...".getBytes(StandardCharsets.UTF_8);

        byte[] encryptedShort = cell.encrypt(message, shortContext);
        byte[] encryptedLong = cell.encrypt(message, longContext);

        // Context is not (directly) included into encrypted message.
        assertEquals(encryptedShort.length, encryptedLong.length);
    }

    @Test
    public void withoutContext() throws SecureCellException {
        SecureCell.Seal cell = SecureCell.SealWithKey(new SymmetricKey());
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);

        // Absent, empty, or nil context are all the same.
        byte[] encrypted1 = cell.encrypt(message);
        byte[] encrypted2 = cell.encrypt(message, null);
        byte[] encrypted3 = cell.encrypt(message, new byte[]{});

        assertArrayEquals(message, cell.decrypt(encrypted1));
        assertArrayEquals(message, cell.decrypt(encrypted2));
        assertArrayEquals(message, cell.decrypt(encrypted3));

        assertArrayEquals(message, cell.decrypt(encrypted1, null));
        assertArrayEquals(message, cell.decrypt(encrypted2, null));
        assertArrayEquals(message, cell.decrypt(encrypted3, null));

        assertArrayEquals(message, cell.decrypt(encrypted1, new byte[]{}));
        assertArrayEquals(message, cell.decrypt(encrypted2, new byte[]{}));
        assertArrayEquals(message, cell.decrypt(encrypted3, new byte[]{}));
    }

    @Test
    @SuppressWarnings("ResultOfMethodCallIgnored")
    public void contextSignificance() throws SecureCellException {
        SecureCell.Seal cell = SecureCell.SealWithKey(new SymmetricKey());
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);
        byte[] correctContext = "We are CATS".getBytes(StandardCharsets.UTF_8);
        byte[] incorrectContext = "Captain !!".getBytes(StandardCharsets.UTF_8);

        byte[] encrypted = cell.encrypt(message, correctContext);

        // You cannot use a different context to decrypt data.
        assertThrows(SecureCellException.class, () -> cell.decrypt(encrypted, incorrectContext));

        // Only the original context will work.
        byte[] decrypted = cell.decrypt(encrypted, correctContext);

        assertArrayEquals(message, decrypted);
    }

    @Test
    @SuppressWarnings("ResultOfMethodCallIgnored")
    public void detectCorruptedData() {
        SecureCell.Seal cell = SecureCell.SealWithKey(new SymmetricKey());
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);

        byte[] encrypted = cell.encrypt(message);

        // Invert every odd byte, this will surely break the message.
        byte[] corrupted = Arrays.copyOf(encrypted, encrypted.length);
        for (int i = 0; i < corrupted.length; i++) {
            if (i % 2 == 1) {
                corrupted[i] = (byte)~corrupted[i];
            }
        }

        assertThrows(SecureCellException.class, () -> cell.decrypt(corrupted));
    }

    @Test
    @SuppressWarnings("ResultOfMethodCallIgnored")
    public void detectTruncatedData() {
        SecureCell.Seal cell = SecureCell.SealWithKey(new SymmetricKey());
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);

        byte[] encrypted = cell.encrypt(message);

        byte[] truncated = Arrays.copyOf(encrypted, encrypted.length - 1);

        assertThrows(SecureCellException.class, () -> cell.decrypt(truncated));
    }

    @Test
    @SuppressWarnings("ResultOfMethodCallIgnored")
    public void detectExtendedData() {
        SecureCell.Seal cell = SecureCell.SealWithKey(new SymmetricKey());
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);

        byte[] encrypted = cell.encrypt(message);

        byte[] extended = Arrays.copyOf(encrypted, encrypted.length + 1);

        assertThrows(SecureCellException.class, () -> cell.decrypt(extended));
    }

    @Test
    @SuppressWarnings("ResultOfMethodCallIgnored")
    public void emptyMessage() {
        SecureCell.Seal cell = SecureCell.SealWithKey(new SymmetricKey());

        assertThrows(NullArgumentException.class, () -> cell.encrypt(null));
        assertThrows(NullArgumentException.class, () -> cell.decrypt(null));
        assertThrows(InvalidArgumentException.class, () -> cell.encrypt(new byte[]{}));
        assertThrows(InvalidArgumentException.class, () -> cell.decrypt(new byte[]{}));
    }

    @Test
    public void oldAPI() throws SecureCellException {
        SymmetricKey key = new SymmetricKey();
        SecureCell.Seal newCell = SecureCell.SealWithKey(key);
        SecureCell oldCell = new SecureCell(key.toByteArray(), SecureCell.MODE_SEAL);
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

    @Test
    public void oldAPIWithoutContext() throws SecureCellException {
        SymmetricKey key = new SymmetricKey();
        SecureCell.Seal newCell = SecureCell.SealWithKey(key);
        SecureCell oldCell = new SecureCell(key.toByteArray(), SecureCell.MODE_SEAL);
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);
        byte[] encrypted, decrypted;

        SecureCellData result = oldCell.protect((byte[])null, message);
        encrypted = result.getProtectedData();
        assertNotNull(encrypted);

        decrypted = newCell.decrypt(encrypted);
        assertArrayEquals(message, decrypted);

        encrypted = newCell.encrypt(message);
        assertNotNull(encrypted);

        decrypted = oldCell.unprotect((byte[])null, new SecureCellData(encrypted, null));
        assertArrayEquals(message, decrypted);
    }
}
