import net from 'net'
import themis from 'wasm-themis'

async function main() {
    await themis.initialized

    let server = net.createServer(function(socket) {
        console.log('Server: accepted connection')

        let comparator = new themis.SecureComparator(Buffer.from('secret'))

        socket.on('data', function(dataFromClient) {
            let dataToClient = comparator.proceed(dataFromClient)
            socket.write(dataToClient)
            if (comparator.complete()) {
                console.log('Server: compare equal: ' + comparator.compareEqual())
                comparator.destroy()
                socket.destroy()
            }
        })

        socket.on('close', function() {
            console.log('Server: connection closed')
        })
    })

    server.listen(1337, '127.0.0.1')
    console.log('Server: waiting for connections')
}

main()
