import exception;
from ctypes import *
themis = cdll.LoadLibrary('./libthemis.so')

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
	return string_at(self.private_key, self.private_key_length.value);

    def export_public_key(self):
	return string_at(self.public_key, self.public_key_length.value);
