import exception;
from ctypes import *;

ON_GET_PUBLIC_KEY = CFUNCTYPE(c_int, POINTER(c_byte), c_size_t, POINTER(c_byte), c_size_t, POINTER(c_byte));
ON_SEND_DATA = CFUNCTYPE(c_int, POINTER(c_byte), c_size_t, POINTER(c_byte));
ON_RECEIVE_DATA = CFUNCTYPE(c_ssize_t, POINTER(c_byte), c_size_t, POINTER(c_byte));
ON_STATE_CHANGE = CFUNCTYPE(int, c_int, c_void_p);

def py_on_get_public_key(id, id_length, key_buffer, key_buffer_length, user_data):
    print("on_get_public_key");
    return 0;


class transport_t(Structure):
    _fields_ = [("send_data", ON_SEND_DATA),
		("receive_data", ON_RECEIVE_DATA),
		("state_changed", ON_STATE_CHANGE),
		("get_public_key_for_id", ON_GET_PUBLIC_KEY),
		("user_data", c_void_p)];

class ssession(object):
    def __init__(self, user_id, sign_key, transport):
	session_ctx=
	if secure_session_init(self.)
	
	