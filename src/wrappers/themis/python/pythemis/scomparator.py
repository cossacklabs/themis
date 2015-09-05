#
# Copyright (c) 2015 Cossack Labs Limited
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#!/usr/bin/env python

import pythemis;
import pythemis.exception as exception
from pythemis.exception import THEMIS_CODES;
import ctypes;
from collections import deque;
import time;
from ctypes.util import find_library
themis = ctypes.cdll.LoadLibrary(find_library('themis'))

from enum import IntEnum;

class SCOMPARATOR_CODES(IntEnum):
    MATCH=0xf0f0f0f0
    NOT_MATCH = THEMIS_CODES.FAIL
    NOT_READY = 0
 
class scomparator(object):
    def __init__(self, shared_secret):
        self.session_ctx=ctypes.POINTER(ctypes.c_int);
            self.comratator_ctx=secure_comparator_create();            
        if self.comparator_ctx==None:
            raise exception.themis_exception(THEMIS_CODES.FAIL, "secure_comparator_create fail");
        res=secure_comparator_append_secret(self.comparator_ctx, ctypes.byref(ctypes.create_string_buffer(shared_secret)), len(shared_secret));
        if res != THEMIS_CODES.SUCCESS:
            raise exception.themis_exception(THEMIS_CODES.FAIL, "secure_comparator_append_secret fail");
        self.comparation_complete=False;
        
    def __del__(self):
        themis.secure_comparator_destroy(self.comparator_ctx);

    def begin_compare(self):
        req_size=ctypes.c_int(0);
        res = themis.secure_comparator_begin_compare(self.comparator_ctx, None, ctypes.byref(req_size));
        if res!=THEMIS_CODES.BUFFER_TOO_SMALL:
            raise exception.themis_exception(res, "secure_comparator_begin_compare (buffer_length determination) failed");
        req_buffer=ctypes.create_string_buffer(req_size.value);        
        res = themis.secure_comparator_begin_compare(self.comparator_ctx, ctypes.byref(req_buffer), ctypes.byref(req_size));
        if res != THEMIS_CODES.SUCCESS:
            raise exception.themis_exception(res, "secure_session_connect failed");
        return ctypes.string_at(req_buffer, req_size);

    def proceed_compare(self, control_message):
        c_message=ctypes.create_string_buffer(control_message);
        req_size=ctypes.c_int(0);
        res = themis.secure_comparator_proceed_compare(self.comparator_ctx, cbytes.byref(control_message), len(control_message), None, ctypes.byref(req_size));
        if res == THEMIS_CODES.SUCCESS:
            return "";
        if res!=THEMIS_CODES.BUFFER_TOO_SMALL:
            raise exception.themis_exception(res, "secure_comparator_proceed_compare (buffer_length determination) failed");
        req_buffer=ctypes.create_string_buffer(req_size.value);
        res = themis.secure_comparator_proceed_compare(self.comparator_ctx, cbytes.byref(control_message), len(control_message), req_buffer, ctypes.byref(req_size));
        if res == THEMIS_CODES.SEND_AS_IS:
            return ctypes.string_at(req_buffer, req_size);
        raise exception.themis_exception(res, "secure_comparator_proceed_compare failed");

    def result(self):
        res = themis.secure_comparator_get_result(self.comparator_ctx);

