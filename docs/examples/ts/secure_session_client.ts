import { Socket } from 'net';
import { SecureSession } from 'jsthemis';

const session = new SecureSession(
    Buffer.from("client"),
    Buffer.from("UkVDMgAAAC3DZR2qAEbvO092R/IKXBttnf9dVSU65R+Fb4eNoyxxlzn2n4GR", "base64"),
    (id: Uint8Array) => {
        if (id.toString() === "server") {
            return Buffer.from("VUVDMgAAAC30/vs+AwciK6egi82A9TkTydVuOzMFsJ9AkA0gMGyNH0tSu5Bk", "base64");
        } else if (id.toString() === "client") {
            return Buffer.from("VUVDMgAAAC15KNjgAr1DQEw+So1oztUarO4Jw/CGgyehBRCbOxbpHrPBKO7s", "base64");
        }
        return null;
    }
);

let retry_count = 5;

const client: Socket = new Socket();
client.connect(1337, '127.0.0.1', () => {
    console.log('Connected');
    client.write(session.connectRequest());
});

client.on('data', (data: Buffer) => {
    const d = session.unwrap(data);
    if (!session.isEstablished()) {
        client.write(d);
    } else {
        if (d !== undefined) {
            console.log(d.toString());
        }
        if (retry_count--) {
            client.write(session.wrap(Buffer.from("Hello server!!!")));
        } else {
            client.destroy();
        }
    }
});

client.on('close', () => {
    console.log('Connection closed');
});
