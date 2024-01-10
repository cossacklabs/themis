import { createServer, Socket } from 'net';
import { SecureComparator } from 'jsthemis';

const server = createServer((socket: Socket) => {
    const comparator = new SecureComparator(Buffer.from("secret"));

    socket.on('data', (data: Buffer) => {
        const d = comparator.proceedCompare(data);
        socket.write(d);
        if (comparator.isCompareComplete()) {
            console.log(comparator.isMatch());
            socket.destroy();
        }
    });
});

server.listen(1337, '127.0.0.1');
