const themis = require('jsthemis')

let message = 'Test Message Please Ignore'
let context = 'Secure Cell example code'
let master_key = Buffer.from('bm8sIHRoaXMgaXMgbm90IGEgdmFsaWQgbWFzdGVyIGtleQ==', 'base64')
let passphrase = 'My Litte Secret: Passphrase Is Magic'

console.log('# Secure Cell in Seal mode\n')

console.log('## Master key API\n')

let scellMK = themis.SecureCellSeal.withKey(master_key)

encrypted_message = scellMK.encrypt(Buffer.from(message, 'UTF-8'))
console.log('Encrypted: ' + encrypted_message.toString('base64'))

decrypted_message = scellMK.decrypt(encrypted_message).toString('UTF-8')
console.log('Decrypted: ' + decrypted_message)

// Visit https://docs.cossacklabs.com/simulator/data-cell/
console.log()
encrypted_message = Buffer.from('AAEBQAwAAAAQAAAAEQAAAC0fCd2mOIxlDUORXz8+qCKuHCXcDii4bMF8OjOCOqsKEdV4+Ga2xTHPMupFvg==', 'base64')
decrypted_message = scellMK.decrypt(encrypted_message).toString('UTF-8')
console.log('Decrypted (simulator): ' + decrypted_message)

console.log()


console.log('## Passphrase API\n')

let scellPW = themis.SecureCellSeal.withPassphrase(passphrase)

encrypted_message = scellPW.encrypt(Buffer.from(message, 'UTF-8'))
console.log('Encrypted: ' + encrypted_message.toString('base64'))

decrypted_message = scellPW.decrypt(encrypted_message).toString('UTF-8')
console.log('Decrypted: ' + decrypted_message)

console.log('')


console.log('# Secure Cell in Token Protect mode\n')

let scellTP = themis.SecureCellTokenProtect.withKey(master_key)

encrypted_message = scellTP.encrypt(Buffer.from(message, 'UTF-8'))
console.log('Encrypted:  ' + encrypted_message.data.toString('base64'))
console.log('Auth token: ' + encrypted_message.token.toString('base64'))

decrypted_message = scellTP.decrypt(encrypted_message.data, encrypted_message.token).toString('UTF-8')
console.log('Decrypted:  ' + decrypted_message)

console.log('')


console.log('# Secure Cell in Context Imprint mode\n')

let scellCI = themis.SecureCellContextImprint.withKey(master_key)

encrypted_message = scellCI.encrypt(Buffer.from(message, 'UTF-8'), Buffer.from(context, 'UTF-8'))
console.log('Encrypted: ' + encrypted_message.toString('base64'))

decrypted_message = scellCI.decrypt(encrypted_message, Buffer.from(context, 'UTF-8')).toString('UTF-8')
console.log('Decrypted: ' + decrypted_message)

console.log('')
