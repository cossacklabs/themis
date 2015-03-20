#ssession wrappers for non event handled transport (like olain socket)
import themis.ssession;

class ssession_server(object):
    def __init__(self, user_id, sign_key, transport):
        self.session=themis.ssession.ssession(user_id, sign_key, transport);
        while self.session.is_established()!=True: 	#1. establish session
            self.session.receive();		   	#2. establish session

    def receive(self):
        return self.session.receive();			#receive message

    def send(self, message):
        self.session.send(message);			#send message



class ssession_client(object):
    def __init__(self, user_id, sign_key, transport):
        self.session=themis.ssession.ssession(user_id, sign_key, transport);
        self.session.connect();				#1. establish session
        while self.session.is_established()!=True:	#2  establish session
            self.session.receive();			#3. establish session

    def receive(self):
        return self.session.receive();			#receive message

    def send(self, message):
        self.session.send(message);			#send message
