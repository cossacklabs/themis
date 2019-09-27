const jsthemis = require('jsthemis');
const fs = require('fs');

if (process.argv.length !== 2 && process.argv.length !== 4) {
    console.log('Usage: node keygen.js <private_key_path> <public_key_path>');
    process.exit(1);
}

if (process.argv.length === 2) {
    var private_key_path = 'key';
    var public_key_path = 'key.pub';
} else if (process.argv.length === 4){
    var private_key_path = process.argv[2];
    var public_key_path = process.argv[3];
}

keypair = new jsthemis.KeyPair();

fs.writeFileSync(private_key_path, keypair.private(), {'mode': 0o400});
fs.writeFileSync(public_key_path, keypair.public());