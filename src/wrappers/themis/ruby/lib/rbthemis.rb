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
    string_buf = FFI::MemoryPointer.from_string(string)
    [string_buf, string.bytesize]
  end

  def empty?(value)
    return value.nil? || value.empty?
  end

  module_function :string_to_pointer_size
  module_function :empty?
end

module ThemisImport
  extend FFI::Library

  def self.canonical_themis_paths
    host_os = RbConfig::CONFIG['host_os']
    paths = []
    if host_os.start_with? 'linux'
      # Default library installation path for "make install"
      paths.append '/usr/local/lib/libthemis.so.0'
      # Don't bother figuring out the "right" absolute path, since
      # that depends on the distro and non-distro-provided Rubies
      # don't know about the right paths at all. The ones installed
      # via RVM certainly don't. Use soname and let ld figure it out.
      paths.append 'libthemis.so.0'
    end
    if host_os.start_with? 'darwin'
      # Default library installation path for "make install"
      paths.append '/usr/local/lib/libthemis.0.dylib'
      # These are install names of libraries installed via Homebrew
      # Add both M1 and Intel paths so that x86 Ruby works on M1
      paths.append '/opt/homebrew/opt/libthemis/lib/libthemis.0.dylib'
      paths.append '/usr/local/opt/libthemis/lib/libthemis.0.dylib'
      # Last try, look for ABI-qualified name
      paths.append 'libthemis.0.dylib'
    end
    return paths
  end

  def self.load_themis
    for path in canonical_themis_paths
      begin
        return ffi_lib path
      rescue LoadError
        next
      end
    end
    warn <<~EOF
      WARN: failed to load the canonical Themis Core library

      Proceeding to find 'themis' library in standard paths.
      This might cause ABI mismatch and crash the process.
    EOF
    return ffi_lib 'themis'
  end

  load_themis

  callback :get_pub_key_by_id_type,
           [:pointer, :int, :pointer, :int, :pointer], :int
  callback :send_callback_type, [:pointer, :int, :uint], :int
  callback :receive_callback_type, [:pointer, :int, :uint], :int

  class CallbacksStruct < FFI::Struct
    layout :send_data, :send_callback_type,
           :receive_data, :receive_callback_type,
           :state_changed, :pointer,
           :get_pub_key_for_id, :get_pub_key_by_id_type,
           :user_data, :pointer
  end

  attach_function :secure_session_create,
                  [:pointer, :uint, :pointer, :uint, :pointer], :pointer
  attach_function :secure_session_destroy, [:pointer], :int
  attach_function :secure_session_generate_connect_request,
                  [:pointer, :pointer, :pointer], :int

  attach_function :secure_session_wrap,
                  [:pointer, :pointer, :int, :pointer, :pointer], :int
  attach_function :secure_session_unwrap,
                  [:pointer, :pointer, :int, :pointer, :pointer], :int
  attach_function :secure_session_is_established, [:pointer], :bool

  attach_function :themis_secure_message_encrypt,
                  [:pointer, :int, :pointer, :int, :pointer,
                   :int, :pointer, :pointer], :int
  attach_function :themis_secure_message_decrypt,
                  [:pointer, :int, :pointer, :int, :pointer,
                   :int, :pointer, :pointer], :int
  attach_function :themis_secure_message_sign,
                  [:pointer, :int, :pointer, :int, :pointer,
                   :pointer], :int
  attach_function :themis_secure_message_verify,
                  [:pointer, :int, :pointer, :int, :pointer,
                   :pointer], :int

  attach_function :themis_gen_rsa_key_pair,
                  [:pointer, :pointer, :pointer, :pointer], :int
  attach_function :themis_gen_ec_key_pair,
                  [:pointer, :pointer, :pointer, :pointer], :int
  attach_function :themis_gen_sym_key,
                  [:pointer, :pointer], :int

  THEMIS_KEY_INVALID = 0
  THEMIS_KEY_RSA_PRIVATE = 1
  THEMIS_KEY_RSA_PUBLIC = 2
  THEMIS_KEY_EC_PRIVATE = 3
  THEMIS_KEY_EC_PUBLIC = 4

  attach_function :themis_is_valid_asym_key, [:pointer, :int], :int
  attach_function :themis_get_asym_key_kind, [:pointer, :int], :int

  attach_function :themis_secure_cell_encrypt_seal,
                  [:pointer, :int, :pointer, :int, :pointer, :int,
                   :pointer, :pointer], :int
  attach_function :themis_secure_cell_decrypt_seal,
                  [:pointer, :int, :pointer, :int, :pointer, :int,
                   :pointer, :pointer], :int

  attach_function :themis_secure_cell_encrypt_seal_with_passphrase,
                   [:pointer, :int, :pointer, :int, :pointer, :int,
                    :pointer, :pointer], :int
  attach_function :themis_secure_cell_decrypt_seal_with_passphrase,
                   [:pointer, :int, :pointer, :int, :pointer, :int,
                    :pointer, :pointer], :int

  attach_function :themis_secure_cell_encrypt_token_protect,
                  [:pointer, :int, :pointer, :int, :pointer, :int,
                   :pointer, :pointer, :pointer, :pointer], :int
  attach_function :themis_secure_cell_decrypt_token_protect,
                  [:pointer, :int, :pointer, :int, :pointer, :int,
                   :pointer, :int, :pointer, :pointer], :int

  attach_function :themis_secure_cell_encrypt_context_imprint,
                  [:pointer, :int, :pointer, :int, :pointer, :int,
                   :pointer, :pointer], :int
  attach_function :themis_secure_cell_decrypt_context_imprint,
                  [:pointer, :int, :pointer, :int, :pointer, :int,
                   :pointer, :pointer], :int

  begin
    attach_function :secure_comparator_create, [], :pointer
    attach_function :secure_comparator_destroy, [:pointer], :int
    attach_function :secure_comparator_append_secret,
                    [:pointer, :pointer, :int], :int
    attach_function :secure_comparator_begin_compare,
                    [:pointer, :pointer, :pointer], :int
    attach_function :secure_comparator_proceed_compare,
                    [:pointer, :pointer, :int, :pointer, :pointer], :int
    attach_function :secure_comparator_get_result, [:pointer], :int
  rescue FFI::NotFoundError
  end
end

module Themis
  extend ThemisCommon
  extend ThemisImport

  extend Gem::Deprecate

  BUFFER_TOO_SMALL = 14
  SUCCESS = 0
  FAIL = 11
  INVALID_ARGUMENT = 12
  SEND_AS_IS = 1

  # Common class of errors caused by Themis functions.
  # You can access the numerical value via "error_code" attribute.
  # Human-readable message is accessible via "message" attribute.
  class ThemisError < StandardError
    attr_reader :error_code

    def initialize(error_code = INVALID_ARGUMENT)
      super
      @error_code = error_code
    end
  end

  class Callbacks
    def get_pub_key_by_id(id)
      raise ThemisError, 'Callback is not implemented: get_pub_key_by_id'
    end

    def send(message)
      raise ThemisError, 'Callback is not implemented: send'
    end

    def receive
      raise ThemisError, 'Callback is not implemented: receive'
    end
  end

  class SKeyPairGen
    include ThemisCommon
    include ThemisImport

    def ec
      private_key_length = FFI::MemoryPointer.new(:uint)
      public_key_length = FFI::MemoryPointer.new(:uint)

      res = themis_gen_ec_key_pair(
        nil, private_key_length, nil, public_key_length)
      if res != BUFFER_TOO_SMALL
        raise ThemisError, "Themis failed generating EC KeyPair: #{res}"
      end

      private_key = FFI::MemoryPointer.new(:char, private_key_length.read_uint)
      public_key = FFI::MemoryPointer.new(:char, public_key_length.read_uint)

      res = themis_gen_ec_key_pair(
        private_key, private_key_length, public_key, public_key_length)
      if res != SUCCESS
        raise ThemisError, "Themis failed generating EC KeyPair: #{res}"
      end

      [private_key.get_bytes(0, private_key_length.read_uint),
       public_key.get_bytes(0, public_key_length.read_uint)]
    end

    def rsa
      private_key_length = FFI::MemoryPointer.new(:uint)
      public_key_length = FFI::MemoryPointer.new(:uint)

      res = themis_gen_rsa_key_pair(
        nil, private_key_length, nil, public_key_length)
      if res != BUFFER_TOO_SMALL
        raise ThemisError, "Themis failed generating RSA KeyPair: #{res}"
      end

      private_key = FFI::MemoryPointer.new(:char, private_key_length.read_uint)
      public_key = FFI::MemoryPointer.new(:char, public_key_length.read_uint)

      res = themis_gen_rsa_key_pair(
        private_key, private_key_length, public_key, public_key_length)
      if res != SUCCESS
        raise ThemisError, "Themis failed generating RSA KeyPair: #{res}"
      end

      [private_key.get_bytes(0, private_key_length.read_uint),
       public_key.get_bytes(0, public_key_length.read_uint)]
    end
  end

  def Themis.valid_key(key)
    if key.nil? || key.empty?
      return false
    end
    key_, len_ = string_to_pointer_size(key)
    return themis_is_valid_asym_key(key_, len_) == SUCCESS
  end

  def Themis.private_key(key)
    key_, len_ = string_to_pointer_size(key)
    kind = themis_get_asym_key_kind(key_, len_)
    return kind == ThemisImport::THEMIS_KEY_RSA_PRIVATE \
        || kind == ThemisImport::THEMIS_KEY_EC_PRIVATE
  end

  def Themis.public_key(key)
    key_, len_ = string_to_pointer_size(key)
    kind = themis_get_asym_key_kind(key_, len_)
    return kind == ThemisImport::THEMIS_KEY_RSA_PUBLIC \
        || kind == ThemisImport::THEMIS_KEY_EC_PUBLIC
  end

  class Ssession
    include ThemisCommon
    include ThemisImport

    extend Gem::Deprecate

    MAPPING = {}

    GetPubKeyByIDCallback =
      FFI::Function.new(:int, [:pointer, :int, :pointer, :int, :pointer]) do |id_buf, id_length, pubkey_buf, pubkey_length, obj|
        pub_key = MAPPING[obj.read_uint64].get_pub_key_by_id(
          id_buf.get_bytes(0, id_length))
        return -1 unless pub_key
        pubkey_buf.put_bytes(0, pub_key)
        0
      end

    def initialize(id, private_key, transport)
      id_buf, id_length = string_to_pointer_size(id)
      private_key_buf, private_key_length = string_to_pointer_size(private_key)

      @callbacks = CallbacksStruct.new
      @callbacks[:get_pub_key_for_id] = GetPubKeyByIDCallback

      MAPPING[transport.object_id] = transport
      @transport_obj_id = transport.object_id

      @callbacks[:user_data] = FFI::MemoryPointer.new(:uint64)
      @callbacks[:user_data].write_uint64(@transport_obj_id)

      @session = secure_session_create(
        id_buf, id_length, private_key_buf, private_key_length, @callbacks)

      raise ThemisError, 'Secure Session failed creating' if @session.null?
    end

    def is_established
      established?
    end
    deprecate(:is_established, :established?, 2018, 6)

    def established?
      secure_session_is_established @session
    end

    def connect_request
      connect_request_length = FFI::MemoryPointer.new(:uint)
      res = secure_session_generate_connect_request(
        @session, nil, connect_request_length)
      if res != BUFFER_TOO_SMALL
        raise(ThemisError,
              "Secure Session failed making connection request: #{res}")
      end
      connect_request = FFI::MemoryPointer.new(
        :char, connect_request_length.read_uint)
      res = secure_session_generate_connect_request(
        @session, connect_request, connect_request_length)
      if res != SUCCESS
        raise(ThemisError,
              "Secure Session failed making connection request: #{res}")
      end
      connect_request.get_bytes(0, connect_request_length.read_uint)
    end

    def unwrap(message)
      message_, message_length_ = string_to_pointer_size message
      unwrapped_message_length = FFI::MemoryPointer.new(:uint)

      res = secure_session_unwrap(
        @session, message_, message_length_, nil, unwrapped_message_length)
      return SUCCESS, '' if res == SUCCESS
      if res != BUFFER_TOO_SMALL
        raise ThemisError, "Secure Session failed decrypting: #{res}"
      end

      unwrapped_message = FFI::MemoryPointer.new(
        :char, unwrapped_message_length.read_uint)
      res = secure_session_unwrap(@session, message_, message_length_,
                                  unwrapped_message, unwrapped_message_length)
      if res != SUCCESS && res != SEND_AS_IS
        raise ThemisError, "Secure Session failed decrypting: #{res}"
      end

      [res, unwrapped_message.get_bytes(0, unwrapped_message_length.read_uint)]
    end

    def wrap(message)
      message_, message_length_ = string_to_pointer_size(message)

      wrapped_message_length = FFI::MemoryPointer.new(:uint)
      res = secure_session_wrap(
        @session, message_, message_length_, nil, wrapped_message_length)
      if res != BUFFER_TOO_SMALL
        raise ThemisError, "Secure Session failed encrypting: #{res}"
      end

      wrapped_message = FFI::MemoryPointer.new(
        :char, wrapped_message_length.read_uint)
      res = secure_session_wrap(@session, message_, message_length_,
                                wrapped_message, wrapped_message_length)
      if res != SUCCESS && res != SEND_AS_IS
        raise ThemisError, "Secure Session failed encrypting: #{res}"
      end

      wrapped_message.get_bytes(0, wrapped_message_length.read_uint)
    end

    def finalize
      res = secure_session_destroy(@session)
      raise ThemisError, 'Secure Session failed destroying' if res != SUCCESS
    end
  end

  class Smessage
    include ThemisCommon
    include ThemisImport

    def initialize(private_key, peer_public_key)
      if not Themis.valid_key(private_key)
        raise ThemisError, "Secure Message: invalid private key"
      end
      if not Themis.valid_key(peer_public_key)
        raise ThemisError, "Secure Message: invalid public key"
      end
      if not Themis.private_key(private_key)
        raise ThemisError, "Secure Message: public key used instead of private"
      end
      if not Themis.public_key(peer_public_key)
        raise ThemisError, "Secure Message: private key used instead of public"
      end

      @private_key, @private_key_length = string_to_pointer_size(private_key)
      @peer_public_key, @peer_public_key_length =
        string_to_pointer_size(peer_public_key)
    end

    def wrap(message)
        message_, message_length_ = string_to_pointer_size(message)

        wrapped_message_length = FFI::MemoryPointer.new(:uint)
        res = themis_secure_message_encrypt(
          @private_key, @private_key_length, @peer_public_key,
          @peer_public_key_length, message_, message_length_,
          nil, wrapped_message_length)
        if res != BUFFER_TOO_SMALL
          raise ThemisError, "Secure Message failed to encrypt: #{res}"
        end

        wrapped_message = FFI::MemoryPointer.new(
          :char, wrapped_message_length.read_uint)
        res = themis_secure_message_encrypt(
          @private_key, @private_key_length, @peer_public_key,
          @peer_public_key_length, message_, message_length_,
          wrapped_message, wrapped_message_length)
        if res != SUCCESS
          raise ThemisError, "Secure Message failed to encrypt: #{res}"
        end

        wrapped_message.get_bytes(0, wrapped_message_length.read_uint)
    end

    def unwrap(message)
      message_, message_length_ = string_to_pointer_size(message)
      unwrapped_message_length = FFI::MemoryPointer.new(:uint)
      res = themis_secure_message_decrypt(
        @private_key, @private_key_length, @peer_public_key,
        @peer_public_key_length, message_, message_length_,
        nil, unwrapped_message_length)
      if res != BUFFER_TOO_SMALL
        raise ThemisError, "Secure Message failed to decrypt: #{res}"
      end

      unwrapped_message = FFI::MemoryPointer.new(
        :char, unwrapped_message_length.read_uint)
      res = themis_secure_message_decrypt(
        @private_key, @private_key_length, @peer_public_key,
        @peer_public_key_length, message_, message_length_,
        unwrapped_message, unwrapped_message_length)
      if res != SUCCESS
        raise ThemisError, "Secure Message failed to decrypt: #{res}"
      end

      unwrapped_message.get_bytes(0, unwrapped_message_length.read_uint)
    end
  end

  def Ssign(*args)
    s_sign(*args)
  end
  deprecate :Ssign, :s_sign, 2018, 6

  def s_sign(private_key, message)
    if not valid_key(private_key)
      raise ThemisError, "Secure Message: invalid private key"
    end
    if not private_key(private_key)
      raise ThemisError, "Secure Message: public key used instead of private"
    end

    private_key_, private_key_length_ = string_to_pointer_size(private_key)
    message_, message_length_ = string_to_pointer_size(message)

    wrapped_message_length = FFI::MemoryPointer.new(:uint)
    res = themis_secure_message_sign(
      private_key_, private_key_length_, message_,
      message_length_, nil, wrapped_message_length)
    if res != BUFFER_TOO_SMALL
      raise ThemisError, "Secure Message failed to sign: #{res}"
    end

    wrapped_message = FFI::MemoryPointer.new(
      :char, wrapped_message_length.read_uint)
    res = themis_secure_message_sign(
      private_key_, private_key_length_, message_,
      message_length_, wrapped_message, wrapped_message_length)
    if res != SUCCESS
      raise ThemisError, "Secure Message failed to sign: #{res}"
    end

    wrapped_message.get_bytes(0, wrapped_message_length.read_uint)
  end

  def Sverify(*args)
    s_verify(*args)
  end
  deprecate :Sverify, :s_verify, 2018, 6

  def s_verify(peer_public_key, message)
    if not valid_key(peer_public_key)
      raise ThemisError, "Secure Message: invalid public key"
    end
    if not public_key(peer_public_key)
      raise ThemisError, "Secure Message: private key used instead of public"
    end

    public_key_, public_key_length_ = string_to_pointer_size(peer_public_key)
    message_, message_length_ = string_to_pointer_size(message)

    unwrapped_message_length = FFI::MemoryPointer.new(:uint)
    res = themis_secure_message_verify(
      public_key_, public_key_length_, message_,
      message_length_, nil, unwrapped_message_length)
    if res != BUFFER_TOO_SMALL
      raise ThemisError, "Secure Message failed to verify: #{res}"
    end

    unwrapped_message = FFI::MemoryPointer.new(
      :char, unwrapped_message_length.read_uint)
    res = themis_secure_message_verify(
      public_key_, public_key_length_, message_,
      message_length_, unwrapped_message, unwrapped_message_length)
    if res != SUCCESS
      raise ThemisError, "Secure Message failed to verify: #{res}"
    end

    unwrapped_message.get_bytes(0, unwrapped_message_length.read_uint)
  end

  def gen_sym_key
    key_length = FFI::MemoryPointer.new(:uint)

    res = themis_gen_sym_key(nil, key_length)
    if res != BUFFER_TOO_SMALL
      raise ThemisError, "failed to get symmetric key size: #{res}"
    end

    key = FFI::MemoryPointer.new(:char, key_length.read_uint)

    res = themis_gen_sym_key(key, key_length)
    if res != SUCCESS
      raise ThemisError, "failed to generate symmetric key: #{res}"
    end

    return key.get_bytes(0, key_length.read_uint)
  end

  # Scell base class is retained for compatibility.
  # New code should use ScellSeal, ScellTokenProtect, or ScellContextImprint.
  class Scell
    include ThemisCommon
    include ThemisImport

    SEAL_MODE = 0
    TOKEN_PROTECT_MODE = 1
    CONTEXT_IMPRINT_MODE = 2

    def initialize(key, mode)
      # We could have replaced this with "self.new" but it's not possible
      # to override new *and* keep Scell as superclass of ScellSeal et al.
      # So we keep an instance of appropriate class here and never call
      # superclass initialize in subclasses.
      case mode
      when SEAL_MODE
        @cell = ScellSeal.new(key)
      when TOKEN_PROTECT_MODE
        @cell = ScellTokenProtect.new(key)
      when CONTEXT_IMPRINT_MODE
        @cell = ScellContextImprint.new(key)
      else
        raise ThemisError, "unknown Secure Cell mode: #{mode}"
      end
      warn "NOTE: #{self.class.name} is deprecated; use #{@cell.class.name} instead."
    end

    def encrypt(message, context = nil)
      @cell.encrypt(message, context)
    end

    def decrypt(message, context = nil)
      @cell.decrypt(message, context)
    end
  end

  # Secure Cell in Seal mode.
  class ScellSeal < Scell
    include ThemisCommon
    include ThemisImport

    # Make a new Secure Cell with given key.
    # The key must not be empty and is treated as binary data.
    # You can use Themis::gen_sym_key to generate new keys.
    def initialize(key)
      if empty? key
        raise ThemisError, "key cannot be empty"
      end
      @key, @key_length = string_to_pointer_size(key)
    end

    # Encrypts message with given optional context.
    # The context is cryptographically combined with message but is not included
    # into encrypted data, you will need to provide the same context for decryption.
    # Resulting encrypted message includes authentication token.
    # Message must not be empty, but context may be omitted.
    # Both message and context are treated as binary data.
    def encrypt(message, context = nil)
      if empty? message
        raise ThemisError, "message cannot be empty"
      end

      message_, message_length_ = string_to_pointer_size(message)
      context_, context_length_ =
        context.nil? ? [nil, 0] : string_to_pointer_size(context)

      encrypted_length = FFI::MemoryPointer.new(:uint)
      res = themis_secure_cell_encrypt_seal(
        @key, @key_length, context_, context_length_,
        message_, message_length_, nil, encrypted_length)
      if res != BUFFER_TOO_SMALL
        raise ThemisError.new(res), "encrypt failed"
      end

      encrypted_message = FFI::MemoryPointer.new(:char, encrypted_length.read_uint)
      res = themis_secure_cell_encrypt_seal(
        @key, @key_length, context_, context_length_,
        message_, message_length_, encrypted_message, encrypted_length)
      if res != SUCCESS
        raise ThemisError.new(res), "encrypt failed"
      end

      encrypted_message.get_bytes(0, encrypted_length.read_uint)
    end

    # Decrypts message with given context.
    # The context must be the same as the one used during encryption,
    # or be omitted or set to nil if no context were used.
    # Decrypted message is returned as binary data.
    def decrypt(message, context = nil)
      if empty? message
        raise ThemisError, "message cannot be empty"
      end

      message_, message_length_ = string_to_pointer_size(message)
      context_, context_length_ =
        context.nil? ? [nil, 0] : string_to_pointer_size(context)

      decrypted_length = FFI::MemoryPointer.new(:uint)
      res = themis_secure_cell_decrypt_seal(
        @key, @key_length, context_, context_length_,
        message_, message_length_, nil, decrypted_length)
      if res != BUFFER_TOO_SMALL
        raise ThemisError.new(res), "decrypt failed"
      end

      decrypted_message = FFI::MemoryPointer.new(:char, decrypted_length.read_uint)
      res = themis_secure_cell_decrypt_seal(
        @key, @key_length, context_, context_length_,
        message_, message_length_, decrypted_message, decrypted_length)
      if res != SUCCESS
        raise ThemisError.new(res), "decrypt failed"
      end

      decrypted_message.get_bytes(0, decrypted_length.read_uint)
    end
  end

  # Secure Cell in Seal mode.
  class ScellSealPassphrase < ScellSeal
    include ThemisCommon
    include ThemisImport

    # Make a new Secure Cell with given passphrase.
    # The passphrase must not be empty.
    # If the passphrase is not binary it will be encoded in UTF-8 by default,
    # you can use optional "encoding:" argument to use a different encoding.
    def initialize(passphrase, encoding: Encoding::UTF_8)
      if empty? passphrase
        raise ThemisError, "passphrase cannot be empty"
      end
      if passphrase.encoding != Encoding::BINARY
        passphrase = passphrase.encode(encoding)
      end
      @passphrase, @passphrase_length = string_to_pointer_size(passphrase)
    end

    # Encrypts message with given optional context.
    # The context is cryptographically combined with message but is not included
    # into encrypted data, you will need to provide the same context for decryption.
    # Resulting encrypted message includes authentication token.
    # Message must not be empty, but context may be omitted.
    # Both message and context are treated as binary data.
    def encrypt(message, context = nil)
      if empty? message
        raise ThemisError, "message cannot be empty"
      end

      message_, message_length_ = string_to_pointer_size(message)
      context_, context_length_ =
        context.nil? ? [nil, 0] : string_to_pointer_size(context)

      encrypted_length = FFI::MemoryPointer.new(:uint)
      res = themis_secure_cell_encrypt_seal_with_passphrase(
        @passphrase, @passphrase_length, context_, context_length_,
        message_, message_length_, nil, encrypted_length)
      if res != BUFFER_TOO_SMALL
        raise ThemisError.new(res), "encrypt failed"
      end

      encrypted_message = FFI::MemoryPointer.new(:char, encrypted_length.read_uint)
      res = themis_secure_cell_encrypt_seal_with_passphrase(
        @passphrase, @passphrase_length, context_, context_length_,
        message_, message_length_, encrypted_message, encrypted_length)
      if res != SUCCESS
        raise ThemisError.new(res), "encrypt failed"
      end

      encrypted_message.get_bytes(0, encrypted_length.read_uint)
    end

    # Decrypts message with given context.
    # The context must be the same as the one used during encryption,
    # or be omitted or set to nil if no context were used.
    # Decrypted message is returned as binary data.
    def decrypt(message, context = nil)
      if empty? message
        raise ThemisError, "message cannot be empty"
      end

      message_, message_length_ = string_to_pointer_size(message)
      context_, context_length_ =
        context.nil? ? [nil, 0] : string_to_pointer_size(context)

      decrypted_length = FFI::MemoryPointer.new(:uint)
      res = themis_secure_cell_decrypt_seal_with_passphrase(
        @passphrase, @passphrase_length, context_, context_length_,
        message_, message_length_, nil, decrypted_length)
      if res != BUFFER_TOO_SMALL
        raise ThemisError.new(res), "decrypt failed"
      end

      decrypted_message = FFI::MemoryPointer.new(:char, decrypted_length.read_uint)
      res = themis_secure_cell_decrypt_seal_with_passphrase(
        @passphrase, @passphrase_length, context_, context_length_,
        message_, message_length_, decrypted_message, decrypted_length)
      if res != SUCCESS
        raise ThemisError.new(res), "decrypt failed"
      end

      decrypted_message.get_bytes(0, decrypted_length.read_uint)
    end
  end

  # Secure Cell in Token Protect mode.
  class ScellTokenProtect < Scell
    include ThemisCommon
    include ThemisImport

    # Make a new Secure Cell with given key.
    # The key must not be empty and is treated as binary data.
    # You can use Themis::gen_sym_key to generate new keys.
    def initialize(key)
      if empty? key
        raise ThemisError, "key cannot be empty"
      end
      @key, @key_length = string_to_pointer_size(key)
    end

    # Encrypts message with given optional context.
    # The context is cryptographically combined with message but is not included
    # into encrypted data, you will need to provide the same context for decryption.
    # Resulting encrypted message (the same length as input) and authentication token
    # are returned separately; you will need to provide them both for decryption.
    # Message must not be empty, but context may be omitted.
    # Both message and context are treated as binary data.
    def encrypt(message, context = nil)
      if empty? message
        raise ThemisError, "message cannot be empty"
      end

      message_, message_length_ = string_to_pointer_size(message)
      context_, context_length_ =
        context.nil? ? [nil, 0] : string_to_pointer_size(context)

      auth_token_length = FFI::MemoryPointer.new(:uint)
      encrypted_length = FFI::MemoryPointer.new(:uint)
      res = themis_secure_cell_encrypt_token_protect(
        @key, @key_length, context_, context_length_, message_, message_length_,
        nil, auth_token_length, nil, encrypted_length)
      if res != BUFFER_TOO_SMALL
        raise ThemisError.new(res), "encrypt failed"
      end

      auth_token = FFI::MemoryPointer.new(:char, auth_token_length.read_uint)
      encrypted_message = FFI::MemoryPointer.new(:char, encrypted_length.read_uint)
      res = themis_secure_cell_encrypt_token_protect(
        @key, @key_length, context_, context_length_, message_, message_length_,
        auth_token, auth_token_length, encrypted_message, encrypted_length)
      if res != SUCCESS
        raise ThemisError.new(res), "encrypt failed"
      end

      [encrypted_message.get_bytes(0, encrypted_length.read_uint),
       auth_token.get_bytes(0, auth_token_length.read_uint),]
    end

    # Decrypts message with given authentication token and context.
    # The context must be the same as the one used during encryption,
    # or be omitted or set to nil if no context were used.
    # The token also must be the one returned during encryption.
    # Decrypted message is returned as binary data.
    def decrypt(message, token = nil, context = nil)
      # For compatibility with older API we allow the message and token to be
      # provided as a list in the first argument. In this case the second one
      # contains (an optional) context. Then there is no third argument.
      if message.kind_of? Array
        context = token
        message, token = message
      end

      if empty? message
        raise ThemisError, "message cannot be empty"
      end
      if empty? token
        raise ThemisError, "token cannot be empty"
      end

      message_, message_length_ = string_to_pointer_size(message)
      token_, token_length_ = string_to_pointer_size(token)
      context_, context_length_ =
        context.nil? ? [nil, 0] : string_to_pointer_size(context)

      decrypted_length = FFI::MemoryPointer.new(:uint)
      res = themis_secure_cell_decrypt_token_protect(
        @key, @key_length, context_, context_length_,
        message_, message_length_, token_, token_length_,
        nil, decrypted_length)
      if res != BUFFER_TOO_SMALL
        raise ThemisError.new(res), "decrypt failed"
      end

      decrypted_message = FFI::MemoryPointer.new(:char, decrypted_length.read_uint)
      res = themis_secure_cell_decrypt_token_protect(
        @key, @key_length, context_, context_length_,
        message_, message_length_, token_, token_length_,
        decrypted_message, decrypted_length)
      if res != SUCCESS
        raise ThemisError.new(res), "decrypt failed"
      end

      decrypted_message.get_bytes(0, decrypted_length.read_uint)
    end
  end

  # Secure Cell in Context Imprint mode.
  class ScellContextImprint < Scell
    include ThemisCommon
    include ThemisImport

    # Make a new Secure Cell with given key.
    # The key must not be empty and is treated as binary data.
    # You can use Themis::gen_sym_key to generate new keys.
    def initialize(key)
      if empty? key
        raise ThemisError, "key cannot be empty"
      end
      @key, @key_length = string_to_pointer_size(key)
    end

    # Encrypts message with given context.
    # The context is cryptographically combined with message but is not included
    # into encrypted data, you will need to provide the same context for decryption.
    # Resulting encrypted message has the same length as input and does not include
    # authentication data, so its integrity cannot be verified.
    # Message and context must not be empty, both are treated as binary data.
    def encrypt(message, context)
      if empty? message
        raise ThemisError, "message cannot be empty"
      end
      if empty? context
        raise ThemisError, "context cannot be empty"
      end

      message_, message_length_ = string_to_pointer_size(message)
      context_, context_length_ =
        context.nil? ? [nil, 0] : string_to_pointer_size(context)

      encrypted_length = FFI::MemoryPointer.new(:uint)
      res = themis_secure_cell_encrypt_context_imprint(
        @key, @key_length, message_, message_length_,
        context_, context_length_, nil, encrypted_length)
      if res != BUFFER_TOO_SMALL
        raise ThemisError.new(res), "encrypt failed"
      end

      encrypted_message = FFI::MemoryPointer.new(:char, encrypted_length.read_uint)
      res = themis_secure_cell_encrypt_context_imprint(
        @key, @key_length, message_, message_length_,
        context_, context_length_, encrypted_message, encrypted_length)
      if res != SUCCESS
        raise ThemisError.new(res), "encrypt failed"
      end

      encrypted_message.get_bytes(0, encrypted_length.read_uint)
    end

    # Decrypts message with given context.
    # The context must be the same as the one used during encryption.
    # Since Context Imprint mode does not include authentication data,
    # integrity of the resulting message is not guaranteed.
    # You need to verify it via some other means.
    # Decrypted message is returned as binary data.
    def decrypt(message, context)
      if empty? message
        raise ThemisError, "message cannot be empty"
      end
      if empty? context
        raise ThemisError, "message cannot be empty"
      end

      message_, message_length_ = string_to_pointer_size(message)
      context_, context_length_ =
        context.nil? ? [nil, 0] : string_to_pointer_size(context)

      decrypted_length = FFI::MemoryPointer.new(:uint)
      res = themis_secure_cell_decrypt_context_imprint(
        @key, @key_length, message_, message_length_,
        context_, context_length_, nil, decrypted_length)
      if res != BUFFER_TOO_SMALL
        raise ThemisError.new(res), "decrypt failed"
      end

      decrypted_message = FFI::MemoryPointer.new(:char, decrypted_length.read_uint)
      res = themis_secure_cell_decrypt_context_imprint(
        @key, @key_length, message_, message_length_,
        context_, context_length_, decrypted_message, decrypted_length)
      if res != SUCCESS
        raise ThemisError.new(res), "decrypt failed"
      end

      decrypted_message.get_bytes(0, decrypted_length.read_uint)
    end
  end

  class Scomparator
    include ThemisCommon
    include ThemisImport

    MATCH = 21
    NOT_MATCH = 22
    NOT_READY = 0

    def initialize(shared_secret)
      shared_secret_buf, shared_secret_length =
        string_to_pointer_size(shared_secret)
      @comparator = secure_comparator_create
      raise ThemisError, 'Secure Comparator failed creating' if @comparator.null?
      res = secure_comparator_append_secret(
        @comparator, shared_secret_buf, shared_secret_length)
      if res != SUCCESS
        raise ThemisError, 'Secure Comparator failed appending secret'
      end
    end

    def finalize
      res = secure_comparator_destroy(@comparator)
      if res != SUCCESS
        raise ThemisError, 'Secure Comparator failed destroying'
      end
    end

    def begin_compare
      res_length = FFI::MemoryPointer.new(:uint)
      res = secure_comparator_begin_compare(@comparator, nil, res_length)
      if res != BUFFER_TOO_SMALL
        raise(ThemisError,
              'Secure Comparator failed making initialisation message')
      end

      res_buffer = FFI::MemoryPointer.new(:char, res_length.read_uint)
      res = secure_comparator_begin_compare(@comparator, res_buffer, res_length)
      if res != SUCCESS && res != SEND_AS_IS
        raise(ThemisError,
              'Secure Comparator failed making initialisation message')
      end

      res_buffer.get_bytes(0, res_length.read_uint)
    end

    def proceed_compare(control_message)
      message, message_length = string_to_pointer_size(control_message)
      res_length = FFI::MemoryPointer.new(:uint)

      res = secure_comparator_proceed_compare(
        @comparator, message, message_length, nil, res_length)
      return '' if res == SUCCESS
      if res != BUFFER_TOO_SMALL
        raise ThemisError, 'Secure Comparator failed proceeding message'
      end

      res_buffer = FFI::MemoryPointer.new(:char, res_length.read_uint)
      res = secure_comparator_proceed_compare(
        @comparator, message, message_length, res_buffer, res_length)
      if res != SUCCESS && res != SEND_AS_IS
        raise ThemisError, 'Secure Comparator failed proceeding message'
      end

      res_buffer.get_bytes(0, res_length.read_uint)
    end

    def result
      secure_comparator_get_result(@comparator)
    end
  end

  module_function :Ssign
  module_function :Sverify
  module_function :s_sign
  module_function :s_verify
  module_function :gen_sym_key
end
