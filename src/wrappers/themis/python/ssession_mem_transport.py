import ssession;
from collections import deque;
import time;

class mem_transport(object):
    def __init__(self):
        self.message_list=deque();

    def send(self, message):
        self.message_list.append(message);

    def receive(self, buffer_length):
        return self.message_list.popleft();

#    def get_pub_key_by_id(self, user_id); //need pure virtual function
