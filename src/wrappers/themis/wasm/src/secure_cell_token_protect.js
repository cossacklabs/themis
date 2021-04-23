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

    /**
     * Makes a new Secure Cell in Token Protect mode with given master key.
     *
     * @param masterKey     non-empty array of master key bytes (Buffer or Uint8Array)
     *
     * @returns a new instance of SecureCellTokenProtect.
     *
     * @throws TypeError if the master key is not a byte buffer.
     * @throws ThemisError if the master key is empty.
     */
    static withKey(masterKey) {
        return new SecureCellTokenProtect(masterKey)
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
        /// allocate() with ALLOC_STACK cannot be called multiple times,
        /// but we need two size_t values so allocate an array, of a sort.
        let result_length_ptr = libthemis.allocate(new ArrayBuffer(2 * 4), libthemis.ALLOC_STACK)
        let token_length_ptr = result_length_ptr + 4
        let master_key_ptr, message_ptr, context_ptr, result_ptr, result_length, token_ptr, token_length
        try {
            master_key_ptr = utils.heapAlloc(this.masterKey.length)
            message_ptr = utils.heapAlloc(message.length)
            context_ptr = utils.heapAlloc(context.length)
            if (!master_key_ptr || !message_ptr || !context_ptr) {
                throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY)
            }

            utils.heapPutArray(this.masterKey, master_key_ptr)
            utils.heapPutArray(message, message_ptr)
            utils.heapPutArray(context, context_ptr)

            status = libthemis._themis_secure_cell_encrypt_token_protect(
                master_key_ptr, this.masterKey.length,
                context_ptr, context.length,
                message_ptr, message.length,
                null, token_length_ptr,
                null, result_length_ptr
            )
            if (status != ThemisErrorCode.BUFFER_TOO_SMALL) {
                throw new ThemisError(cryptosystem_name, status)
            }

            result_length = libthemis.getValue(result_length_ptr, 'i32')
            token_length = libthemis.getValue(token_length_ptr, 'i32')
            result_ptr = utils.heapAlloc(result_length)
            token_ptr = utils.heapAlloc(token_length)
            if (!result_ptr || !token_ptr) {
                throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY)
            }

            status = libthemis._themis_secure_cell_encrypt_token_protect(
                master_key_ptr, this.masterKey.length,
                context_ptr, context.length,
                message_ptr, message.length,
                token_ptr, token_length_ptr,
                result_ptr, result_length_ptr
            )
            if (status != ThemisErrorCode.SUCCESS) {
                throw new ThemisError(cryptosystem_name, status)
            }

            result_length = libthemis.getValue(result_length_ptr, 'i32')
            token_length = libthemis.getValue(token_length_ptr, 'i32')

            return {
                data: utils.heapGetArray(result_ptr, result_length),
                token: utils.heapGetArray(token_ptr, token_length)
            }
        }
        finally {
            utils.heapFree(master_key_ptr, this.masterKey.length)
            utils.heapFree(message_ptr, message.length)
            utils.heapFree(context_ptr, context.length)
            utils.heapFree(result_ptr, result_length)
            utils.heapFree(token_ptr, token_length)
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
        let result_length_ptr = libthemis.allocate(new ArrayBuffer(4), libthemis.ALLOC_STACK)
        let master_key_ptr, message_ptr, context_ptr, token_ptr, result_ptr, result_length
        try {
            master_key_ptr = utils.heapAlloc(this.masterKey.length)
            message_ptr = utils.heapAlloc(message.length)
            context_ptr = utils.heapAlloc(context.length)
            token_ptr = utils.heapAlloc(token.length)
            if (!master_key_ptr || !message_ptr || !context_ptr || !token_ptr) {
                throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY)
            }

            utils.heapPutArray(this.masterKey, master_key_ptr)
            utils.heapPutArray(message, message_ptr)
            utils.heapPutArray(context, context_ptr)
            utils.heapPutArray(token, token_ptr)

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
            result_ptr = utils.heapAlloc(result_length)
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

            return utils.heapGetArray(result_ptr, result_length)
        }
        finally {
            utils.heapFree(master_key_ptr, this.masterKey.length)
            utils.heapFree(message_ptr, message.length)
            utils.heapFree(context_ptr, context.length)
            utils.heapFree(result_ptr, result_length)
            utils.heapFree(token_ptr, token.length)
        }
    }
}
