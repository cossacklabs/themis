var net = require('net');
var themis = require('jsthemis');

var server = net.createServer(function(socket) {

    session = new themis.SecureSession(new Buffer.from("server"), new Buffer.from("UkVDMgAAAC0U6AK7AAm6ha0cgHmovSTpZax01+icg9xwFlZAqqGWeGTqbHUt","base64"), function(id){
	if(id.toString()=="server".toString())
	    return new Buffer.from("VUVDMgAAAC30/vs+AwciK6egi82A9TkTydVuOzMFsJ9AkA0gMGyNH0tSu5Bk", "base64");
	else if(id.toString()=="client".toString()){
	    return new Buffer.from("VUVDMgAAAC15KNjgAr1DQEw+So1oztUarO4Jw/CGgyehBRCbOxbpHrPBKO7s","base64");
	}
    });
  
    socket.on('data', function (data) {
	if(!(session.isEstablished())){
	    d=session.unwrap(data);
	    socket.write(d);
	}else{
	    d=session.unwrap(data);
	    console.log(d.toString());
	    socket.write(session.wrap(d));
	}
    });
});

server.listen(1337, '127.0.0.1');