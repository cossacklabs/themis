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
import java.util.*
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

        val result = cell.encrypt(message, context)
        assertNotNull(result)

        val encrypted = result.protectedData
        val authToken = result.additionalData
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

        val result = cell.encrypt(message)
        assertEquals(message.size.toLong(), result.protectedData.size.toLong())
        assertTrue(result.additionalData.isNotEmpty())
    }

    @Test
    fun contextInclusion() {
        val cell = SecureCell.TokenProtectWithKey(SymmetricKey())
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)
        val shortContext = ".".toByteArray(StandardCharsets.UTF_8)
        val longContext = "You have no chance to survive make your time. Ha ha ha ha ...".toByteArray(StandardCharsets.UTF_8)

        val resultShort = cell.encrypt(message, shortContext)
        val resultLong = cell.encrypt(message, longContext)

        // Context is not (directly) included into encrypted message.
        assertEquals(resultShort.protectedData.size.toLong(), resultLong.protectedData.size.toLong())
        assertEquals(resultShort.additionalData.size.toLong(), resultLong.additionalData.size.toLong())
    }

    @Test
    @Throws(SecureCellException::class)
    fun withoutContext() {
        val cell = SecureCell.TokenProtectWithKey(SymmetricKey())
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)

        // Absent, empty, or nil context are all the same.
        val result1 = cell.encrypt(message)
        val result2 = cell.encrypt(message, null)
        val result3 = cell.encrypt(message, byteArrayOf())

        assertArrayEquals(message, cell.decrypt(result1.protectedData, result1.additionalData))
        assertArrayEquals(message, cell.decrypt(result2.protectedData, result2.additionalData))
        assertArrayEquals(message, cell.decrypt(result3.protectedData, result3.additionalData))
        assertArrayEquals(message, cell.decrypt(result1.protectedData, result1.additionalData, null))
        assertArrayEquals(message, cell.decrypt(result2.protectedData, result2.additionalData, null))
        assertArrayEquals(message, cell.decrypt(result3.protectedData, result3.additionalData, null))
        assertArrayEquals(message, cell.decrypt(result1.protectedData, result1.additionalData, byteArrayOf()))
        assertArrayEquals(message, cell.decrypt(result2.protectedData, result2.additionalData, byteArrayOf()))
        assertArrayEquals(message, cell.decrypt(result3.protectedData, result3.additionalData, byteArrayOf()))
    }

    @Test
    @Throws(SecureCellException::class)
    fun contextSignificance() {
        val cell = SecureCell.TokenProtectWithKey(SymmetricKey())
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)
        val correctContext = "We are CATS".toByteArray(StandardCharsets.UTF_8)
        val incorrectContext = "Captain !!".toByteArray(StandardCharsets.UTF_8)

        val result = cell.encrypt(message, correctContext)
        val encrypted = result.protectedData
        val authToken = result.additionalData

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

        val result1 = cell.encrypt(message)
        val encrypted1 = result1.protectedData
        val authToken1 = result1.additionalData

        val result2 = cell.encrypt(message)
        val encrypted2 = result2.protectedData
        val authToken2 = result2.additionalData

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

        val result = cell.encrypt(message)
        val encrypted = result.protectedData
        val authToken = result.additionalData

        // Invert every odd byte, this will surely break the message.
        val corrupted = Arrays.copyOf(encrypted, encrypted.size)
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

        val result = cell.encrypt(message)
        val encrypted = result.protectedData
        val authToken = result.additionalData

        val truncated = Arrays.copyOf(encrypted, encrypted.size - 1)

        assertThrows(SecureCellException::class.java) {
            cell.decrypt(truncated, authToken)
        }
    }

    @Test
    fun detectExtendedData() {
        val cell = SecureCell.TokenProtectWithKey(SymmetricKey())
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)

        val result = cell.encrypt(message)
        val encrypted = result.protectedData
        val authToken = result.additionalData

        val extended = Arrays.copyOf(encrypted, encrypted.size + 1)

        assertThrows(SecureCellException::class.java) {
            cell.decrypt(extended, authToken)
        }
    }

    @Test
    fun detectCorruptedToken() {
        val cell = SecureCell.TokenProtectWithKey(SymmetricKey())
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)

        val result = cell.encrypt(message)
        val encrypted = result.protectedData
        val authToken = result.additionalData

        // Invert every odd byte, this will surely break the token.
        val corruptedToken = Arrays.copyOf(authToken, authToken.size)
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

        val result = cell.encrypt(message)
        val encrypted = result.protectedData
        val authToken = result.additionalData

        val truncatedToken = Arrays.copyOf(authToken, authToken.size - 1)

        assertThrows(SecureCellException::class.java) {
            cell.decrypt(encrypted, truncatedToken)
        }
    }

    @Test
    @Throws(SecureCellException::class)
    fun detectExtendedToken() {
        val cell = SecureCell.TokenProtectWithKey(SymmetricKey())
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)

        val result = cell.encrypt(message)
        val encrypted = result.protectedData
        val authToken = result.additionalData

        val extendedToken = Arrays.copyOf(authToken, authToken.size + 1)

        // Current implementation of Secure Cell allows the token to be overlong.
        // Extra data is simply ignored.
        val decrypted = cell.decrypt(encrypted, extendedToken)
        assertArrayEquals(message, decrypted)
    }

    @Test
    fun swapTokenAndData() {
        val cell = SecureCell.TokenProtectWithKey(SymmetricKey())
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)

        val result = cell.encrypt(message)
        val encrypted = result.protectedData
        val authToken = result.additionalData

        assertThrows(SecureCellException::class.java) {
            cell.decrypt(authToken, encrypted)
        }
    }

    @Test
    fun swapContextAndToken() {
        val cell = SecureCell.TokenProtectWithKey(SymmetricKey())
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)
        val context = "We are CATS".toByteArray(StandardCharsets.UTF_8)

        val result = cell.encrypt(message, context)
        val encrypted = result.protectedData

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

        val result = cell.encrypt(message)
        val encrypted = result.protectedData
        val authToken = result.additionalData

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

        var encrypted: ByteArray
        var authToken: ByteArray
        var decrypted: ByteArray?
        var result = oldCell.protect(context, message)
        assertNotNull(result)
        encrypted = result.protectedData
        authToken = result.additionalData
        assertNotNull(encrypted)
        assertNotNull(authToken)
        decrypted = newCell.decrypt(encrypted, authToken, context)
        assertArrayEquals(message, decrypted)

        result = newCell.encrypt(message, context)
        assertNotNull(result)
        encrypted = result.protectedData
        authToken = result.additionalData
        assertNotNull(encrypted)
        assertNotNull(authToken)
        decrypted = oldCell.unprotect(context, SecureCellData(encrypted, authToken))
        assertArrayEquals(message, decrypted)
    }

    @Test
    @Suppress("DEPRECATION")
    @Throws(SecureCellException::class)
    fun oldAPIWithoutContext() {
        val key = SymmetricKey()
        val newCell = SecureCell.TokenProtectWithKey(key)
        val oldCell = SecureCell(key.toByteArray(), SecureCell.MODE_TOKEN_PROTECT)
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)

        var encrypted: ByteArray
        var authToken: ByteArray
        var decrypted: ByteArray?
        var result = oldCell.protect(null as ByteArray?, message)
        assertNotNull(result)
        encrypted = result.protectedData
        authToken = result.additionalData
        assertNotNull(encrypted)
        assertNotNull(authToken)
        decrypted = newCell.decrypt(encrypted, authToken)
        assertArrayEquals(message, decrypted)

        result = newCell.encrypt(message)
        assertNotNull(result)
        encrypted = result.protectedData
        authToken = result.additionalData
        assertNotNull(encrypted)
        assertNotNull(authToken)
        decrypted = oldCell.unprotect(null as ByteArray?, SecureCellData(encrypted, authToken))
        assertArrayEquals(message, decrypted)
    }
}
