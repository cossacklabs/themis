import themis.ssession;

class ssession_server(object):
    def __init__(self, user_id, sign_key, transport):
        self.session=themis.ssession.ssession(user_id, sign_key, transport);
        while self.session.is_established()!=True:
            self.session.receive();

    def receive(self):
        return self.session.receive();

    def send(self, message):
        self.session.send(message);



class ssession_client(object):
    def __init__(self, user_id, sign_key, transport):
        self.session=themis.ssession.ssession(user_id, sign_key, transport);
        self.session.connect();
        while self.session.is_established()!=True:
            self.session.receive();

    def receive(self):
        return self.session.receive();

    def send(self, message):
        self.session.send(message);
