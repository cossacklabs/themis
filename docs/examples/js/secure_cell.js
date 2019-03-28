var addon = require('jsthemis');


if(process.argv.length==5){
    command=process.argv[2];
    password=new Buffer.from(process.argv[3]);

    seal = new addon.SecureCellSeal(password);
    if(command=="enc"){
	console.log(seal.encrypt(new Buffer.from(process.argv[4])).toString("base64"));
    }else if(command=="dec"){
	console.log(seal.decrypt(new Buffer.from(process.argv[4], "base64")).toString());
    }else{
	console.log("usage node secure_cell.js <enc/dec> <password> <message>");
    }
}else{
    console.log("usage node secure_cell.js <enc/dec> <password> <message>");
}