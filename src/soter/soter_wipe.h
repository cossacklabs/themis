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

#ifndef SOTER_WIPE_H
#define SOTER_WIPE_H

#include <soter/soter_api.h>
#include <soter/soter_error.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Securely erases sensitive data from memory.
 *
 * Use this function to wipe sensitive data (e.g., passwords, private keys)
 * after you don't need it anymore. This function call will not be optimized
 * out by the compiler.
 *
 * @param [in] data   pointer to data to wiped
 * @param [in] length length of the data in bytes
 * @return SOTER_SUCCESS on success,
 *         SOTER_INVALID_PARAMETER on NULL data
 */
SOTER_API
soter_status_t soter_wipe(void* data, size_t length);

#ifdef __cplusplus
}
#endif

#endif /* SOTER_WIPE_H */
