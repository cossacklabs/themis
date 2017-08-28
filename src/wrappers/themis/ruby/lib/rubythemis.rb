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

require 'ffi'

module ThemisCommon
    def string_to_pointer_size(string)
	string_buf = FFI::MemoryPointer.new(:char, string.force_encoding("BINARY").size)
	string_buf.put_bytes(0, string.force_encoding("BINARY"), 0, string.force_encoding("BINARY").size)
	return string_buf, string.force_encoding("BINARY").size
    end
    module_function :string_to_pointer_size
end

module ThemisImport
  extend FFI::Library
  ffi_lib 'themis'



  callback :get_pub_key_by_id_type, 	[:pointer, :int, :pointer, :int, :pointer], :int
  callback :send_callback_type,	[:pointer, :int, :uint], :int
  callback :receive_callback_type,	[:pointer, :int, :uint], :int

  class Callbacks_struct < FFI::Struct
    layout	:send_data,		:send_callback_type,
		:receive_data,		:receive_callback_type,
		:state_changed,		:pointer,
		:get_pub_key_for_id,	:get_pub_key_by_id_type,
		:user_data,		:pointer
  end

  attach_function :secure_session_create, [ :pointer, :uint, :pointer, :uint, :pointer], :pointer
  attach_function :secure_session_destroy, [ :pointer], :int
  attach_function :secure_session_generate_connect_request, [ :pointer, :pointer, :pointer], :int
  attach_function :secure_session_wrap, [ :pointer, :pointer, :int, :pointer, :pointer], :int
  attach_function :secure_session_unwrap, [ :pointer, :pointer, :int, :pointer, :pointer], :int
  attach_function :secure_session_is_established, [ :pointer], :bool

  attach_function :themis_secure_message_wrap, [:pointer, :int, :pointer, :int, :pointer, :int, :pointer, :pointer], :int
  attach_function :themis_secure_message_unwrap, [:pointer, :int, :pointer, :int, :pointer, :int, :pointer, :pointer], :int

  attach_function :themis_gen_rsa_key_pair, [:pointer, :pointer, :pointer, :pointer], :int
  attach_function :themis_gen_ec_key_pair, [:pointer, :pointer, :pointer, :pointer], :int
  attach_function :themis_version, [], :string

  attach_function :themis_secure_cell_encrypt_seal, [:pointer, :int, :pointer, :int, :pointer, :int, :pointer, :pointer], :int
  attach_function :themis_secure_cell_decrypt_seal, [:pointer, :int, :pointer, :int, :pointer, :int, :pointer, :pointer], :int

  attach_function :themis_secure_cell_encrypt_token_protect, [:pointer, :int, :pointer, :int, :pointer, :int, :pointer, :pointer, :pointer, :pointer], :int
  attach_function :themis_secure_cell_decrypt_token_protect, [:pointer, :int, :pointer, :int, :pointer, :int, :pointer, :int, :pointer, :pointer], :int

  attach_function :themis_secure_cell_encrypt_context_imprint, [:pointer, :int, :pointer, :int, :pointer, :int, :pointer, :pointer], :int
  attach_function :themis_secure_cell_decrypt_context_imprint, [:pointer, :int, :pointer, :int, :pointer, :int, :pointer, :pointer], :int

  begin
    attach_function :secure_comparator_create, [], :pointer
    attach_function :secure_comparator_destroy, [ :pointer], :int
    attach_function :secure_comparator_append_secret, [:pointer, :pointer, :int], :int
    attach_function :secure_comparator_begin_compare, [:pointer, :pointer, :pointer], :int
    attach_function :secure_comparator_proceed_compare, [:pointer, :pointer, :int, :pointer, :pointer], :int
    attach_function :secure_comparator_get_result, [:pointer], :int
  rescue FFI::NotFoundError => e
  end
end

module Themis
    extend ThemisCommon
    extend ThemisImport

    BUFFER_TOO_SMALL = 14
    SUCCESS = 0
    FAIL = 11
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

    class SKeyPairGen
	include ThemisCommon
	include ThemisImport

	def ec()
		private_key_length=FFI::MemoryPointer.new(:uint)
		public_key_length= FFI::MemoryPointer.new(:uint)
		res=themis_gen_ec_key_pair(nil, private_key_length, nil, public_key_length)
		raise ThemisError, "Themis failed generating EC KeyPair: #{res}" unless res == BUFFER_TOO_SMALL 
		private_key = FFI::MemoryPointer.new(:char, private_key_length.read_uint)
		public_key = FFI::MemoryPointer.new(:char, public_key_length.read_uint)
		res=themis_gen_ec_key_pair(private_key, private_key_length, public_key, public_key_length)
		raise ThemisError, "Themis failed generating EC KeyPair: #{res}" unless res == SUCCESS
		return private_key.get_bytes(0, private_key_length.read_uint), public_key.get_bytes(0, public_key_length.read_uint)
	end

	def rsa()
		private_key_length=FFI::MemoryPointer.new(:uint)
		public_key_length= FFI::MemoryPointer.new(:uint)
		res=themis_gen_rsa_key_pair(nil, private_key_length, nil, public_key_length)
		raise ThemisError, "Themis failed generating RSA KeyPair: #{res}" unless res == BUFFER_TOO_SMALL 
		private_key = FFI::MemoryPointer.new(:char, private_key_length.read_uint)
		public_key = FFI::MemoryPointer.new(:char, public_key_length.read_uint)
		res=themis_gen_rsa_key_pair(private_key, private_key_length, public_key, public_key_length)
		raise ThemisError, "Themis failed generating RSA KeyPair: #{res}" unless res == SUCCESS
		return private_key.get_bytes(0, private_key_length.read_uint), public_key.get_bytes(0, public_key_length.read_uint)
	end
    end
    class Ssession
	include ThemisCommon
	include ThemisImport

	MAPPING = {}

	Get_pub_key_by_id_callback = FFI::Function.new(:int, [:pointer, :int, :pointer, :int, :pointer]) do |id_buf, id_length, pubkey_buf, pubkey_length, obj|
	    pub_key=MAPPING[obj.read_uint64].get_pub_key_by_id(id_buf.get_bytes(0,id_length))
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
		@transport_obj_id=transport.object_id
		@callbacks[:user_data] = FFI::MemoryPointer.new(:uint64)
		@callbacks[:user_data].write_uint64(@transport_obj_id)
		@session=secure_session_create(id_buf, id_length, private_key_buf, private_key_length, @callbacks);
	        raise ThemisError, "Secure Session failed creating" unless @session
	end
	
	def is_established()
		return secure_session_is_established(@session)
	end

	def connect_request()
		connect_request_length = FFI::MemoryPointer.new(:uint)
		res=secure_session_generate_connect_request(@session, nil, connect_request_length)
		raise ThemisError, "Secure Session failed making connection request: #{res}" unless res == BUFFER_TOO_SMALL
		connect_request = FFI::MemoryPointer.new(:char, connect_request_length.read_uint)
		res=secure_session_generate_connect_request(@session, connect_request, connect_request_length)
		raise ThemisError, "Secure Session failed making connection request: #{res}" unless res == SUCCESS
		return connect_request.get_bytes(0, connect_request_length.read_uint);
	end

	def unwrap(message)
		message_, message_length_=string_to_pointer_size(message)
		unwrapped_message_length = FFI::MemoryPointer.new(:uint)
		res=secure_session_unwrap(@session,  message_, message_length_, nil, unwrapped_message_length)
		raise ThemisError, "Secure Session failed decrypting: #{res}" unless res == BUFFER_TOO_SMALL || res == SUCCESS
		if res == SUCCESS
		    return SUCCESS, ""
		end
		unwrapped_message = FFI::MemoryPointer.new(:char, unwrapped_message_length.read_uint)
		res=secure_session_unwrap(@session, message_, message_length_, unwrapped_message, unwrapped_message_length)
		raise ThemisError, "Secure Session failed decrypting: #{res}" unless res == SUCCESS || res == SEND_AS_IS 
		return res, unwrapped_message.get_bytes(0, unwrapped_message_length.read_uint);
	end

	def wrap(message)
		message_, message_length_=string_to_pointer_size(message)
		wrapped_message_length = FFI::MemoryPointer.new(:uint)
		res=secure_session_wrap(@session,  message_, message_length_, nil, wrapped_message_length)
		raise ThemisError, "Secure Session failed encrypting: #{res}" unless res == BUFFER_TOO_SMALL
		wrapped_message = FFI::MemoryPointer.new(:char, wrapped_message_length.read_uint)
		res=secure_session_wrap(@session, message_, message_length_, wrapped_message, wrapped_message_length)
		raise ThemisError, "Secure Session failed encrypting: #{res}" unless res == SUCCESS || res == SEND_AS_IS 
		return wrapped_message.get_bytes(0, wrapped_message_length.read_uint);
	end

	def finalize
		res=secure_session_destroy(@session)
	        raise ThemisError, "Secure Session failed destroying" unless res == SUCCESS
		
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
	    raise ThemisError, "Secure Message failed encrypting: #{res}" unless res == BUFFER_TOO_SMALL
	    wrapped_message = FFI::MemoryPointer.new(:char, wrapped_message_length.read_uint)
	    res=themis_secure_message_wrap(@private_key, @private_key_length, @peer_public_key, @peer_public_key_length, message_, message_length_, wrapped_message, wrapped_message_length)
	    raise ThemisError, "Secure Message failed encrypting: #{res}" unless res == SUCCESS
	    return wrapped_message.get_bytes(0, wrapped_message_length.read_uint);
	end

	def unwrap(message)
	    message_, message_length_=string_to_pointer_size(message)
	    unwrapped_message_length = FFI::MemoryPointer.new(:uint)
	    res=themis_secure_message_unwrap(@private_key, @private_key_length, @peer_public_key, @peer_public_key_length, message_, message_length_, nil, unwrapped_message_length)
	    raise ThemisError, "Secure Message failed decrypting: #{res}" unless res == BUFFER_TOO_SMALL
	    unwrapped_message = FFI::MemoryPointer.new(:char, unwrapped_message_length.read_uint)
	    res=themis_secure_message_unwrap(@private_key, @private_key_length, @peer_public_key, @peer_public_key_length, message_, message_length_, unwrapped_message, unwrapped_message_length)
	    raise ThemisError, "Secure Message failed decrypting: #{res}" unless res == SUCCESS
	    return unwrapped_message.get_bytes(0, unwrapped_message_length.read_uint);
	end
    end

    def Ssign(private_key, message)
	    private_key_, private_key_length_= string_to_pointer_size(private_key)
	    message_, message_length_=string_to_pointer_size(message)
	    wrapped_message_length = FFI::MemoryPointer.new(:uint)
	    res=themis_secure_message_wrap(private_key_, private_key_length_, nil, 0, message_, message_length_, nil, wrapped_message_length)
	    raise ThemisError, "Secure Message failed singing: #{res}" unless res == BUFFER_TOO_SMALL
	    wrapped_message = FFI::MemoryPointer.new(:char, wrapped_message_length.read_uint)
	    res=themis_secure_message_wrap(private_key_, private_key_length_, nil, 0, message_, message_length_, wrapped_message, wrapped_message_length)
	    raise ThemisError, "Secure Message failed singing: #{res}" unless res == SUCCESS
	    return wrapped_message.get_bytes(0, wrapped_message_length.read_uint);
    end

    def Sverify(peer_public_key, message)
	    include ThemisCommon
	    include ThemisImport
	    public_key_, public_key_length_= string_to_pointer_size(peer_public_key)
	    message_, message_length_=string_to_pointer_size(message)
	    unwrapped_message_length = FFI::MemoryPointer.new(:uint)
	    res=themis_secure_message_unwrap(nil, 0, public_key_, public_key_length_, message_, message_length_, nil, unwrapped_message_length)
	    raise ThemisError, "Secure Message failed verifying: #{res}" unless res == BUFFER_TOO_SMALL
	    unwrapped_message = FFI::MemoryPointer.new(:char, unwrapped_message_length.read_uint)
	    res=themis_secure_message_unwrap(nil, 0, public_key_, public_key_length_, message_, message_length_, unwrapped_message, unwrapped_message_length)
	    raise ThemisError, "Secure Message failed verifying: #{res}" unless res == SUCCESS
	    return unwrapped_message.get_bytes(0, unwrapped_message_length.read_uint);
    end

    class Scell
	include ThemisCommon
	include ThemisImport

	SEAL_MODE = 0
	TOKEN_PROTECT_MODE = 1
	CONTEXT_IMPRINT_MODE = 2

	def initialize(key, mode)
	    @key, @key_length = string_to_pointer_size(key)
	    @mode = mode
	end

	def encrypt(message, context=nil)
	    message_, message_length_ = string_to_pointer_size(message)
	    context_, context_length_ = context.nil? ? [nil,0] : string_to_pointer_size(context)
	    encrypted_message_length=FFI::MemoryPointer.new(:uint)
	    enccontext_length=FFI::MemoryPointer.new(:uint)
	    case @mode
	    when SEAL_MODE
		res=themis_secure_cell_encrypt_seal(@key, @key_length, context_, context_length_, message_, message_length_, nil, encrypted_message_length)
		raise ThemisError, "Secure Cell (Seal) failed encrypting: #{res}" unless res == BUFFER_TOO_SMALL
		encrypted_message = FFI::MemoryPointer.new(:char, encrypted_message_length.read_uint)
		res=themis_secure_cell_encrypt_seal(@key, @key_length, context_, context_length_, message_, message_length_, encrypted_message, encrypted_message_length)
		raise ThemisError, "Secure Cell (Seal) failed encrypting: #{res}" unless res == SUCCESS
		return encrypted_message.get_bytes(0, encrypted_message_length.read_uint)
	    when TOKEN_PROTECT_MODE
		res=themis_secure_cell_encrypt_token_protect(@key, @key_length, context_, context_length_, message_, message_length_, nil, enccontext_length, nil, encrypted_message_length)
		raise ThemisError, "Secure Cell (Token protect) failed encrypting: #{res}" unless res == BUFFER_TOO_SMALL
		encrypted_message = FFI::MemoryPointer.new(:char, encrypted_message_length.read_uint)
		enccontext = FFI::MemoryPointer.new(:char, enccontext_length.read_uint)
		res=themis_secure_cell_encrypt_token_protect(@key, @key_length, context_, context_length_, message_, message_length_, enccontext, enccontext_length, encrypted_message, encrypted_message_length)
		raise ThemisError, "Secure Cell (Token Protect) failed encrypting: #{res}" unless res == SUCCESS
		return enccontext.get_bytes(0, enccontext_length.read_uint), encrypted_message.get_bytes(0, encrypted_message_length.read_uint)
	    when CONTEXT_IMPRINT_MODE
		res=themis_secure_cell_encrypt_context_imprint(@key, @key_length, message_, message_length_, context_, context_length_, nil, encrypted_message_length)
		raise ThemisError, "Secure Cell (Context Imprint) failed encrypting: #{res}" unless res == BUFFER_TOO_SMALL
		encrypted_message = FFI::MemoryPointer.new(:char, encrypted_message_length.read_uint)
		res=themis_secure_cell_encrypt_context_imprint(@key, @key_length, message_, message_length_, context_, context_length_, encrypted_message, encrypted_message_length)
		raise ThemisError, "Secure Cell (Context Imprint) failed encrypting: #{res}" unless res == SUCCESS
		return encrypted_message.get_bytes(0, encrypted_message_length.read_uint)
	    else
		raise ThemisError, "Secure Cell failed encrypting, undefined mode"
	    end
	end

	def decrypt(message, context=nil)
	    context_, context_length_ = context.nil? ? [nil,0] : string_to_pointer_size(context)
	    decrypted_message_length=FFI::MemoryPointer.new(:uint)
	    case @mode
	    when SEAL_MODE
		message_, message_length_ = string_to_pointer_size(message)
		res=themis_secure_cell_decrypt_seal(@key, @key_length, context_, context_length_, message_, message_length_, nil, decrypted_message_length)
		raise ThemisError, "Secure Cell (Seal) failed decrypting: #{res}" unless res == BUFFER_TOO_SMALL
		decrypted_message = FFI::MemoryPointer.new(:char, decrypted_message_length.read_uint)
		res=themis_secure_cell_decrypt_seal(@key, @key_length, context_, context_length_, message_, message_length_, decrypted_message, decrypted_message_length)
		raise ThemisError, "Secure Cell (Seal) failed decrypting: #{res}" unless res == SUCCESS
		return decrypted_message.get_bytes(0, decrypted_message_length.read_uint)
	    when TOKEN_PROTECT_MODE
		enccontext, message_ = message
		message__, message_length__ = string_to_pointer_size(message_)
		enccontext_, enccontext_length = string_to_pointer_size(enccontext)
		res=themis_secure_cell_decrypt_token_protect(@key, @key_length, context_, context_length_, message__, message_length__, enccontext_, enccontext_length, nil, decrypted_message_length)
		raise ThemisError, "Secure Cell (Token Protect) failed decrypting: #{res}" unless res == BUFFER_TOO_SMALL
		decrypted_message = FFI::MemoryPointer.new(:char, decrypted_message_length.read_uint)
		res=themis_secure_cell_decrypt_token_protect(@key, @key_length, context_, context_length_, message__, message_length__, enccontext_, enccontext_length, decrypted_message, decrypted_message_length)
		raise ThemisError, "Secure Cell (Token Protect) failed decrypting: #{res}" unless res == SUCCESS
		return  decrypted_message.get_bytes(0, decrypted_message_length.read_uint)
	    when CONTEXT_IMPRINT_MODE
		message_, message_length_ = string_to_pointer_size(message)
		res=themis_secure_cell_decrypt_context_imprint(@key, @key_length, message_, message_length_, context_, context_length_, nil, decrypted_message_length)
		raise ThemisError, "Secure Cell (Context Imprint) failed decrypting: #{res}" unless res == BUFFER_TOO_SMALL
		decrypted_message = FFI::MemoryPointer.new(:char, decrypted_message_length.read_uint)
		res=themis_secure_cell_decrypt_context_imprint(@key, @key_length, message_, message_length_, context_, context_length_, decrypted_message, decrypted_message_length)
		raise ThemisError, "Secure Cell (Context Imprint) failed decrypting: #{res}" unless res == SUCCESS
		return decrypted_message.get_bytes(0, decrypted_message_length.read_uint)
	    else
		raise ThemisError, "Secure Cell failed encrypting, undefined mode"
	    end
	end
    end

    class Scomparator
      include ThemisCommon
      include ThemisImport
      
      MATCH = 21
      NOT_MATCH = 22
      NOT_READY = 0

      def initialize(shared_secret)
	shared_secret_buf, shared_secret_length = string_to_pointer_size(shared_secret)
	@comparator=secure_comparator_create()
	raise ThemisError, "Secure Comparator failed creating" unless @comparator
        res=secure_comparator_append_secret(@comparator, shared_secret_buf, shared_secret_length)
	raise ThemisError, "Secure Comparator failed appending secret" unless res==SUCCESS            
      end

      def finalize()
        res=secure_comparator_destroy(@comparator)
	raise ThemisError, "Secure Comparator failed destroying" unless res==SUCCESS                      
      end

      def begin_compare()
        res_length=FFI::MemoryPointer.new(:uint)
        res=secure_comparator_begin_compare(@comparator, nil, res_length)
        raise ThemisError, "Secure Comparator failed making initialisation message" unless res==BUFFER_TOO_SMALL
        res_buffer=FFI::MemoryPointer.new(:char, res_length.read_uint)
        res=secure_comparator_begin_compare(@comparator, res_buffer, res_length)
        raise ThemisError, "Secure Comparator failed making initialisation message" unless res==SUCCESS || res==SEND_AS_IS
        return res_buffer.get_bytes(0,res_length.read_uint)
      end

      def proceed_compare(control_message)
        message, message_length = string_to_pointer_size(control_message)
        res_length=FFI::MemoryPointer.new(:uint)
        res=secure_comparator_proceed_compare(@comparator, message, message_length, nil, res_length)
        raise ThemisError, "Secure Comparator failed proeeding message" unless res==SUCCESS || res == BUFFER_TOO_SMALL
        if res == SUCCESS
          return ""
        end
        res_buffer=FFI::MemoryPointer.new(:char, res_length.read_uint)
        res=secure_comparator_proceed_compare(@comparator, message, message_length, res_buffer, res_length)
        raise ThemisError, "Secure Comparator failed proeeding message" unless res==SUCCESS || res==SEND_AS_IS
        return res_buffer.get_bytes(0,res_length.read_uint)          
      end

      def result()
	return secure_comparator_get_result(@comparator)
      end
    end

    module_function :Ssign
    module_function :Sverify

end
