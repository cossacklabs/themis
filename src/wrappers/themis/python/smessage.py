from ctypes import *
themis = cdll.LoadLibrary('./libthemis.so')

class themis_exception(Exception):
    def __init__(self, value):
	 self.value = value
    def __str__(self):
	return repr(self.value)

class themis_gen_key_pair(object):
    def __init__(self, alg):
	self.private_key_length = c_int(4096);
	self.public_key_length = c_int(4096);
	self.private_key = create_string_buffer(self.private_key_length.value);
	self.public_key = create_string_buffer(self.public_key_length.value);
	if alg == "EC" :
	    if themis.themis_gen_ec_key_pair(self.private_key, byref(self.private_key_length), self.public_key, byref(self.public_key_length)) != 0:
		raise themis_exception("themis_gen_ec_key_pair error")
	elif alg == "RSA" :
	    if themis.themis_gen_rsa_key_pair(self.private_key, byref(self.private_key_length), self.public_key, byref(self.public_key_length)) !=0:
		raise themis_exception("themis_gen_rsa_key_pair error")

    def export_private_key(self):
	return (self.private_key, self.private_key_length.value);

    def export_public_key(self):
	return (self.public_key, self.public_key_length.value);


class themis_smessage_signer(object):
    def __init__(self, private_key_):
	self.private_key=private_key_;

    def sign(self, message):
	signed_message_length=c_int(0);
	if themis.themis_secure_message_wrap(self.private_key[0] ,self.private_key[1], None, 0, message[0], message[1], None, byref(signed_message_length)) != -4:
	    raise themis_exception("themis_secure_message_wrap singing (signed message length determination) error");
	signed_message=create_string_buffer(signed_message_length.value);
	if themis.themis_secure_message_wrap(self.private_key[0] ,self.private_key[1], None, 0, message[0], message[1], signed_message, byref(signed_message_length)) != 0:
	    raise themis_exception("themis_secure_message_wrap singing error");
	return (signed_message, signed_message_length.value);
	
class themis_smessage_verifier(object):
    def __init__(self, public_key_):
	self.public_key=public_key_;

    def verify(self, message):
	plain_message_length=c_int(0);
	if themis.themis_secure_message_unwrap(None, 0, self.public_key[0] ,self.public_key[1], message[0], message[1], None, byref(plain_message_length)) != -4:
	    raise themis_exception("themis_secure_message_unwrap virifing (plain message length determination) error");
	plain_message=create_string_buffer(plain_message_length.value);
	if themis.themis_secure_message_unwrap(None, 0, self.public_key[0] ,self.public_key[1], message[0], message[1], plain_message, byref(plain_message_length)) !=0:
	    return ("",0);
	return (plain_message, plain_message_length.value);

class themis_smessage_encrypter(object):
    def __init__(self, private_key_, peer_public_key_):
	self.private_key=private_key_;
	self.peer_public_key= peer_public_key_;

    def encrypt(self, message):
	encrypted_message_length=c_int(0);
	if themis.themis_secure_message_wrap(self.private_key[0] ,self.private_key[1], self.peer_public_key[0], self.peer_public_key[1], message[0], message[1], None, byref(encrypted_message_length)) != -4:
	    raise themis_exception("themis_secure_message_wrap encrypting (encrypted message length determination) error");
	encrypted_message=create_string_buffer(encrypted_message_length.value);
	if themis.themis_secure_message_wrap(self.private_key[0] ,self.private_key[1], self.peer_public_key[0], self.peer_public_key[1], message[0], message[1], encrypted_message, byref(encrypted_message_length)) != 0:
	    raise themis_exception("themis_secure_message_wrap encrypting error");
	return (encrypted_message, encrypted_message_length.value);
	
class themis_smessage_decrypter(object):
    def __init__(self, private_key_, peer_public_key_):
	self.private_key=private_key_;
	self.peer_public_key=peer_public_key_;

    def decrypt(self, message):
	plain_message_length=c_int(0);
	if themis.themis_secure_message_unwrap(self.private_key[0], self.private_key[1], self.peer_public_key[0] ,self.peer_public_key[1], message[0], message[1], None, byref(plain_message_length)) != -4:
	    raise themis_exception("themis_secure_message_unwrap decrypting (plain message length determination) error");
	plain_message=create_string_buffer(plain_message_length.value);
	if themis.themis_secure_message_unwrap(self.private_key[0], self.private_key[1], self.peer_public_key[0] ,self.peer_public_key[1], message[0], message[1], plain_message, byref(plain_message_length)) !=0:
	    raise themis_exception("themis_secure_message_unwrap decrypting error");
	return (plain_message, plain_message_length.value);


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