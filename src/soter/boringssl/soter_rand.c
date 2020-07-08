/*
 * Copyright (c) 2015 Cossack Labs Limited
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

#include "soter/soter_rand.h"

#include <openssl/rand.h>

#include "soter/soter_wipe.h"

soter_status_t soter_rand(uint8_t* buffer, size_t length)
{
    int result;

    if (!buffer || !length) {
        return SOTER_INVALID_PARAMETER;
    }

    /* BoringSSL's RAND_bytes() accepts size_t, no need to cast */
    result = RAND_bytes(buffer, length);

    if (result == 1) {
        return SOTER_SUCCESS;
    }

    /*
     * Make sure we don't leak PRNG state in case the buffer has been
     * partially filled and we have to return an error.
     */
    soter_wipe(buffer, length);

    return (result < 0) ? SOTER_NOT_SUPPORTED : SOTER_FAIL;
}
