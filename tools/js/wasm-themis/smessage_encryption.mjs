import fs from 'fs'
import themis from 'wasm-themis'

let command, privateKeyPath, publicKeyPath, message
if (process.argv.length == 6) {
    command = process.argv[2]
    privateKeyPath = process.argv[3]
    publicKeyPath  = process.argv[4]
    message = process.argv[5]
} else {
    console.log('usage: node smessage_encryption.js <enc|dec|sign|verify> <sender/private.key> <recipient/public.key> <message>')
    process.exit(1);
}

fs.readFile(privateKeyPath, function(err, privateKey) {
    if (err) {
        console.log('failed to read ' + privateKeyPath + ': ' + err)
        process.exit(1)
    }
    fs.readFile(publicKeyPath, function(err, publicKey) {
        if (err) {
            console.log('failed to read ' + publicKeyPath + ': ' + err)
            process.exit(1)
        }
        themis.initialized.then(function() {
            privateKey = new themis.PrivateKey(privateKey)
            publicKey = new themis.PublicKey(publicKey)
            let smessage = new themis.SecureMessage(privateKey, publicKey)
            let smessage_sign = new themis.SecureMessageSign(privateKey)
            let smessage_verify = new themis.SecureMessageVerify(publicKey)
            let result
            switch (command) {
                case 'enc':
                    result = smessage.encrypt(Buffer.from(message))
                    console.log(Buffer.from(result).toString('base64'))
                    break
                case 'dec':
                    result = smessage.decrypt(Buffer.from(message, 'base64'))
                    console.log(Buffer.from(result).toString('utf-8'))
                    break
                case 'sign':
                    result = smessage_sign.sign(Buffer.from(message))
                    console.log(Buffer.from(result).toString('base64'))
                    break
                case 'verify':
                    result = smessage_verify.verify(Buffer.from(message, 'base64'))
                    console.log(Buffer.from(result).toString('utf-8'))
                    break
                default:
                    console.log('invalid command "' + command + '": use "enc", "dec", "sign", "verify"')
                    process.exit(1)
            }
        })
    })
})
