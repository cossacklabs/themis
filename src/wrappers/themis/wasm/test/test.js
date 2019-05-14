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
