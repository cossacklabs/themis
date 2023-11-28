import { Socket } from 'net';
import { SecureComparator } from 'jsthemis';

const comparator = new SecureComparator(Buffer.from("secret"));

const client = new Socket();
client.connect(1337, '127.0.0.1', () => {
    console.log('Connected');
    client.write(comparator.beginCompare());
});

client.on('data', (data: Buffer) => {
    const d = comparator.proceedCompare(data);
    if (!comparator.isCompareComplete()) {
        client.write(d);
    } else {
        console.log(comparator.isMatch());
        client.destroy();
    }
});

client.on('close', () => {
    console.log('Connection closed');
});
