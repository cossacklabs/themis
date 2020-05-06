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

// Java 8 and above has "java.utils.Base64", but it's not available on all Android API levels.
// Android has its own "android.util.Base64" (with a different API) and it requires API 26+ too.
// Java also has an obscure "javax.xml.bind.DatatypeConverter" class which is not available on
// Android, unfortunately. We provide our own polyfill for java.utils.Base64 using Apache Commons.
//
// Also note that some older Android systems already include an older version of Apache Commons
// (version 1.2, it seems) loaded from "/system/framework/org.apache.http.legacy.boot.jar",
// which overrides whatever is specified in Gradle dependencies. Use 1.2 API only.

final class Base64 {
    static final class Decoder {
        byte[] decode(String src) {
            // Modern versions can accept String directly, but Android makes it hard.
            byte[] base64bytes = src.getBytes(StandardCharsets.US_ASCII);
            return org.apache.commons.codec.binary.Base64.decodeBase64(base64bytes);
        }
    }

    static Decoder getDecoder() {
        return new Decoder();
    }
}
