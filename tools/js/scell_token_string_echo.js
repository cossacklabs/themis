var jsthemis = require('jsthemis');

if (![5, 6].includes(process.argv.length)) {
    console.log('Usage: <command: enc | dec > <key> <message> <context (optional)>');
    process.exit(1);
}

var command = process.argv[2];
var key = new Buffer(process.argv[3]);
var message = process.argv[4];
var context = undefined;
if(process.argv.length === 6){
    var context = new Buffer(process.argv[5]);
}

if (command === "enc"){
    var message_buf = new Buffer(message);
    var encrypter = new jsthemis.SecureCellTokenProtect(key);
    if (context){
        var result = encrypter.encrypt(message_buf, context);
    } else {
        var result = encrypter.encrypt(message_buf);
    }
    console.log([result.data.toString('base64'), result.token.toString('base64')].join(','));
    process.exit(0)
} else if (command === 'dec'){
    var splitted_message = message.split(',');
    var decrypter = new jsthemis.SecureCellTokenProtect(key);
    var decoded_message = new Buffer(splitted_message[0], 'base64');
    var decoded_token = new Buffer(splitted_message[1], 'base64');
    if(context){
        var result = decrypter.decrypt(decoded_message, decoded_token, context);
    } else {
        var result = decrypter.decrypt(decoded_message, decoded_token);
    }
    console.log(result.toString('ascii'));
    process.exit(0)
} else {
    console.log('Wrong command, use "enc" or "dec"');
    process.exit(1)
}