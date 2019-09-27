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
 * Themis Secure Session.
 */

const libthemis = require('./libthemis.js')
const keygen = require('./secure_keygen.js')
const errors = require('./themis_error.js')
const utils = require('./utils.js')

const cryptosystem_name = 'SecureSession'

const ThemisError = errors.ThemisError
const ThemisErrorCode = errors.ThemisErrorCode

function isFunction(obj) {
    return !!(obj && obj.constructor && obj.call && obj.apply)
}

// Secure Session C API operatates with "secure_session_user_callbacks_t" context structure
// defined in <themis/secure_session.h>. It must be filled in with C function pointers and
// an arbitrary C context pointer. Associated C functions are called by Secure Session
// at appropriate moments. They receive the C context pointer as their last argument which
// allows to restore context.
//
// First, we need to be able to allocate and free "secure_session_user_callbacks_t", and to
// initialize it before passing them to Secure Session constructor.
//
// Here's how it looks in C:
//
//     struct secure_session_user_callbacks_type {
//          send_protocol_data_callback     send_data;
//          receive_protocol_data_callback  receive_data;
//          protocol_state_changed_callback state_changed;
//          get_public_key_for_id_callback  get_public_key_for_id;
//
//          void *user_data;
//     };
//
// On Emscripten target all pointers take up 4 bytes and are aligned at 4-byte boundary.
// Hence the following sizes and offsets:

const sizeof_secure_session_user_callbacks_t = 20
const offsetof_user_data                     = 16
const offsetof_get_public_key_for_id         = 12

// One does not simply pass JavaScript function into Emscripten memory. We need to use
// the "addFunction" API which registers a limited number of JavaScript functions and
// provides corresponding C function pointers to thunks. We need only one callback so
// it's relatively easy for us. We just need to ensure that the function is registered
// only once.
//
// As for the context, we'll save a pointer to "secure_session_user_callbacks_t" itself
// in its "user_data" field.

var getPublicKeyForIdThunkPtr = 0

function initUserCallbacks(callbacksPtr) {
    if (getPublicKeyForIdThunkPtr == 0) {
        // LLVM needs to know the prototype of the corresponding C function.
        // See comment for "getPublicKeyForIdThunk" below.
        getPublicKeyForIdThunkPtr = libthemis.addFunction(getPublicKeyForIdThunk, 'iiiiii')
    }
    libthemis._memset(callbacksPtr, 0, sizeof_secure_session_user_callbacks_t)
    // We're writing LLVM pointers into memory, hence the '*' type specifier
    libthemis.setValue(callbacksPtr + offsetof_get_public_key_for_id, getPublicKeyForIdThunkPtr, '*')
    libthemis.setValue(callbacksPtr + offsetof_user_data, callbacksPtr, '*')
}

// Now we need to be able to identify JavaScript Secure Session object associated with
// a particular "secure_session_user_callbacks_t" instance. Unfortunately, keeping another
// global registry in JavaScript is the only feasible choice because JavaScript object
// memory is managed completely opaquely.

var SecureSessionCallbackRegistry = {}

function registerSecureSession(callbacksPtr, session) {
    SecureSessionCallbackRegistry[callbacksPtr] = session
}

function unregisterSecureSession(callbacksPtr) {
    delete SecureSessionCallbackRegistry[callbacksPtr]
}

function getSecureSession(callbacksPtr) {
    return SecureSessionCallbackRegistry[callbacksPtr]
}

// Finally, here's our JavaScript implementation of the C function callback.
// It has the following prototype:
//
//     int get_public_key_for_id_callback(
//         const void* id,   size_t id_length,
//         void* key_buffer, size_t key_buffer_length,
//         void* user_data
//     );
//
// It is expected to look up a public key corresponding to the provided session ID,
// write the key into provided buffer, and return zero on success. Non-zero return
// values are considered errors.
//
// The function accepts five integer argumentes and returns an integer. Hence its
// LLVM signature is "iiiiii".
//
// We can throw JavaScript exceptions from inside the function, thanks to Emscripten.

const GetPublicKeySuccess = 0
const GetPublicKeyFailure = -1

function getPublicKeyForIdThunk(idPtr, idLen, keyPtr, keyLen, userData) {
    let id = libthemis.HEAPU8.slice(idPtr, idPtr + idLen)
    let session = getSecureSession(userData)

    let publicKey = session.keyCallback(id)
    if (publicKey == null) {
        return GetPublicKeyFailure
    }

    if (!(publicKey instanceof keygen.PublicKey)) {
        throw new ThemisError(cryptosystem_name, ThemisErrorCode.INVALID_PARAMETER,
            'Secure Session callback must return PublicKey or null')
    }
    if (publicKey.length > keyLen) {
        throw new ThemisError(cryptosystem_name, ThemisErrorCode.BUFFER_TOO_SMALL,
            'public key cannot fit into provided buffer')
    }
    utils.heapPutArray(publicKey, keyPtr)

    return GetPublicKeySuccess
}

class SecureSession {
    constructor(sessionID, privateKey, keyCallback) {
        sessionID = utils.coerceToBytes(sessionID)
        if (sessionID.length == 0) {
            throw new ThemisError(cryptosystem_name, ThemisErrorCode.INVALID_PARAMETER,
                'session ID must be not empty')
        }
        if (!(privateKey instanceof keygen.PrivateKey)) {
            throw new ThemisError(cryptosystem_name, ThemisErrorCode.INVALID_PARAMETER,
                'expected PrivateKey as second argument')
        }
        if (!isFunction(keyCallback)) {
            throw new ThemisError(cryptosystem_name, ThemisErrorCode.INVALID_PARAMETER,
                'expected callback as third argument')
        }

        let session_id_ptr, private_key_ptr, callbacks_ptr
        try {
            session_id_ptr = utils.heapAlloc(sessionID.length)
            private_key_ptr = utils.heapAlloc(privateKey.length)
            callbacks_ptr = utils.heapAlloc(sizeof_secure_session_user_callbacks_t)
            if (!session_id_ptr || !private_key_ptr || !callbacks_ptr) {
                throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY)
            }

            utils.heapPutArray(sessionID, session_id_ptr)
            utils.heapPutArray(privateKey, private_key_ptr)

            this.keyCallback = keyCallback
            initUserCallbacks(callbacks_ptr)

            this.sessionPtr = libthemis._secure_session_create(
                session_id_ptr, sessionID.length,
                private_key_ptr, privateKey.length,
                callbacks_ptr
            )
            if (!this.sessionPtr) {
                throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY,
                    'failed to allocate Secure Session')
            }

            registerSecureSession(callbacks_ptr, this)
            this.callbacksPtr = callbacks_ptr
            callbacks_ptr = null
        }
        finally {
            utils.heapFree(private_key_ptr, privateKey.length)
            utils.heapFree(session_id_ptr, sessionID.length)
            utils.heapFree(callbacks_ptr, sizeof_secure_session_user_callbacks_t)
        }
    }

    destroy() {
        let status = libthemis._secure_session_destroy(this.sessionPtr)
        if (status != ThemisErrorCode.SUCCESS) {
            throw new ThemisError(cryptosystem_name, status,
                'failed to destroy Secure Session')
        }
        this.sessionPtr = null

        unregisterSecureSession(this.callbacksPtr)
        utils.heapFree(this.callbacksPtr, sizeof_secure_session_user_callbacks_t)
        this.callbacksPtr = null
    }

    established() {
        let value = libthemis._secure_session_is_established(this.sessionPtr)
        return !(value == 0)
    }

    connectionRequest() {
        let status
        /// C API uses "size_t" for lengths, it's defined as "i32" in Emscripten
        let request_length_ptr = libthemis.allocate(4, 'i32', libthemis.ALLOC_STACK)
        let request_ptr, request_length
        try {
            status = libthemis._secure_session_generate_connect_request(
                this.sessionPtr,
                null, request_length_ptr
            )
            if (status != ThemisErrorCode.BUFFER_TOO_SMALL) {
                throw new ThemisError(cryptosystem_name, status)
            }

            request_length = libthemis.getValue(request_length_ptr, 'i32')
            request_ptr = utils.heapAlloc(request_length)
            if (!request_ptr) {
                throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY)
            }

            status = libthemis._secure_session_generate_connect_request(
                this.sessionPtr,
                request_ptr, request_length_ptr
            )
            if (status != ThemisErrorCode.SUCCESS) {
                throw new ThemisError(cryptosystem_name, status)
            }

            request_length = libthemis.getValue(request_length_ptr, 'i32')

            return utils.heapGetArray(request_ptr, request_length)
        }
        finally {
            utils.heapFree(request_ptr, request_length)
        }
    }

    negotiateReply(message) {
        message = utils.coerceToBytes(message)
        if (message.length == 0) {
            throw new ThemisError(cryptosystem_name, ThemisErrorCode.INVALID_PARAMETER,
                'message must be not empty')
        }

        let status
        /// C API uses "size_t" for lengths, it's defined as "i32" in Emscripten
        let reply_length_ptr = libthemis.allocate(4, 'i32', libthemis.ALLOC_STACK)
        let message_ptr, reply_ptr, reply_length
        try {
            message_ptr = utils.heapAlloc(message.length)
            if (!message_ptr) {
                throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY)
            }

            utils.heapPutArray(message, message_ptr)

            status = libthemis._secure_session_unwrap(
                this.sessionPtr,
                message_ptr, message.length,
                null, reply_length_ptr
            )
            if (status == ThemisErrorCode.SUCCESS) {
                return new Uint8Array()
            }
            if (status != ThemisErrorCode.BUFFER_TOO_SMALL) {
                throw new ThemisError(cryptosystem_name, status)
            }

            reply_length = libthemis.getValue(reply_length_ptr, 'i32')
            reply_ptr = utils.heapAlloc(reply_length)
            if (!reply_ptr) {
                throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY)
            }

            status = libthemis._secure_session_unwrap(
                this.sessionPtr,
                message_ptr, message.length,
                reply_ptr, reply_length_ptr
            )
            if (status != ThemisErrorCode.SSESSION_SEND_OUTPUT_TO_PEER) {
                throw new ThemisError(cryptosystem_name, status)
            }

            reply_length = libthemis.getValue(reply_length_ptr, 'i32')

            return utils.heapGetArray(reply_ptr, reply_length)
        }
        finally {
            utils.heapFree(message_ptr, message.length)
            utils.heapFree(reply_ptr, reply_length)
        }
    }

    wrap(message) {
        message = utils.coerceToBytes(message)
        if (message.length == 0) {
            throw new ThemisError(cryptosystem_name, ThemisErrorCode.INVALID_PARAMETER,
                'message must be not empty')
        }

        let status
        /// C API uses "size_t" for lengths, it's defined as "i32" in Emscripten
        let wrapped_length_ptr = libthemis.allocate(4, 'i32', libthemis.ALLOC_STACK)
        let message_ptr, wrapped_ptr, wrapped_length
        try {
            message_ptr = utils.heapAlloc(message.length)
            if (!message_ptr) {
                throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY)
            }

            utils.heapPutArray(message, message_ptr)

            status = libthemis._secure_session_wrap(
                this.sessionPtr,
                message_ptr, message.length,
                null, wrapped_length_ptr
            )
            if (status != ThemisErrorCode.BUFFER_TOO_SMALL) {
                throw new ThemisError(cryptosystem_name, status)
            }

            wrapped_length = libthemis.getValue(wrapped_length_ptr, 'i32')
            wrapped_ptr = utils.heapAlloc(wrapped_length)
            if (!wrapped_ptr) {
                throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY)
            }

            status = libthemis._secure_session_wrap(
                this.sessionPtr,
                message_ptr, message.length,
                wrapped_ptr, wrapped_length_ptr
            )
            if (status != ThemisErrorCode.SUCCESS) {
                throw new ThemisError(cryptosystem_name, status)
            }

            wrapped_length = libthemis.getValue(wrapped_length_ptr, 'i32')

            return utils.heapGetArray(wrapped_ptr, wrapped_length)
        }
        finally {
            utils.heapFree(message_ptr, message.length)
            utils.heapFree(wrapped_ptr, wrapped_length)
        }
    }

    unwrap(message) {
        message = utils.coerceToBytes(message)
        if (message.length == 0) {
            throw new ThemisError(cryptosystem_name, ThemisErrorCode.INVALID_PARAMETER,
                'message must be not empty')
        }

        let status
        /// C API uses "size_t" for lengths, it's defined as "i32" in Emscripten
        let unwrapped_length_ptr = libthemis.allocate(4, 'i32', libthemis.ALLOC_STACK)
        let message_ptr, unwrapped_ptr, unwrapped_length
        try {
            message_ptr = utils.heapAlloc(message.length)
            if (!message_ptr) {
                throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY)
            }

            utils.heapPutArray(message, message_ptr)

            status = libthemis._secure_session_unwrap(
                this.sessionPtr,
                message_ptr, message.length,
                null, unwrapped_length_ptr
            )
            if (status != ThemisErrorCode.BUFFER_TOO_SMALL) {
                throw new ThemisError(cryptosystem_name, status)
            }

            unwrapped_length = libthemis.getValue(unwrapped_length_ptr, 'i32')
            unwrapped_ptr = utils.heapAlloc(unwrapped_length)
            if (!unwrapped_ptr) {
                throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY)
            }

            status = libthemis._secure_session_unwrap(
                this.sessionPtr,
                message_ptr, message.length,
                unwrapped_ptr, unwrapped_length_ptr
            )
            if (status != ThemisErrorCode.SUCCESS) {
                throw new ThemisError(cryptosystem_name, status)
            }

            unwrapped_length = libthemis.getValue(unwrapped_length_ptr, 'i32')

            return utils.heapGetArray(unwrapped_ptr, unwrapped_length)
        }
        finally {
            utils.heapFree(message_ptr, message.length)
            utils.heapFree(unwrapped_ptr, unwrapped_length)
        }
    }
}

module.exports.SecureSession = SecureSession
