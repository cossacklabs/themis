var net = require('net');
var themis = require('jsthemis');

comparator = new themis.SecureComparator(new Buffer("secret"));

var client = new net.Socket();
client.connect(1337, '127.0.0.1', function() {
    console.log('Connected');
    client.write(comparator.beginCompare());
});

client.on('data', function(data) {
    d = comparator.proceedCompare(data);
    if(!comparator.isCompareComplete())
	client.write(d);
    else{
	console.log(comparator.isMatch());
        client.destroy();
    }
});

client.on('close', function() {
    console.log('Connection closed');
});
