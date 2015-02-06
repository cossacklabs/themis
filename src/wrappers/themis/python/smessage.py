import exception;
from ctypes import *
themis = cdll.LoadLibrary('./libthemis.so')

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


