#!/usr/bin/env python
# coding: utf-8
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

import warnings
import ctypes
from enum import IntEnum

from . import exception, themis
from .exception import THEMIS_CODES

themis.secure_comparator_get_result.restype = ctypes.c_int32

scomparator_create = themis.secure_comparator_create
scomparator_create.restype = ctypes.POINTER(ctypes.c_int)


class SCOMPARATOR_CODES(IntEnum):
    MATCH = 21
    NOT_MATCH = 22
    NOT_READY = 0


class SComparator(object):
    def __init__(self, shared_secret):
        self.session_ctx = ctypes.POINTER(ctypes.c_int)
        self.comparator_ctx = scomparator_create()
        if not self.comparator_ctx:
            raise exception.ThemisError(THEMIS_CODES.FAIL,
                                        "Secure Comparator failed creating")
        res = themis.secure_comparator_append_secret(
            self.comparator_ctx,
            ctypes.byref(ctypes.create_string_buffer(shared_secret)),
            len(shared_secret))
        if res != THEMIS_CODES.SUCCESS:
            raise exception.ThemisError(
                THEMIS_CODES.FAIL, "Secure Comparator failed appending secret")

        self.comparation_complete = False

    def __del__(self):
        themis.secure_comparator_destroy(self.comparator_ctx)

    def begin_compare(self):
        req_size = ctypes.c_int(0)
        res = themis.secure_comparator_begin_compare(self.comparator_ctx, None,
                                                     ctypes.byref(req_size))
        if res != THEMIS_CODES.BUFFER_TOO_SMALL:
            raise exception.ThemisError(
                res, "Secure Comparator failed making initialization message")
        req_buffer = ctypes.create_string_buffer(req_size.value)
        res = themis.secure_comparator_begin_compare(
            self.comparator_ctx, ctypes.byref(req_buffer),
            ctypes.byref(req_size))
        if res != THEMIS_CODES.SUCCESS and res != THEMIS_CODES.SEND_AS_IS:
            raise exception.ThemisError(
                res, "Secure Comparator failed making initialization message")
        return ctypes.string_at(req_buffer, req_size)

    def proceed_compare(self, control_message):
        c_message = ctypes.create_string_buffer(control_message)
        req_size = ctypes.c_int(0)
        res = themis.secure_comparator_proceed_compare(
            self.comparator_ctx, ctypes.byref(c_message), len(control_message),
            None, ctypes.byref(req_size))

        if res == THEMIS_CODES.SUCCESS:
            return ""

        if res != THEMIS_CODES.BUFFER_TOO_SMALL:
            raise exception.ThemisError(
                res, "Secure Comparator failed proceeding message")
        req_buffer = ctypes.create_string_buffer(req_size.value)
        res = themis.secure_comparator_proceed_compare(
            self.comparator_ctx, ctypes.byref(c_message), len(control_message),
            req_buffer, ctypes.byref(req_size))
        if res == THEMIS_CODES.SEND_AS_IS or res == THEMIS_CODES.SUCCESS:
            return ctypes.string_at(req_buffer, req_size)

        raise exception.ThemisError(
            res, "Secure Comparator failed proceeding message")

    def is_compared(self):
        return not (themis.secure_comparator_get_result(self.comparator_ctx) ==
                    SCOMPARATOR_CODES.NOT_READY)

    def is_equal(self):
        return (themis.secure_comparator_get_result(self.comparator_ctx) ==
                SCOMPARATOR_CODES.MATCH)

    def result(self):
        return themis.secure_comparator_get_result(self.comparator_ctx)


class scomparator(SComparator):
    def __init__(self, *args, **kwargs):
        warnings.warn("scomparator is deprecated in favor of SComparator.")
        super(scomparator, self).__init__(*args, **kwargs)
