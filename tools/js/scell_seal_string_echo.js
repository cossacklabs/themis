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
    var encrypter = new jsthemis.SecureCellSeal(key);
    if (context){
        var result = encrypter.encrypt(message_buf, context);
    } else {
        var result = encrypter.encrypt(message_buf);
    }
    console.log(result.toString('base64'));
    process.exit(0)
} else if (command === 'dec'){
    var decrypter = new jsthemis.SecureCellSeal(key);
    var decoded_message = new Buffer(message, 'base64');
    if(context){
        var result = decrypter.decrypt(decoded_message, context);
    } else {
        var result = decrypter.decrypt(decoded_message);
    }
    console.log(result.toString('ascii'));
    process.exit(0)
} else {
    console.log('Wrong command, use "enc" or "dec"');
    process.exit(1)
}