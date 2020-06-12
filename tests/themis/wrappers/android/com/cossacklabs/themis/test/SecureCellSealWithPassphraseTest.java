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

import java.nio.charset.CharacterCodingException;
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

public class SecureCellSealWithPassphraseTest {

    @Test
    public void initWithString() {
        SecureCell.Seal cell = SecureCell.SealWithPassphrase("day 56 of the Q");

        assertNotNull(cell);
    }

    @Test
    public void initWithBytes() {
        byte[] encoded = "day 56 of the Q".getBytes(StandardCharsets.US_ASCII);
        SecureCell.Seal cell = SecureCell.SealWithPassphrase(encoded);

        assertNotNull(cell);
    }

    @Test
    @SuppressWarnings("ResultOfMethodCallIgnored")
    public void initWithEmpty() {
        try {
            SecureCell.SealWithPassphrase((String)null);
            fail("expected NullArgumentException");
        }
        catch (NullArgumentException ignored) {}
        try {
            SecureCell.SealWithPassphrase((byte[])null);
            fail("expected NullArgumentException");
        }
        catch (NullArgumentException ignored) {}
        try {
            SecureCell.SealWithPassphrase(null, StandardCharsets.UTF_8);
            fail("expected NullArgumentException");
        }
        catch (NullArgumentException ignored) {}
        try {
            SecureCell.SealWithPassphrase("day 56 of the Q", null);
            fail("expected NullArgumentException");
        }
        catch (NullArgumentException ignored) {}
        try {
            SecureCell.SealWithPassphrase(new byte[]{});
            fail("expected InvalidArgumentException");
        }
        catch (InvalidArgumentException ignored) {}
        try {
            SecureCell.SealWithPassphrase("");
            fail("expected InvalidArgumentException");
        }
        catch (InvalidArgumentException ignored) {}
        try {
            SecureCell.SealWithPassphrase("", StandardCharsets.UTF_16BE);
            fail("expected InvalidArgumentException");
        }
        catch (InvalidArgumentException ignored) {}
    }

    @Test
    public void roundtrip() throws SecureCellException {
        SecureCell.Seal cell = SecureCell.SealWithPassphrase("day 56 of the Q");
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
        SecureCell.Seal cell = SecureCell.SealWithPassphrase("day 56 of the Q");
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);

        byte[] encrypted = cell.encrypt(message);

        assertTrue(encrypted.length > message.length);
    }

    @Test
    public void contextInclusion() {
        SecureCell.Seal cell = SecureCell.SealWithPassphrase("day 56 of the Q");
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
        SecureCell.Seal cell = SecureCell.SealWithPassphrase("day 56 of the Q");
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
        SecureCell.Seal cell = SecureCell.SealWithPassphrase("day 56 of the Q");
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);
        byte[] correctContext = "We are CATS".getBytes(StandardCharsets.UTF_8);
        byte[] incorrectContext = "Captain !!".getBytes(StandardCharsets.UTF_8);

        byte[] encrypted = cell.encrypt(message, correctContext);

        // You cannot use a different context to decrypt data.
        try {
            cell.decrypt(encrypted, incorrectContext);
            fail("expected SecureCellException");
        }
        catch (SecureCellException ignored) {}

        // Only the original context will work.
        byte[] decrypted = cell.decrypt(encrypted, correctContext);

        assertArrayEquals(message, decrypted);
    }

    @Test
    @SuppressWarnings("ResultOfMethodCallIgnored")
    public void detectCorruptedData() {
        SecureCell.Seal cell = SecureCell.SealWithPassphrase("day 56 of the Q");
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);

        byte[] encrypted = cell.encrypt(message);

        // Invert every odd byte, this will surely break the message.
        byte[] corrupted = Arrays.copyOf(encrypted, encrypted.length);
        for (int i = 0; i < corrupted.length; i++) {
            if (i % 2 == 1) {
                corrupted[i] = (byte)~corrupted[i];
            }
        }

        try {
            cell.decrypt(corrupted);
            fail("expected SecureCellException");
        }
        catch (SecureCellException ignored) {}
    }

    @Test
    @SuppressWarnings("ResultOfMethodCallIgnored")
    public void detectTruncatedData() {
        SecureCell.Seal cell = SecureCell.SealWithPassphrase("day 56 of the Q");
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);

        byte[] encrypted = cell.encrypt(message);

        byte[] truncated = Arrays.copyOf(encrypted, encrypted.length - 1);

        try {
            cell.decrypt(truncated);
            fail("expected SecureCellException");
        }
        catch (SecureCellException ignored) {}

    }

    @Test
    @SuppressWarnings("ResultOfMethodCallIgnored")
    public void detectExtendedData() {
        SecureCell.Seal cell = SecureCell.SealWithPassphrase("day 56 of the Q");
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);

        byte[] encrypted = cell.encrypt(message);

        byte[] extended = Arrays.copyOf(encrypted, encrypted.length + 1);

        try {
            cell.decrypt(extended);
            fail("expected SecureCellException");
        }
        catch (SecureCellException ignored) {}
    }

    @Test
    @SuppressWarnings("ResultOfMethodCallIgnored")
    public void emptyMessage() throws SecureCellException {
        SecureCell.Seal cell = SecureCell.SealWithPassphrase("day 56 of the Q");

        try {
            cell.encrypt(null);
            fail("expected NullArgumentException");
        }
        catch (NullArgumentException ignored) {}
        try {
            cell.decrypt(null);
            fail("expected NullArgumentException");
        }
        catch (NullArgumentException ignored) {}
        try {
            cell.encrypt(new byte[]{});
            fail("expected InvalidArgumentException");
        }
        catch (InvalidArgumentException ignored) {}
        try {
            cell.decrypt(new byte[]{});
            fail("expected InvalidArgumentException");
        }
        catch (InvalidArgumentException ignored) {}
    }

    @Test
    public void defaultEncoding() throws SecureCellException {
        // Passphrases are encoded in UTF-8 by default.
        String passphrase = "\u6697\u53F7";
        SecureCell.Seal cellA = SecureCell.SealWithPassphrase(passphrase);
        SecureCell.Seal cellB = SecureCell.SealWithPassphrase(passphrase.getBytes(StandardCharsets.UTF_8));
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);

        byte[] encrypted = cellA.encrypt(message);
        byte[] decrypted = cellB.decrypt(encrypted);

        assertArrayEquals(message, decrypted);
    }

    @Test
    public void specificEncoding() throws SecureCellException {
        SecureCell.Seal cell = SecureCell.SealWithPassphrase("secret", StandardCharsets.UTF_16BE);
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);

        // Message encrypted by PyThemis
        byte[] encrypted = Base64.getDecoder().decode("AAEBQQwAAAAQAAAAHwAAABYAAAAZzELB2A0q/vzRB0FVP664QBPjKA/AER9v5oxAQA0DABAAuidXTI+22ukcDOfifuHCtP7PGu73GI7Ga7hlZfUrczMYrg9/LdV22vdRwfYbdIU=");
        byte[] decrypted = cell.decrypt(encrypted);

        assertArrayEquals(message, decrypted);
    }

    @Test
    @SuppressWarnings("ResultOfMethodCallIgnored")
    public void passphraseIsNotSymmetricKey() {
        // Passphrases are not keys. Keys are not passphrases.
        SymmetricKey secret = new SymmetricKey();
        SecureCell.Seal cellMK = SecureCell.SealWithKey(secret);
        SecureCell.Seal cellPW = SecureCell.SealWithPassphrase(secret.toByteArray());
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);

        byte[] encryptedMK = cellMK.encrypt(message);
        byte[] encryptedPW = cellPW.encrypt(message);

        try {
            cellPW.decrypt(encryptedMK);
            fail("expected SecureCellException");
        }
        catch (SecureCellException ignored) {}
        try {
            cellMK.decrypt(encryptedPW);
            fail("expected SecureCellException");
        }
        catch (SecureCellException ignored) {}
    }

    @Test
    @SuppressWarnings({"ResultOfMethodCallIgnored", "deprecation"})
    public void passphrasesNotCompatibleWithOldAPI() throws SecureCellException {
        // Old 'passphrase-like' API is not passphrase API at all. Don't use it.
        SecureCell cellOld = new SecureCell("day 56 of the Q", SecureCell.MODE_SEAL);
        SecureCell.Seal cellNew = SecureCell.SealWithPassphrase("day 56 of the Q", StandardCharsets.UTF_16);
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);

        byte[] encryptedOld = cellOld.protect("", message).getProtectedData();
        byte[] encryptedNew = cellNew.encrypt(message);

        try {
            cellNew.decrypt(encryptedOld);
            fail("expected SecureCellException");
        }
        catch (SecureCellException ignored) {}
        try {
            cellOld.unprotect("", new SecureCellData(encryptedNew, null));
            fail("expected SecureCellException");
        }
        catch (SecureCellException ignored) {}
    }

    @Test
    @SuppressWarnings("deprecation")
    public void oldPassphraseAPIIsActuallyUTF16Key() throws SecureCellException {
        // Yes, it's so weird due to hysterical raisins. So don't use it, really.
        SecureCell cellOld = new SecureCell("day 56 of the Q", SecureCell.MODE_SEAL);
        SecureCell.Seal cellNew = SecureCell.SealWithKey("day 56 of the Q".getBytes(StandardCharsets.UTF_16));
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);

        byte[] encryptedOld = cellOld.protect("", message).getProtectedData();
        byte[] encryptedNew = cellNew.encrypt(message);

        byte[] decryptedOld = cellNew.decrypt(encryptedOld);
        byte[] decryptedNew = cellOld.unprotect("", new SecureCellData(encryptedNew, null));

        assertArrayEquals(message, decryptedOld);
        assertArrayEquals(message, decryptedNew);
    }

    @Test
    @SuppressWarnings("ResultOfMethodCallIgnored")
    public void passphraseEncodingFailure() {
        try {
            // It is an error if the passphrase cannot be represented in the requested charset
            // without data loss.
            SecureCell.SealWithPassphrase("\u043F\u0430\u0440\u043E\u043B\u044C", StandardCharsets.US_ASCII);
            fail("expected RuntimeException(CharacterCodingException)");
        }
        catch (RuntimeException e) {
            assertTrue(e.getCause() instanceof CharacterCodingException);
        }
    }
}
