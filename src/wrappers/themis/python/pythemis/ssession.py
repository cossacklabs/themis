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

from collections import deque

import ctypes
from ctypes.util import find_library

from . import exception as exception
from .exception import THEMIS_CODES

themis = ctypes.cdll.LoadLibrary(find_library('themis'))


ON_GET_PUBLIC_KEY = ctypes.CFUNCTYPE(
    ctypes.c_int, ctypes.POINTER(ctypes.c_byte),
    ctypes.c_size_t, ctypes.POINTER(ctypes.c_byte),
    ctypes.c_size_t, ctypes.POINTER(ctypes.py_object))
ON_SEND_DATA = ctypes.CFUNCTYPE(
    ctypes.c_ssize_t, ctypes.POINTER(ctypes.c_byte),
    ctypes.c_size_t, ctypes.POINTER(ctypes.py_object))
ON_RECEIVE_DATA = ctypes.CFUNCTYPE(
    ctypes.c_ssize_t, ctypes.POINTER(ctypes.c_byte),
    ctypes.c_size_t, ctypes.POINTER(ctypes.py_object))
ON_STATE_CHANGE = ctypes.CFUNCTYPE(ctypes.c_int, ctypes.c_int, ctypes.c_void_p)


class transport_t(ctypes.Structure):
    # set of callbacks
    _fields_ = [
        # use for "sending" data.
        # if set to None - secure session methods send/receive not usable
        ("send_data", ON_SEND_DATA),
        # use for "receiving" data.
        # if set to None - secure session methods send/receive not usable
        ("receive_data", ON_RECEIVE_DATA),
        # not used for in current version
        ("state_changed", ON_STATE_CHANGE),
        # [necessary] use for getting peer public key by it ID
        # (see ssession.__init__ method).
        ("get_public_key_for_id", ON_GET_PUBLIC_KEY),
        # some user_data, that will be passed to any of callbacks
        ("user_data", ctypes.POINTER(ctypes.py_object))]

ssession_create = themis.secure_session_create
ssession_create.restype = ctypes.POINTER(ctypes.c_int)
ssession_create.argtypes = [ctypes.c_void_p, ctypes.c_size_t,
                            ctypes.c_void_p, ctypes.c_size_t,
                            ctypes.POINTER(transport_t)]


def on_send(data, data_length, user_data):
    try:
        user_data[0].send(ctypes.string_at(data, data_length))
    except Exception:
        return -2222
    return data_length


def on_receive(data, data_length, user_data):
    try:
        received_data = user_data[0].receive(data_length)
    except Exception as e:
        print(e)
        return -2222
    ctypes.memmove(data, received_data, len(received_data))
    return len(received_data)


def on_get_pub_key(user_id, id_length, key_buffer, key_buffer_length,
                   user_data):
    real_user_id = ctypes.string_at(user_id, id_length)
    pub_key = user_data[0].get_pub_key_by_id(real_user_id)
    ctypes.memmove(key_buffer, pub_key, len(pub_key))
    return 0


def on_change_status(buffer_length, user_data):
    return 0


on_send_ = ON_SEND_DATA(on_send)
on_receive_ = ON_RECEIVE_DATA(on_receive)
on_change_status_ = ON_STATE_CHANGE(on_change_status)
on_get_pub_key_ = ON_GET_PUBLIC_KEY(on_get_pub_key)
lp_conn_type = ctypes.POINTER(ctypes.py_object)


class sstring(bytes):
    def __new__(cls, value):
        obj = bytes.__new__(cls, value)
        obj.is_control = False
        return obj

    def set_control(self):
        self.is_control = True

    def unset_control(self):
        self.is_control = False

    def is_control(self):
        return self.is_control

    def __str__(self):
        return bytes.__str__(self)


class ssession(object):
    def __init__(self, user_id, sign_key, transport):
        # user_id - user identification ("server" for example)
        # sign_key - private key of session owner
        # transport - refference for transport_t object.
        self.session_ctx = ctypes.POINTER(ctypes.c_int)
        if transport is None:
            self.session_ctx = ssession_create(
                ctypes.byref(ctypes.create_string_buffer(user_id)),
                len(user_id),
                ctypes.byref(ctypes.create_string_buffer(sign_key)),
                len(sign_key),
                0)
        else:
            self.lp_conn = lp_conn_type(ctypes.py_object(transport))
            self.transport_ = transport_t(on_send_, on_receive_,
                                          on_change_status_, on_get_pub_key_,
                                          self.lp_conn)
            self.session_ctx = ssession_create(
                ctypes.byref(ctypes.create_string_buffer(user_id)),
                len(user_id),
                ctypes.byref(ctypes.create_string_buffer(sign_key)),
                len(sign_key),
                ctypes.byref(self.transport_))
        if self.session_ctx is None:
            raise exception.themis_exception(THEMIS_CODES.FAIL,
                                             "Secure Session failed creating")

    def __del__(self):
        themis.secure_session_destroy(self.session_ctx)

    def connect(self):
        res = themis.secure_session_connect(self.session_ctx)
        if res != THEMIS_CODES.SUCCESS:
            raise exception.themis_exception(
                res, "Secure Session failed connecting")

    def send(self, message):
        send_message = ctypes.create_string_buffer(message)
        res = themis.secure_session_send(self.session_ctx,
                                         ctypes.byref(send_message),
                                         len(message))
        if res == THEMIS_CODES.NETWORK_ERROR:
            raise exception.themis_exception(res, "Secure Session failed sending")
        return res

    def receive(self):
        message = ctypes.create_string_buffer(1024)
        message_length = ctypes.c_size_t(1024)
        res = themis.secure_session_receive(self.session_ctx, message,
                                            message_length)
        if res == THEMIS_CODES.NETWORK_ERROR:
            raise exception.themis_exception(
                res, "Secure Session failed receiving")
        elif res < 0:
            return ""
        return ctypes.string_at(message, res)

    def is_established(self): 
        return themis.secure_session_is_established(self.session_ctx)

    def connect_request(self):
        req_size = ctypes.c_int(0)
        res = themis.secure_session_generate_connect_request(
            self.session_ctx, None, ctypes.byref(req_size))
        if res != THEMIS_CODES.BUFFER_TOO_SMALL:
            raise exception.themis_exception(
                res, "Secure Session failed generating connect request")
        req_buffer = ctypes.create_string_buffer(req_size.value)
        res = themis.secure_session_generate_connect_request(
            self.session_ctx, ctypes.byref(req_buffer), ctypes.byref(req_size))
        if res != THEMIS_CODES.SUCCESS:
            raise exception.themis_exception(
                res, "Secure Session failed generating connect request")
        return ctypes.string_at(req_buffer, req_size)

    def wrap(self, message):
        send_message = ctypes.create_string_buffer(message)
        wrapped_message_length = ctypes.c_int(0)
        res = themis.secure_session_wrap(
            self.session_ctx, ctypes.byref(send_message), len(message), 0,
            ctypes.byref(wrapped_message_length))
        if res != THEMIS_CODES.BUFFER_TOO_SMALL:
            raise exception.themis_exception(
                res, "Secure Session failed encrypting")
        wrapped_message = ctypes.create_string_buffer(
            wrapped_message_length.value)
        res = themis.secure_session_wrap(
            self.session_ctx, ctypes.byref(send_message), len(message),
            ctypes.byref(wrapped_message), ctypes.byref(wrapped_message_length))
        if res != THEMIS_CODES.SUCCESS:
            raise exception.themis_exception(res,"Secure Session failed encrypting")
        return ctypes.string_at(wrapped_message, wrapped_message_length)

    def unwrap(self, message):
        wrapped_message = ctypes.create_string_buffer(message)
        unwrapped_message_length = ctypes.c_int(0)
        res = themis.secure_session_unwrap(
            self.session_ctx, wrapped_message, len(message), 0,
            ctypes.byref(unwrapped_message_length))
        if res == THEMIS_CODES.SUCCESS:
            return sstring(b"")
        if res != THEMIS_CODES.BUFFER_TOO_SMALL:
            raise exception.themis_exception(
                res, "Secure Session failed decrypting")
        unwrapped_message = ctypes.create_string_buffer(
            unwrapped_message_length.value)
        res = themis.secure_session_unwrap(
            self.session_ctx, wrapped_message, len(message),
            ctypes.byref(unwrapped_message),
            ctypes.byref(unwrapped_message_length))
        rez = sstring(ctypes.string_at(unwrapped_message,
                                       unwrapped_message_length))
        if res == THEMIS_CODES.SEND_AS_IS:
            rez.set_control()
        elif res != THEMIS_CODES.SUCCESS:
            raise exception.themis_exception(
                res, "Secure Session failed decrypting")
        return rez


class mem_transport(object):
    def __init__(self):
        self.message_list = deque()

    def send(self, message):
        self.message_list.append(message)

    def receive(self, buffer_length):
        return self.message_list.popleft()

#    def get_pub_key_by_id(self, user_id); //need pure virtual function
