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
import org.junit.Test

class SecureCellContextImprintTestKotlin {
    @Test
    fun initWithGenerated() {
        val cell = SecureCell.ContextImprintWithKey(SymmetricKey())
        assertNotNull(cell)
    }

    @Test
    fun initWithFixed() {
        val keyBase64 = "UkVDMgAAAC13PCVZAKOczZXUpvkhsC+xvwWnv3CLmlG0Wzy8ZBMnT+2yx/dg"
        val keyBytes = Base64.getDecoder().decode(keyBase64)
        val cell = SecureCell.ContextImprintWithKey(keyBytes)
        assertNotNull(cell)
    }

    @Test
    fun initWithEmpty() {
        assertThrows(NullArgumentException::class.java) {
            SecureCell.ContextImprintWithKey(null as SymmetricKey?)
        }
        assertThrows(NullArgumentException::class.java) {
            SecureCell.ContextImprintWithKey(null as ByteArray?)
        }
        assertThrows(InvalidArgumentException::class.java) {
            SecureCell.ContextImprintWithKey(byteArrayOf())
        }
    }

    @Test
    fun roundtrip() {
        val cell = SecureCell.ContextImprintWithKey(SymmetricKey())
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)
        val context = "For great justice".toByteArray(StandardCharsets.UTF_8)

        val encrypted = cell.encrypt(message, context)
        assertNotNull(encrypted)

        val decrypted = cell.decrypt(encrypted, context)
        assertNotNull(decrypted)
        assertArrayEquals(message, decrypted)
    }

    @Test
    fun dataLengthPreservation() {
        val cell = SecureCell.ContextImprintWithKey(SymmetricKey())
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)
        val context = "For great justice".toByteArray(StandardCharsets.UTF_8)

        val encrypted = cell.encrypt(message, context)
        assertEquals(message.size.toLong(), encrypted.size.toLong())
    }

    @Test
    fun contextInclusion() {
        val cell = SecureCell.ContextImprintWithKey(SymmetricKey())
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)
        val shortContext = ".".toByteArray(StandardCharsets.UTF_8)
        val longContext = "You have no chance to survive make your time. Ha ha ha ha ...".toByteArray(StandardCharsets.UTF_8)

        val encryptedShort = cell.encrypt(message, shortContext)
        val encryptedLong = cell.encrypt(message, longContext)

        // Context is not (directly) included into encrypted message.
        assertEquals(encryptedShort.size.toLong(), encryptedLong.size.toLong())
    }

    @Test
    fun contextSignificance() {
        val cell = SecureCell.ContextImprintWithKey(SymmetricKey())
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)
        val correctContext = "We are CATS".toByteArray(StandardCharsets.UTF_8)
        val incorrectContext = "Captain !!".toByteArray(StandardCharsets.UTF_8)

        val encrypted = cell.encrypt(message, correctContext)

        // You can use a different context to decrypt data, but you'll get garbage.
        var decrypted = cell.decrypt(encrypted, incorrectContext)
        assertNotNull(decrypted)
        assertEquals(message.size.toLong(), decrypted.size.toLong())
        assertFalse(message.contentEquals(decrypted))

        // Only the original context will work.
        decrypted = cell.decrypt(encrypted, correctContext)
        assertArrayEquals(message, decrypted)
    }

    @Test
    fun noDetectCorruptedData() {
        val cell = SecureCell.ContextImprintWithKey(SymmetricKey())
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)
        val context = "We are CATS".toByteArray(StandardCharsets.UTF_8)

        val encrypted = cell.encrypt(message, context)

        // Invert every odd byte, this will surely break the message.
        val corrupted = encrypted.copyOf(encrypted.size)
        for (i in corrupted.indices) {
            if (i % 2 == 1) {
                corrupted[i] = corrupted[i].inv()
            }
        }

        // Decrypts successfully but the content is garbage.
        val decrypted = cell.decrypt(corrupted, context)
        assertNotNull(decrypted)
        assertEquals(message.size.toLong(), decrypted.size.toLong())
        assertFalse(message.contentEquals(decrypted))
    }

    @Test
    fun noDetectTruncatedData() {
        val cell = SecureCell.ContextImprintWithKey(SymmetricKey())
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)
        val context = "We are CATS".toByteArray(StandardCharsets.UTF_8)

        val encrypted = cell.encrypt(message, context)
        val truncated = encrypted.copyOf(encrypted.size - 1)

        // Decrypts successfully but the content is garbage.
        val decrypted = cell.decrypt(truncated, context)
        assertNotNull(decrypted)
        assertEquals(truncated.size.toLong(), decrypted.size.toLong())
        assertFalse(message.contentEquals(decrypted))
    }

    @Test
    fun noDetectExtendedData() {
        val cell = SecureCell.ContextImprintWithKey(SymmetricKey())
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)
        val context = "We are CATS".toByteArray(StandardCharsets.UTF_8)

        val encrypted = cell.encrypt(message, context)
        val extended = encrypted.copyOf(encrypted.size + 1)

        // Decrypts successfully but the content is garbage.
        val decrypted = cell.decrypt(extended, context)
        assertNotNull(decrypted)
        assertEquals(extended.size.toLong(), decrypted.size.toLong())
        assertFalse(message.contentEquals(decrypted))
    }

    @Test
    fun requiredMessageAndContext() {
        val cell = SecureCell.ContextImprintWithKey(SymmetricKey())
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)
        val context = "We are CATS".toByteArray(StandardCharsets.UTF_8)

        assertThrows(NullArgumentException::class.java) { cell.encrypt(message, null) }
        assertThrows(NullArgumentException::class.java) { cell.decrypt(message, null) }
        assertThrows(NullArgumentException::class.java) { cell.encrypt(null, context) }
        assertThrows(NullArgumentException::class.java) { cell.decrypt(null, context) }

        assertThrows(InvalidArgumentException::class.java) { cell.encrypt(message, byteArrayOf()) }
        assertThrows(InvalidArgumentException::class.java) { cell.decrypt(message, byteArrayOf()) }
        assertThrows(InvalidArgumentException::class.java) { cell.encrypt(byteArrayOf(), context) }
        assertThrows(InvalidArgumentException::class.java) { cell.decrypt(byteArrayOf(), context) }
    }

    @Test
    @Suppress("DEPRECATION")
    @Throws(SecureCellException::class)
    fun oldAPI() {
        val key = SymmetricKey()
        val newCell = SecureCell.ContextImprintWithKey(key)
        val oldCell = SecureCell(key.toByteArray(), SecureCell.MODE_CONTEXT_IMPRINT)
        val message = "All your base are belong to us!".toByteArray(StandardCharsets.UTF_8)
        val context = "We are CATS".toByteArray(StandardCharsets.UTF_8)

        var encrypted: ByteArray
        var decrypted: ByteArray?
        val result = oldCell.protect(context, message)
        encrypted = result.protectedData
        assertNotNull(encrypted)
        decrypted = newCell.decrypt(encrypted, context)
        assertArrayEquals(message, decrypted)

        encrypted = newCell.encrypt(message, context)
        assertNotNull(encrypted)
        decrypted = oldCell.unprotect(context, SecureCellData(encrypted, null))
        assertArrayEquals(message, decrypted)
    }
}
