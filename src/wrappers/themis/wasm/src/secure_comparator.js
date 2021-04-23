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
 * Themis Secure Comparator.
 */

const libthemis = require('./libthemis.js')
const errors = require('./themis_error.js')
const utils = require('./utils.js')

const cryptosystem_name = 'SecureComparator'

const ThemisError = errors.ThemisError
const ThemisErrorCode = errors.ThemisErrorCode

class SecureComparator {
    constructor() {
        this.haveSecret = false
        this.comparatorPtr = libthemis._secure_comparator_create()
        if (!this.comparatorPtr) {
            throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY,
                'failed to allocate Secure Comparator')
        }
        for (var i = 0; i < arguments.length; i++) {
            this.append(arguments[i])
        }
    }

    // Unfortunately, JavsScript does not have a ubiquitious way to handle
    // scoped objects and does not specify any object finalization. Thus
    // it is VERY important to call destroy() on SecureComparators after
    // using them in order to avoid exhausting Emscripten heap memory.
    destroy() {
        let status = libthemis._secure_comparator_destroy(this.comparatorPtr)
        if (status != ThemisErrorCode.SUCCESS) {
            throw new ThemisError(cryptosystem_name, status,
                'failed to destroy Secure Comparator')
        }
        this.comparatorPtr = null
    }

    append(secret) {
        secret = utils.coerceToBytes(secret)
        if (secret.length == 0) {
            throw new ThemisError(cryptosystem_name, ThemisErrorCode.INVALID_PARAMETER,
                'secret must be not empty')
        }

        let secret_ptr
        try {
            secret_ptr = utils.heapAlloc(secret.length)
            if (!secret_ptr) {
                throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY)
            }

            utils.heapPutArray(secret, secret_ptr)

            let status = libthemis._secure_comparator_append_secret(
                this.comparatorPtr, secret_ptr, secret.length
            )
            if (status != ThemisErrorCode.SUCCESS) {
                throw new ThemisError(cryptosystem_name, status)
            }
        }
        finally {
            utils.heapFree(secret_ptr, secret.length)
        }

        this.haveSecret = true
    }

    complete() {
        let status = libthemis._secure_comparator_get_result(this.comparatorPtr)
        return (status != ThemisErrorCode.SCOMPARE_NOT_READY)
    }

    compareEqual() {
        let status = libthemis._secure_comparator_get_result(this.comparatorPtr)
        if (status == ThemisErrorCode.SCOMPARE_MATCH) {
            return true
        }
        if (status == ThemisErrorCode.SCOMPARE_NO_MATCH) {
            return false
        }
        throw new ThemisError(cryptosystem_name, status)
    }

    begin() {
        if (!this.haveSecret) {
            throw new ThemisError(cryptosystem_name, ThemisErrorCode.INVALID_PARAMETER,
                'secret must be not empty')
        }

        let status
        /// C API uses "size_t" for lengths, it's defined as "i32" in Emscripten
        let message_length_ptr = libthemis.allocate(new ArrayBuffer(4), libthemis.ALLOC_STACK)
        let message_ptr, message_length
        try {
            status = libthemis._secure_comparator_begin_compare(this.comparatorPtr,
                null, message_length_ptr
            )
            if (status != ThemisErrorCode.BUFFER_TOO_SMALL) {
                throw new ThemisError(cryptosystem_name, status)
            }

            message_length = libthemis.getValue(message_length_ptr, 'i32')
            message_ptr = utils.heapAlloc(message_length)
            if (!message_ptr) {
                throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY)
            }

            status = libthemis._secure_comparator_begin_compare(this.comparatorPtr,
                message_ptr, message_length_ptr
            )
            if (status != ThemisErrorCode.SCOMPARE_SEND_OUTPUT_TO_PEER) {
                throw new ThemisError(cryptosystem_name, status)
            }

            message_length = libthemis.getValue(message_length_ptr, 'i32')

            return utils.heapGetArray(message_ptr, message_length)
        }
        finally {
            utils.heapFree(message_ptr, message_length)
        }
    }

    proceed(request) {
        request = utils.coerceToBytes(request)

        let status
        /// C API uses "size_t" for lengths, it's defined as "i32" in Emscripten
        let reply_length_ptr = libthemis.allocate(new ArrayBuffer(4), libthemis.ALLOC_STACK)
        let request_ptr, reply_ptr, reply_length
        try {
            request_ptr = utils.heapAlloc(request.length)
            if (!request_ptr) {
                throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY)
            }

            utils.heapPutArray(request, request_ptr)

            status = libthemis._secure_comparator_proceed_compare(this.comparatorPtr,
                request_ptr, request.length,
                null, reply_length_ptr
            )
            if (status != ThemisErrorCode.BUFFER_TOO_SMALL) {
                throw new ThemisError(cryptosystem_name, status)
            }

            reply_length = libthemis.getValue(reply_length_ptr, 'i32')
            reply_ptr = utils.heapAlloc(reply_length)
            if (!reply_ptr) {
                throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY)
            }

            status = libthemis._secure_comparator_proceed_compare(this.comparatorPtr,
                request_ptr, request.length,
                reply_ptr, reply_length_ptr
            )
            if ((status != ThemisErrorCode.SCOMPARE_SEND_OUTPUT_TO_PEER)
             && (status != ThemisErrorCode.SUCCESS))
            {
                throw new ThemisError(cryptosystem_name, status)
            }

            reply_length = libthemis.getValue(reply_length_ptr, 'i32')

            return utils.heapGetArray(reply_ptr, reply_length)
        }
        finally {
            utils.heapFree(request_ptr, request.length)
            utils.heapFree(reply_ptr, reply_length)
        }
    }
}

module.exports.SecureComparator = SecureComparator
