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

#!/usr/bin/env ruby

require 'base64'
require 'rubygems'
require 'rbthemis'
require 'test/unit'

def suppress_warnings
  original = $VERBOSE
  $VERBOSE = nil
  yield
ensure
  $VERBOSE = original
end

class TestScell < Test::Unit::TestCase
  def setup
    @master_key = Base64.decode64('b0gyNlM4LTFKRDI5anFIRGJ4SmQyLGE7MXN5YWUzR2U=')
    @passphrase = 'This is test passphrase'
    @context = 'This is test context'
    @message = 'This is test message'
  end

  def test_keygen
    default_length = 32
    key = Themis::gen_sym_key()
    assert_not_nil(key)
    assert_equal(key.length, default_length)
  end

  def test_hierarchy
    assert_kind_of Themis::Scell, Themis::ScellSeal.new(@master_key)
    assert_kind_of Themis::Scell, Themis::ScellTokenProtect.new(@master_key)
    assert_kind_of Themis::Scell, Themis::ScellContextImprint.new(@master_key)
  end

  #
  # Seal Mode (master key)
  #

  def test_seal
    seal = Themis::ScellSeal.new(@master_key)

    encrypted = seal.encrypt(@message, @context)
    decrypted = seal.decrypt(encrypted, @context)

    assert(encrypted.length > @message.length)
    assert_equal(@message, decrypted)
  end

  def test_seal_required_key
    assert_raise(Themis::ThemisError) do
      seal = Themis::ScellSeal.new(nil)
    end
    assert_raise(Themis::ThemisError) do
      seal = Themis::ScellSeal.new('')
    end
  end

  def test_seal_required_message
    seal = Themis::ScellSeal.new(@master_key)

    assert_raise(Themis::ThemisError) do
      encrypted = seal.encrypt('')
    end
    assert_raise(Themis::ThemisError) do
      encrypted = seal.encrypt(nil)
    end

    assert_raise(Themis::ThemisError) do
      décrypted_message = seal.decrypt('')
    end
    assert_raise(Themis::ThemisError) do
      décrypted_message = seal.decrypt(nil)
    end
  end

  def test_seal_optional_context
    seal = Themis::ScellSeal.new(@master_key)

    # Omitted context is equivalent to empty context
    encrypted = seal.encrypt(@message)
    decrypted = seal.decrypt(encrypted, '')

    assert_equal(@message, decrypted)
  end

  def test_seal_integrity
    seal_correct = Themis::ScellSeal.new(@master_key)
    seal_invalid = Themis::ScellSeal.new(@master_key + '1')

    encrypted = seal_correct.encrypt(@message)

    assert_raise(Themis::ThemisError) do
      decrypted = seal_correct.decrypt(encrypted + '1', @context)
    end
    assert_raise(Themis::ThemisError) do
      decrypted = seal_correct.decrypt(encrypted, @context + '1')
    end
    assert_raise(Themis::ThemisError) do
      decrypted = seal_invalid.decrypt(encrypted, @context)
    end
  end

  def test_seal_old_api
    seal_old = suppress_warnings {
      Themis::Scell.new(@master_key, Themis::Scell::SEAL_MODE)
    }
    seal_new = Themis::ScellSeal.new(@master_key)

    encrypted = seal_old.encrypt(@message, @context)
    decrypted = seal_new.decrypt(encrypted, @context)

    assert_equal(@message, decrypted)
  end

  def test_seal_incompatibility_pw
    # Master keys are not passphrases
    seal_1 = Themis::ScellSeal.new(@master_key)
    seal_2 = Themis::ScellSealPassphrase.new(@master_key)

    encrypted = seal_1.encrypt(@message, @context)
    assert_raise(Themis::ThemisError) do
      decrypted = seal_2.decrypt(encrypted, @context)
    end
  end

  def test_seal_message_encoding
    message = @message.dup.encode(Encoding::UTF_16BE)
    scell = Themis::ScellSeal.new(@master_key)

    encrypted = scell.encrypt(message, @context)
    decrypted = scell.decrypt(encrypted, @context)

    assert_equal(message.encoding, Encoding::UTF_16BE)
    assert_equal(decrypted.encoding, Encoding::BINARY)
    assert_equal(decrypted.b, message.b)

    decrypted.force_encoding(Encoding::UTF_16BE)
    assert_equal(decrypted, message)
  end

  #
  # Seal Mode (passphrase)
  #

  def test_seal_pw
    seal = Themis::ScellSealPassphrase.new(@passphrase)

    encrypted = seal.encrypt(@message, @context)
    decrypted = seal.decrypt(encrypted, @context)

    assert(encrypted.length > @message.length)
    assert_equal(@message, decrypted)
  end

  def test_seal_pw_required_passphrase
    assert_raise(Themis::ThemisError) do
      seal = Themis::ScellSealPassphrase.new(nil)
    end
    assert_raise(Themis::ThemisError) do
      seal = Themis::ScellSealPassphrase.new('')
    end
  end

  def test_seal_pw_required_message
    seal = Themis::ScellSealPassphrase.new(@passphrase)

    assert_raise(Themis::ThemisError) do
      encrypted = seal.encrypt('')
    end
    assert_raise(Themis::ThemisError) do
      encrypted = seal.encrypt(nil)
    end

    assert_raise(Themis::ThemisError) do
      décrypted_message = seal.decrypt('')
    end
    assert_raise(Themis::ThemisError) do
      décrypted_message = seal.decrypt(nil)
    end
  end

  def test_seal_pw_optional_context
    seal = Themis::ScellSealPassphrase.new(@passphrase)

    # Omitted context is equivalent to empty context
    encrypted = seal.encrypt(@message)
    decrypted = seal.decrypt(encrypted, '')

    assert_equal(@message, decrypted)
  end

  def test_seal_pw_integrity
    seal_correct = Themis::ScellSealPassphrase.new(@passphrase)
    seal_invalid = Themis::ScellSealPassphrase.new(@passphrase + '1')

    encrypted = seal_correct.encrypt(@message)

    assert_raise(Themis::ThemisError) do
      decrypted = seal_correct.decrypt(encrypted + '1', @context)
    end
    assert_raise(Themis::ThemisError) do
      decrypted = seal_correct.decrypt(encrypted, @context + '1')
    end
    assert_raise(Themis::ThemisError) do
      decrypted = seal_invalid.decrypt(encrypted, @context)
    end
  end

  def test_seal_pw_incompatibility
    # Master keys are not passphrases
    seal_1 = Themis::ScellSealPassphrase.new(@passphrase)
    seal_2 = Themis::ScellSeal.new(@passphrase)

    encrypted = seal_1.encrypt(@message, @context)
    assert_raise(Themis::ThemisError) do
      decrypted = seal_2.decrypt(encrypted, @context)
    end
  end

  def test_seal_pw_encoding
    seal_1 = Themis::ScellSealPassphrase.new(@passphrase, encoding: Encoding::UTF_16)
    encoded_passphrase = @passphrase.encode(Encoding::UTF_16).force_encoding(Encoding::BINARY)
    seal_2 = Themis::ScellSealPassphrase.new(encoded_passphrase)

    encrypted = seal_1.encrypt(@message, @context)
    decrypted = seal_2.decrypt(encrypted, @context)

    assert_equal(@message, decrypted)
  end

  def test_seal_pw_message_encoding
    message = @message.dup.encode(Encoding::UTF_16BE)
    scell = Themis::ScellSealPassphrase.new(@passphrase)

    encrypted = scell.encrypt(message, @context)
    decrypted = scell.decrypt(encrypted, @context)

    assert_equal(message.encoding, Encoding::UTF_16BE)
    assert_equal(decrypted.encoding, Encoding::BINARY)
    assert_equal(decrypted.b, message.b)

    decrypted.force_encoding(Encoding::UTF_16BE)
    assert_equal(decrypted, message)
  end

  #
  # Token Protect mode (master key)
  #

  def test_token_protect
    token_protect = Themis::ScellTokenProtect.new(@master_key)

    encrypted, token = token_protect.encrypt(@message, @context)
    decrypted = token_protect.decrypt(encrypted, token, @context)

    assert_equal(@message.length, encrypted.length)
    assert_equal(@message, decrypted)
  end

  def test_token_protect_required_key
    assert_raise(Themis::ThemisError) do
      token_protect = Themis::ScellTokenProtect.new(nil)
    end
    assert_raise(Themis::ThemisError) do
      token_protect = Themis::ScellTokenProtect.new('')
    end
  end

  def test_token_protect_required_message_token
    token_protect = Themis::ScellTokenProtect.new(@master_key)

    assert_raise(Themis::ThemisError) do
      encrypted = token_protect.encrypt('')
    end
    assert_raise(Themis::ThemisError) do
      encrypted = token_protect.encrypt(nil)
    end

    encrypted, token = token_protect.encrypt(@message)

    assert_raise(Themis::ThemisError) do
      décrypted_message = token_protect.decrypt('', token)
    end
    assert_raise(Themis::ThemisError) do
      décrypted_message = token_protect.decrypt(nil, token)
    end
    assert_raise(Themis::ThemisError) do
      décrypted_message = token_protect.decrypt(encrypted, '')
    end
    assert_raise(Themis::ThemisError) do
      décrypted_message = token_protect.decrypt(encrypted, nil)
    end
  end

  def test_token_protect_optional_context
    token_protect = Themis::ScellTokenProtect.new(@master_key)

    # Omitted context is equivalent to empty context
    encrypted, token = token_protect.encrypt(@message)
    decrypted = token_protect.decrypt(encrypted, token, '')

    assert_equal(@message, decrypted)
  end

  def test_token_protect_integrity
    token_protect_correct = Themis::ScellTokenProtect.new(@master_key)
    token_protect_invalid = Themis::ScellTokenProtect.new(@master_key + '1')

    encrypted, token = token_protect_correct.encrypt(@message)

    assert_raise(Themis::ThemisError) do
      decrypted = token_protect_correct.decrypt(encrypted + '1', token, @context)
    end
    assert_raise(Themis::ThemisError) do
      decrypted = token_protect_correct.decrypt(encrypted, token + '1', @context)
    end
    assert_raise(Themis::ThemisError) do
      decrypted = token_protect_correct.decrypt(encrypted, token, @context + '1')
    end
    assert_raise(Themis::ThemisError) do
      decrypted = token_protect_invalid.decrypt(encrypted, token, @context)
    end
  end

  def test_token_protect_old_api
    token_protect_old = suppress_warnings {
      Themis::Scell.new(@master_key, Themis::Scell::TOKEN_PROTECT_MODE)
    }
    token_protect_new = Themis::ScellTokenProtect.new(@master_key)

    encrypted, token = token_protect_old.encrypt(@message, @context)
    decrypted = token_protect_new.decrypt(encrypted, token, @context)

    assert_equal(@message, decrypted)

    # New API accepts old-style list syntax too
    decrypted = token_protect_new.decrypt([encrypted, token], @context)
    assert_equal(@message, decrypted)

    encrypted, token = token_protect_old.encrypt(@message)
    decrypted = token_protect_new.decrypt([encrypted, token])
    assert_equal(@message, decrypted)
  end

  def test_token_protect_message_encoding
    message = @message.dup.encode(Encoding::UTF_16BE)
    scell = Themis::ScellTokenProtect.new(@master_key)

    encrypted, token = scell.encrypt(message, @context)
    decrypted = scell.decrypt(encrypted, token, @context)

    assert_equal(message.encoding, Encoding::UTF_16BE)
    assert_equal(decrypted.encoding, Encoding::BINARY)
    assert_equal(decrypted.b, message.b)

    decrypted.force_encoding(Encoding::UTF_16BE)
    assert_equal(decrypted, message)
  end

  #
  # Context Imprint mode (master key)
  #

  def test_context_imprint
    context_imprint = Themis::ScellContextImprint.new(@master_key)

    encrypted = context_imprint.encrypt(@message, @context)
    decrypted = context_imprint.decrypt(encrypted, @context)

    assert_equal(@message.length, encrypted.length)
    assert_equal(@message, decrypted)
  end

  def test_context_imprint_required_key
    assert_raise(Themis::ThemisError) do
      context_imprint = Themis::ScellContextImprint.new(nil)
    end
    assert_raise(Themis::ThemisError) do
      context_imprint = Themis::ScellContextImprint.new('')
    end
  end

  def test_context_imprint_required_message
    context_imprint = Themis::ScellContextImprint.new(@master_key)

    assert_raise(Themis::ThemisError) do
      encrypted = context_imprint.encrypt('', @context)
    end
    assert_raise(Themis::ThemisError) do
      encrypted = context_imprint.encrypt(nil, @context)
    end

    assert_raise(Themis::ThemisError) do
      décrypted_message = context_imprint.decrypt('', @context)
    end
    assert_raise(Themis::ThemisError) do
      décrypted_message = context_imprint.decrypt(nil, @context)
    end
  end

  def test_context_imprint_required_context
    context_imprint = Themis::ScellContextImprint.new(@master_key)

    assert_raise(ArgumentError) do
      encrypted = context_imprint.encrypt(@message)
    end
    assert_raise(Themis::ThemisError) do
      encrypted = context_imprint.encrypt(@message, nil)
    end
    assert_raise(Themis::ThemisError) do
      encrypted = context_imprint.encrypt(@message, '')
    end

    encrypted = context_imprint.encrypt(@message, @context)

    assert_raise(ArgumentError) do
      decrypted = context_imprint.decrypt(encrypted)
    end
    assert_raise(Themis::ThemisError) do
      decrypted = context_imprint.decrypt(encrypted, nil)
    end
    assert_raise(Themis::ThemisError) do
      decrypted = context_imprint.decrypt(encrypted, '')
    end
  end

  def test_context_imprint_no_integrity
    context_imprint_correct = Themis::ScellContextImprint.new(@master_key)
    context_imprint_invalid = Themis::ScellContextImprint.new(@master_key + '1')

    encrypted   = context_imprint_correct.encrypt(@message, @context)
    decrypted_1 = context_imprint_correct.decrypt(encrypted + '1', @context)
    decrypted_2 = context_imprint_correct.decrypt(encrypted, @context + '1')
    decrypted_3 = context_imprint_invalid.decrypt(encrypted, @context)

    assert_not_equal(@message, decrypted_1)
    assert_not_equal(@message, decrypted_2)
    assert_not_equal(@message, decrypted_3)
  end

  def test_context_imprint_old_api
    context_imprint_old = suppress_warnings {
      Themis::Scell.new(@master_key, Themis::Scell::CONTEXT_IMPRINT_MODE)
    }
    context_imprint_new = Themis::ScellContextImprint.new(@master_key)

    encrypted = context_imprint_old.encrypt(@message, @context)
    decrypted = context_imprint_new.decrypt(encrypted, @context)

    assert_equal(@message, decrypted)
  end

  def test_context_imprint_message_encoding
    message = @message.dup.encode(Encoding::UTF_16BE)
    scell = Themis::ScellContextImprint.new(@master_key)

    encrypted = scell.encrypt(message, @context)
    decrypted = scell.decrypt(encrypted, @context)

    assert_equal(message.encoding, Encoding::UTF_16BE)
    assert_equal(decrypted.encoding, Encoding::BINARY)
    assert_equal(decrypted.b, message.b)

    decrypted.force_encoding(Encoding::UTF_16BE)
    assert_equal(decrypted, message)
  end

end
