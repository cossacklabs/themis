import net from 'net'
import themis from 'wasm-themis'

async function main() {
    await themis.initialized

    let comparator = new themis.SecureComparator(Buffer.from('secret'))

    let socket = new net.Socket()
    socket.connect(1337, '127.0.0.1', function() {
        console.log('Client: connected');
        socket.write(comparator.begin())
    })

    socket.on('data', function(dataFromServer) {
        let dataToServer = comparator.proceed(dataFromServer)
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
}

main()
