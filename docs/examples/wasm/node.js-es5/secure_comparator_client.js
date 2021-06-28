var net = require('net')
var themis = require('wasm-themis')

themis.initialized.then(function() {
    var comparator = new themis.SecureComparator(Buffer.from('secret'))

    var socket = new net.Socket()
    socket.connect(1337, '127.0.0.1', function() {
        console.log('Client: connected');
        socket.write(comparator.begin())
    })

    socket.on('data', function(dataFromServer) {
        var dataToServer = comparator.proceed(dataFromServer)
        if (!comparator.complete()) {
            socket.write(dataToServer)
        } else {
            console.log('Client: compare equal: ' + comparator.compareEqual())
            comparator.destroy()
            socket.destroy()
        }
    })

    socket.on('close', function() {
        console.log('Client: connection closed')
    })
})
