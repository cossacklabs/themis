const themis = require('../../src/index.ts')
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
    })
})
