import exception;
import ctypes;
themis = ctypes.cdll.LoadLibrary('./libthemis.so');


ON_GET_PUBLIC_KEY = ctypes.CFUNCTYPE(ctypes.c_int, ctypes.POINTER(ctypes.c_byte), ctypes.c_size_t, ctypes.POINTER(ctypes.c_byte), ctypes.c_size_t, ctypes.py_object);
ON_SEND_DATA = ctypes.CFUNCTYPE(ctypes.c_ssize_t, ctypes.POINTER(ctypes.c_byte), ctypes.c_size_t, ctypes.POINTER(ctypes.py_object));
ON_RECEIVE_DATA = ctypes.CFUNCTYPE(ctypes.c_ssize_t, ctypes.POINTER(ctypes.c_byte), ctypes.c_size_t, ctypes.POINTER(ctypes.py_object));
ON_STATE_CHANGE = ctypes.CFUNCTYPE(ctypes.c_int, ctypes.c_int, ctypes.c_void_p);

class transport_t(ctypes.Structure):
    _fields_ = [("send_data", ON_SEND_DATA),
		("receive_data", ON_RECEIVE_DATA),
		("state_changed", ON_STATE_CHANGE),
		("get_public_key_for_id", ON_GET_PUBLIC_KEY),
		("user_data", ctypes.POINTER(ctypes.py_object))];

class ssession(object):
    def __init__(self, user_id, sign_key, transport):
	self.session_ctx=ctypes.POINTER(ctypes.c_int);
	self.session_ctx=themis.secure_session_create(user_id, len(user_id), sign_key, len(sign_key), transport);
	if self.session_ctx==None:
	    raise exception.themis_exception("secure_session_create fail");

    def __del__(self):
	themis.secure_session_destroy(self.session_ctx);

    def connect(self):
	res = themis.secure_session_connect(self.session_ctx);
	if res != 0:
	    raise exception.themis_exception("secure_session_connect failed: " + str(res));

    def send(self, message):
	send_message=ctypes.create_string_buffer(message);
	res = themis.secure_session_send(self.session_ctx, ctypes.byref(send_message), len(message));
#	if res < 0:
#	    raise exception.themis_exception("secure_session_send fail: " + str(res));
	return res;

    def receive(self):
	message=ctypes.create_string_buffer(1024);
	message_length=ctypes.c_size_t(1024);
	res=themis.secure_session_receive(self.session_ctx, message, message_length);
	if res > 0 :
	    return ctypes.string_at(message, res), res;
	return message, res;