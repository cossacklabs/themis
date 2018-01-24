const fs = require('fs');
const jsthemis = require('jsthemis');

if (process.argv.length !== 6) {
    console.log('Usage: <command: enc | dec | sign | verify > <send_private_key> <recipient_public_key> <message>');
    process.exit(1);
}

var command = process.argv[2];
var private_key = fs.readFileSync(process.argv[3])
var public_key = fs.readFileSync(process.argv[4])
var message = process.argv[5];

var smessage = jsthemis.SecureMessage(private_key, public_key);

if (command === "enc"){
    var message_buf = new Buffer(message);
    var result = smessage.encrypt(message_buf);
    console.log(result.toString('base64'));
    process.exit(0)
} else if (command === 'dec'){
    var decoded_message = new Buffer(message, 'base64');
    var result = smessage.decrypt(decoded_message);
    console.log(result.toString('ascii'));
    process.exit(0)
} else if (command === 'sign') {
    var message_buf = new Buffer(message);
    var result = smessage.sign(message_buf);
    console.log(result.toString('base64'));
    process.exit(0)
} else if (command === 'verify'){
    var decoded_message = new Buffer(message, 'base64');
    var result = smessage.verify(decoded_message);
    console.log(result.toString('ascii'));
    process.exit(0)
}else {
    console.log('Wrong command, use <enc | dev | sign | verify>');
    process.exit(1)
}