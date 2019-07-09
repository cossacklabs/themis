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
    describe('SecureCell', function() {
        let masterKey1 = new Uint8Array([1, 2, 3, 4])
        let masterKey2 = new Uint8Array([5, 6, 7, 8, 9])
        let emptyArray = new Uint8Array()
        let testInput = new Uint8Array([1, 1, 2, 3, 5, 8, 13])
        let testContext = new Uint8Array([42])
        describe('Seal mode', function() {
            it('does not accept strings', function() {
                // It's a really nice idea to accept strings as 'master keys', but we have
                // to define their interpretetaion. So treat them as errors for now.
                assert.throws(() => new themis.SecureCellSeal('master key'), TypeError)
            })
            it('encrypts without context', function() {
                let cell = new themis.SecureCellSeal(masterKey1)
                let encrypted = cell.encrypt(testInput)
                let decrypted = cell.decrypt(encrypted)
                assert.deepStrictEqual(decrypted, testInput)
            })
            it('encrypts with context', function() {
                let cell = new themis.SecureCellSeal(masterKey1)
                let encrypted = cell.encrypt(testInput, testContext)
                let decrypted = cell.decrypt(encrypted, testContext)
                assert.deepStrictEqual(decrypted, testInput)
            })
            it('produces extended results', function() {
                let cell = new themis.SecureCellSeal(masterKey1)
                let encrypted = cell.encrypt(testInput)
                assert(encrypted.length > testInput.length)
            })
            it('forbits empty inputs', function() {
                assert.throws(() => new themis.SecureCellSeal(emptyArray),
                    expectError(ThemisErrorCode.INVALID_PARAMETER)
                )
                let cell = new themis.SecureCellSeal(masterKey1)
                assert.throws(() => cell.encrypt(emptyArray),
                    expectError(ThemisErrorCode.INVALID_PARAMETER)
                )
            })
            it('empty context == no context', function() {
                let cell = new themis.SecureCellSeal(masterKey1)
                let encrypted = cell.encrypt(testInput)
                let decrypted = cell.decrypt(encrypted, emptyArray)
                assert.deepStrictEqual(decrypted, testInput)
            })
            it('null context == no context', function() {
                let cell = new themis.SecureCellSeal(masterKey1)
                let encrypted = cell.encrypt(testInput, null)
                let decrypted = cell.decrypt(encrypted)
                assert.deepStrictEqual(decrypted, testInput)
            })
            it('detects invalid master key', function() {
                let cell1 = new themis.SecureCellSeal(masterKey1)
                let cell2 = new themis.SecureCellSeal(masterKey2)
                let encrypted = cell1.encrypt(testInput)
                assert.throws(() => cell2.decrypt(encrypted),
                    expectError(ThemisErrorCode.FAIL)
                )
            })
            it('detects invalid context', function() {
                let cell = new themis.SecureCellSeal(masterKey1)
                let encrypted = cell.encrypt(testInput, testContext)
                assert.throws(() => cell.decrypt(encrypted, testInput),
                    expectError(ThemisErrorCode.FAIL)
                )
            })
            it('detects corrupted data', function() {
                let cell = new themis.SecureCellSeal(masterKey1)
                let encrypted = cell.encrypt(testInput)
                encrypted[20] = 256 - encrypted[20]
                assert.throws(() => cell.decrypt(encrypted),
                    expectError(ThemisErrorCode.FAIL)
                )
            })
        })
        describe('Token Protect mode', function() {
            it('does not accept strings', function() {
                // It's a really nice idea to accept strings as 'master keys', but we have
                // to define their interpretetaion. So treat them as errors for now.
                assert.throws(() => new themis.SecureCellTokenProtect('master key'), TypeError)
            })
            it('encrypts without context', function() {
                let cell = new themis.SecureCellTokenProtect(masterKey1)
                let result = cell.encrypt(testInput)
                let decrypted = cell.decrypt(result.data, result.token)
                assert.deepStrictEqual(decrypted, testInput)
            })
            it('encrypts with context', function() {
                let cell = new themis.SecureCellTokenProtect(masterKey1)
                let result = cell.encrypt(testInput, testContext)
                let decrypted = cell.decrypt(result.data, result.token, testContext)
                assert.deepStrictEqual(decrypted, testInput)
            })
            it('does not change encrypted data length', function() {
                let cell = new themis.SecureCellTokenProtect(masterKey1)
                let result = cell.encrypt(testInput)
                assert.strictEqual(result.data.length, testInput.length)
                assert(result.token.length > 0)
            })
            it('forbids empty inputs', function() {
                assert.throws(() => new themis.SecureCellTokenProtect(emptyArray),
                    expectError(ThemisErrorCode.INVALID_PARAMETER)
                )
                let cell = new themis.SecureCellTokenProtect(masterKey1)
                assert.throws(() => cell.encrypt(emptyArray),
                    expectError(ThemisErrorCode.INVALID_PARAMETER)
                )
                let result = cell.encrypt(testInput)
                assert.throws(() => cell.decrypt(emptyArray, result.token),
                    expectError(ThemisErrorCode.INVALID_PARAMETER)
                )
                assert.throws(() => cell.decrypt(result.data, emptyArray),
                    expectError(ThemisErrorCode.INVALID_PARAMETER)
                )
            })
            it('empty context == no context', function() {
                let cell = new themis.SecureCellTokenProtect(masterKey1)
                let result = cell.encrypt(testInput, emptyArray)
                let decrypted = cell.decrypt(result.data, result.token)
                assert.deepStrictEqual(decrypted, testInput)
            })
            it('null context == no context', function() {
                let cell = new themis.SecureCellTokenProtect(masterKey1)
                let result = cell.encrypt(testInput)
                let decrypted = cell.decrypt(result.data, result.token, null)
                assert.deepStrictEqual(decrypted, testInput)
            })
            it('detects incorrect master key', function() {
                let cell1 = new themis.SecureCellTokenProtect(masterKey1)
                let cell2 = new themis.SecureCellTokenProtect(masterKey2)
                let result = cell1.encrypt(testInput)
                assert.throws(() => cell2.decrypt(result.data, result.token),
                    expectError(ThemisErrorCode.FAIL)
                )
            })
            it('detects incorrect token', function() {
                let cell1 = new themis.SecureCellTokenProtect(masterKey1)
                let cell2 = new themis.SecureCellTokenProtect(masterKey2)
                let result1 = cell1.encrypt(testInput)
                let result2 = cell2.encrypt(testInput)
                assert.throws(() => cell1.decrypt(result1.data, result2.token),
                    expectError(ThemisErrorCode.FAIL)
                )
                assert.throws(() => cell2.decrypt(result2.data, result1.token),
                    expectError(ThemisErrorCode.FAIL)
                )
            })
            it('detects incorrect context', function() {
                let cell = new themis.SecureCellTokenProtect(masterKey1)
                let result = cell.encrypt(testInput, testContext)
                assert.throws(() => cell.decrypt(result.data, result.token, testInput),
                    expectError(ThemisErrorCode.FAIL)
                )
            })
            it('detects corrupted data', function() {
                let cell = new themis.SecureCellTokenProtect(masterKey1)
                let result = cell.encrypt(testInput)
                result.data[5] = 256 - result.data[5]
                assert.throws(() => cell.decrypt(result.data, result.token),
                    expectError(ThemisErrorCode.FAIL)
                )
            })
            it('detects corrupted token', function() {
                let cell = new themis.SecureCellTokenProtect(masterKey1)
                let result = cell.encrypt(testInput)
                result.token[8] = 256 - result.token[8]
                assert.throws(() => cell.decrypt(result.data, result.token),
                    expectError(ThemisErrorCode.INVALID_PARAMETER)
                )
            })
        })
        describe('Context Imprint mode', function() {
            it('does not accept strings', function() {
                // It's a really nice idea to accept strings as 'master keys', but we have
                // to define their interpretetaion. So treat them as errors for now.
                assert.throws(() => new themis.SecureCellContextImprint('master key'), TypeError)
            })
            it('encrypts only with context', function() {
                let cell = new themis.SecureCellContextImprint(masterKey1)
                assert.throws(() => cell.encrypt(testInput))
                let encrypted = cell.encrypt(testInput, testContext)
                let decrypted = cell.decrypt(encrypted, testContext)
                assert.deepStrictEqual(decrypted, testInput)
            })
            it('encryption does not change data length', function() {
                let cell = new themis.SecureCellContextImprint(masterKey1)
                let encrypted = cell.encrypt(testInput, testContext)
                assert.equal(encrypted.length, testInput.length)
            })
            it('forbids empty message and context', function() {
                assert.throws(() => new themis.SecureCellContextImprint(emptyArray),
                    expectError(ThemisErrorCode.INVALID_PARAMETER)
                )
                let cell = new themis.SecureCellContextImprint(masterKey1)
                assert.throws(() => cell.encrypt(emptyArray, testContext))
                assert.throws(() => cell.encrypt(null,       testContext))
                assert.throws(() => cell.encrypt(testInput,  emptyArray))
                assert.throws(() => cell.encrypt(testInput,  null))
                assert.throws(() => cell.encrypt(emptyArray, emptyArray))
                assert.throws(() => cell.encrypt(null,       null))
                assert.throws(() => cell.decrypt(emptyArray, testContext))
                assert.throws(() => cell.decrypt(null,       testContext))
                assert.throws(() => cell.decrypt(testInput,  emptyArray))
                assert.throws(() => cell.decrypt(testInput,  null))
                assert.throws(() => cell.decrypt(emptyArray, emptyArray))
                assert.throws(() => cell.decrypt(null,       null))
            })
            it('does not detect incorrect master key', function() {
                let cell1 = new themis.SecureCellContextImprint(masterKey1)
                let cell2 = new themis.SecureCellContextImprint(masterKey2)
                let encrypted = cell1.encrypt(testInput, testContext)
                let decrypted = cell2.decrypt(encrypted, testContext)
                assert.notDeepStrictEqual(testInput, decrypted)
            })
            it('does not detect incorrect context', function() {
                let cell = new themis.SecureCellContextImprint(masterKey1)
                let encrypted = cell.encrypt(testInput, testContext)
                let decrypted = cell.decrypt(encrypted, testInput)
                assert.notDeepStrictEqual(testInput, decrypted)
            })
            it('does not detect corrupted data', function() {
                let cell = new themis.SecureCellContextImprint(masterKey1)
                let encrypted = cell.encrypt(testInput, testContext)
                encrypted[5] = 256 - encrypted[5]
                let decrypted = cell.decrypt(encrypted, testContext)
                assert.notDeepStrictEqual(testInput, decrypted)
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
    describe('Secure Comparator', function() {
        let secretBytes = new Uint8Array([0x73, 0x65, 0x63, 0x72, 0x65, 0x74])
        let randomBytes = new Uint8Array([0x11, 0x22, 0x33, 0x44, 0x55, 0x66])
        it('confirms matching data', function() {
            let comparison1 = new themis.SecureComparator(secretBytes)
            let comparison2 = new themis.SecureComparator(secretBytes)

            let data = comparison1.begin()
            while (!comparison1.complete() && !comparison2.complete()) {
                data = comparison2.proceed(data)
                data = comparison1.proceed(data)
            }

            assert(comparison1.compareEqual())
            assert(comparison2.compareEqual())

            comparison1.destroy()
            comparison2.destroy()
        })
        it('notices different data', function() {
            let comparison1 = new themis.SecureComparator(secretBytes)
            let comparison2 = new themis.SecureComparator(randomBytes)

            let data = comparison1.begin()
            while (!comparison1.complete() && !comparison2.complete()) {
                data = comparison2.proceed(data)
                data = comparison1.proceed(data)
            }

            assert(!comparison1.compareEqual())
            assert(!comparison2.compareEqual())

            comparison1.destroy()
            comparison2.destroy()
        })
        it('allows appending secrets', function() {
            let comparison1 = new themis.SecureComparator(secretBytes)
            let comparison2 = new themis.SecureComparator()
            comparison2.append(new Uint8Array([0x73]))
            comparison2.append(new Uint8Array([0x65, 0x63]))
            comparison2.append(new Uint8Array([0x72, 0x65, 0x74]))

            let data = comparison1.begin()
            while (!comparison1.complete() && !comparison2.complete()) {
                data = comparison2.proceed(data)
                data = comparison1.proceed(data)
            }

            assert(comparison1.compareEqual())
            assert(comparison2.compareEqual())

            comparison1.destroy()
            comparison2.destroy()
        })
        it('handles simultaneous start', function() {
            let comparison1 = new themis.SecureComparator(secretBytes)
            let comparison2 = new themis.SecureComparator(secretBytes)

            let data1 = comparison1.begin()
            let data2 = comparison2.begin()

            assert.throws(() => comparison1.proceed(data2))
            assert.throws(() => comparison2.proceed(data1))
            assert(!comparison1.complete())
            assert(!comparison2.complete())

            comparison1.destroy()
            comparison2.destroy()
        })
        it('does not allow reusing', function() {
            let comparison1 = new themis.SecureComparator(secretBytes)
            let comparison2 = new themis.SecureComparator(secretBytes)

            let data = comparison1.begin()
            while (!comparison1.complete() && !comparison2.complete()) {
                data = comparison2.proceed(data)
                data = comparison1.proceed(data)
            }

            // Cannot restart comparison and append after starting it
            assert.throws(() => comparison1.begin())
            assert.throws(() => comparison2.append(randomBytes))

            comparison1.destroy()
            comparison2.destroy()
        })
        it('does not allow strings', function() {
            // Technically, it is possible to allow strings (e.g., decode them as UTF-8)
            // but for consistency with other wrappers we allow only byte arrays for now.
            assert.throws(() => new themis.SecureComparator('secret'))
            assert.throws(function() {
                let comparison = new themis.SecureComparator()
                comparison.append('secret')
                comparison.destroy()
            })
        })
        it('does not allow empty data', function() {
            let comparison = new themis.SecureComparator()
            assert.throws(() => comparison.begin())
            assert.throws(() => comparison.append(new Uint8Array()))
            comparison.destroy()
        })
    })
})
