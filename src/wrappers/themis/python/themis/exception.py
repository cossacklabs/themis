from enum import Enum;

class THEMIS_CODES(Enum):
    NETWORK_ERROR=-2222
    BUFFER_TOO_SMALL=-4
    FAIL=-1
    SUCCESS=0
    SEND_AS_IS=1


Animal = Enum('Animal', 'ant bee cat dog')

class themis_exception(Exception):
    def __init__(self, error_code, message):
	self.error_code=error_code
	self.message = message
    def __str__(self):
	return repr("Themis error: "+str(error_code)+" --"+self.message)

