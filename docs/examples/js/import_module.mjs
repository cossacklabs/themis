import themis from 'jsthemis'; 
const { SymmetricKey, KeyPair} = themis;

let masterKey = new SymmetricKey()
console.log(masterKey);

let keypair = new KeyPair()
let privateKey = keypair.private()
let publicKey = keypair.public()
console.log(privateKey);
console.log(publicKey);
