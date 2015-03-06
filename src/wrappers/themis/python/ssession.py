import exception;
import ctypes;
themis = ctypes.cdll.LoadLibrary('./libthemis.so');


ON_GET_PUBLIC_KEY = ctypes.CFUNCTYPE(ctypes.c_int, ctypes.POINTER(ctypes.c_byte), ctypes.c_size_t, ctypes.POINTER(ctypes.c_byte), ctypes.c_size_t, ctypes.POINTER(ctypes.py_object));
ON_SEND_DATA = ctypes.CFUNCTYPE(ctypes.c_ssize_t, ctypes.POINTER(ctypes.c_byte), ctypes.c_size_t, ctypes.POINTER(ctypes.py_object));
ON_RECEIVE_DATA = ctypes.CFUNCTYPE(ctypes.c_ssize_t, ctypes.POINTER(ctypes.c_byte), ctypes.c_size_t, ctypes.POINTER(ctypes.py_object));
ON_STATE_CHANGE = ctypes.CFUNCTYPE(ctypes.c_int, ctypes.c_int, ctypes.c_void_p);

class transport_t(ctypes.Structure):
    _fields_ = [("send_data", ON_SEND_DATA),
		("receive_data", ON_RECEIVE_DATA),
		("state_changed", ON_STATE_CHANGE),
		("get_public_key_for_id", ON_GET_PUBLIC_KEY),
		("user_data", ctypes.POINTER(ctypes.py_object))];

ssession_create=themis.secure_session_create;
ssession_create.restype = ctypes.POINTER(ctypes.c_int);
ssession_create.argtypes = [ctypes.c_void_p, ctypes.c_size_t, ctypes.c_void_p, ctypes.c_size_t, ctypes.POINTER(transport_t)];


def on_send(data, data_length, user_data):
#    print("send ", data_length, "bytes:", ctypes.string_at(data,data_length));
    try:
        user_data[0].send(ctypes.string_at(data,data_length));
    except Exception as e:
        return -2222;
    return data_length;

def on_receive(data, data_length, user_data):
    try:
        received_data=user_data[0].receive(data_length);
    except Exception as e:
	print e;
        return -2222;
    ctypes.memmove(data, received_data, len(received_data));
#    print "receive", len(received_data), "bytes";
    return len(received_data);

def on_get_pub_key(user_id, id_length, key_buffer, key_buffer_length, user_data):
    real_user_id=ctypes.string_at(user_id, id_length);
#    print "get_pub_key", real_user_id, user_data;
    pub_key=user_data[0].get_pub_key_by_id(real_user_id);
    ctypes.memmove(key_buffer, pub_key, len(pub_key));
    return 0;

def on_change_status(buffer_length, user_data):
    return 0;

on_send_=ON_SEND_DATA(on_send);
on_receive_=ON_RECEIVE_DATA(on_receive);
on_change_status_=ON_STATE_CHANGE(on_change_status);
on_get_pub_key_=ON_GET_PUBLIC_KEY(on_get_pub_key);
lp_conn_type=ctypes.POINTER(ctypes.py_object);


class ssession(object):
    def __init__(self, user_id, sign_key, transport):
	self.session_ctx=ctypes.POINTER(ctypes.c_int);
        if transport != None:
            self.lp_conn=lp_conn_type(ctypes.py_object(transport));
            self.transport_=transport_t(on_send_, on_receive_, on_change_status_, on_get_pub_key_, self.lp_conn);
            self.session_ctx=ssession_create(ctypes.byref(ctypes.create_string_buffer(user_id)), len(user_id), ctypes.byref(ctypes.create_string_buffer(sign_key)), len(sign_key), ctypes.byref(self.transport_));
        else:
            self.session_ctx=ssession_create(ctypes.byref(ctypes.create_string_buffer(user_id)), len(user_id), ctypes.byref(ctypes.create_string_buffer(sign_key)), len(sign_key), 0);            
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
        if res == -2222:
	    raise exception.themis_exception("secure_session_send failed: " + str(res));            
	return res;

    def receive(self):
	message=ctypes.create_string_buffer(1024);
	message_length=ctypes.c_size_t(1024);
	res=themis.secure_session_receive(self.session_ctx, message, message_length);
	if res == -2222 :
	    raise exception.themis_exception("secure_session_receive failed: " + str(res));
	elif res<0:
	    return "";
        return ctypes.string_at(message, res);

    def is_established(self):
        return themis.secure_session_is_established(self.session_ctx);

    def connect_request(self):
        req_size=ctypes.c_int(0);
 	res = themis.secure_session_generate_connect_request(self.session_ctx, None, ctypes.byref(req_size));
        if res!=-4:
	    raise exception.themis_exception("secure_session_generate_connect_request (buffer_length determination) failed: " + str(res));
        req_buffer=ctypes.create_string_buffer(req_size.value);
 	res = themis.secure_session_generate_connect_request(self.session_ctx, ctypes.byref(req_buffer), ctypes.byref(req_size));
        if res!=0:
	    raise exception.themis_exception("secure_session_generate_connect_request failed: " + str(res));
        return ctypes.string_at(req_buffer, req_size);

    def wrap(self, message):
	send_message=ctypes.create_string_buffer(message);
        wrapped_message_length=ctypes.c_int(0);
	res = themis.secure_session_wrap(self.session_ctx, ctypes.byref(send_message), len(message), 0, ctypes.byref(wrapped_message_length));
        if res!=-4:
	    raise exception.themis_exception("secure_session_wrap (buffer_length determination) failed: " + str(res));
        wrapped_message=ctypes.create_string_buffer(wrapped_message_length.value);
	res = themis.secure_session_wrap(self.session_ctx, ctypes.byref(send_message), len(message), ctypes.byref(wrapped_message), ctypes.byref(wrapped_message_length));
        if res!=0:
	    raise exception.themis_exception("secure_session_wrap failed: " + str(res));
	return ctypes.string_at(wrapped_message, wrapped_message_length);

    def unwrap(self, message):
	wrapped_message=ctypes.create_string_buffer(message);
	unwrapped_message_length=ctypes.c_int(0);
	res=themis.secure_session_unwrap(self.session_ctx, wrapped_message, len(message), 0, ctypes.byref(unwrapped_message_length));
        if res==0:
            return (res, "");
        if res!=-4:
	    raise exception.themis_exception("secure_session_unwrap (buffer_length determination) failed: " + str(res));
        unwrapped_message=ctypes.create_string_buffer(unwrapped_message_length.value);
	res=themis.secure_session_unwrap(self.session_ctx, wrapped_message, len(message), ctypes.byref(unwrapped_message), ctypes.byref(unwrapped_message_length));
        if res<0:
	    raise exception.themis_exception("secure_session_unwrap failed: " + str(res));
        return (res, ctypes.string_at(unwrapped_message, unwrapped_message_length));    

class ssession_server(object):
    def __init__(self, user_id, sign_key, transport):
        self.session=ssession(user_id, sign_key, transport);
        while self.session.is_established()!=True:
            self.session.receive();

    def receive(self):
        return self.session.receive();

    def send(self, message):
        self.session.send(message);
        
class ssession_client(object):
    def __init__(self, user_id, sign_key, transport):
        self.session=ssession(user_id, sign_key, transport);
        self.session.connect();
        while self.session.is_established()!=True:
            self.session.receive();

    def receive(self):
        return self.session.receive();

    def send(self, message):
        self.session.send(message);
    
