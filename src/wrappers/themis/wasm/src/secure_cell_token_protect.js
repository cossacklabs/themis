// Copyright (c) 2019 Cossack Labs Limited
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file
 * Themis Secure Cell (Token Protect mode).
 */

const libthemis = require('./libthemis.js')
const errors = require('./themis_error.js')
const utils = require('./utils.js')

const cryptosystem_name = 'SecureCellTokenProtect'

const ThemisError = errors.ThemisError
const ThemisErrorCode = errors.ThemisErrorCode

module.exports = class SecureCellTokenProtect {
    constructor(masterKey) {
        masterKey = utils.coerceToBytes(masterKey)
        if (masterKey.length == 0) {
            throw new ThemisError(cryptosystem_name, ThemisErrorCode.INVALID_PARAMETER,
                'master key must be not empty')
        }
        this.masterKey = masterKey
    }

    encrypt(message) {
        message = utils.coerceToBytes(message)
        if (message.length == 0) {
            throw new ThemisError(cryptosystem_name, ThemisErrorCode.INVALID_PARAMETER,
                'message must be not empty')
        }

        let context
        if (arguments.length > 1 && arguments[1] !== null) {
            context = utils.coerceToBytes(arguments[1])
        } else {
            context = new Uint8Array()
        }

        let status
        /// C API uses "size_t" for lengths, it's defined as "i32" in Emscripten
        let result_length_ptr = libthemis.allocate(4, 'i32', libthemis.ALLOC_STACK)
        let token_length_ptr = libthemis.allocate(4, 'i32', libthemis.ALLOC_STACK)
        let master_key_ptr, message_ptr, context_ptr, result_ptr, result_length, token_ptr, token_length
        try {
            master_key_ptr = libthemis._malloc(this.masterKey.length)
            message_ptr = libthemis._malloc(message.length)
            context_ptr = libthemis._malloc(context.length)
            if (!master_key_ptr || !message_ptr || !context_ptr) {
                throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY)
            }

            libthemis.writeArrayToMemory(this.masterKey, master_key_ptr)
            libthemis.writeArrayToMemory(message, message_ptr)
            libthemis.writeArrayToMemory(context, context_ptr)

            status = libthemis._themis_secure_cell_encrypt_token_protect(
                master_key_ptr, this.masterKey.length,
                context_ptr, context.length,
                message_ptr, message.length,
                null, token_length_ptr,
                null, result_length_ptr,
            )
            if (status != ThemisErrorCode.BUFFER_TOO_SMALL) {
                throw new ThemisError(cryptosystem_name, status)
            }

            result_length = libthemis.getValue(result_length_ptr, 'i32')
            token_length = libthemis.getValue(token_length_ptr, 'i32')
            result_ptr = libthemis._malloc(result_length)
            token_ptr = libthemis._malloc(token_length)
            if (!result_ptr || !token_ptr) {
                throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY)
            }

            status = libthemis._themis_secure_cell_encrypt_token_protect(
                master_key_ptr, this.masterKey.length,
                context_ptr, context.length,
                message_ptr, message.length,
                token_ptr, token_length_ptr,
                result_ptr, result_length_ptr,
            )
            if (status != ThemisErrorCode.SUCCESS) {
                throw new ThemisError(cryptosystem_name, status)
            }

            result_length = libthemis.getValue(result_length_ptr, 'i32')
            token_length = libthemis.getValue(token_length_ptr, 'i32')

            return {
                data: libthemis.HEAPU8.slice(result_ptr, result_ptr + result_length),
                token: libthemis.HEAPU8.slice(token_ptr, token_ptr + token_length)
            }
        }
        finally {
            libthemis._memset(master_key_ptr, 0, this.masterKey.length)
            libthemis._free(master_key_ptr)
            libthemis._free(message_ptr)
            libthemis._free(context_ptr)
            libthemis._free(result_ptr)
            libthemis._free(token_ptr)
        }
    }

    decrypt(message, token) {
        message = utils.coerceToBytes(message)
        if (message.length == 0) {
            throw new ThemisError(cryptosystem_name, ThemisErrorCode.INVALID_PARAMETER,
                'message must be not empty')
        }

        token = utils.coerceToBytes(token)
        if (token.length == 0) {
            throw new ThemisError(cryptosystem_name, ThemisErrorCode.INVALID_PARAMETER,
                'token must be not empty')
        }

        let context
        if (arguments.length > 2 && arguments[2] !== null) {
            context = utils.coerceToBytes(arguments[2])
        } else {
            context = new Uint8Array()
        }

        let status
        /// C API uses "size_t" for lengths, it's defined as "i32" in Emscripten
        let result_length_ptr = libthemis.allocate(4, 'i32', libthemis.ALLOC_STACK)
        let master_key_ptr, message_ptr, context_ptr, token_ptr, result_ptr, result_length
        try {
            master_key_ptr = libthemis._malloc(this.masterKey.length)
            message_ptr = libthemis._malloc(message.length)
            context_ptr = libthemis._malloc(context.length)
            token_ptr = libthemis._malloc(token.length)
            if (!master_key_ptr || !message_ptr || !context_ptr || !token_ptr) {
                throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY)
            }

            libthemis.writeArrayToMemory(this.masterKey, master_key_ptr)
            libthemis.writeArrayToMemory(message, message_ptr)
            libthemis.writeArrayToMemory(context, context_ptr)
            libthemis.writeArrayToMemory(token, token_ptr)

            status = libthemis._themis_secure_cell_decrypt_token_protect(
                master_key_ptr, this.masterKey.length,
                context_ptr, context.length,
                message_ptr, message.length,
                token_ptr, token.length,
                null, result_length_ptr
            )
            if (status != ThemisErrorCode.BUFFER_TOO_SMALL) {
                throw new ThemisError(cryptosystem_name, status)
            }

            result_length = libthemis.getValue(result_length_ptr, 'i32')
            result_ptr = libthemis._malloc(result_length)
            if (!result_ptr) {
                throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY)
            }

            status = libthemis._themis_secure_cell_decrypt_token_protect(
                master_key_ptr, this.masterKey.length,
                context_ptr, context.length,
                message_ptr, message.length,
                token_ptr, token.length,
                result_ptr, result_length_ptr
            )
            if (status != ThemisErrorCode.SUCCESS) {
                throw new ThemisError(cryptosystem_name, status)
            }

            result_length = libthemis.getValue(result_length_ptr, 'i32')

            return libthemis.HEAPU8.slice(result_ptr, result_ptr + result_length)
        }
        finally {
            libthemis._memset(master_key_ptr, 0, this.masterKey.length)
            libthemis._free(master_key_ptr)
            libthemis._free(message_ptr)
            libthemis._free(context_ptr)
            libthemis._free(token_ptr)
            libthemis._free(result_ptr)
        }
    }
}
