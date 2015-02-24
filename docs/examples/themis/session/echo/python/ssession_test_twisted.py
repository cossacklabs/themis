import ssession;
import ssession_mem_transport;
from twisted.internet import protocol, reactor, endpoints;

client_pub = str('\x55\x45\x43\x32\x00\x00\x00\x2d\x13\x8b\xdf\x0c\x02\x1f\x09\x88\x39\xd9\x73\x3a\x84\x8f\xa8\x50\xd9\x2b\xed\x3d\x38\xcf\x1d\xd0\xce\xf4\xae\xdb\xcf\xaf\xcb\x6b\xa5\x4a\x08\x11\x21');

server_priv= str('\x52\x45\x43\x32\x00\x00\x00\x2d\x49\x87\x04\x6b\x00\xf2\x06\x07\x7d\xc7\x1c\x59\xa1\x8f\x39\xfc\x94\x81\x3f\x9e\xc5\xba\x70\x6f\x93\x08\x8d\xe3\x85\x82\x5b\xf8\x3f\xc6\x9f\x0b\xdf');

class transport(ssession_mem_transport.mem_transport):
    def get_pub_key_by_id(self, user_id):
        if user_id != "client":
            raise Exception("no such id");
        return client_pub; 


class Tw_protocol(protocol.Protocol):
    def __init__(self):
	self.transport_=transport();
	self.session=ssession.ssession("server", server_priv, self.transport_);

    def dataReceived(self, data):
	self.transport_.send(data);
	message=self.session.receive();
	if len(message)==0:
	    self.transport.write(self.transport_.receive(2048));
	else: 
	    print message;
	    self.session.send(message);
	    self.transport.write(self.transport_.receive(2048));

class Tw_Factory(protocol.Factory):
    def buildProtocol(self, addr):
        return Tw_protocol();

endpoints.serverFromString(reactor, "tcp:26260").listen(Tw_Factory());
reactor.run();