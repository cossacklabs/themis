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

function forEachCombination(closure) {
    if (arguments.length <= 1) {
        return
    }
    let args = new Array(arguments.length - 1)
    let indices = new Array(arguments.length - 1)
    for (var i = 0; i < indices.length; i++) {
        indices[i] = 0
    }
    // So that we get all-zeros values on the first entry into the loop:
    indices[indices.length - 1] = -1
    // For each iteration increase the last index and propagate carry-over,
    // continue until we iterate through all possible index combinations.
    while (true) {
        indices[indices.length - 1]++
        for (var i = indices.length - 1; i >=0; i--) {
            // If there is no carry-over then we're done
            if (indices[i] < arguments[i + 1].length) {
                break
            }
            // If we overflow then there are no more combinations to try
            if (i == 0) {
                return
            }
            indices[i] = 0
            indices[i - 1]++
        }
        // Construct argument array with actual values and pass it to the closure
        for (var i = 0; i < indices.length; i++) {
            args[i] = arguments[i + 1][indices[i]]
        }
        closure.apply(null, args)
    }
}

describe('wasm-themis', function() {
    describe('test utilities', function() {
        it('enumerates all combinations', function() {
            var combinations = []
            forEachCombination(function(n1, n2, n3) {
                combinations.push([n1, n2, n3])
            }, [1, 2], [3, 4, 5], [6, 7])
            assert.deepStrictEqual(combinations, [
                [1, 3, 6], [1, 3, 7], [1, 4, 6], [1, 4, 7], [1, 5, 6], [1, 5, 7],
                [2, 3, 6], [2, 3, 7], [2, 4, 6], [2, 4, 7], [2, 5, 6], [2, 5, 7]
            ])
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
})
