var addon = require('jsthemis');

if(process.argv.length==6){
    command=process.argv[2];
    private_key=process.argv[3];
    peer_public_key=process.argv[4];

    sm = new addon.SecureMessage(new Buffer.from(private_key, "base64"), new Buffer.from(peer_public_key, "base64"));
    if(command=="enc"){
        console.log(sm.encrypt(new Buffer.from(process.argv[5])).toString("base64"));
    }else if(command=="dec"){
	console.log(sm.decrypt(new Buffer.from(process.argv[5], "base64")).toString());
    }else{
        console.log("usage node secure_message.js <enc/dec> <private key> <peer public key> <message>");
    }
}else{
    console.log("usage node secure_message.js <enc/dec> <private key> <peer public key> <message>");
}