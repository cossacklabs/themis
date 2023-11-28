import { SecureCellSeal, SecureCellTokenProtect, SecureCellContextImprint } from "jsthemis"

const message = Buffer.from('Test Message Please Ignore', 'utf-8')
const context = Buffer.from('Secure Cell example code','utf-8')
const master_key = Buffer.from('bm8sIHRoaXMgaXMgbm90IGEgdmFsaWQgbWFzdGVyIGtleQ==', 'base64')
const passphrase = 'My Litte Secret: Passphrase Is Magic'

console.log('# Secure Cell in Seal mode\n')
console.log('## Master key API\n')
const scellMK = SecureCellSeal.withKey(master_key)
const encrypted_message = scellMK.encrypt(message)
console.log('Encrypted: ' + Buffer.from(encrypted_message).toString('base64'))
const decrypted_message = scellMK.decrypt(encrypted_message)
console.log('Decrypted: ' + Buffer.from(decrypted_message).toString())
console.log()

const encrypted_message2 = Buffer.from('AAEBQAwAAAAQAAAAEQAAAC0fCd2mOIxlDUORXz8+qCKuHCXcDii4bMF8OjOCOqsKEdV4+Ga2xTHPMupFvg==', 'base64')
const decrypted_message2 = scellMK.decrypt(encrypted_message2)
console.log('Decrypted (simulator): ' + Buffer.from(decrypted_message2).toString())
console.log()
console.log('## Passphrase API\n')

const scellPW = SecureCellSeal.withPassphrase(passphrase)
const encrypted_message3 = scellPW.encrypt(message)
console.log('Encrypted: ' + Buffer.from(encrypted_message3).toString('base64'))
const decrypted_message3 = scellPW.decrypt(encrypted_message3)
console.log('Decrypted: ' + Buffer.from(decrypted_message3).toString())
console.log()

console.log('# Secure Cell in Token Protect mode\n')

const scellTP = SecureCellTokenProtect.withKey(master_key)
const encrypted_message4 = scellTP.encrypt(message)
console.log('Encrypted:  ' + Buffer.from(encrypted_message4.data).toString('base64'))
console.log('Auth token: ' + Buffer.from(encrypted_message4.token).toString('base64'))
const decrypted_message4 = scellTP.decrypt(encrypted_message4.data, encrypted_message4.token)
console.log('Decrypted:  ' + Buffer.from(decrypted_message4).toString())
console.log('')

console.log('# Secure Cell in Context Imprint mode\n')
const scellCI = SecureCellContextImprint.withKey(master_key)
const encrypted_message5 = scellCI.encrypt(message, context)
console.log('Encrypted: ' + Buffer.from(encrypted_message5).toString('base64'))
const decrypted_message5 = scellCI.decrypt(encrypted_message5, context)
console.log('Decrypted: ' + Buffer.from(decrypted_message5).toString())
console.log('')
console.log('SecureCell example code finished')


