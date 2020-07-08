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

import org.jetbrains.annotations.NotNull;

// We use Java 7 -- particularly for Android support -- where JUnit does not have "assertThrows()".
// That makes tests very verbose and repetitive. Provide a polyfill a for it.

final class Assert {
    static <T extends Throwable> T assertThrows(@NotNull Class<T> expected, Runnable runnable) {
        try {
            runnable.run();
        }
        catch (Throwable e) {
            if (expected.isInstance(e)) {
                return expected.cast(e);
            }
            throw new AssertionError("expected exception of type "+ expected, e);
        }
        throw new AssertionError("no expected exception of type " + expected + " was thrown");
    }
}
