var addon = require('jsthemis');

keypair = new addon.KeyPair();
console.log("private key: ", keypair.private().toString("base64"));
console.log("public key : ", keypair.public().toString("base64"));
