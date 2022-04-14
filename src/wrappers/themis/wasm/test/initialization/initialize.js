const themis = require('../../src/index.ts')
const ThemisErrorCode = themis.ThemisErrorCode
const assert = require('assert')

describe('wasm-themis', function() {
    describe('initialization', function() {
        it('resolves "initialize()" promise', function(done) {
            themis.initialize().then(function() {
                let key = new themis.SymmetricKey()
                assert.ok(key.length > 0)
                done()
            })
        })
        it('can only be initialized once', function(done) {
            themis.initialize().catch(function(e) {
                assert.strictEqual(e.errorCode, ThemisErrorCode.FAIL)
                done()
            })
        })
    })
})
