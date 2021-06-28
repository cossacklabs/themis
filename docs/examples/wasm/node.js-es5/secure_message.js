var themis = require('wasm-themis')

var command, privateKeyText, publicKeyText, message
if (process.argv.length == 6) {
    command        = process.argv[2]
    privateKeyText = process.argv[3]
    publicKeyText  = process.argv[4]
    message        = process.argv[5]
} else {
    console.error('usage:\n\tnode ' + process.argv[1] + '{enc|dec} <private-key> <public-key> <message>')
    process.exit(1)
}

themis.initialized.then(function() {
    var privateKey = new themis.PrivateKey(Buffer.from(privateKeyText, 'base64'))
    var publicKey = new themis.PublicKey(Buffer.from(publicKeyText, 'base64'))

    var secureMessage = new themis.SecureMessage(privateKey, publicKey)

    switch (command) {
    case 'enc':
        var encrypted = secureMessage.encrypt(Buffer.from(message, 'UTF-8'))
        console.log(Buffer.from(encrypted).toString('base64'))
        break
    case 'dec':
        var decrypted = secureMessage.decrypt(Buffer.from(message, 'base64'))
        console.log(Buffer.from(decrypted).toString('UTF-8'))
        break
    default:
        console.error('usage:\n\tnode ' + process.argv[1] + '{enc|dec} <private-key> <public-key> <message>')
        process.exit(1)
    }
})
