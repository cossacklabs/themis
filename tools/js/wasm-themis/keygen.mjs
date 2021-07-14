import fs from 'fs'
import themis from 'wasm-themis'

let private_key_path, public_key_path
switch (process.argv.length) {
    case 2:
        private_key_path = 'key'
        public_key_path = 'key.pub'
        break
    case 4:
        private_key_path = process.argv[2]
        public_key_path = process.argv[3]
        break
    default:
        console.log('usage: node keygen.js [<path/to/key> <path/to/key.pub>]')
        process.exit(1)
}

themis.initialized.then(function() {
    let keypair = new themis.KeyPair()

    fs.writeFile(private_key_path, keypair.privateKey, {'mode': 0o600}, function(err) {
        if (err) {
            console.log('failed to write ' + private_key_path + ': ' + err)
        }
    })
    fs.writeFile(public_key_path, keypair.publicKey, function(err) {
        if (err) {
            console.log('failed to write ' + public_key_path + ': ' + err)
        }
    })
})
