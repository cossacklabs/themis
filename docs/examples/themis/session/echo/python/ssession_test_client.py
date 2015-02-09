import ssession;
import socket;
import ctypes;

client_priv_type = ctypes.c_byte*45;
client_priv = client_priv_type(0x52, 0x45, 0x43, 0x32, 0x00, 0x00, 0x00, 0x2d, 0x51, 0xf4, 0xaa, 0x72, 0x00, 0x9f, 0x0f, 0x09, 0xce, 0xbe, 0x09, 0x33, 0xc2, 0x5e, 0x9a, 0x05, 0x99, 0x53, 0x9d, 0xb2, 0x32, 0xa2, 0x34, 0x64, 0x7a, 0xde, 0xde, 0x83, 0x8f, 0x65, 0xa9, 0x2a, 0x14, 0x6d, 0xaa, 0x90, 0x01);

server_pub_type = ctypes.c_byte*45;
server_pub=server_pub_type(0x55, 0x45, 0x43, 0x32, 0x00, 0x00, 0x00, 0x2d, 0x75, 0x58, 0x33, 0xd4, 0x02, 0x12, 0xdf, 0x1f, 0xe9, 0xea, 0x48, 0x11, 0xe1, 0xf9, 0x71, 0x8e, 0x24, 0x11, 0xcb, 0xfd, 0xc0, 0xa3, 0x6e, 0xd6, 0xac, 0x88, 0xb6, 0x44, 0xc2, 0x9a, 0x24, 0x84, 0xee, 0x50, 0x4c, 0x3e, 0xa0);

def on_send(data, data_length, user_data):
#    print("send", data_length, " bytes: ",ctypes.string_at(data,data_length));
    user_data[0].sendall(ctypes.string_at(data,data_length));
    return data_length;

def on_receive(buffer, buffer_length, user_data):
    received_data=user_data[0].recv(buffer_length);
    ctypes.memmove(buffer, received_data, len(received_data));
#    print("receive ", len(received_data), " bytes:", repr(buffer));
    return len(received_data);

def on_get_pub_key(user_id, id_length, key_buffer, key_buffer_length, user_data):
    real_user_id=ctypes.string_at(user_id, id_length);
    if id_length != len("server"):
	return -1;
    if real_user_id != "server":
	return -1;
    ctypes.memmove(key_buffer, server_pub, len(server_pub));
    return 0;

def on_change_status(buffer_length, user_data):
    return 0;

conn = ctypes.py_object(socket.socket());
conn.value.connect(("127.0.0.1", 26260));

lp_conn_type=ctypes.POINTER(ctypes.py_object);
lp_conn=lp_conn_type(conn);

on_send_=ssession.ON_SEND_DATA(on_send);
on_receive_=ssession.ON_RECEIVE_DATA(on_receive);
on_change_status_=ssession.ON_STATE_CHANGE(on_change_status);
on_get_pub_key_=ssession.ON_GET_PUBLIC_KEY(on_get_pub_key);

transport=ssession.transport_t(on_send_,on_receive_, on_change_status_, on_get_pub_key_, lp_conn);
lp_transport_type=ctypes.POINTER(ssession.transport_t);
lp_transport=lp_transport_type(transport)


session=ssession.ssession("client", client_priv, lp_transport);
session.connect();
for i in range(0, 9):
    session.send("This is a test message");
    message, res=session.receive();
    if res < 0:
	print "some error";
    if res > 0:
	print "receive: ", message;

session.send("finish");

conn.value.close();