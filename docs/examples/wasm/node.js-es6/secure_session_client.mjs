import net from 'net'
import themis from 'wasm-themis'

const clientPrivateKeyBuffer = Buffer.from('UkVDMgAAAC3DZR2qAEbvO092R/IKXBttnf9dVSU65R+Fb4eNoyxxlzn2n4GR', 'base64')
const serverPublicKeyBuffer = Buffer.from('VUVDMgAAAC30/vs+AwciK6egi82A9TkTydVuOzMFsJ9AkA0gMGyNH0tSu5Bk', 'base64')

const clientID = Buffer.from('client')
const serverID = Buffer.from('server')

async function main() {
    await themis.initialized

    let clientPrivateKey = new themis.PrivateKey(clientPrivateKeyBuffer)
    let serverPublicKey = new themis.PublicKey(serverPublicKeyBuffer)

    let session = new themis.SecureSession(clientID, clientPrivateKey, function(id) {
        if (serverID.equals(id)) {
            return serverPublicKey
        }
        console.error('Client: unknown peer ID: ' + Buffer.from(id).toString())
        return null
    })

    let counter = 5

    let socket = new net.Socket()
    socket.connect(1337, '127.0.0.1', function() {
        console.log('Client: connected')
        let request = session.connectionRequest()
        socket.write(request)
    })

    socket.on('data', function(dataFromServer) {
        if (!session.established()) {
            let dataToServer = session.negotiateReply(dataFromServer)
            socket.write(dataToServer)
            if (session.established()) {
                console.log('Client: Secure Session established')
                let dataToServer = session.wrap(Buffer.from('Hello ' + counter))
                socket.write(dataToServer)
            }
        } else {
            let message = session.unwrap(dataFromServer)
            console.log('Client: server says: ' + Buffer.from(message).toString())
            if (counter--) {
                let dataToServer = session.wrap(Buffer.from('Hello ' + counter))
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
}

main()
