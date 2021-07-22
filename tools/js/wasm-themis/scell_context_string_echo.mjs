import themis from 'wasm-themis'

let command, key, message, context
if (process.argv.length == 6) {
    command = process.argv[2]
    key     = process.argv[3]
    message = process.argv[4]
    context = process.argv[5]
} else {
    console.log('usage: node scell_context_string_echo.js <enc|dec> <key> <message> <context>')
    process.exit(1);
}

themis.initialized.then(function() {
    let cell = themis.SecureCellContextImprint.withKey(Buffer.from(key))
    let result
    switch (command) {
        case 'enc':
            result = cell.encrypt(Buffer.from(message), Buffer.from(context))
            console.log(Buffer.from(result).toString('base64'))
            break
        case 'dec':
            result = cell.decrypt(Buffer.from(message, 'base64'), Buffer.from(context))
            console.log(Buffer.from(result).toString('utf-8'))
            break
        default:
            console.log('invalid command "' + command + '": use "enc" or "dec"')
            process.exit(1)
    }
})
