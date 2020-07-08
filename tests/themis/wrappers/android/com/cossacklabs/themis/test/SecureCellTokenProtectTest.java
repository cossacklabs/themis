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

import org.junit.Ignore;
import org.junit.Test;

public class SecureCellTokenProtectTest {

    @Test
    public void initWithGenerated() {
        SecureCell.TokenProtect cell = SecureCell.TokenProtectWithKey(new SymmetricKey());

        assertNotNull(cell);
    }

    @Test
    public void initWithFixed() {
        String keyBase64 = "UkVDMgAAAC13PCVZAKOczZXUpvkhsC+xvwWnv3CLmlG0Wzy8ZBMnT+2yx/dg";
        byte[] keyBytes = Base64.getDecoder().decode(keyBase64);

        SecureCell.TokenProtect cell = SecureCell.TokenProtectWithKey(keyBytes);

        assertNotNull(cell);
    }

    @Test
    @SuppressWarnings("ResultOfMethodCallIgnored")
    public void initWithEmpty() {
        try {
            SecureCell.TokenProtectWithKey((SymmetricKey)null);
            fail("expected NullArgumentException");
        }
        catch (NullArgumentException ignored) {}
        try {
            SecureCell.TokenProtectWithKey((byte[])null);
            fail("expected NullArgumentException");
        }
        catch (NullArgumentException ignored) {}
        try {
            SecureCell.TokenProtectWithKey(new byte[]{});
            fail("expected InvalidArgumentException");
        }
        catch (InvalidArgumentException ignored) {}
    }

    @Test
    public void roundtrip() throws SecureCellException {
        SecureCell.TokenProtect cell = SecureCell.TokenProtectWithKey(new SymmetricKey());
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);
        byte[] context = "For great justice".getBytes(StandardCharsets.UTF_8);

        SecureCellData result = cell.encrypt(message, context);
        assertNotNull(result);
        byte[] encrypted = result.getProtectedData();
        byte[] authToken = result.getAdditionalData();
        assertNotNull(encrypted);
        assertNotNull(authToken);

        byte[] decrypted = cell.decrypt(encrypted, authToken, context);
        assertNotNull(decrypted);

        assertArrayEquals(message, decrypted);
    }

    @Test
    public void dataLengthPreservation() {
        SecureCell.TokenProtect cell = SecureCell.TokenProtectWithKey(new SymmetricKey());
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);

        SecureCellData result = cell.encrypt(message);

        assertEquals(message.length, result.getProtectedData().length);
        assertTrue(result.getAdditionalData().length > 0);
    }

    @Test
    public void contextInclusion() {
        SecureCell.TokenProtect cell = SecureCell.TokenProtectWithKey(new SymmetricKey());
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);
        byte[] shortContext = ".".getBytes(StandardCharsets.UTF_8);
        byte[] longContext = "You have no chance to survive make your time. Ha ha ha ha ...".getBytes(StandardCharsets.UTF_8);

        SecureCellData resultShort = cell.encrypt(message, shortContext);
        SecureCellData resultLong = cell.encrypt(message, longContext);

        // Context is not (directly) included into encrypted message.
        assertEquals(resultShort.getProtectedData().length, resultLong.getProtectedData().length);
        assertEquals(resultShort.getAdditionalData().length, resultLong.getAdditionalData().length);
    }

    @Test
    public void withoutContext() throws SecureCellException {
        SecureCell.TokenProtect cell = SecureCell.TokenProtectWithKey(new SymmetricKey());
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);

        // Absent, empty, or nil context are all the same.
        SecureCellData result1 = cell.encrypt(message);
        SecureCellData result2 = cell.encrypt(message, null);
        SecureCellData result3 = cell.encrypt(message, new byte[]{});

        assertArrayEquals(message, cell.decrypt(result1.getProtectedData(), result1.getAdditionalData()));
        assertArrayEquals(message, cell.decrypt(result2.getProtectedData(), result2.getAdditionalData()));
        assertArrayEquals(message, cell.decrypt(result3.getProtectedData(), result3.getAdditionalData()));

        assertArrayEquals(message, cell.decrypt(result1.getProtectedData(), result1.getAdditionalData(), null));
        assertArrayEquals(message, cell.decrypt(result2.getProtectedData(), result2.getAdditionalData(), null));
        assertArrayEquals(message, cell.decrypt(result3.getProtectedData(), result3.getAdditionalData(), null));

        assertArrayEquals(message, cell.decrypt(result1.getProtectedData(), result1.getAdditionalData(), new byte[]{}));
        assertArrayEquals(message, cell.decrypt(result2.getProtectedData(), result2.getAdditionalData(), new byte[]{}));
        assertArrayEquals(message, cell.decrypt(result3.getProtectedData(), result3.getAdditionalData(), new byte[]{}));
    }

    @Test
    @SuppressWarnings("ResultOfMethodCallIgnored")
    public void contextSignificance() throws SecureCellException {
        SecureCell.TokenProtect cell = SecureCell.TokenProtectWithKey(new SymmetricKey());
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);
        byte[] correctContext = "We are CATS".getBytes(StandardCharsets.UTF_8);
        byte[] incorrectContext = "Captain !!".getBytes(StandardCharsets.UTF_8);

        SecureCellData result = cell.encrypt(message, correctContext);
        byte[] encrypted = result.getProtectedData();
        byte[] authToken = result.getAdditionalData();

        // You cannot use a different context to decrypt data.
        try {
            cell.decrypt(encrypted, authToken, incorrectContext);
            fail("expected SecureCellException");
        }
        catch (SecureCellException ignored) {}

        // Only the original context will work.
        byte[] decrypted = cell.decrypt(encrypted, authToken, correctContext);

        assertArrayEquals(message, decrypted);
    }

    @Test
    @SuppressWarnings("ResultOfMethodCallIgnored")
    public void tokenSignificance() throws SecureCellException {
        SecureCell.TokenProtect cell = SecureCell.TokenProtectWithKey(new SymmetricKey());
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);

        SecureCellData result1 = cell.encrypt(message);
        byte[] encrypted1 = result1.getProtectedData();
        byte[] authToken1 = result1.getAdditionalData();

        SecureCellData result2 = cell.encrypt(message);
        byte[] encrypted2 = result2.getProtectedData();
        byte[] authToken2 = result2.getAdditionalData();

        // You cannot use a different token to decrypt data.
        try {
            cell.decrypt(encrypted1, authToken2);
            fail("expected SecureCellException");
        }
        catch (SecureCellException ignored) {}
        try {
            cell.decrypt(encrypted2, authToken1);
            fail("expected SecureCellException");
        }
        catch (SecureCellException ignored) {}

        // Only the matching token will work.
        assertArrayEquals(message, cell.decrypt(encrypted1, authToken1));
        assertArrayEquals(message, cell.decrypt(encrypted2, authToken2));
    }

    @Test
    @SuppressWarnings("ResultOfMethodCallIgnored")
    public void detectCorruptedData() {
        SecureCell.TokenProtect cell = SecureCell.TokenProtectWithKey(new SymmetricKey());
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);

        SecureCellData result = cell.encrypt(message);
        byte[] encrypted = result.getProtectedData();
        byte[] authToken = result.getAdditionalData();

        // Invert every odd byte, this will surely break the message.
        byte[] corrupted = Arrays.copyOf(encrypted, encrypted.length);
        for (int i = 0; i < corrupted.length; i++) {
            if (i % 2 == 1) {
                corrupted[i] = (byte)~corrupted[i];
            }
        }

        try {
            cell.decrypt(corrupted, authToken);
            fail("expected SecureCellException");
        }
        catch (SecureCellException ignored) {}
    }

    @Test
    @SuppressWarnings("ResultOfMethodCallIgnored")
    public void detectTruncatedData() {
        SecureCell.TokenProtect cell = SecureCell.TokenProtectWithKey(new SymmetricKey());
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);

        SecureCellData result = cell.encrypt(message);
        byte[] encrypted = result.getProtectedData();
        byte[] authToken = result.getAdditionalData();

        byte[] truncated = Arrays.copyOf(encrypted, encrypted.length - 1);

        try {
            cell.decrypt(truncated, authToken);
            fail("expected SecureCellException");
        }
        catch (SecureCellException ignored) {}
    }

    @Test
    @SuppressWarnings("ResultOfMethodCallIgnored")
    public void detectExtendedData() {
        SecureCell.TokenProtect cell = SecureCell.TokenProtectWithKey(new SymmetricKey());
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);

        SecureCellData result = cell.encrypt(message);
        byte[] encrypted = result.getProtectedData();
        byte[] authToken = result.getAdditionalData();

        byte[] extended = Arrays.copyOf(encrypted, encrypted.length + 1);

        try {
            cell.decrypt(extended, authToken);
            fail("expected SecureCellException");
        }
        catch (SecureCellException ignored) {}
    }


    @Test
    @SuppressWarnings("ResultOfMethodCallIgnored")
    public void detectCorruptedToken() {
        SecureCell.TokenProtect cell = SecureCell.TokenProtectWithKey(new SymmetricKey());
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);

        SecureCellData result = cell.encrypt(message);
        byte[] encrypted = result.getProtectedData();
        byte[] authToken = result.getAdditionalData();

        // Invert every odd byte, this will surely break the token.
        byte[] corruptedToken = Arrays.copyOf(authToken, authToken.length);
        for (int i = 0; i < corruptedToken.length; i++) {
            if (i % 2 == 1) {
                corruptedToken[i] = (byte)~corruptedToken[i];
            }
        }

        try {
            cell.decrypt(encrypted, corruptedToken);
            fail("expected SecureCellException");
        }
        catch (SecureCellException ignored) {}
    }

    @Test
    @SuppressWarnings("ResultOfMethodCallIgnored")
    public void detectTruncatedToken() {
        SecureCell.TokenProtect cell = SecureCell.TokenProtectWithKey(new SymmetricKey());
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);

        SecureCellData result = cell.encrypt(message);
        byte[] encrypted = result.getProtectedData();
        byte[] authToken = result.getAdditionalData();

        byte[] truncatedToken = Arrays.copyOf(authToken, authToken.length - 1);

        try {
            cell.decrypt(encrypted, truncatedToken);
            fail("expected SecureCellException");
        }
        catch (SecureCellException ignored) {}
    }

    @Test
    public void detectExtendedToken() throws SecureCellException {
        SecureCell.TokenProtect cell = SecureCell.TokenProtectWithKey(new SymmetricKey());
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);

        SecureCellData result = cell.encrypt(message);
        byte[] encrypted = result.getProtectedData();
        byte[] authToken = result.getAdditionalData();

        byte[] extendedToken = Arrays.copyOf(authToken, authToken.length + 1);

        // Current implementation of Secure Cell allows the token to be overlong.
        // Extra data is simply ignored.
        byte[] decrypted = cell.decrypt(encrypted, extendedToken);
        assertArrayEquals(message, decrypted);
    }

    @Test
    @SuppressWarnings("ResultOfMethodCallIgnored")
    public void swapTokenAndData() {
        SecureCell.TokenProtect cell = SecureCell.TokenProtectWithKey(new SymmetricKey());
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);

        SecureCellData result = cell.encrypt(message);
        byte[] encrypted = result.getProtectedData();
        byte[] authToken = result.getAdditionalData();

        try {
            cell.decrypt(authToken, encrypted);
            fail("expected SecureCellException");
        }
        // Depending on how lucky you are, Themis might or might not detect the error early enough.
        // If it does not, it proceeds to allocate some weird buffer which might be too big.
        catch (SecureCellException | OutOfMemoryError ignored) {}
    }

    @Test
    @SuppressWarnings("ResultOfMethodCallIgnored")
    public void swapContextAndToken() {
        SecureCell.TokenProtect cell = SecureCell.TokenProtectWithKey(new SymmetricKey());
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);
        byte[] context = "We are CATS".getBytes(StandardCharsets.UTF_8);

        SecureCellData result = cell.encrypt(message, context);
        byte[] encrypted = result.getProtectedData();

        try {
            cell.decrypt(encrypted, context);
            fail("expected SecureCellException");
        }
        catch (SecureCellException ignored) {}
    }

    @Test
    @SuppressWarnings("ResultOfMethodCallIgnored")
    public void emptyMessageOrToken() throws SecureCellException {
        SecureCell.TokenProtect cell = SecureCell.TokenProtectWithKey(new SymmetricKey());
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);

        try {
            cell.encrypt(null);
            fail("expected NullArgumentException");
        }
        catch (NullArgumentException ignored) {}
        try {
            cell.encrypt(new byte[]{});
            fail("expected InvalidArgumentException");
        }
        catch (InvalidArgumentException ignored) {}

        SecureCellData result = cell.encrypt(message);
        byte[] encrypted = result.getProtectedData();
        byte[] authToken = result.getAdditionalData();

        try {
            cell.decrypt(encrypted, null);
            fail("expected NullArgumentException");
        }
        catch (NullArgumentException ignored) {}
        try {
            cell.decrypt(null, authToken);
            fail("expected NullArgumentException");
        }
        catch (NullArgumentException ignored) {}
        try {
            cell.decrypt(encrypted, new byte[]{});
            fail("expected InvalidArgumentException");
        }
        catch (InvalidArgumentException ignored) {}
        try {
            cell.decrypt(new byte[]{}, authToken);
            fail("expected InvalidArgumentException");
        }
        catch (InvalidArgumentException ignored) {}
    }

    @Test
    @SuppressWarnings("deprecation")
    public void oldAPI() throws SecureCellException {
        SymmetricKey key = new SymmetricKey();
        SecureCell.TokenProtect newCell = SecureCell.TokenProtectWithKey(key);
        SecureCell oldCell = new SecureCell(key.toByteArray(), SecureCell.MODE_TOKEN_PROTECT);
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);
        byte[] context = "We are CATS".getBytes(StandardCharsets.UTF_8);
        byte[] encrypted, authToken, decrypted;

        SecureCellData result = oldCell.protect(context, message);
        assertNotNull(result);
        encrypted = result.getProtectedData();
        authToken = result.getAdditionalData();
        assertNotNull(encrypted);
        assertNotNull(authToken);

        decrypted = newCell.decrypt(encrypted, authToken, context);
        assertArrayEquals(message, decrypted);

        result = newCell.encrypt(message, context);
        assertNotNull(result);
        encrypted = result.getProtectedData();
        authToken = result.getAdditionalData();
        assertNotNull(encrypted);
        assertNotNull(authToken);

        decrypted = oldCell.unprotect(context, new SecureCellData(encrypted, authToken));
        assertArrayEquals(message, decrypted);
    }

    @Test
    @SuppressWarnings("deprecation")
    public void oldAPIWithoutContext() throws SecureCellException {
        SymmetricKey key = new SymmetricKey();
        SecureCell.TokenProtect newCell = SecureCell.TokenProtectWithKey(key);
        SecureCell oldCell = new SecureCell(key.toByteArray(), SecureCell.MODE_TOKEN_PROTECT);
        byte[] message = "All your base are belong to us!".getBytes(StandardCharsets.UTF_8);
        byte[] encrypted, authToken, decrypted;

        SecureCellData result = oldCell.protect((byte[])null, message);
        assertNotNull(result);
        encrypted = result.getProtectedData();
        authToken = result.getAdditionalData();
        assertNotNull(encrypted);
        assertNotNull(authToken);

        decrypted = newCell.decrypt(encrypted, authToken);
        assertArrayEquals(message, decrypted);

        result = newCell.encrypt(message);
        assertNotNull(result);
        encrypted = result.getProtectedData();
        authToken = result.getAdditionalData();
        assertNotNull(encrypted);
        assertNotNull(authToken);

        decrypted = oldCell.unprotect((byte[])null, new SecureCellData(encrypted, authToken));
        assertArrayEquals(message, decrypted);
    }
}
