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

package com.cossacklabs.themis.test;

import com.cossacklabs.themis.SymmetricKey;
import com.cossacklabs.themis.InvalidArgumentException;
import com.cossacklabs.themis.NullArgumentException;

import static org.junit.Assert.*;
import org.junit.Test;

public class SymmetricKeyTest {

    private static final int defaultLength = 32;

    @Test
    public void generateNewKey() {
        SymmetricKey key = new SymmetricKey();

        byte[] keyBytes = key.toByteArray();
        assertNotNull(keyBytes);
        assertEquals(keyBytes.length, defaultLength);
    }

    @Test
    public void restoreKeyFromBytes() {
        byte[] buffer = { 3, 14, 15, 92, 6 };
        SymmetricKey key = new SymmetricKey(buffer);
        assertArrayEquals(buffer, key.toByteArray());
    }

    @Test(expected = NullArgumentException.class)
    public void restoreKeyFromNull() {
        SymmetricKey key = new SymmetricKey(null);
    }

    @Test(expected = InvalidArgumentException.class)
    public void restoreKeyFromEmpty() {
        SymmetricKey key = new SymmetricKey(new byte[0]);
    }
}
