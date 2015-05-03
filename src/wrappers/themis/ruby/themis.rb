#
# Copyright (C) 2015 CossackLabs
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

require 'ffi'

module ThemisCommon
    def string_to_pointer_size(string)
	string_buf = FFI::MemoryPointer.new(:char, string.size)
	string_buf.put_bytes(0, string)
	return string_buf, string.size
    end
    module_function :string_to_pointer_size
end

module ThemisImport
    extend FFI::Library
    ffi_lib 'themis'



    callback :get_pub_key_by_id_type, 	[:pointer, :int, :pointer, :int, :uint], :int
    callback :send_callback_type,	[:pointer, :int, :uint], :int
    callback :receive_callback_type,	[:pointer, :int, :uint], :int

    class Callbacks_struct < FFI::Struct
    layout	:send_data,		:send_callback_type,
		:receive_data,		:receive_callback_type,
		:state_changed,		:pointer,
		:get_pub_key_for_id,	:get_pub_key_by_id_type,
		:user_data,		:uint
    end

    attach_function :secure_session_create, [ :pointer, :uint, :pointer, :uint, :pointer], :pointer
    attach_function :secure_session_destroy, [ :pointer], :int
    attach_function :secure_session_generate_connect_request, [ :pointer, :pointer, :pointer], :int
    attach_function :secure_session_wrap, [ :pointer, :pointer, :int, :pointer, :pointer], :int
    attach_function :secure_session_unwrap, [ :pointer, :pointer, :int, :pointer, :pointer], :int
    attach_function :secure_session_is_established, [ :pointer], :int

    attach_function :themis_secure_message_wrap, [:pointer, :int, :pointer, :int, :pointer, :int, :pointer, :pointer], :int
    attach_function :themis_secure_message_unwrap, [:pointer, :int, :pointer, :int, :pointer, :int, :pointer, :pointer], :int

    attach_function :themis_gen_rsa_key_pair, [:pointer, :int, :pointer, :int], :int
    attach_function :themis_gen_ec_key_pair, [:pointer, :int, :pointer, :int], :int

end

module Themis

    BUFFER_TOO_SMALL = -4
    SUCCESS = 0
    FAIL = -1
    SEND_AS_IS = 1

    class ThemisError < StandardError
    end
    
    class Callbacks
	def get_pub_key_by_id(id)
	    raise ThemisError, "Callbacks not implemented: get_pub_key_by_id"
	end
	
	def send(message)
	    raise ThemisError, "Callbacks not implemented: send"
	end

	def receive()
	    raise ThemisError, "Callbacks not implemented: receive"
	end
    end

    class Ssession
	include ThemisCommon
	include ThemisImport

	MAPPING = {}

	Get_pub_key_by_id_callback = FFI::Function.new(:int, [:pointer, :int, :pointer, :int, :uint]) do |id_buf, id_length, pubkey_buf, pubkey_length, obj|
	    pub_key=MAPPING[obj].get_pub_key_by_id(id_buf.get_bytes(0,id_length))
	    if !pub_key
		-1
	    end
	    pubkey_buf.put_bytes(0, pub_key)
	    0
	end

	def initialize(id, private_key, transport)
	        id_buf, id_length = string_to_pointer_size(id)
		private_key_buf, private_key_length = string_to_pointer_size(private_key)
		@callbacks = Callbacks_struct.new
		@callbacks[:get_pub_key_for_id] = Get_pub_key_by_id_callback
		MAPPING[transport.object_id]=transport
		@callbacks[:user_data] =transport.object_id
		@session=secure_session_create(id_buf, id_length, private_key_buf, private_key_length, @callbacks);
	        raise ThemisError, "secure_session_create error" unless @session
	end
	
	def connect_request()
		connect_request_length = FFI::MemoryPointer.new(:uint)
		res=secure_session_generate_connect_request(@session, nil, connect_request_length)
		raise ThemisError, "secure_session_generate_connect_request (length determination) error: #{res}" unless res == BUFFER_TOO_SMALL
		connect_request = FFI::MemoryPointer.new(:char, connect_request_length.read_uint)
		res=secure_session_generate_connect_request(@session, connect_request, connect_request_length)
		raise ThemisError, "secure_session_generate_connect_request error: #{res}" unless res == SUCCESS
		return connect_request.get_bytes(0, connect_request_length.read_uint);
	end

	def unwrap(message)
		message_, message_length_=string_to_pointer_size(message)
		unwrapped_message_length = FFI::MemoryPointer.new(:uint)
		res=secure_session_unwrap(@session,  message_, message_length_, nil, unwrapped_message_length)
		raise ThemisError, "secure_session_unwrap (length determination) error: #{res}" unless res == BUFFER_TOO_SMALL || res == SUCCESS
		if res == SUCCESS
		    return SUCCESS, ""
		end
		unwrapped_message = FFI::MemoryPointer.new(:char, unwrapped_message_length.read_uint)
		res=secure_session_unwrap(@session, message_, message_length_, unwrapped_message, unwrapped_message_length)
		raise ThemisError, "secure_session_unwrap error: #{res}" unless res == SUCCESS || res == SEND_AS_IS 
		return res, unwrapped_message.get_bytes(0, unwrapped_message_length.read_uint);
	end

	def wrap(message)
		message_, message_length_=string_to_pointer_size(message)
		wrapped_message_length = FFI::MemoryPointer.new(:uint)
		res=secure_session_wrap(@session,  message_, message_length_, nil, wrapped_message_length)
		raise ThemisError, "secure_session_wrap (length determination) error: #{res}" unless res == BUFFER_TOO_SMALL
		wrapped_message = FFI::MemoryPointer.new(:char, wrapped_message_length.read_uint)
		res=secure_session_wrap(@session, message_, message_length_, wrapped_message, wrapped_message_length)
		raise ThemisError, "secure_session_wrap error: #{res}" unless res == SUCCESS || res == SEND_AS_IS 
		return wrapped_message.get_bytes(0, wrapped_message_length.read_uint);
	end

	def finalize
		res=secure_session_destroy(@session)
	        raise ThemisError, "secure_session_destroy error" unless res == SUCCESS
		
	end
    end

    class Smessage
	include ThemisCommon
	include ThemisImport
	def initialize(private_key, peer_public_key)
	    @private_key, @private_key_length = string_to_pointer_size(private_key)
	    @peer_public_key, @peer_public_key_length = string_to_pointer_size(peer_public_key)
	end

	def wrap(message)
	    message_, message_length_=string_to_pointer_size(message)
	    wrapped_message_length = FFI::MemoryPointer.new(:uint)
	    res=themis_secure_message_wrap(@private_key, @private_key_length, @peer_public_key, @peer_public_key_length, message_, message_length_, nil, wrapped_message_length)
	    raise ThemisError, "themis_secure_message_wrap (length determination) error: #{res}" unless res == BUFFER_TOO_SMALL
	    wrapped_message = FFI::MemoryPointer.new(:char, wrapped_message_length.read_uint)
	    res=themis_secure_message_wrap(@private_key, @private_key_length, @peer_public_key, @peer_public_key_length, message_, message_length_, wrapped_message, wrapped_message_length)
	    raise ThemisError, "themis_secure_message_wrap error: #{res}" unless res == SUCCESS
	    return wrapped_message.get_bytes(0, wrapped_message_length.read_uint);
	end

	def unwrap(message)
	    message_, message_length_=string_to_pointer_size(message)
	    unwrapped_message_length = FFI::MemoryPointer.new(:uint)
	    res=themis_secure_message_unwrap(@private_key, @private_key_length, @peer_public_key, @peer_public_key_length, message_, message_length_, nil, unwrapped_message_length)
	    raise ThemisError, "themis_secure_message_unwrap (length determination) error: #{res}" unless res == BUFFER_TOO_SMALL
	    unwrapped_message = FFI::MemoryPointer.new(:char, unwrapped_message_length.read_uint)
	    res=themis_secure_message_unwrap(@private_key, @private_key_length, @peer_public_key, @peer_public_key_length, message_, message_length_, unwrapped_message, unwrapped_message_length)
	    raise ThemisError, "themis_secure_message_unwrap error: #{res}" unless res == SUCCESS
	    return unwrapped_message.get_bytes(0, unwrapped_message_length.read_uint);
	end

    end
end