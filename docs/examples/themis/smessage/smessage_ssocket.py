import smessage;
import socket;

class ssocket(object):
    def __init__(self, priv_key, peer_public_key=None, socket_=None):
        if socket_==None:
            self.socket=socket.socket();
        else:
            self.socket=socket_;
        self.priv_key=priv_key;
        if peer_public_key==None:
            self.message=None;
        else:
            self.smessage=smessage.smessage(priv_key, peer_public_key);
        
    def connect(self, conn_param):
        return self.socket.connect(conn_param);
    
    def sendall(self, message):
        self.socket.sendall(self.smessage.wrap(message));

    def recv(self, buffer_length):
        aa=self.socket.recv(buffer_length);
        return self.smessage.unwrap(aa);

    def close(self):
        return self.socket.close();

    def bind(self, param):
        return self.socket.bind(param);

    def listen(self, param):
        return self.socket.listen(param);

    def accept(self):
        self.accepted, self.addr = self.socket.accept();
        return ssocket(self.priv_key, None, self.accepted), self.addr;
        
    def set_peer_pub_key(self,peer_public_key):
        self.smessage=smessage.smessage(self.priv_key, peer_public_key);
