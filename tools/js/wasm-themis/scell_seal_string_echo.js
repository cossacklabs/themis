const themis = require('wasm-themis')

let command, key, message, context
if (5 <= process.argv.length && process.argv.length <= 6) {
    command = process.argv[2]
    key     = process.argv[3]
    message = process.argv[4]
    if (process.argv.length == 6) {
        context = process.argv[5]
    }
} else {
    console.log('usage: node scell_seal_string_echo.js <enc|dec> <key> <message> [<context>]')
    process.exit(1);
}

themis.initialized.then(function() {
    let cell = themis.SecureCellSeal.withKey(Buffer.from(key))
    let result
    switch (command) {
        case 'enc':
            if (context) {
                result = cell.encrypt(Buffer.from(message), Buffer.from(context))
            } else {
                result = cell.encrypt(Buffer.from(message))
            }
            console.log(Buffer.from(result).toString('base64'))
            break
        case 'dec':
            if (context) {
                result = cell.decrypt(Buffer.from(message, 'base64'), Buffer.from(context))
            } else {
                result = cell.decrypt(Buffer.from(message, 'base64'))
            }
            console.log(Buffer.from(result).toString('utf-8'))
            break
        default:
            console.log('invalid command "' + command + '": use "enc" or "dec"')
            process.exit(1)
    }
})
