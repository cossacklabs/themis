import sys
import math

from base64 import b64encode, b64decode
from json.scanner import NUMBER_RE
from json.decoder import NaN, PosInf, NegInf

from pythemis.scell import scell_seal

__author__ = 'Lagovas <lagovas.lagovas@gmail.com>'


PY2 = sys.version_info[0] == 2
PY3 = sys.version_info[0] == 3
PY34 = sys.version_info[0:2] >= (3, 4)

if PY3:
    string_types = str,
    integer_types = int,
    class_types = type,
    text_type = str
    binary_type = bytes

    MAXSIZE = sys.maxsize
else:
    import types
    string_types = basestring,
    integer_types = (int, long)
    class_types = (type, types.ClassType)
    text_type = unicode
    binary_type = str

    if sys.platform.startswith("java"):
        # Jython always uses 32 bits.
        MAXSIZE = int((1 << 31) - 1)
    else:
        # It's possible to have sizeof(long) != sizeof(Py_ssize_t).
        class X(object):

            def __len__(self):
                return 1 << 31
        try:
            len(X())
        except OverflowError:
            # 32-bit
            MAXSIZE = int((1 << 31) - 1)
        else:
            # 64-bit
            MAXSIZE = int((1 << 63) - 1)
        del X


class ReverseJSONValue(object):
    def encode(self, obj):
        """
        according to http://json.org/ json accept next types:
        string, number, object, array, true, false, null
        let handle all of them
        """
        if isinstance(obj, integer_types):
            return -obj
        elif isinstance(obj, float):
            if math.isnan(obj):
                return obj
            elif obj == float('-inf'):
                return float('inf')
            elif obj == float('inf'):
                return float('-inf')
            else:
                return -obj
        elif isinstance(obj, bool):
            return not obj
        elif obj is None:
            return None
        elif isinstance(obj, string_types):
            return obj[::-1]
        elif isinstance(obj, list):
            return obj[::-1]
        elif isinstance(obj, dict):
            # recursively call for every key and value
            obj = {self.encode(k): self.encode(v)
                   for k, v in obj.items()}
            return obj
        else:
            raise TypeError()

    def decode(self, obj):
        return self.encode(obj)


class SCellJSONEncoderDecoder(object):
    def __init__(self, shared_key):
        self.scell = scell_seal(shared_key)

    def wrap(self, value):
        """encrypt using scell and encode in base64"""
        value = self.scell.encrypt(value.encode('utf-8'))
        return b64encode(value).decode('ascii')

    def unwrap(self, value):
        """decode from base64 and decrypt using scell"""
        value = b64decode(value.encode('ascii'))
        value = self.scell.decrypt(value)
        return value.decode('utf-8')

    def encode(self, obj):
        """encrypt object that can be int, long, float, str, unicode, bool, None,
        inf, -inf, nan, """
        if isinstance(obj, integer_types):
            return self.wrap(text_type(obj))
        elif isinstance(obj, float):
            if math.isnan(obj):
                return self.wrap('NaN')
            elif obj == float('-inf'):
                return self.wrap('-Infinity')
            elif obj == float('inf'):
                return self.wrap('Infinity')
            else:
                return self.wrap(text_type(obj))
        elif isinstance(obj, bool):
            value = 'true' if obj else 'false'
            return self.wrap(value)
        elif obj is None:
            return self.wrap('null')
        elif isinstance(obj, string_types):
            return self.wrap(obj)
        elif isinstance(obj, list):
            obj = [self.encode(i) for i in obj]
            return obj
        elif isinstance(obj, dict):
            obj = {key: self.encode(value)
                   for key, value in obj.items()}
            return obj
        else:
            raise TypeError()

    def decode(self, obj):
        if isinstance(obj, list):
            obj = [self.decode(i) for i in obj]
            return obj
        elif isinstance(obj, dict):
            decoded = {}
            for key, value in obj.items():
                value = self.decode(value)
                decoded[key] = value
            return decoded
        elif isinstance(obj, string_types):
            obj = self.unwrap(obj)
            nextchar = obj[0]
            if nextchar == 'n' and obj == 'null':
                return None
            elif nextchar == 't' and obj == 'true':
                return True
            elif nextchar == 'f' and obj == 'false':
                return False

            m = NUMBER_RE.match(obj)
            if m is not None:
                integer, frac, exp = m.groups()
                if frac or exp:
                    res = float(integer + (frac or '') + (exp or ''))
                else:
                    res = int(integer)
                return res
            elif nextchar == 'N' and obj == 'NaN':
                return NaN
            elif nextchar == 'I' and obj == 'Infinity':
                return PosInf
            elif nextchar == '-' and obj == '-Infinity':
                return NegInf
            else:
                if not isinstance(obj, string_types):
                    try:
                        return obj.decode('utf-8')
                    except UnicodeDecodeError:
                        return obj
                else:
                    return obj

        else:
            raise TypeError()


class SCellJSONContextAwareEncoderDecoder(object):
    def __init__(self, shared_key):
        self.scell = scell_seal(shared_key)

    def wrap(self, value, context=None):
        """encrypt using scell and encode in base64"""
        if context:
            value = self.scell.encrypt(value.encode('utf-8'), context)
        else:
            value = self.scell.encrypt(value.encode('utf-8'))
        return b64encode(value).decode('ascii')

    def unwrap(self, value, context=None):
        """decode from base64 and decrypt using scell"""
        value = b64decode(value.encode('ascii'))
        if context:
            value = self.scell.decrypt(value, context)
        else:
            value = self.scell.decrypt(value)
        return value.decode('utf-8')

    def encode(self, obj, context=None):
        """encrypt object that can be int, long, float, str, unicode, bool, None,
        inf, -inf, nan, """
        if isinstance(obj, integer_types):
            return self.wrap(text_type(obj), context)
        elif isinstance(obj, float):
            if math.isnan(obj):
                return self.wrap('NaN', context)
            elif obj == float('-inf'):
                return self.wrap('-Infinity', context)
            elif obj == float('inf'):
                return self.wrap('Infinity', context)
            else:
                return self.wrap(text_type(obj), context)
        elif isinstance(obj, bool):
            value = 'true' if obj else 'false'
            return self.wrap(value, context)
        elif obj is None:
            return self.wrap('null', context)
        elif isinstance(obj, string_types):
            return self.wrap(obj, context)
        elif isinstance(obj, list):
            obj = [self.encode(i, context) for i in obj]
            return obj
        elif isinstance(obj, dict):
            output = {}
            for key, value in obj.items():
                if context:
                    # use explicit context
                    output[key] = self.encode(value, context)
                else:
                    # if context is None use key as context for end values
                    if isinstance(value, dict):
                        output[key] = self.encode(value, None)
                    else:
                        output[key] = self.encode(value, key.encode('utf-8'))
            return output
        else:
            raise TypeError()

    def decode(self, obj, context=None):
        if isinstance(obj, list):
            obj = [self.decode(i, context) for i in obj]
            return obj
        elif isinstance(obj, dict):
            output = {}
            for key, value in obj.items():
                if context:
                    # use explicit context
                    output[key] = self.decode(value, context)
                else:
                    # if context is None use key as context for end values
                    if isinstance(value, dict):
                        output[key] = self.decode(value, None)
                    else:
                        output[key] = self.decode(value, key.encode('utf-8'))
            return output
        elif isinstance(obj, string_types):
            obj = self.unwrap(obj, context)
            nextchar = obj[0]
            if nextchar == 'n' and obj == 'null':
                return None
            elif nextchar == 't' and obj == 'true':
                return True
            elif nextchar == 'f' and obj == 'false':
                return False

            m = NUMBER_RE.match(obj)
            if m is not None:
                integer, frac, exp = m.groups()
                if frac or exp:
                    res = float(integer + (frac or '') + (exp or ''))
                else:
                    res = int(integer)
                return res
            elif nextchar == 'N' and obj == 'NaN':
                return NaN
            elif nextchar == 'I' and obj == 'Infinity':
                return PosInf
            elif nextchar == '-' and obj == '-Infinity':
                return NegInf
            else:
                if not isinstance(obj, string_types):
                    try:
                        return obj.decode('utf-8')
                    except UnicodeDecodeError:
                        return obj
                else:
                    return obj

        else:
            raise TypeError()
