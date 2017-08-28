var net = require('net');
var themis = require('jsthemis');

var server = net.createServer(function(socket) {

    comparator = new themis.SecureComparator(new Buffer("secret"));
  
    socket.on('data', function (data) {
	d=comparator.proceedCompare(data);
	socket.write(d);
	if(comparator.isCompareComplete()){
            console.log(comparator.isMatch());
            socket.destroy();
	}
    });
});

server.listen(1337, '127.0.0.1');
