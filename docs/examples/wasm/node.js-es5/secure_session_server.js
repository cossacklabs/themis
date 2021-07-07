var net = require('net')
var themis = require('wasm-themis')

var serverPrivateKeyBuffer = Buffer.from('UkVDMgAAAC0U6AK7AAm6ha0cgHmovSTpZax01+icg9xwFlZAqqGWeGTqbHUt', 'base64')
var clientPublicKeyBuffer = Buffer.from('VUVDMgAAAC15KNjgAr1DQEw+So1oztUarO4Jw/CGgyehBRCbOxbpHrPBKO7s', 'base64')

var clientID = Buffer.from('client')
var serverID = Buffer.from('server')

themis.initialized.then(function() {
    var serverPrivateKey = new themis.PrivateKey(serverPrivateKeyBuffer)
    var clientPublicKey = new themis.PublicKey(clientPublicKeyBuffer)

    var server = net.createServer(function(socket) {
        console.log('Server: accepted connection')

        var session = new themis.SecureSession(serverID, serverPrivateKey, function(id) {
            if (clientID.equals(id)) {
                return clientPublicKey
            }
            console.error('Server: unknown peer ID: ' + Buffer.from(id).toString())
            return null
        })

        socket.on('data', function(dataFromClient) {
            if (!session.established()) {
                var dataToClient = session.negotiateReply(dataFromClient)
                socket.write(dataToClient)
                if (session.established()) {
                    console.log('Server: Secure Session established')
                }
            } else {
                var message = session.unwrap(dataFromClient)
                console.log('Server: client says: ' + Buffer.from(message).toString())
                var reply = session.wrap(message)
                socket.write(reply)
            }
        })

        socket.on('close', function() {
            console.log('Server: connection closed')
            session.destroy()
        })
    })

    server.listen(1337, '127.0.0.1')
    console.log('Server: waiting for connections')
})
