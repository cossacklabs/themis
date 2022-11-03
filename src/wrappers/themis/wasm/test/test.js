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

const themis = require('../src/index.ts')
const assert = require('assert')
const { performance } = require('perf_hooks')

const ThemisErrorCode = themis.ThemisErrorCode

function expectError(errorCode) {
    return function(error) {
        if (error.errorCode) {
            return error.errorCode === errorCode
        }
        return false
    }
}

function measureTime(thunk) {
    let t1 = performance.now()
    thunk()
    let t2 = performance.now()
    return t2 - t1
}

describe('wasm-themis', function() {
    let generallyInvalidArguments = [
        null, undefined, 'string',
        new Int16Array([1, 2, 3]), [4, 5, 6],
        () => new Uint8Array([27, 18, 28, 18, 28]),
        { value: [3, 14, 15, 92, 6] }
    ]
    describe('initialization', function() {
        it('resolves "initialized" promise', function(done) {
            themis.initialized.then(function() {
                done()
            })
        })
    })
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
        it('handles type mismatches', function() {
            let keyPair = new themis.KeyPair()
            generallyInvalidArguments.forEach(function(invalid) {
                assert.throws(() => new themis.KeyPair(keyPair.privateKey, invalid), TypeError)
                assert.throws(() => new themis.KeyPair(invalid, keyPair.publicKey), TypeError)
            })
        })
        describe('individual keys', function() {
            it('ensure matching kinds', function() {
                let pair = new themis.KeyPair()
                assert.throws(() => new themis.PrivateKey(pair.publicKey))
                assert.throws(() => new themis.PublicKey(pair.privateKey))
            })
            it('do not allow empty keys', function() {
                assert.throws(() => new themis.PrivateKey(new Uint8Array()))
                assert.throws(() => new themis.PublicKey(new Uint8Array()))
                assert.throws(() => new themis.PrivateKey(''))
                assert.throws(() => new themis.PublicKey(''))
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
                key[20] = ~key[20]
                assert.throws(() => new themis.PrivateKey(key))
            })
            it('handles type mismatches', function() {
                generallyInvalidArguments.forEach(function(invalid) {
                    assert.throws(() => new themis.PrivateKey(invalid), TypeError)
                    assert.throws(() => new themis.PublicKey(invalid), TypeError)
                })
            })
            it('keys are Uint8Arrays', function() {
                let pair = new themis.KeyPair()
                assert.ok(pair.privateKey instanceof themis.PrivateKey)
                assert.ok(pair.privateKey instanceof Uint8Array)
                assert.ok(pair.publicKey instanceof themis.PublicKey)
                assert.ok(pair.publicKey instanceof Uint8Array)
            })
        })
    })
    describe('SecureCell', function() {
        describe('key generation', function() {
            const defaultLength = 32
            it('generates new keys', function() {
                let key = new themis.SymmetricKey()
                assert.equal(key.length, defaultLength)
            })
            it('wraps existing keys', function() {
                let buffer = new Uint8Array([1, 2, 3, 4])
                let key = new themis.SymmetricKey(buffer)
                assert.deepEqual(key, buffer)
            })
            it('fails with empty buffer', function() {
                assert.throws(() => new themis.SymmetricKey(new Uint8Array()),
                    expectError(ThemisErrorCode.INVALID_PARAMETER)
                )
            })
            it('fails with invalid types', function() {
                generallyInvalidArguments.forEach(function(invalid) {
                    // TypeScript implementation handles "undefined" as "no argument".
                    // themis.SymmetricKey() is a valid constructor, so allow this.
                    if (invalid === undefined) {
                        return
                    }
                    assert.throws(() => new themis.SymmetricKey(invalid),
                        TypeError
                    )
                })
            })
            it('keys are Uint8Arrays', function() {
                let key = new themis.SymmetricKey()
                assert.ok(key instanceof themis.SymmetricKey)
                assert.ok(key instanceof Uint8Array)
            })
        })
        let masterKey1 = new Uint8Array([1, 2, 3, 4])
        let masterKey2 = new Uint8Array([5, 6, 7, 8, 9])
        let passphrase1 = 'open sesame'
        let passphrase2 = 'pretty please'
        let emptyArray = new Uint8Array()
        let testInput = new Uint8Array([1, 1, 2, 3, 5, 8, 13])
        let testContext = new Uint8Array([42])
        describe('Seal mode', function() {
            it('does not accept strings', function() {
                assert.throws(() => themis.SecureCellSeal.withKey('master key'), TypeError)
            })
            it('encrypts without context', function() {
                let cell = themis.SecureCellSeal.withKey(masterKey1)
                let encrypted = cell.encrypt(testInput)
                let decrypted = cell.decrypt(encrypted)
                assert.deepStrictEqual(decrypted, testInput)
            })
            it('encrypts with context', function() {
                let cell = themis.SecureCellSeal.withKey(masterKey1)
                let encrypted = cell.encrypt(testInput, testContext)
                let decrypted = cell.decrypt(encrypted, testContext)
                assert.deepStrictEqual(decrypted, testInput)
            })
            it('produces extended results', function() {
                let cell = themis.SecureCellSeal.withKey(masterKey1)
                let encrypted = cell.encrypt(testInput)
                assert(encrypted.length > testInput.length)
            })
            it('forbids empty inputs', function() {
                assert.throws(() => themis.SecureCellSeal.withKey(emptyArray),
                    expectError(ThemisErrorCode.INVALID_PARAMETER)
                )
                let cell = themis.SecureCellSeal.withKey(masterKey1)
                assert.throws(() => cell.encrypt(emptyArray),
                    expectError(ThemisErrorCode.INVALID_PARAMETER)
                )
            })
            it('empty context == no context', function() {
                let cell = themis.SecureCellSeal.withKey(masterKey1)
                let encrypted = cell.encrypt(testInput)
                let decrypted = cell.decrypt(encrypted, emptyArray)
                assert.deepStrictEqual(decrypted, testInput)
            })
            it('null context == no context', function() {
                let cell = themis.SecureCellSeal.withKey(masterKey1)
                let encrypted = cell.encrypt(testInput, null)
                let decrypted = cell.decrypt(encrypted)
                assert.deepStrictEqual(decrypted, testInput)
            })
            it('undefined context == no context', function() {
                let cell = themis.SecureCellSeal.withKey(masterKey1)
                let encrypted = cell.encrypt(testInput, undefined)
                let decrypted = cell.decrypt(encrypted, undefined)
                assert.deepStrictEqual(decrypted, testInput)
            })
            it('omitted context == no context', function() {
                let cell = themis.SecureCellSeal.withKey(masterKey1)
                let encrypted = cell.encrypt(testInput)
                let decrypted = cell.decrypt(encrypted)
                assert.deepStrictEqual(decrypted, testInput)
            })

            it('detects invalid master key', function() {
                let cell1 = themis.SecureCellSeal.withKey(masterKey1)
                let cell2 = themis.SecureCellSeal.withKey(masterKey2)
                let encrypted = cell1.encrypt(testInput)
                assert.throws(() => cell2.decrypt(encrypted),
                    expectError(ThemisErrorCode.FAIL)
                )
            })
            it('detects invalid context', function() {
                let cell = themis.SecureCellSeal.withKey(masterKey1)
                let encrypted = cell.encrypt(testInput, testContext)
                assert.throws(() => cell.decrypt(encrypted, testInput),
                    expectError(ThemisErrorCode.FAIL)
                )
            })
            it('detects corrupted data', function() {
                let cell = themis.SecureCellSeal.withKey(masterKey1)
                let encrypted = cell.encrypt(testInput)
                encrypted[20] = ~encrypted[20]
                assert.throws(() => cell.decrypt(encrypted),
                    expectError(ThemisErrorCode.FAIL)
                )
            })
            it('handles type mismatches', function() {
                let cell = themis.SecureCellSeal.withKey(masterKey1)
                let encrypted = cell.encrypt(testInput)
                generallyInvalidArguments.forEach(function(invalid) {
                    assert.throws(() => themis.SecureCellSeal.withKey(invalid), TypeError)
                    assert.throws(() => cell.encrypt(invalid), TypeError)
                    assert.throws(() => cell.decrypt(invalid), TypeError)
                    // null context is okay, it should not throw
                    // undefined is interpreted as omitted context, it's okay too
                    if (invalid !== null && invalid !== undefined) {
                        assert.throws(() => cell.encrypt(testInput, invalid), TypeError)
                        assert.throws(() => cell.decrypt(encrypted, invalid), TypeError)
                    }
                })
            })
            it('supports deprecated constructor API', function() {
                let cellA = themis.SecureCellSeal.withKey(masterKey1)
                let cellB = new themis.SecureCellSeal(masterKey1)
                let encrypted = cellA.encrypt(testInput)
                let decrypted = cellB.decrypt(encrypted)
                assert.deepStrictEqual(decrypted, testInput)
            })
        })
        describe('Seal mode (passphrase)', function() {
            // Passphrase API uses KDF so it can be quite slow.
            // Mocha uses default threshold of 75 ms which is not enough.
            this.slow(1500) // milliseconds
            const bottomLimit = 100

            it('encrypts without context', function() {
                let cell = themis.SecureCellSeal.withPassphrase(passphrase1)
                let encrypted = cell.encrypt(testInput)
                let decrypted = cell.decrypt(encrypted)
                assert.deepStrictEqual(decrypted, testInput)
            })
            it('encrypts with context', function() {
                let cell = themis.SecureCellSeal.withPassphrase(passphrase1)
                let encrypted = cell.encrypt(testInput, testContext)
                let decrypted = cell.decrypt(encrypted, testContext)
                assert.deepStrictEqual(decrypted, testInput)
            })
            it('produces extended results', function() {
                let cell = themis.SecureCellSeal.withPassphrase(passphrase1)
                let encrypted = cell.encrypt(testInput)
                assert(encrypted.length > testInput.length)
            })
            it('forbids empty inputs', function() {
                assert.throws(() => themis.SecureCellSeal.withPassphrase(''),
                    expectError(ThemisErrorCode.INVALID_PARAMETER)
                )
                assert.throws(() => themis.SecureCellSeal.withPassphrase(emptyArray),
                    expectError(ThemisErrorCode.INVALID_PARAMETER)
                )
                let cell = themis.SecureCellSeal.withPassphrase(passphrase1)
                assert.throws(() => cell.encrypt(emptyArray),
                    expectError(ThemisErrorCode.INVALID_PARAMETER)
                )
            })
            it('empty context == no context', function() {
                let cell = themis.SecureCellSeal.withPassphrase(passphrase1)
                let encrypted = cell.encrypt(testInput)
                let decrypted = cell.decrypt(encrypted, emptyArray)
                assert.deepStrictEqual(decrypted, testInput)
            })
            it('null context == no context', function() {
                let cell = themis.SecureCellSeal.withPassphrase(passphrase1)
                let encrypted = cell.encrypt(testInput, null)
                let decrypted = cell.decrypt(encrypted)
                assert.deepStrictEqual(decrypted, testInput)
            })
            it('undefined context == no context', function() {
                let cell = themis.SecureCellSeal.withPassphrase(passphrase1)
                let encrypted = cell.encrypt(testInput, undefined)
                let decrypted = cell.decrypt(encrypted, undefined)
                assert.deepStrictEqual(decrypted, testInput)
            })
            it('omitted context == no context', function() {
                let cell = themis.SecureCellSeal.withPassphrase(passphrase1)
                let encrypted = cell.encrypt(testInput)
                let decrypted = cell.decrypt(encrypted)
                assert.deepStrictEqual(decrypted, testInput)
            })
            it('detects invalid passphrase', function() {
                let cell1 = themis.SecureCellSeal.withPassphrase(passphrase1)
                let cell2 = themis.SecureCellSeal.withPassphrase(passphrase2)
                let encrypted = cell1.encrypt(testInput)
                assert.throws(() => cell2.decrypt(encrypted),
                    expectError(ThemisErrorCode.FAIL)
                )
            })
            it('detects invalid context', function() {
                let cell = themis.SecureCellSeal.withPassphrase(passphrase1)
                let encrypted = cell.encrypt(testInput, testContext)
                assert.throws(() => cell.decrypt(encrypted, testInput),
                    expectError(ThemisErrorCode.FAIL)
                )
            })
            it('detects corrupted data', function() {
                let cell = themis.SecureCellSeal.withPassphrase(passphrase1)
                let encrypted = cell.encrypt(testInput)
                encrypted[20] = ~encrypted[20]
                assert.throws(() => cell.decrypt(encrypted),
                    expectError(ThemisErrorCode.FAIL)
                )
            })
            it('accepts byte arrays', function() {
                let cell = themis.SecureCellSeal.withPassphrase(masterKey1)
                let encrypted = cell.encrypt(testInput)
                let decrypted = cell.decrypt(encrypted)
                assert.deepStrictEqual(decrypted, testInput)
            })
            it('passphrase is not master key', function() {
                let cellMK = themis.SecureCellSeal.withKey(masterKey1)
                let cellPW = themis.SecureCellSeal.withPassphrase(masterKey1)
                let encryptedMK = cellMK.encrypt(testInput)
                assert.throws(() => cellPW.decrypt(encryptedMK),
                    expectError(ThemisErrorCode.FAIL)
                )
                let encryptedPW = cellPW.encrypt(testInput)
                assert.throws(() => cellMK.decrypt(encryptedPW),
                    expectError(ThemisErrorCode.FAIL)
                )
            })
            it('handles type mismatches', function() {
                let cell = themis.SecureCellSeal.withPassphrase(passphrase1)
                let encrypted = cell.encrypt(testInput)
                generallyInvalidArguments.forEach(function(invalid) {
                    // strings are okay (only) for passphrase API
                    if (typeof invalid !== 'string') {
                        assert.throws(() => themis.SecureCellSeal.withPassphrase(invalid), TypeError)
                    }
                    assert.throws(() => cell.encrypt(invalid), TypeError)
                    assert.throws(() => cell.decrypt(invalid), TypeError)
                    // null context is okay, it should not throw
                    if (invalid !== null && invalid !== undefined) {
                        assert.throws(() => cell.encrypt(testInput, invalid), TypeError)
                        assert.throws(() => cell.decrypt(encrypted, invalid), TypeError)
                    }
                })
            })
            it('takes its time to process data', function() {
                // KDF deliberately slows down encryption and decryption.
                // Make sure WebAssembly keeps it slow enough but not too slow.
                let cell = themis.SecureCellSeal.withPassphrase(passphrase1)
                var encrypted, decrypted
                let timeEncrypt = measureTime(() => encrypted = cell.encrypt(testInput))
                let timeDecrypt = measureTime(() => decrypted = cell.decrypt(encrypted))
                assert.deepStrictEqual(decrypted, testInput)
                assert(timeEncrypt >= bottomLimit)
                assert(timeDecrypt >= bottomLimit)
            })
        })
        describe('Token Protect mode', function() {
            it('does not accept strings', function() {
                assert.throws(() => themis.SecureCellTokenProtect.withKey('master key'), TypeError)
            })
            it('encrypts without context', function() {
                let cell = themis.SecureCellTokenProtect.withKey(masterKey1)
                let result = cell.encrypt(testInput)
                let decrypted = cell.decrypt(result.data, result.token)
                assert.deepStrictEqual(decrypted, testInput)
            })
            it('encrypts with context', function() {
                let cell = themis.SecureCellTokenProtect.withKey(masterKey1)
                let result = cell.encrypt(testInput, testContext)
                let decrypted = cell.decrypt(result.data, result.token, testContext)
                assert.deepStrictEqual(decrypted, testInput)
            })
            it('does not change encrypted data length', function() {
                let cell = themis.SecureCellTokenProtect.withKey(masterKey1)
                let result = cell.encrypt(testInput)
                assert.strictEqual(result.data.length, testInput.length)
                assert(result.token.length > 0)
            })
            it('forbids empty inputs', function() {
                assert.throws(() => themis.SecureCellTokenProtect.withKey(emptyArray),
                    expectError(ThemisErrorCode.INVALID_PARAMETER)
                )
                let cell = themis.SecureCellTokenProtect.withKey(masterKey1)
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
                let cell = themis.SecureCellTokenProtect.withKey(masterKey1)
                let result = cell.encrypt(testInput, emptyArray)
                let decrypted = cell.decrypt(result.data, result.token)
                assert.deepStrictEqual(decrypted, testInput)
            })
            it('null context == no context', function() {
                let cell = themis.SecureCellTokenProtect.withKey(masterKey1)
                let result = cell.encrypt(testInput)
                let decrypted = cell.decrypt(result.data, result.token, null)
                assert.deepStrictEqual(decrypted, testInput)
            })
            it('detects incorrect master key', function() {
                let cell1 = themis.SecureCellTokenProtect.withKey(masterKey1)
                let cell2 = themis.SecureCellTokenProtect.withKey(masterKey2)
                let result = cell1.encrypt(testInput)
                assert.throws(() => cell2.decrypt(result.data, result.token),
                    expectError(ThemisErrorCode.FAIL)
                )
            })
            it('detects incorrect token', function() {
                let cell1 = themis.SecureCellTokenProtect.withKey(masterKey1)
                let cell2 = themis.SecureCellTokenProtect.withKey(masterKey2)
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
                let cell = themis.SecureCellTokenProtect.withKey(masterKey1)
                let result = cell.encrypt(testInput, testContext)
                assert.throws(() => cell.decrypt(result.data, result.token, testInput),
                    expectError(ThemisErrorCode.FAIL)
                )
            })
            it('detects corrupted data', function() {
                let cell = themis.SecureCellTokenProtect.withKey(masterKey1)
                let result = cell.encrypt(testInput)
                result.data[5] = ~result.data[5]
                assert.throws(() => cell.decrypt(result.data, result.token),
                    expectError(ThemisErrorCode.FAIL)
                )
            })
            it('detects corrupted token', function() {
                let cell = themis.SecureCellTokenProtect.withKey(masterKey1)
                let result = cell.encrypt(testInput)
                result.token[8] = ~result.token[8]
                assert.throws(() => cell.decrypt(result.data, result.token),
                    expectError(ThemisErrorCode.FAIL)
                )
            })
            it('handles type mismatches', function() {
                let cell = themis.SecureCellTokenProtect.withKey(masterKey1)
                let result = cell.encrypt(testInput)
                generallyInvalidArguments.forEach(function(invalid) {
                    assert.throws(() => themis.SecureCellTokenProtect.withKey(invalid), TypeError)
                    assert.throws(() => cell.encrypt(invalid), TypeError)
                    assert.throws(() => cell.decrypt(result.data, invalid), TypeError)
                    assert.throws(() => cell.decrypt(invalid, result.token), TypeError)
                    // null context is okay, it should not throw
                    if (invalid !== null) {
                        assert.throws(() => cell.encrypt(testInput, invalid), TypeError)
                        assert.throws(() => cell.decrypt(result.data, result.token, invalid), TypeError)
                    }
                })
            })
            it('supports deprecated constructor API', function() {
                let cellA = themis.SecureCellTokenProtect.withKey(masterKey1)
                let cellB = new themis.SecureCellTokenProtect(masterKey1)
                let result = cellA.encrypt(testInput)
                let decrypted = cellB.decrypt(result.data, result.token)
                assert.deepStrictEqual(decrypted, testInput)
            })
        })
        describe('Context Imprint mode', function() {
            it('does not accept strings', function() {
                assert.throws(() => themis.SecureCellContextImprint.withKey('master key'), TypeError)
            })
            it('encrypts only with context', function() {
                let cell = themis.SecureCellContextImprint.withKey(masterKey1)
                assert.throws(() => cell.encrypt(testInput))
                let encrypted = cell.encrypt(testInput, testContext)
                let decrypted = cell.decrypt(encrypted, testContext)
                assert.deepStrictEqual(decrypted, testInput)
            })
            it('encryption does not change data length', function() {
                let cell = themis.SecureCellContextImprint.withKey(masterKey1)
                let encrypted = cell.encrypt(testInput, testContext)
                assert.equal(encrypted.length, testInput.length)
            })
            it('forbids empty message and context', function() {
                assert.throws(() => themis.SecureCellContextImprint.withKey(emptyArray),
                    expectError(ThemisErrorCode.INVALID_PARAMETER)
                )
                let cell = themis.SecureCellContextImprint.withKey(masterKey1)
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
                let cell1 = themis.SecureCellContextImprint.withKey(masterKey1)
                let cell2 = themis.SecureCellContextImprint.withKey(masterKey2)
                let encrypted = cell1.encrypt(testInput, testContext)
                let decrypted = cell2.decrypt(encrypted, testContext)
                assert.notDeepStrictEqual(testInput, decrypted)
            })
            it('does not detect incorrect context', function() {
                let cell = themis.SecureCellContextImprint.withKey(masterKey1)
                let encrypted = cell.encrypt(testInput, testContext)
                let decrypted = cell.decrypt(encrypted, testInput)
                assert.notDeepStrictEqual(testInput, decrypted)
            })
            it('does not detect corrupted data', function() {
                let cell = themis.SecureCellContextImprint.withKey(masterKey1)
                let encrypted = cell.encrypt(testInput, testContext)
                encrypted[5] = ~encrypted[5]
                let decrypted = cell.decrypt(encrypted, testContext)
                assert.notDeepStrictEqual(testInput, decrypted)
            })
            it('handles type mismatches', function() {
                let cell = themis.SecureCellContextImprint.withKey(masterKey1)
                let encrypted = cell.encrypt(testInput, testContext)
                generallyInvalidArguments.forEach(function(invalid) {
                    assert.throws(() => themis.SecureCellContextImprint.withKey(invalid), TypeError)
                    assert.throws(() => cell.encrypt(invalid, testContext), TypeError)
                    assert.throws(() => cell.decrypt(invalid, testContext), TypeError)
                    // Contest Imprint mode has a custom error for omitted context
                    let expectedError = (invalid === undefined)
                                      ? expectError(ThemisErrorCode.INVALID_PARAMETER)
                                      : TypeError
                    assert.throws(() => cell.encrypt(testInput, invalid), expectedError)
                    assert.throws(() => cell.decrypt(encrypted, invalid), expectedError)
                })
            })
            it('supports deprecated constructor API', function() {
                let cellA = themis.SecureCellContextImprint.withKey(masterKey1)
                let cellB = new themis.SecureCellContextImprint(masterKey1)
                let encrypted = cellA.encrypt(testInput, testContext)
                let decrypted = cellB.decrypt(encrypted, testContext)
                assert.deepStrictEqual(decrypted, testInput)
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
                encrypted[10] = ~encrypted[10]
                assert.throws(() => secureMessage.decrypt(encrypted),
                    expectError(ThemisErrorCode.FAIL)
                )
            })
            it('handles type mismatches', function() {
                let keyPair = new themis.KeyPair()
                let secureMessage = new themis.SecureMessage(keyPair)
                generallyInvalidArguments.forEach(function(invalid) {
                    assert.throws(() => new themis.SecureMessage(invalid),
                                  expectError(ThemisErrorCode.INVALID_PARAMETER))
                    assert.throws(() => new themis.SecureMessage(keyPair.publicKey, invalid),
                                  expectError(ThemisErrorCode.INVALID_PARAMETER))
                    assert.throws(() => new themis.SecureMessage(invalid, keyPair.privateKey),
                                  expectError(ThemisErrorCode.INVALID_PARAMETER))

                    assert.throws(() => secureMessage.encrypt(invalid), TypeError)
                    assert.throws(() => secureMessage.decrypt(invalid), TypeError)
                })
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
                signed[12] = ~signed[12]
                assert.throws(() => verifier.verify(signed),
                    expectError(ThemisErrorCode.FAIL)
                )
            })
            it('handles type mismatches', function() {
                let keyPair = new themis.KeyPair()
                let signer = new themis.SecureMessageSign(keyPair.privateKey)
                let verifier = new themis.SecureMessageVerify(keyPair.publicKey)
                generallyInvalidArguments.forEach(function(invalid) {
                    assert.throws(() => new themis.SecureMessageSign(invalid),
                                  expectError(ThemisErrorCode.INVALID_PARAMETER))
                    assert.throws(() => new themis.SecureMessageVerify(invalid),
                                  expectError(ThemisErrorCode.INVALID_PARAMETER))

                    assert.throws(() => signer.sign(invalid), TypeError)
                    assert.throws(() => verifier.verify(invalid), TypeError)
                })
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
        it('handles type mismatches', function() {
            let comparison = new themis.SecureComparator()
            generallyInvalidArguments.forEach(function(invalid) {
                assert.throws(() => comparison.append(invalid), TypeError)
            })
            comparison.append(secretBytes)
            generallyInvalidArguments.forEach(function(invalid) {
                assert.throws(() => comparison.proceed(invalid), TypeError)
            })
            comparison.destroy()
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
                throw Error('something')
            })

            let request = client.connectionRequest()
            assert.throws(() => server.negotiateReply(request), function(error) {
                return (error instanceof Error) && (error.message == 'something')
            })

            client.destroy()
            server.destroy()
        })
        it('handles type mismatches', function() {
            function isFunction(obj) {
                return !!(obj && obj.constructor && obj.call && obj.apply)
            }

            let keys = makeKeyMaterial()
            let client = new themis.SecureSession(clientID, keys.client.privateKey, keys.clientCallback)
            let server = new themis.SecureSession(serverID, keys.server.privateKey, keys.serverCallback)

            generallyInvalidArguments.forEach(function(invalid) {
                assert.throws(
                    () => new themis.SecureSession(invalid, keys.client.privateKey, keys.clientCallback),
                    TypeError
                )
                assert.throws(
                    () => new themis.SecureSession(clientID, invalid, keys.clientCallback),
                    expectError(ThemisErrorCode.INVALID_PARAMETER)
                )
                // Functions are actually okay for callbacks
                if (!isFunction(invalid)) {
                    assert.throws(
                        () => new themis.SecureSession(clientID, keys.client.privateKey, invalid),
                        expectError(ThemisErrorCode.INVALID_PARAMETER)
                    )
                }

                assert.throws(() => client.negotiateReply(invalid), TypeError)
                assert.throws(() => server.negotiateReply(invalid), TypeError)
            })

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

            generallyInvalidArguments.forEach(function(invalid) {
                assert.throws(() => client.wrap(invalid), TypeError)
                assert.throws(() => client.unwrap(invalid), TypeError)
                assert.throws(() => server.wrap(invalid), TypeError)
                assert.throws(() => server.unwrap(invalid), TypeError)
            })

            client.destroy()
            server.destroy()
        })
    })
})
