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
package com.cossacklabs.themis.test

import java.nio.charset.StandardCharsets
import kotlin.experimental.inv

import com.cossacklabs.themis.*
import com.cossacklabs.themis.test.Assert.assertThrows

import org.junit.Assert.*
import org.junit.Ignore
import org.junit.Test

class SecureCellTokenProtectTestKotlin {
    @Test
    fun initWithGenerated() {
        val cell = SecureCell.TokenProtectWithKey(SymmetricKey())
        assertNotNull(cell)
    }

    @Test
    fun initWithFixed() {
        val keyBase64 = "UkVDMgAAAC13PCVZAKOczZXUpvkhsC+xvwWnv3CLmlG0Wzy8ZBMnT+2yx/dg"
        val keyBytes = Base64.getDecoder().decode(keyBase64)
        val cell = SecureCell.TokenProtectWithKey(keyBytes)
        assertNotNull(cell)
    }

    @Test
    fun initWithEmpty() {
        assertThrows(NullArgumentException::class.java) {
            SecureCell.TokenProtectWithKey(null as SymmetricKey?)
        }
        assertThrows(NullArgumentException::class.java) {
            SecureCell.TokenProtectWithKey(null as ByteArray?)
        }
        assertThrows(InvalidArgumentException::class.java) {
            SecureCell.TokenProtectWithKey(byteArrayOf())
        }
    }

    @Test
    @Throws(SecureCellException::class)
    fun roundtrip() {
        val cell = SecureCell.TokenProtectWithKey(SymmetricKey())
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)
        val context = "For great justice".toByteArray(StandardCharsets.UTF_8)

        val (encrypted, authToken) = cell.encrypt(message, context)
        assertNotNull(encrypted)
        assertNotNull(authToken)

        val decrypted = cell.decrypt(encrypted, authToken, context)
        assertNotNull(decrypted)
        assertArrayEquals(message, decrypted)
    }

    @Test
    fun dataLengthPreservation() {
        val cell = SecureCell.TokenProtectWithKey(SymmetricKey())
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)

        val (encrypted, authToken) = cell.encrypt(message)

        assertEquals(message.size, encrypted.size)
        assertTrue(authToken.isNotEmpty())
    }

    @Test
    fun contextInclusion() {
        val cell = SecureCell.TokenProtectWithKey(SymmetricKey())
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)
        val shortContext = ".".toByteArray(StandardCharsets.UTF_8)
        val longContext = "You have no chance to survive make your time. Ha ha ha ha ...".toByteArray(StandardCharsets.UTF_8)

        val (encryptedShort, authTokenShort) = cell.encrypt(message, shortContext)
        val (encryptedLong, authTokenLong) = cell.encrypt(message, longContext)

        // Context is not (directly) included into encrypted message.
        assertEquals(encryptedShort.size, encryptedLong.size)
        assertEquals(authTokenShort.size, authTokenLong.size)
    }

    @Test
    @Throws(SecureCellException::class)
    fun withoutContext() {
        val cell = SecureCell.TokenProtectWithKey(SymmetricKey())
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)

        // Absent, empty, or nil context are all the same.
        val (encrypted1, authToken1) = cell.encrypt(message)
        val (encrypted2, authToken2) = cell.encrypt(message, null)
        val (encrypted3, authToken3) = cell.encrypt(message, byteArrayOf())

        assertArrayEquals(message, cell.decrypt(encrypted1, authToken1))
        assertArrayEquals(message, cell.decrypt(encrypted2, authToken2))
        assertArrayEquals(message, cell.decrypt(encrypted3, authToken3))
        assertArrayEquals(message, cell.decrypt(encrypted1, authToken1, null))
        assertArrayEquals(message, cell.decrypt(encrypted2, authToken2, null))
        assertArrayEquals(message, cell.decrypt(encrypted3, authToken3, null))
        assertArrayEquals(message, cell.decrypt(encrypted1, authToken1, byteArrayOf()))
        assertArrayEquals(message, cell.decrypt(encrypted2, authToken2, byteArrayOf()))
        assertArrayEquals(message, cell.decrypt(encrypted3, authToken3, byteArrayOf()))
    }

    @Test
    @Throws(SecureCellException::class)
    fun contextSignificance() {
        val cell = SecureCell.TokenProtectWithKey(SymmetricKey())
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)
        val correctContext = "We are CATS".toByteArray(StandardCharsets.UTF_8)
        val incorrectContext = "Captain !!".toByteArray(StandardCharsets.UTF_8)

        val (encrypted, authToken) = cell.encrypt(message, correctContext)

        // You cannot use a different context to decrypt data.
        assertThrows(SecureCellException::class.java) {
            cell.decrypt(encrypted, authToken, incorrectContext)
        }

        // Only the original context will work.
        val decrypted = cell.decrypt(encrypted, authToken, correctContext)
        assertArrayEquals(message, decrypted)
    }

    @Test
    @Throws(SecureCellException::class)
    fun tokenSignificance() {
        val cell = SecureCell.TokenProtectWithKey(SymmetricKey())
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)

        val (encrypted1, authToken1) = cell.encrypt(message)
        val (encrypted2, authToken2) = cell.encrypt(message)

        // You cannot use a different token to decrypt data.
        assertThrows(SecureCellException::class.java) { cell.decrypt(encrypted1, authToken2) }
        assertThrows(SecureCellException::class.java) { cell.decrypt(encrypted2, authToken1) }

        // Only the matching token will work.
        assertArrayEquals(message, cell.decrypt(encrypted1, authToken1))
        assertArrayEquals(message, cell.decrypt(encrypted2, authToken2))
    }

    @Test
    fun detectCorruptedData() {
        val cell = SecureCell.TokenProtectWithKey(SymmetricKey())
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)

        val (encrypted, authToken) = cell.encrypt(message)

        // Invert every odd byte, this will surely break the message.
        val corrupted = encrypted.copyOf(encrypted.size)
        for (i in corrupted.indices) {
            if (i % 2 == 1) {
                corrupted[i] = corrupted[i].inv()
            }
        }

        assertThrows(SecureCellException::class.java) {
            cell.decrypt(corrupted, authToken)
        }
    }

    @Test
    fun detectTruncatedData() {
        val cell = SecureCell.TokenProtectWithKey(SymmetricKey())
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)

        val (encrypted, authToken) = cell.encrypt(message)

        val truncated = encrypted.copyOf(encrypted.size - 1)

        assertThrows(SecureCellException::class.java) {
            cell.decrypt(truncated, authToken)
        }
    }

    @Test
    fun detectExtendedData() {
        val cell = SecureCell.TokenProtectWithKey(SymmetricKey())
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)

        val (encrypted, authToken) = cell.encrypt(message)

        val extended = encrypted.copyOf(encrypted.size + 1)

        assertThrows(SecureCellException::class.java) {
            cell.decrypt(extended, authToken)
        }
    }

    @Test
    fun detectCorruptedToken() {
        val cell = SecureCell.TokenProtectWithKey(SymmetricKey())
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)

        val (encrypted, authToken) = cell.encrypt(message)

        // Invert every odd byte, this will surely break the token.
        val corruptedToken = authToken.copyOf(authToken.size)
        for (i in corruptedToken.indices) {
            if (i % 2 == 1) {
                corruptedToken[i] = corruptedToken[i].inv()
            }
        }

        assertThrows(SecureCellException::class.java) {
            cell.decrypt(encrypted, corruptedToken)
        }
    }

    @Test
    fun detectTruncatedToken() {
        val cell = SecureCell.TokenProtectWithKey(SymmetricKey())
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)

        val (encrypted, authToken) = cell.encrypt(message)

        val truncatedToken = authToken.copyOf(authToken.size - 1)

        assertThrows(SecureCellException::class.java) {
            cell.decrypt(encrypted, truncatedToken)
        }
    }

    @Test
    @Throws(SecureCellException::class)
    fun detectExtendedToken() {
        val cell = SecureCell.TokenProtectWithKey(SymmetricKey())
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)

        val (encrypted, authToken) = cell.encrypt(message)

        val extendedToken = authToken.copyOf(authToken.size + 1)

        // Current implementation of Secure Cell allows the token to be overlong.
        // Extra data is simply ignored.
        val decrypted = cell.decrypt(encrypted, extendedToken)
        assertArrayEquals(message, decrypted)
    }

    @Test
    fun swapTokenAndData() {
        val cell = SecureCell.TokenProtectWithKey(SymmetricKey())
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)

        val (encrypted, authToken) = cell.encrypt(message)

        // Depending on how lucky you are, Themis might or might not detect the error early enough.
        // If it does not, it proceeds to allocate some weird buffer which might be too big.
        val e = assertThrows(Throwable::class.java) {
            cell.decrypt(authToken, encrypted)
        }
        assertTrue(e is SecureCellException || e is OutOfMemoryError)
    }

    @Test
    fun swapContextAndToken() {
        val cell = SecureCell.TokenProtectWithKey(SymmetricKey())
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)
        val context = "We are CATS".toByteArray(StandardCharsets.UTF_8)

        val (encrypted, _) = cell.encrypt(message, context)

        assertThrows(SecureCellException::class.java) {
            cell.decrypt(encrypted, context)
        }
    }

    @Test
    @Throws(SecureCellException::class)
    fun emptyMessageOrToken() {
        val cell = SecureCell.TokenProtectWithKey(SymmetricKey())
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)

        assertThrows(NullArgumentException::class.java) { cell.encrypt(null) }
        assertThrows(InvalidArgumentException::class.java) { cell.encrypt(byteArrayOf()) }

        val (encrypted, authToken) = cell.encrypt(message)

        assertThrows(NullArgumentException::class.java) { cell.decrypt(encrypted, null) }
        assertThrows(NullArgumentException::class.java) { cell.decrypt(null, authToken) }

        assertThrows(InvalidArgumentException::class.java) { cell.decrypt(encrypted, byteArrayOf()) }
        assertThrows(InvalidArgumentException::class.java) { cell.decrypt(byteArrayOf(), authToken) }
    }

    @Test
    @Suppress("DEPRECATION")
    @Throws(SecureCellException::class)
    fun oldAPI() {
        val key = SymmetricKey()
        val newCell = SecureCell.TokenProtectWithKey(key)
        val oldCell = SecureCell(key.toByteArray(), SecureCell.MODE_TOKEN_PROTECT)
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)
        val context = "We are CATS".toByteArray(StandardCharsets.UTF_8)

        val result = oldCell.protect(context, message)
        assertNotNull(result)
        val encryptedOld = result.protectedData
        val authTokenOld = result.additionalData
        assertNotNull(encryptedOld)
        assertNotNull(authTokenOld)
        val decryptedOld = newCell.decrypt(encryptedOld, authTokenOld, context)
        assertArrayEquals(message, decryptedOld)

        val (encryptedNew, authTokenNew) = newCell.encrypt(message, context)
        assertNotNull(encryptedNew)
        assertNotNull(authTokenNew)
        val decryptedNew = oldCell.unprotect(context, SecureCellData(encryptedNew, authTokenNew))
        assertArrayEquals(message, decryptedNew)
    }

    @Test
    @Suppress("DEPRECATION")
    @Throws(SecureCellException::class)
    fun oldAPIWithoutContext() {
        val key = SymmetricKey()
        val newCell = SecureCell.TokenProtectWithKey(key)
        val oldCell = SecureCell(key.toByteArray(), SecureCell.MODE_TOKEN_PROTECT)
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)

        val result = oldCell.protect(null as ByteArray?, message)
        assertNotNull(result)
        val encryptedOld = result.protectedData
        val authTokenOld = result.additionalData
        assertNotNull(encryptedOld)
        assertNotNull(authTokenOld)
        val decryptedOld = newCell.decrypt(encryptedOld, authTokenOld)
        assertArrayEquals(message, decryptedOld)

        val (encryptedNew, authTokenNew) = newCell.encrypt(message)
        assertNotNull(encryptedNew)
        assertNotNull(authTokenNew)
        val decryptedNew = oldCell.unprotect(null as ByteArray?, SecureCellData(encryptedNew, authTokenNew))
        assertArrayEquals(message, decryptedNew)
    }
}
