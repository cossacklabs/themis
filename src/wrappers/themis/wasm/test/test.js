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

const themis = require('..')
const assert = require('assert')

const ThemisErrorCode = themis.ThemisErrorCode

function expectError(errorCode) {
    return function(error) {
        if (error instanceof themis.ThemisError) {
            return error.errorCode == errorCode
        }
        return false
    }
}

describe('wasm-themis', function() {
    describe('KeyPair', function() {
        it('generates EC key pairs', function() {
            let pair = new themis.KeyPair()
            assert(pair.privateKey.length > 0)
            assert(pair.publicKey.length > 0)
        })
        it('reconstructs key pairs', function() {
            let thatPair = new themis.KeyPair()
            let thisPair = new themis.KeyPair(thatPair.privateKey, thatPair.publicKey)
            assert.deepStrictEqual(thisPair.privateKey, thatPair.privateKey)
            assert.deepStrictEqual(thisPair.publicKey, thatPair.publicKey)
        })
        it('validates key kinds', function() {
            let thatPair = new themis.KeyPair()
            // Arguably, we could accept inverted order, as long as the keys
            // are not both public or private. But let's be strict for now.
            assert.throws(
                function() {
                    new themis.KeyPair(thatPair.publicKey, thatPair.privateKey)
                },
                expectError(ThemisErrorCode.INVALID_PARAMETER)
            )
        })
        describe('invididual keys', function() {
            it('ensure matching kinds', function() {
                let pair = new themis.KeyPair()
                assert.throws(() => new themis.PrivateKey(pair.publicKey))
                assert.throws(() => new themis.PublicKey(pair.privateKey))
            })
            it('do not allow empty keys', function() {
                assert.throws(() => new themis.PrivateKey(new Uint8Array()))
                assert.throws(() => new themis.PublicKey(new Uint8Array()))
            })
            it('check strict types', function() {
                let key = new themis.KeyPair().publicKey
                assert.deepStrictEqual(key, new themis.PublicKey(new Uint8Array(key)))
                assert.deepStrictEqual(key, new themis.PublicKey(key.buffer))
                assert.throws(() => new themis.PublicKey(new Int8Array(key)))
            })
            it('do not accept strings', function() {
                let base64key = 'UkVDMgAAAC1JhwRrAPIGB33HHFmhjzn8lIE/nsW6cG+TCI3jhYJb+D/Gnwvf'
                assert.throws(() => new themis.PrivateKey(base64key))
            })
            it('detect data corruption', function() {
                let key = new themis.KeyPair().privateKey
                key[20] = 256 - key[20]
                assert.throws(() => new themis.PrivateKey(key))
            })
        })
    })
    describe('Secure Session', function() {
        let emptyArray = new Uint8Array()
        let randomID = new Uint8Array([0x72, 0x61, 0x6E, 0x64, 0x6F, 0x6D])
        let clientID = new Uint8Array([0x63, 0x6C, 0x69, 0x65, 0x6E, 0x74])
        let serverID = new Uint8Array([0x73, 0x65, 0x72, 0x76, 0x65, 0x72])
        let messageA = new Uint8Array([0x6D, 0x65, 0x73, 0x73, 0x61, 0x67, 0x65])
        let messageB = new Uint8Array([0x73, 0x65, 0x63, 0x72, 0x65, 0x74, 0x73])
        // Really nice of JavaScript to not have a standard library...
        function arraysEqual(a, b) {
            if (a === b) return true;
            if (a == null || b == null) return false;
            if (a.length != b.length) return false;
            for (var i = 0; i < a.length; ++i) {
                if (a[i] !== b[i]) return false;
            }
            return true;
        }
        function makeKeyMaterial() {
            let keys = {}
            keys.client = new themis.KeyPair()
            keys.server = new themis.KeyPair()
            keys.clientCallback = function(id) {
                if (arraysEqual(id, serverID)) {
                    return keys.server.publicKey
                }
                return null
            }
            keys.serverCallback = function(id) {
                if (arraysEqual(id, clientID)) {
                    return keys.client.publicKey
                }
                return null
            }
            return keys
        }
        it('establishes communication', function() {
            let keys = makeKeyMaterial()
            let client = new themis.SecureSession(clientID, keys.client.privateKey, keys.clientCallback)
            let server = new themis.SecureSession(serverID, keys.server.privateKey, keys.serverCallback)

            let data = client.connectionRequest()
            let peer = server
            while (!(client.established() && server.established())) {
                data = peer.negotiateReply(data)
                if (peer == server) {
                    peer = client
                } else {
                    peer = server
                }
            }

            let wrappedMessageA = client.wrap(messageA)
            let unwrappedMessageA = server.unwrap(wrappedMessageA)
            assert.deepStrictEqual(unwrappedMessageA, messageA)

            let wrappedMessageB = server.wrap(messageB)
            let unwrappedMessageB = client.unwrap(wrappedMessageB)
            assert.deepStrictEqual(unwrappedMessageB, messageB)

            client.destroy()
            server.destroy()
        })
        it('handles unknown clients', function() {
            let keys = makeKeyMaterial()
            let client = new themis.SecureSession(randomID, keys.client.privateKey, keys.clientCallback)
            let server = new themis.SecureSession(serverID, keys.server.privateKey, keys.serverCallback)

            let request = client.connectionRequest()
            assert.throws(() => server.negotiateReply(request),
                expectError(ThemisErrorCode.SSESSION_GET_PUB_FOR_ID_CALLBACK_ERROR)
            )

            client.destroy()
            server.destroy()
        })
        it('handles unknown servers', function() {
            let keys = makeKeyMaterial()
            let client = new themis.SecureSession(clientID, keys.client.privateKey, keys.clientCallback)
            let server = new themis.SecureSession(randomID, keys.server.privateKey, keys.serverCallback)

            let request = client.connectionRequest()
            let reply = server.negotiateReply(request)
            assert.throws(() => client.negotiateReply(reply),
                expectError(ThemisErrorCode.SSESSION_GET_PUB_FOR_ID_CALLBACK_ERROR)
            )

            client.destroy()
            server.destroy()
        })
        it('does not allow empty client ID', function() {
            let keyPair = new themis.KeyPair()
            assert.throws(() => new themis.SecureSession(emptyArray, keyPair.privateKey, function(){}),
                expectError(ThemisErrorCode.INVALID_PARAMETER)
            )
        })
        it('does not allow empty message', function() {
            let keys = makeKeyMaterial()
            let client = new themis.SecureSession(clientID, keys.client.privateKey, keys.clientCallback)
            let server = new themis.SecureSession(serverID, keys.server.privateKey, keys.serverCallback)

            let data = client.connectionRequest()
            let peer = server
            while (!(client.established() && server.established())) {
                data = peer.negotiateReply(data)
                if (peer == server) {
                    peer = client
                } else {
                    peer = server
                }
            }

            assert.throws(() => server.wrap(emptyArray),
                expectError(ThemisErrorCode.INVALID_PARAMETER)
            )

            client.destroy()
            server.destroy()
        })
        it('does not allow public keys in constructor', function() {
            let keyPair = new themis.KeyPair()
            assert.throws(() => new themis.SecureSession(clientID, keyPair.publicKey, function(){}),
                expectError(ThemisErrorCode.INVALID_PARAMETER)
            )
        })
        it('does not allow private keys in callback', function() {
            let keys = makeKeyMaterial()
            let client = new themis.SecureSession(clientID, keys.client.privateKey, function(id) {
                if (arraysEqual(id, serverID)) {
                    return keys.server.privateKey
                }
            })
            let server = new themis.SecureSession(serverID, keys.server.privateKey, keys.serverCallback)

            let request = client.connectionRequest()
            let reply = server.negotiateReply(request)
            assert.throws(() => client.negotiateReply(reply),
                expectError(ThemisErrorCode.INVALID_PARAMETER)
            )

            client.destroy()
            server.destroy()

        })
        it('handles exception in callback', function() {
            let keys = makeKeyMaterial()
            let client = new themis.SecureSession(randomID, keys.client.privateKey, keys.clientCallback)
            let server = new themis.SecureSession(serverID, keys.server.privateKey, function(id) {
                throw "a ball"
            })

            let request = client.connectionRequest()
            assert.throws(() => server.negotiateReply(request), "a ball")

            client.destroy()
            server.destroy()
        })
    })
})
