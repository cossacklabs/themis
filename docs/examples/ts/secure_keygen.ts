import { KeyPair, SymmetricKey } from 'jsthemis';

const masterKey = new SymmetricKey() as Uint8Array;
console.log("master key: ", Buffer.from(masterKey).toString("base64"));

const keypair = new KeyPair();
console.log("private key: ", Buffer.from(keypair.private()).toString("base64"));
console.log("public key : ", Buffer.from(keypair.public()).toString("base64"));
