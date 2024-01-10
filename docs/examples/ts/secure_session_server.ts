import { createServer, Socket } from 'net';
import { SecureSession } from 'jsthemis';

const server = createServer((socket: Socket) => {
    const session = new SecureSession(
        Buffer.from("server"),
        Buffer.from("UkVDMgAAAC0U6AK7AAm6ha0cgHmovSTpZax01+icg9xwFlZAqqGWeGTqbHUt", "base64"),
        (id: Uint8Array) => {
            if (id.toString() === "server") {
                return Buffer.from("VUVDMgAAAC30/vs+AwciK6egi82A9TkTydVuOzMFsJ9AkA0gMGyNH0tSu5Bk", "base64");
            } else if (id.toString() === "client") {
                return Buffer.from("VUVDMgAAAC15KNjgAr1DQEw+So1oztUarO4Jw/CGgyehBRCbOxbpHrPBKO7s", "base64");
            }
            return null;
        }
    );

    socket.on('data', (data: Buffer) => {
        if (!session.isEstablished()) {
            const d = session.unwrap(data);
            socket.write(d);
        } else {
            const d = session.unwrap(data);
            console.log(d.toString());
            socket.write(session.wrap(d));
        }
    });
});

server.listen(1337, '127.0.0.1');
