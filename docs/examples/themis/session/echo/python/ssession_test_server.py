import ssession;
import socket;
import ctypes;

client_pub_type = ctypes.c_byte*45;
client_pub = client_pub_type(0x55, 0x45, 0x43, 0x32, 0x00, 0x00, 0x00, 0x2d, 0x13, 0x8b, 0xdf, 0x0c, 0x02, 0x1f, 0x09, 0x88, 0x39, 0xd9, 0x73, 0x3a, 0x84, 0x8f, 0xa8, 0x50, 0xd9, 0x2b, 0xed, 0x3d, 0x38, 0xcf, 0x1d, 0xd0, 0xce, 0xf4, 0xae, 0xdb, 0xcf, 0xaf, 0xcb, 0x6b, 0xa5, 0x4a, 0x08, 0x11, 0x21);

server_priv_type = ctypes.c_byte*45;
server_priv=server_priv_type(0x52, 0x45, 0x43, 0x32, 0x00, 0x00, 0x00, 0x2d, 0x49, 0x87, 0x04, 0x6b, 0x00, 0xf2, 0x06, 0x07, 0x7d, 0xc7, 0x1c, 0x59, 0xa1, 0x8f, 0x39, 0xfc, 0x94, 0x81, 0x3f, 0x9e, 0xc5, 0xba, 0x70, 0x6f, 0x93, 0x08, 0x8d, 0xe3, 0x85, 0x82, 0x5b, 0xf8, 0x3f, 0xc6, 0x9f, 0x0b, 0xdf);

def on_send(data, data_length, user_data):
#    print("send ", data_length, "bytes:", ctypes.string_at(data,data_length));
    user_data[0].sendall(ctypes.string_at(data,data_length));
    return data_length;

def on_receive(buffer, buffer_length, user_data):
    received_data=user_data[0].recv(buffer_length);
    ctypes.memmove(buffer, received_data, len(received_data));
#    print "receive", len(received_data), "bytes", repr(buffer);
    return len(received_data);

def on_get_pub_key(user_id, id_length, key_buffer, key_buffer_length, user_data):
    real_user_id=ctypes.string_at(user_id, id_length);
    if id_length != len("client"):
	return -1;
    if real_user_id != "client":
	return -1;
    ctypes.memmove(key_buffer, client_pub, len(client_pub));
    return 0;

def on_change_status(buffer_length, user_data):
    return 0;

conn = ctypes.py_object(socket.socket());
conn.value.bind(("127.0.0.1", 26260));
conn.value.listen(1);
accepted, addr = conn.value.accept();

lp_conn_type=ctypes.POINTER(ctypes.py_object);
lp_conn=lp_conn_type(ctypes.py_object(accepted));

on_send_=ssession.ON_SEND_DATA(on_send);
on_receive_=ssession.ON_RECEIVE_DATA(on_receive);
on_change_status_=ssession.ON_STATE_CHANGE(on_change_status);
on_get_pub_key_=ssession.ON_GET_PUBLIC_KEY(on_get_pub_key);

transport=ssession.transport_t(on_send_,on_receive_, on_change_status_, on_get_pub_key_, lp_conn);
lp_transport_type=ctypes.POINTER(ssession.transport_t);
lp_transport=lp_transport_type(transport)


session=ssession.ssession("server", server_priv, lp_transport);

while True:
    message, res=session.receive();
    if res > 0:
	print "receive: ", message;
	if message == "finish":
	    break;
	session.send(message);

accepted.close();
conn.value.close();