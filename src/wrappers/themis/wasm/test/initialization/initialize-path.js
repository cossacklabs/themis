const themis = require('../../src/index.ts')
const ThemisErrorCode = themis.ThemisErrorCode
const assert = require('assert')

describe('wasm-themis', function() {
    describe('initialization', function() {
        it('catches error for invalid paths in initialize()', function(done) {
            themis.initialize('/does/not/exist').catch(function() {
                done()
            })
        })
        it('resolves "initialize()" promise with path', function(done) {
            // Assuming we're run via "npm test" from wrapper's directory.
            themis.initialize('src/libthemis.wasm').then(function() {
                let key = new themis.SymmetricKey()
                assert.ok(key.length > 0)
                done()
            })
        })
        it('can only be initialized once', function(done) {
            themis.initialize('src/libthemis.wasm').catch(function(e) {
                assert.strictEqual(e.errorCode, ThemisErrorCode.FAIL)
                done()
            })
        })
    })
})
