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
 * Themis Secure Cell (Context Imprint mode).
 */

const libthemis = require('./libthemis.js')
const errors = require('./themis_error.js')
const utils = require('./utils.js')

const cryptosystem_name = 'SecureCellContextImprint'

const ThemisError = errors.ThemisError
const ThemisErrorCode = errors.ThemisErrorCode

module.exports = class SecureCellContextImprint {
    constructor(masterKey) {
        masterKey = utils.coerceToBytes(masterKey)
        if (masterKey.length == 0) {
            throw new ThemisError(cryptosystem_name, ThemisErrorCode.INVALID_PARAMETER,
                'master key must be not empty')
        }
        this.masterKey = masterKey
    }

    encrypt(message, context) {
        message = utils.coerceToBytes(message)
        if (message.length == 0) {
            throw new ThemisError(cryptosystem_name, ThemisErrorCode.INVALID_PARAMETER,
                'message must be not empty')
        }

        // Other Secure Cell kinds have optional context, so the users are likely to omit
        // the context here as well. Let's produce a more helpful error message instead
        // of an error about 'undefined' being invalid byte buffer type.
        if (context === undefined) {
            throw new ThemisError(cryptosystem_name, ThemisErrorCode.INVALID_PARAMETER,
                'SecureCellContextImprint requires context for encrypting')
        }

        context = utils.coerceToBytes(context)
        if (context.length == 0) {
            throw new ThemisError(cryptosystem_name, ThemisErrorCode.INVALID_PARAMETER,
                'context must be not empty')
        }

        let status
        /// C API uses "size_t" for lengths, it's defined as "i32" in Emscripten
        let result_length_ptr = libthemis.allocate(4, 'i32', libthemis.ALLOC_STACK)
        let master_key_ptr, message_ptr, context_ptr, result_ptr, result_length
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

            status = libthemis._themis_secure_cell_encrypt_context_imprint(
                master_key_ptr, this.masterKey.length,
                message_ptr, message.length,
                context_ptr, context.length,
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

            status = libthemis._themis_secure_cell_encrypt_context_imprint(
                master_key_ptr, this.masterKey.length,
                message_ptr, message.length,
                context_ptr, context.length,
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
            libthemis._free(result_ptr)
        }
    }

    decrypt(message, context) {
        message = utils.coerceToBytes(message)
        if (message.length == 0) {
            throw new ThemisError(cryptosystem_name, ThemisErrorCode.INVALID_PARAMETER,
                'message must be not empty')
        }

        // Other Secure Cell kinds have optional context, so the users are likely to omit
        // the context here as well. Let's produce a more helpful error message instead
        // of an error about 'undefined' being invalid byte buffer type.
        if (context === undefined) {
            throw new ThemisError(cryptosystem_name, ThemisErrorCode.INVALID_PARAMETER,
                'SecureCellContextImprint requires context for decrypting')
        }

        context = utils.coerceToBytes(context)
        if (context.length == 0) {
            throw new ThemisError(cryptosystem_name, ThemisErrorCode.INVALID_PARAMETER,
                'context must be not empty')
        }

        let status
        /// C API uses "size_t" for lengths, it's defined as "i32" in Emscripten
        let result_length_ptr = libthemis.allocate(4, 'i32', libthemis.ALLOC_STACK)
        let master_key_ptr, message_ptr, context_ptr, result_ptr, result_length
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

            status = libthemis._themis_secure_cell_decrypt_context_imprint(
                master_key_ptr, this.masterKey.length,
                message_ptr, message.length,
                context_ptr, context.length,
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

            status = libthemis._themis_secure_cell_decrypt_context_imprint(
                master_key_ptr, this.masterKey.length,
                message_ptr, message.length,
                context_ptr, context.length,
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
            libthemis._free(result_ptr)
        }
    }
}
