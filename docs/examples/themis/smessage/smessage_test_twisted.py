import smessage;
from twisted.internet import protocol, reactor, endpoints;

client_pub = str('\x55\x45\x43\x32\x00\x00\x00\x2d\x13\x8b\xdf\x0c\x02\x1f\x09\x88\x39\xd9\x73\x3a\x84\x8f\xa8\x50\xd9\x2b\xed\x3d\x38\xcf\x1d\xd0\xce\xf4\xae\xdb\xcf\xaf\xcb\x6b\xa5\x4a\x08\x11\x21');

server_priv= str('\x52\x45\x43\x32\x00\x00\x00\x2d\x49\x87\x04\x6b\x00\xf2\x06\x07\x7d\xc7\x1c\x59\xa1\x8f\x39\xfc\x94\x81\x3f\x9e\xc5\xba\x70\x6f\x93\x08\x8d\xe3\x85\x82\x5b\xf8\x3f\xc6\x9f\x0b\xdf');

class Tw_protocol(protocol.Protocol):
    def __init__(self):
	self.encrypter=smessage.themis_smessage_encrypter(server_priv, client_pub);
        self.decrypter=smessage.themis_smessage_decrypter(server_priv, client_pub);

    def dataReceived(self, data):
        message = self.decrypter.decrypt(data);
        print message;
        self.transport.write(self.encrypter.encrypt(message));

class Tw_Factory(protocol.Factory):
    def buildProtocol(self, addr):
        return Tw_protocol();

endpoints.serverFromString(reactor, "tcp:26260").listen(Tw_Factory());
reactor.run();
