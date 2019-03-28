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

require 'rubygems'
require 'rbthemis'
require 'test/unit'

class TestScell < Test::Unit::TestCase
  def setup
    @key = 'This is test key'
    @context = 'This is test context'
    @message = 'This is test message'
  end

  def test_seal
    assert_raise(NoMethodError) do
      seal = Themis::Scell.new(nil, Themis::Scell::SEAL_MODE)
    end
    seal = Themis::Scell.new(@key, Themis::Scell::SEAL_MODE)

    assert_raise(Themis::ThemisError) do
      encrypted_message = seal.encrypt('', @context)
    end
    encrypted_message = seal.encrypt(@message, @context)

    assert_raise(Themis::ThemisError) do
      decrypted_message = seal.decrypt(encrypted_message + '1', @context)
    end
    assert_raise(Themis::ThemisError) do
      decrypted_message = seal.decrypt(encrypted_message, @context + '1')
    end
    decrypted_message = seal.decrypt(encrypted_message, @context)

    assert_equal(@message, decrypted_message)
    encrypted_message = seal.encrypt(@message)
    decrypted_message = seal.decrypt(encrypted_message)
    assert_equal(@message, decrypted_message)
  end

  def test_token_protect
    token_protect = Themis::Scell.new(@key, Themis::Scell::TOKEN_PROTECT_MODE)
    encrypted_message, token = token_protect.encrypt(@message, @context)
    assert_equal(@message.length, encrypted_message.length)

    assert_raise(Themis::ThemisError) do
      decrypted_message = token_protect.decrypt(
        [encrypted_message + '1', token], @context)
    end
    decrypted_message = token_protect.decrypt(
      [encrypted_message, token], @context)

    assert_equal(@message, decrypted_message)
    encrypted_message, token = token_protect.encrypt(@message)
    assert_equal(@message.length, encrypted_message.length)
    decrypted_message = token_protect.decrypt([encrypted_message, token])
    assert_equal(@message, decrypted_message)
  end

  def test_context_imprint
    context_imprint = Themis::Scell.new(
      @key, Themis::Scell::CONTEXT_IMPRINT_MODE)

    encrypted_message = context_imprint.encrypt(@message, @context)
    decrypted_message = context_imprint.decrypt(encrypted_message, @context)
    assert_equal(@message, decrypted_message)
    assert_raise(Themis::ThemisError) do
      encrypted_message = context_imprint.encrypt(@message)
    end
  end
end
