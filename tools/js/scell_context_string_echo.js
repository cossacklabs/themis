var jsthemis = require('jsthemis');

if (process.argv.length !== 6) {
    console.log('Usage: <command: enc | dec > <key> <message> <context>');
    process.exit(1);
}

var command = process.argv[2];
var key = new Buffer(process.argv[3]);
var message = process.argv[4];
var context = new Buffer(process.argv[5]);

if (command === "enc"){
    var message_buf = new Buffer(message);
    var encrypted = new jsthemis.SecureCellContextImprint(key);
    var result = encrypted.encrypt(message_buf, context);
    console.log(result.toString('base64'));
    process.exit(0)
} else if (command === 'dec'){
    var decrypter = new jsthemis.SecureCellContextImprint(key);
    var decoded_message = new Buffer(message, 'base64');
    var result = decrypter.decrypt(decoded_message, context);
    console.log(result.toString('ascii'));
    process.exit(0)
} else {
    console.log('Wrong command, use "enc" or "dec"');
    process.exit(1)
}