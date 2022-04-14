const themis = require('../../src/index.ts')
const ThemisErrorCode = themis.ThemisErrorCode
const assert = require('assert')

describe('wasm-themis', function() {
    describe('initialization', function() {
        it('resolves "initialized" promise', function(done) {
            themis.initialized.then(function() {
                let key = new themis.SymmetricKey()
                assert.ok(key.length > 0)
                done()
            })
        })
        // "themis.initialized" stays resolved but you cannot initialize again.
        it('can only be initialized once', function(done) {
            themis.initialize().catch(function(e) {
                assert.strictEqual(e.errorCode, ThemisErrorCode.FAIL)
                done()
            })
        })
    })
})
