var net = require('net')
var themis = require('wasm-themis')

var clientPrivateKeyBuffer = Buffer.from('UkVDMgAAAC3DZR2qAEbvO092R/IKXBttnf9dVSU65R+Fb4eNoyxxlzn2n4GR', 'base64')
var serverPublicKeyBuffer = Buffer.from('VUVDMgAAAC30/vs+AwciK6egi82A9TkTydVuOzMFsJ9AkA0gMGyNH0tSu5Bk', 'base64')

var clientID = Buffer.from('client')
var serverID = Buffer.from('server')

themis.initialized.then(function() {
    var clientPrivateKey = new themis.PrivateKey(clientPrivateKeyBuffer)
    var serverPublicKey = new themis.PublicKey(serverPublicKeyBuffer)

    var session = new themis.SecureSession(clientID, clientPrivateKey, function(id) {
        if (serverID.equals(id)) {
            return serverPublicKey
        }
        console.error('Client: unknown peer ID: ' + Buffer.from(id).toString())
        return null
    })

    var counter = 5

    var socket = new net.Socket()
    socket.connect(1337, '127.0.0.1', function() {
        console.log('Client: connected')
        var request = session.connectionRequest()
        socket.write(request)
    })

    socket.on('data', function(dataFromServer) {
        if (!session.established()) {
            var dataToServer = session.negotiateReply(dataFromServer)
            socket.write(dataToServer)
            if (session.established()) {
                console.log('Client: Secure Session established')
                var dataToServer = session.wrap(Buffer.from('Hello ' + counter))
                socket.write(dataToServer)
            }
        } else {
            var message = session.unwrap(dataFromServer)
            console.log('Client: server says: ' + Buffer.from(message).toString())
            if (counter--) {
                var dataToServer = session.wrap(Buffer.from('Hello ' + counter))
                socket.write(dataToServer)
            } else {
                session.destroy()
                socket.destroy()
            }
        }
    })

    socket.on('close', function() {
        console.log('Client: connection closed')
    })
})
