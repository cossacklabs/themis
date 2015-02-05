from smessage import *

alg="EC";

obj = themis_gen_key_pair(alg);
private_key = obj.export_private_key();
public_key = obj.export_public_key();
signer = themis_smessage_signer(private_key);
verifier = themis_smessage_verifier(public_key);

message = "Hello world!!!";
signed_message = signer.sign((message, len(message)));
print signed_message[1], repr(signed_message[0].raw);

plain_message = verifier.verify(signed_message);
print plain_message[1], repr(plain_message[0].raw);

obj2 = themis_gen_key_pair(alg);

peer_private_key = obj2.export_private_key();
peer_public_key = obj2.export_public_key();

encrypter = themis_smessage_encrypter(private_key, peer_public_key);

decrypter = themis_smessage_decrypter(peer_private_key, public_key);

encrypted_message = encrypter.encrypt((message, len(message)));
print encrypted_message[1], repr(encrypted_message[0].raw);

decrypted_message = decrypter.decrypt(encrypted_message);
print decrypted_message[1], repr(decrypted_message[0].raw);