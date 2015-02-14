from skeygen import *
from smessage import *

alg="EC";

obj = themis_gen_key_pair(alg);
private_key = obj.export_private_key();
public_key = obj.export_public_key();

signer = themis_smessage_signer(private_key);
verifier = themis_smessage_verifier(public_key);

message = "Hello world!!!";
signed_message = signer.sign(message);
print repr(signed_message);

plain_message = verifier.verify(signed_message);
print repr(plain_message);

obj2 = themis_gen_key_pair(alg);

peer_private_key = obj2.export_private_key();
peer_public_key = obj2.export_public_key();

encrypter = themis_smessage_encrypter(private_key, peer_public_key);

decrypter = themis_smessage_decrypter(peer_private_key, public_key);

encrypted_message = encrypter.encrypt(message);
print repr(encrypted_message);

decrypted_message = decrypter.decrypt(encrypted_message);
print repr(decrypted_message);