import net from 'net'
import themis from 'wasm-themis'

const serverPrivateKeyBuffer = Buffer.from('UkVDMgAAAC0U6AK7AAm6ha0cgHmovSTpZax01+icg9xwFlZAqqGWeGTqbHUt', 'base64')
const clientPublicKeyBuffer = Buffer.from('VUVDMgAAAC15KNjgAr1DQEw+So1oztUarO4Jw/CGgyehBRCbOxbpHrPBKO7s', 'base64')

const clientID = Buffer.from('client')
const serverID = Buffer.from('server')

async function main() {
    await themis.initialized

    let serverPrivateKey = new themis.PrivateKey(serverPrivateKeyBuffer)
    let clientPublicKey = new themis.PublicKey(clientPublicKeyBuffer)

    let server = net.createServer(function(socket) {
        console.log('Server: accepted connection')

        let session = new themis.SecureSession(serverID, serverPrivateKey, function(id) {
            if (clientID.equals(id)) {
                return clientPublicKey
            }
            console.error('Server: unknown peer ID: ' + Buffer.from(id).toString())
            return null
        })

        socket.on('data', function(dataFromClient) {
            if (!session.established()) {
                let dataToClient = session.negotiateReply(dataFromClient)
                socket.write(dataToClient)
                if (session.established()) {
                    console.log('Server: Secure Session established')
                }
            } else {
                let message = session.unwrap(dataFromClient)
                console.log('Server: client says: ' + Buffer.from(message).toString())
                let reply = session.wrap(message)
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
}

main()
