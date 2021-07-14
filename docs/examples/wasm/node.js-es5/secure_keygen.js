var themis = require('wasm-themis')

themis.initialized.then(function() {
    var keypair = new themis.KeyPair()

    console.log('private key: ' + Buffer.from(keypair.privateKey).toString('base64'))
    console.log('public key : ' + Buffer.from(keypair.publicKey).toString('base64'))
})
