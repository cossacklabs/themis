var net = require('net');
var themis = require('jsthemis');

session = new themis.SecureSession(new Buffer.from("client"), new Buffer.from("UkVDMgAAAC3DZR2qAEbvO092R/IKXBttnf9dVSU65R+Fb4eNoyxxlzn2n4GR","base64"), function(id){
    if(id.toString()=="server".toString()){
	return new Buffer.from("VUVDMgAAAC30/vs+AwciK6egi82A9TkTydVuOzMFsJ9AkA0gMGyNH0tSu5Bk", "base64");
    }
    else if(id.toString()=="client".toString())
	return new Buffer.from("VUVDMgAAAC15KNjgAr1DQEw+So1oztUarO4Jw/CGgyehBRCbOxbpHrPBKO7s", "base64");
});

retry_count=5;

var client = new net.Socket();
client.connect(1337, '127.0.0.1', function() {
    console.log('Connected');
    client.write(session.connectRequest());
});

client.on('data', function(data) {
    d=session.unwrap(data);
    if(!session.isEstablished())
	client.write(d);
    else{
	if(d != undefined)
	    console.log(d.toString());
	if(retry_count--)
	    client.write(session.wrap(new Buffer.from("Hello server!!!")));
	else
	    client.destroy();
    }
});

client.on('close', function() {
    console.log('Connection closed');
});
