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

import java.nio.charset.CharacterCodingException
import java.nio.charset.StandardCharsets
import kotlin.experimental.inv

import com.cossacklabs.themis.*
import com.cossacklabs.themis.test.Assert.assertThrows

import org.junit.Assert.*
import org.junit.Test

class SecureCellSealWithPassphraseTestKotlin {
    @Test
    fun initWithString() {
        val cell = SecureCell.SealWithPassphrase("day 56 of the Q")
        assertNotNull(cell)
    }

    @Test
    fun initWithBytes() {
        val encoded = "day 56 of the Q".toByteArray(StandardCharsets.US_ASCII)
        val cell = SecureCell.SealWithPassphrase(encoded)
        assertNotNull(cell)
    }

    @Test
    fun initWithEmpty() {
        assertThrows(NullArgumentException::class.java) {
            SecureCell.SealWithPassphrase(null as String?)
        }
        assertThrows(NullArgumentException::class.java) {
            SecureCell.SealWithPassphrase(null as ByteArray?)
        }
        assertThrows(NullArgumentException::class.java) {
            SecureCell.SealWithPassphrase(null, StandardCharsets.UTF_8)
        }
        assertThrows(NullArgumentException::class.java) {
            SecureCell.SealWithPassphrase("day 56 of the Q", null)
        }

        assertThrows(InvalidArgumentException::class.java) {
            SecureCell.SealWithPassphrase(byteArrayOf())
        }
        assertThrows(InvalidArgumentException::class.java) {
            SecureCell.SealWithPassphrase("")
        }
        assertThrows(InvalidArgumentException::class.java) {
            SecureCell.SealWithPassphrase("", StandardCharsets.UTF_16BE)
        }
    }

    @Test
    @Throws(SecureCellException::class)
    fun roundtrip() {
        val cell = SecureCell.SealWithPassphrase("day 56 of the Q")
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)
        val context = "For great justice".toByteArray(StandardCharsets.UTF_8)

        val encrypted = cell.encrypt(message, context)
        assertNotNull(encrypted)

        val decrypted = cell.decrypt(encrypted, context)
        assertNotNull(decrypted)
        assertArrayEquals(message, decrypted)
    }

    @Test
    fun dataLengthExtension() {
        val cell = SecureCell.SealWithPassphrase("day 56 of the Q")
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)

        val encrypted = cell.encrypt(message)
        assertTrue(encrypted.size > message.size)
    }

    @Test
    fun contextInclusion() {
        val cell = SecureCell.SealWithPassphrase("day 56 of the Q")
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)
        val shortContext = ".".toByteArray(StandardCharsets.UTF_8)
        val longContext = "You have no chance to survive make your time. Ha ha ha ha ...".toByteArray(StandardCharsets.UTF_8)

        val encryptedShort = cell.encrypt(message, shortContext)
        val encryptedLong = cell.encrypt(message, longContext)

        // Context is not (directly) included into encrypted message.
        assertEquals(encryptedShort.size.toLong(), encryptedLong.size.toLong())
    }

    @Test
    @Throws(SecureCellException::class)
    fun withoutContext() {
        val cell = SecureCell.SealWithPassphrase("day 56 of the Q")
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)

        // Absent, empty, or nil context are all the same.
        val encrypted1 = cell.encrypt(message)
        val encrypted2 = cell.encrypt(message, null)
        val encrypted3 = cell.encrypt(message, byteArrayOf())

        assertArrayEquals(message, cell.decrypt(encrypted1))
        assertArrayEquals(message, cell.decrypt(encrypted2))
        assertArrayEquals(message, cell.decrypt(encrypted3))
        assertArrayEquals(message, cell.decrypt(encrypted1, null))
        assertArrayEquals(message, cell.decrypt(encrypted2, null))
        assertArrayEquals(message, cell.decrypt(encrypted3, null))
        assertArrayEquals(message, cell.decrypt(encrypted1, byteArrayOf()))
        assertArrayEquals(message, cell.decrypt(encrypted2, byteArrayOf()))
        assertArrayEquals(message, cell.decrypt(encrypted3, byteArrayOf()))
    }

    @Test
    @Throws(SecureCellException::class)
    fun contextSignificance() {
        val cell = SecureCell.SealWithPassphrase("day 56 of the Q")
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)
        val correctContext = "We are CATS".toByteArray(StandardCharsets.UTF_8)
        val incorrectContext = "Captain !!".toByteArray(StandardCharsets.UTF_8)

        val encrypted = cell.encrypt(message, correctContext)

        // You cannot use a different context to decrypt data.
        assertThrows(SecureCellException::class.java) {
            cell.decrypt(encrypted, incorrectContext)
        }

        // Only the original context will work.
        val decrypted = cell.decrypt(encrypted, correctContext)
        assertArrayEquals(message, decrypted)
    }

    @Test
    fun detectCorruptedData() {
        val cell = SecureCell.SealWithPassphrase("day 56 of the Q")
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)

        val encrypted = cell.encrypt(message)

        // Invert every odd byte, this will surely break the message.
        val corrupted = encrypted.copyOf(encrypted.size)
        for (i in corrupted.indices) {
            if (i % 2 == 1) {
                corrupted[i] = corrupted[i].inv()
            }
        }

        assertThrows(SecureCellException::class.java) {
            cell.decrypt(corrupted)
        }
    }

    @Test
    fun detectTruncatedData() {
        val cell = SecureCell.SealWithPassphrase("day 56 of the Q")
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)

        val encrypted = cell.encrypt(message)

        val truncated = encrypted.copyOf(encrypted.size - 1)

        assertThrows(SecureCellException::class.java) {
            cell.decrypt(truncated)
        }
    }

    @Test
    fun detectExtendedData() {
        val cell = SecureCell.SealWithPassphrase("day 56 of the Q")
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)

        val encrypted = cell.encrypt(message)

        val extended = encrypted.copyOf(encrypted.size + 1)

        assertThrows(SecureCellException::class.java) {
            cell.decrypt(extended)
        }
    }

    @Test
    @Throws(SecureCellException::class)
    fun emptyMessage() {
        val cell = SecureCell.SealWithPassphrase("day 56 of the Q")

        assertThrows(NullArgumentException::class.java) { cell.encrypt(null) }
        assertThrows(NullArgumentException::class.java) { cell.decrypt(null) }

        assertThrows(InvalidArgumentException::class.java) { cell.encrypt(byteArrayOf()) }
        assertThrows(InvalidArgumentException::class.java) { cell.decrypt(byteArrayOf()) }
    }

    @Test
    @Throws(SecureCellException::class)
    fun defaultEncoding() {
        // Passphrases are encoded in UTF-8 by default.
        val passphrase = "暗号"
        val cellA = SecureCell.SealWithPassphrase(passphrase)
        val cellB = SecureCell.SealWithPassphrase(passphrase.toByteArray(StandardCharsets.UTF_8))
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)

        val encrypted = cellA.encrypt(message)
        val decrypted = cellB.decrypt(encrypted)
        assertArrayEquals(message, decrypted)
    }

    @Test
    @Throws(SecureCellException::class)
    fun specificEncoding() {
        val cell = SecureCell.SealWithPassphrase("secret", StandardCharsets.UTF_16BE)
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)

        // Message encrypted by PyThemis
        val encrypted = Base64.getDecoder().decode("AAEBQQwAAAAQAAAAHwAAABYAAADy+4zWeqGQjLNlN3EVreeo2dnnL9UyFSFCLeOmQA0DABAAtVl1a5G5CDLNxZ/kOxErxj6xgu+0f5m+HBLgzcdLvbgcvO93buExyghbOmGBXoA=")
        val decrypted = cell.decrypt(encrypted)
        assertArrayEquals(message, decrypted)
    }

    @Test
    fun passphraseIsNotSymmetricKey() {
        // Passphrases are not keys. Keys are not passphrases.
        val secret = SymmetricKey()
        val cellMK = SecureCell.SealWithKey(secret)
        val cellPW = SecureCell.SealWithPassphrase(secret.toByteArray())
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)

        val encryptedMK = cellMK.encrypt(message)
        val encryptedPW = cellPW.encrypt(message)

        assertThrows(SecureCellException::class.java) { cellPW.decrypt(encryptedMK) }
        assertThrows(SecureCellException::class.java) { cellMK.decrypt(encryptedPW) }
    }

    @Test
    @Suppress("DEPRECATION")
    @Throws(SecureCellException::class)
    fun passphrasesNotCompatibleWithOldAPI() {
        // Old 'passphrase-like' API is not passphrase API at all. Don't use it.
        val cellOld = SecureCell("day 56 of the Q", SecureCell.MODE_SEAL)
        val cellNew = SecureCell.SealWithPassphrase("day 56 of the Q", StandardCharsets.UTF_16)
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)

        val encryptedOld = cellOld.protect("", message).protectedData
        val encryptedNew = cellNew.encrypt(message)

        assertThrows(SecureCellException::class.java) { cellNew.decrypt(encryptedOld) }
        assertThrows(SecureCellException::class.java) {
            cellOld.unprotect("", SecureCellData(encryptedNew, null))
        }
    }

    @Test
    @Suppress("DEPRECATION")
    @Throws(SecureCellException::class)
    fun oldPassphraseAPIIsActuallyUTF16Key() {
        // Yes, it's so weird due to hysterical raisins. So don't use it, really.
        val cellOld = SecureCell("day 56 of the Q", SecureCell.MODE_SEAL)
        val cellNew = SecureCell.SealWithKey("day 56 of the Q".toByteArray(StandardCharsets.UTF_16))
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)

        val encryptedOld = cellOld.protect("", message).protectedData
        val encryptedNew = cellNew.encrypt(message)

        val decryptedOld = cellNew.decrypt(encryptedOld)
        val decryptedNew = cellOld.unprotect("", SecureCellData(encryptedNew, null))

        assertArrayEquals(message, decryptedOld)
        assertArrayEquals(message, decryptedNew)
    }

    @Test
    fun passphraseEncodingFailure() {
        // It is an error if the passphrase cannot be represented in the requested charset
        // without data loss.
        val e = assertThrows(RuntimeException::class.java) {
            SecureCell.SealWithPassphrase("пароль", StandardCharsets.US_ASCII)
        }
        assertTrue(e.cause is CharacterCodingException)
    }
}
