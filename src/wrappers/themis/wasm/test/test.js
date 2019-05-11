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
    describe('Secure Message', function() {
        let emptyArray = new Uint8Array()
        let testInput = new Uint8Array([1, 1, 2, 3, 5, 8, 13])
        describe('encrypt/decrypt mode', function() {
            it('requires valid keys', function() {
                let keyPair = new themis.KeyPair()
                // It's okay to use key pairs or individual keys
                let ok
                ok = new themis.SecureMessage(keyPair)
                ok = new themis.SecureMessage(keyPair.privateKey, keyPair.publicKey)
                ok = new themis.SecureMessage(keyPair.publicKey, keyPair.privateKey)
                // But they must be private *and* public (or vice versa)
                assert.throws(() => new themis.SecureMessage(keyPair.privateKey, keyPair.privateKey))
                assert.throws(() => new themis.SecureMessage(keyPair.publicKey, keyPair.publicKey))
                // And both must be specified, no nulls allowed
                assert.throws(() => new themis.SecureMessage(keyPair.privateKey))
                assert.throws(() => new themis.SecureMessage(keyPair.publicKey))
                assert.throws(() => new themis.SecureMessage(keyPair.publicKey, null))
                assert.throws(() => new themis.SecureMessage(null, keyPair.privateKey))
                // Byte arrays are not okay, even if they are valid
                let privateKey = new Uint8Array(keyPair.privateKey)
                let publicKey = new Uint8Array(keyPair.publicKey)
                assert.throws(() => new themis.SecureMessage(privateKey, publicKey))
            })
            it('encrypts and decrypts', function() {
                let secureMessage = new themis.SecureMessage(new themis.KeyPair())
                let encrypted = secureMessage.encrypt(testInput)
                let decrypted = secureMessage.decrypt(encrypted)
                assert.deepStrictEqual(testInput, decrypted)
            })
            it('does not allow empty messages', function() {
                let secureMessage = new themis.SecureMessage(new themis.KeyPair())
                assert.throws(() => secureMessage.encrypt(emptyArray))
            })
            it('cannot decrypt with a different key', function() {
                let secureMessageAlpha = new themis.SecureMessage(new themis.KeyPair())
                let secureMessageBravo = new themis.SecureMessage(new themis.KeyPair())
                let encrypted = secureMessageAlpha.encrypt(testInput)
                assert.throws(() => secureMessageBravo.decrypt(encrypted),
                    expectError(ThemisErrorCode.FAIL)
                )
            })
            it('detects corrupted data', function() {
                let secureMessage = new themis.SecureMessage(new themis.KeyPair())
                let encrypted = secureMessage.encrypt(testInput)
                encrypted[10] = 256 - encrypted[10]
                assert.throws(() => secureMessage.decrypt(encrypted),
                    expectError(ThemisErrorCode.FAIL)
                )
            })
        })
        describe('sign/verify mode', function() {
            it('requires valid keys', function() {
                let keyPair = new themis.KeyPair()
                // Signer requires private key and verifier needs public key
                let ok
                ok = new themis.SecureMessageSign(keyPair.privateKey)
                ok = new themis.SecureMessageVerify(keyPair.publicKey)
                // Different key types are not okay
                assert.throws(() => new themis.SecureMessageSign(keyPair.publicKey))
                assert.throws(() => new themis.SecureMessageVerify(keyPair.privateKey))
                // Key pairs are not allowed
                assert.throws(() => new themis.SecureMessageSign(keyPair))
                assert.throws(() => new themis.SecureMessageVerify(keyPair))
                // And raw byte arrays are not allowed too
                let privateKey = new Uint8Array(keyPair.privateKey)
                let publicKey = new Uint8Array(keyPair.publicKey)
                assert.throws(() => new themis.SecureMessageSign(privateKey))
                assert.throws(() => new themis.SecureMessageVerify(publicKey))
            })
            it('signs and verifies', function() {
                let keyPair = new themis.KeyPair()
                let signer = new themis.SecureMessageSign(keyPair.privateKey)
                let verifier = new themis.SecureMessageVerify(keyPair.publicKey)
                let signed = signer.sign(testInput)
                let verified = verifier.verify(signed)
                assert.deepStrictEqual(testInput, verified)
            })
            it('does not allow empty messages', function() {
                let signer = new themis.SecureMessageSign(new themis.KeyPair().privateKey)
                assert.throws(() => signer.sign(emptyArray))
            })
            it('leaves signed data in plaintext', function() {
                let keyPair = new themis.KeyPair()
                let signer = new themis.SecureMessageSign(keyPair.privateKey)
                let verifier = new themis.SecureMessageVerify(keyPair.publicKey)
                let signed = signer.sign(testInput)
                // TODO: there has to be more idiomatic way for this check...
                for (var i = 0; i < signed.length - testInput.length; i++) {
                    let slice = signed.slice(i, i + testInput.length)
                    var allEqual = true
                    for (var j = 0; j < testInput.length; j++){
                        if (slice[j] != testInput[j]) {
                            allEqual = false
                            break
                        }
                    }
                    if (allEqual) {
                        return
                    }
                }
                assert.fail('plaintext not found in signed message')
            })
            it('cannot verify with a different key', function() {
                let signer = new themis.SecureMessageSign(new themis.KeyPair().privateKey)
                let verifier = new themis.SecureMessageVerify(new themis.KeyPair().publicKey)
                let signed = signer.sign(testInput)
                assert.throws(() => verifier.verify(signed),
                    expectError(ThemisErrorCode.FAIL)
                )
            })
            it('detects corrupted data', function() {
                let keyPair = new themis.KeyPair()
                let signer = new themis.SecureMessageSign(keyPair.privateKey)
                let verifier = new themis.SecureMessageVerify(keyPair.publicKey)
                let signed = signer.sign(testInput)
                signed[12] = 256 - signed[12]
                assert.throws(() => verifier.verify(signed),
                    expectError(ThemisErrorCode.FAIL)
                )
            })
        })
    })
})
