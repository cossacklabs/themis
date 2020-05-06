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

import javax.xml.bind.DatatypeConverter;

// Java 8 and above has "java.utils.Base64", but it's not available on all Android API levels.
// Android has its own "android.util.Base64" (with a different API) and it requires API 26+ too.
// Of course we could pull Apache Commons or Guava, but that's an overkill for base64 decoding.
// We provide our own polyfill using an obscure but standard JRE class instead.

final class Base64 {
    static final class Decoder {
        byte[] decode(String src) {
            return DatatypeConverter.parseBase64Binary(src);
        }
    }

    static Decoder getDecoder() {
        return new Decoder();
    }
}
