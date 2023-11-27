import { SecureMessage } from 'jsthemis';

if (process.argv.length == 6) {
    const command = process.argv[2];
    const private_key = process.argv[3];
    const peer_public_key = process.argv[4];

    const secure_message = new SecureMessage(
        Buffer.from(private_key, "base64"),
        Buffer.from(peer_public_key, "base64"));

    if (command == "enc") {
        const encrypted_message = secure_message.encrypt(Buffer.from(process.argv[5]));
        console.log(Buffer.from(encrypted_message).toString("base64"));
    } else if (command == "dec") {
        const decrypted_message = secure_message.decrypt(Buffer.from(process.argv[5], "base64"));
        console.log(Buffer.from(decrypted_message).toString("utf8"));
    } else {
        console.log("usage node secure_message.js <enc/dec> <private key> <peer public key> <message>");
    }
} else {
    console.log("usage node secure_message.js <enc/dec> <private key> <peer public key> <message>");
}