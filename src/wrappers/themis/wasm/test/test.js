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
                assert.throws(() => cell.encrypt(testInput,  emptyArray))
                assert.throws(() => cell.encrypt(emptyArray, emptyArray))
                assert.throws(() => cell.decrypt(emptyArray, testContext))
                assert.throws(() => cell.decrypt(testInput,  emptyArray))
                assert.throws(() => cell.decrypt(emptyArray, emptyArray))
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
})
