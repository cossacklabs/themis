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
 * Themis secure key generation.
 */

const libthemis = require('./libthemis.js')
const errors = require('./themis_error.js')
const utils = require('./utils.js')

const cryptosystem_name = 'KeyPair'

const ThemisError = errors.ThemisError
const ThemisErrorCode = errors.ThemisErrorCode

/**
 * Themis key kinds.
 *
 * Keep in sync with <themis/secure_keygen.h>.
 */
const keyKinds = {
    INVALID:        0,
    RSA_PRIVATE:    1,
    RSA_PUBLIC:     2,
    EC_PRIVATE:     3,
    EC_PUBLIC:      4,
}

class PrivateKey extends Uint8Array {
    constructor(array) {
        array = utils.coerceToBytes(array)
        super(array)
        validateKeyBuffer(this, [keyKinds.EC_PRIVATE, keyKinds.RSA_PRIVATE])
    }
}

class PublicKey extends Uint8Array {
    constructor(array) {
        array = utils.coerceToBytes(array)
        super(array)
        validateKeyBuffer(this, [keyKinds.EC_PUBLIC, keyKinds.RSA_PUBLIC])
    }
}

function validateKeyBuffer(buffer, expectedKinds) {
    if (buffer.length == 0) {
        throw new ThemisError(cryptosystem_name, ThemisErrorCode.INVALID_PARAMETER,
            'key must not be empty')
    }

    // Calling C functions requires copying the key into Emscripten heap,
    // so we validate the key and query its kind as a single operation
    // to avoid copying the key twice.
    let buffer_len = buffer.length
    let buffer_ptr = utils.heapAlloc(buffer_len)
    if (!buffer_ptr) {
        throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY)
    }

    var kind
    try {
        utils.heapPutArray(buffer, buffer_ptr)

        let err = libthemis._themis_is_valid_asym_key(buffer_ptr, buffer_len)
        if (err != ThemisErrorCode.SUCCESS) {
            throw new ThemisError(cryptosystem_name, err, 'invalid key')
        }

        kind = libthemis._themis_get_asym_key_kind(buffer_ptr, buffer_len)
    }
    finally {
        utils.heapFree(buffer_ptr, buffer_len)
    }

    if (!expectedKinds.includes(kind)) {
        throw new ThemisError(cryptosystem_name, ThemisErrorCode.INVALID_PARAMETER,
            'invalid key kind')
    }
}

class KeyPair {
    constructor() {
        var privateKey, publicKey

        // ECMAScript6 classes does not support private properties (yet),
        // so we have to implement them manually with closures.
        Object.defineProperty(this, 'privateKey', { get: () => privateKey })
        Object.defineProperty(this, 'publicKey', { get: () => publicKey })

        if (arguments.length == 2) {
            privateKey = new PrivateKey(arguments[0])
            publicKey = new PublicKey(arguments[1])
            return
        }

        if (arguments.length == 0) {
            let keyPair = generateECKeyPair()
            privateKey = new PrivateKey(keyPair.private)
            publicKey = new PublicKey(keyPair.public)
            return
        }

        throw new ThemisError(cryptosystem_name, ThemisErrorCode.INVALID_PARAMETER,
            'invalid argument count: expected either no arguments, or private and public keys')
    }
}

function generateECKeyPair() {
    var err

    // C API uses "size_t" for lengths, it's defined as "i32" on Emscripten
    let private_len_ptr = libthemis.allocate(4, 'i32', libthemis.ALLOC_STACK)
    let public_len_ptr = libthemis.allocate(4, 'i32', libthemis.ALLOC_STACK)

    err = libthemis._themis_gen_ec_key_pair(null, private_len_ptr, null, public_len_ptr)
    if (err != ThemisErrorCode.BUFFER_TOO_SMALL) {
        throw new ThemisError(cryptosystem_name, err)
    }

    let private_len = libthemis.getValue(private_len_ptr, 'i32')
    let public_len = libthemis.getValue(public_len_ptr, 'i32')

    let private_ptr = utils.heapAlloc(private_len)
    let public_ptr = utils.heapAlloc(public_len)

    try {
        if (!private_ptr || !public_ptr) {
            throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY)
        }

        err = libthemis._themis_gen_ec_key_pair(private_ptr, private_len_ptr, public_ptr, public_len_ptr)
        if (err != ThemisErrorCode.SUCCESS) {
            throw new ThemisError(cryptosystem_name, err, 'failed to generate key pair')
        }

        let private_len = libthemis.getValue(private_len_ptr, 'i32')
        let public_len = libthemis.getValue(public_len_ptr, 'i32')

        return {
            private: utils.heapGetArray(private_ptr, private_len),
            public: utils.heapGetArray(public_ptr, public_len),
        }
    }
    finally {
        utils.heapFree(private_ptr, private_len)
        utils.heapFree(public_ptr, public_len)
    }
}

module.exports.KeyPair = KeyPair
module.exports.PrivateKey = PrivateKey
module.exports.PublicKey = PublicKey
