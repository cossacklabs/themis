<?php
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

require __DIR__ . '/vendor/autoload.php';

use PHPUnit\Framework\TestCase;

class ScellTest extends TestCase {

    public function testKeyGeneration() {
        $defaultLength = 32;
        $key = phpthemis_gen_sym_key();
        $this->assertEquals(strlen($key), $defaultLength);
    }

    public function testKeyInstances() {
        // Make sure that wrapper code generates distinct PHP objects
        // and does not do anything silly like reusing memory buffers.
        $key1 = phpthemis_gen_sym_key();
        $key2 = phpthemis_gen_sym_key();
        $this->assertNotEquals($key1, $key2);
        $this->assertNotSame($key1, $key2);
        $this->assertTrue($key1 !== $key2);
    }

    public function InvalidValues() {
        return array(
            array(NULL),
            array(''),
            array(array('something')),
            array(array('key' => 'value')),
            array(function(){})
        );
    }

    // Secure Cell - Seal - master key

    public function testSealMasterKey() {
        $master_key = phpthemis_gen_sym_key();
        $message = 'precious message';
        $context = 'unit testing now';

        $encrypted = phpthemis_scell_seal_encrypt($master_key, $message, $context);
        $this->assertGreaterThan(strlen($message), strlen($encrypted));
        $decrypted = phpthemis_scell_seal_decrypt($master_key, $encrypted, $context);
        $this->assertEquals($decrypted, $message);
    }

    public function testSealMasterKeyIncorrectKey() {
        $master_key_alice = phpthemis_gen_sym_key();
        $master_key_bob = phpthemis_gen_sym_key();
        $message = 'precious message';
        $context = 'unit testing now';

        $encrypted = phpthemis_scell_seal_encrypt($master_key_alice, $message, $context);
        $this->expectException(Exception::class);
        $decrypted = phpthemis_scell_seal_decrypt($master_key_bob, $encrypted, $context);
    }

    public function testSealMasterKeyInvalidMessage() {
        $master_key = phpthemis_gen_sym_key();
        $message = 'precious message';
        $context = 'unit testing now';

        $encrypted = phpthemis_scell_seal_encrypt($master_key, $message, $context);
        $this->expectException(Exception::class);
        $decrypted = phpthemis_scell_seal_decrypt($master_key, $encrypted.'X', $context);
    }

    public function testSealMasterKeyIncorrectContext() {
        $master_key = phpthemis_gen_sym_key();
        $message = 'precious message';
        $context_alice = 'secure cell';
        $context_bob = 'peanut butter';

        $encrypted = phpthemis_scell_seal_encrypt($master_key, $message, $context_alice);
        $this->expectException(Exception::class);
        $decrypted = phpthemis_scell_seal_decrypt($master_key, $encrypted, $context_bob);
    }

    public function testSealMasterKeyNoContext() {
        $master_key = phpthemis_gen_sym_key();
        $message = 'precious message';
        $context = 'unit testing now';

        // Omitted context is the same as null or empty context
        $encrypted  = phpthemis_scell_seal_encrypt($master_key, $message);
        $decrypted1 = phpthemis_scell_seal_decrypt($master_key, $encrypted, NULL);
        $decrypted2 = phpthemis_scell_seal_decrypt($master_key, $encrypted, '');

        $this->assertEquals($decrypted1, $message);
        $this->assertEquals($decrypted2, $message);
    }

    public function testSealMasterKeyIsNotPassphrase() {
        $master_key = phpthemis_gen_sym_key();
        $message = 'precious message';

        $encrypted = phpthemis_scell_seal_encrypt($master_key, $message);
        $this->expectException(Exception::class);
        $decrypted = phpthemis_scell_seal_decrypt_with_passphrase($master_key, $encrypted);
    }

    /** @dataProvider InvalidValues */
    public function testSealMasterKeyNoKeyEncrypt($empty) {
        $this->expectException(Exception::class);
        $encrypted = phpthemis_scell_seal_encrypt($empty, 'precious message');
    }

    /** @dataProvider InvalidValues */
    public function testSealMasterKeyNoMessageEncrypt($empty) {
        $master_key = phpthemis_gen_sym_key();
        $this->expectException(Exception::class);
        $encrypted = phpthemis_scell_seal_encrypt($master_key, $empty);
    }

    /** @dataProvider InvalidValues */
    public function testSealMasterKeyNoKeyDecrypt($empty) {
        $master_key = phpthemis_gen_sym_key();
        $encrypted = phpthemis_scell_seal_encrypt($master_key, 'precious message');
        $this->expectException(Exception::class);
        $decrypted = phpthemis_scell_seal_decrypt($empty, $encrypted);
    }

    /** @dataProvider InvalidValues */
    public function testSealMasterKeyNoMessageDecrypt($empty) {
        $master_key = phpthemis_gen_sym_key();
        $encrypted = phpthemis_scell_seal_encrypt($master_key, 'precious message');
        $this->expectException(Exception::class);
        $decrypted = phpthemis_scell_seal_decrypt($master_key, $empty);
    }

    // Secure Cell - Seal - passphrase

    public function testSealPassphrase() {
        $passphrase = 'my secret key';
        $message = 'precious message';
        $context = 'unit testing now';

        $encrypted = phpthemis_scell_seal_encrypt_with_passphrase($passphrase, $message, $context);
        $this->assertGreaterThan(strlen($message), strlen($encrypted));
        $decrypted = phpthemis_scell_seal_decrypt_with_passphrase($passphrase, $encrypted, $context);
        $this->assertEquals($decrypted, $message);
    }

    public function testSealPassphraseIncorrectPassphrase() {
        $passphrase_alice = 'eclair twiddling expel eggplant';
        $passphrase_bob = 'drove valuables mortality faceplate';
        $message = 'precious message';
        $context = 'unit testing now';

        $encrypted = phpthemis_scell_seal_encrypt_with_passphrase($passphrase_alice, $message, $context);
        $this->expectException(Exception::class);
        $decrypted = phpthemis_scell_seal_decrypt_with_passphrase($passphrase_bob, $encrypted, $context);
    }

    public function testSealPassphraseInvalidMessage() {
        $passphrase = 'my secret key';
        $message = 'precious message';
        $context = 'unit testing now';

        $encrypted = phpthemis_scell_seal_encrypt_with_passphrase($passphrase, $message, $context);
        $this->expectException(Exception::class);
        $decrypted = phpthemis_scell_seal_decrypt_with_passphrase($passphrase, $encrypted.'X', $context);
    }

    public function testSealPassphraseIncorrectContext() {
        $passphrase = 'my secret key';
        $message = 'precious message';
        $context_alice = 'secure cell';
        $context_bob = 'peanut butter';

        $encrypted = phpthemis_scell_seal_encrypt_with_passphrase($passphrase, $message, $context_alice);
        $this->expectException(Exception::class);
        $decrypted = phpthemis_scell_seal_decrypt_with_passphrase($passphrase, $encrypted, $context_bob);
    }

    public function testSealPassphraseNoContext() {
        $passphrase = 'my secret key';
        $message = 'precious message';
        $context = 'unit testing now';

        // Omitted context is the same as null or empty context
        $encrypted  = phpthemis_scell_seal_encrypt_with_passphrase($passphrase, $message);
        $decrypted1 = phpthemis_scell_seal_decrypt_with_passphrase($passphrase, $encrypted, NULL);
        $decrypted2 = phpthemis_scell_seal_decrypt_with_passphrase($passphrase, $encrypted, '');

        $this->assertEquals($decrypted1, $message);
        $this->assertEquals($decrypted2, $message);
    }

    public function testSealPassphraseIsNotMasterKey() {
        $passphrase = 'my secret key';
        $message = 'precious message';

        $encrypted = phpthemis_scell_seal_encrypt_with_passphrase($passphrase, $message);
        $this->expectException(Exception::class);
        $decrypted = phpthemis_scell_seal_decrypt($passphrase, $encrypted);
    }

    /** @dataProvider InvalidValues */
    public function testSealPassphraseNoKeyEncrypt($empty) {
        $this->expectException(Exception::class);
        $encrypted = phpthemis_scell_seal_encrypt_with_passphrase($empty, 'precious message');
    }

    /** @dataProvider InvalidValues */
    public function testSealPassphraseNoMessageEncrypt($empty) {
        $master_key = phpthemis_gen_sym_key();
        $this->expectException(Exception::class);
        $encrypted = phpthemis_scell_seal_encrypt_with_passphrase('passphrase', $empty);
    }

    /** @dataProvider InvalidValues */
    public function testSealPassphraseNoKeyDecrypt($empty) {
        $master_key = phpthemis_gen_sym_key();
        $encrypted = phpthemis_scell_seal_encrypt_with_passphrase('passphrase', 'precious message');
        $this->expectException(Exception::class);
        $decrypted = phpthemis_scell_seal_decrypt_with_passphrase($empty, $encrypted);
    }

    /** @dataProvider InvalidValues */
    public function testSealPassphraseNoMessageDecrypt($empty) {
        $encrypted = phpthemis_scell_seal_encrypt_with_passphrase('passphrase', 'precious message');
        $this->expectException(Exception::class);
        $decrypted = phpthemis_scell_seal_decrypt_with_passphrase('passphrase', $empty);
    }

    /**
     * @dataProvider SealWithContextProvider
     */
    public function testSealWithContext($key, $message, $context, $iscorrect) {
        if (!$iscorrect) {
            $this->expectException(Exception::class);
        }
        $encrypted_message = phpthemis_scell_seal_encrypt($key, $message, $context);
        $decrypted_message = phpthemis_scell_seal_decrypt($key, $encrypted_message, $context);
        $this->assertEquals($decrypted_message, $message);
    }

    public function SealWithContextProvider() {
        $master_key = phpthemis_gen_sym_key();
        return array(
            array(NULL, NULL, NULL, false),
            array("", "", "", false),
            array(NULL, "This is test message", NULL, false),
            array("", "This is test message", "This is test context", false),
            array($master_key, NULL, NULL, false),
            array($master_key, "", "This is test context", false),
            array($master_key, "This is test message", "", true),
            array($master_key, "This is test message", NULL, true),
            array($master_key, "This is test message", "This is test context", true)
        );
    }

    /**
     * @dataProvider SealWithContextProviderNoDecrypt
     */
    public function testSealWithContextNoDecrypt($key, $message, $context, $iscorrect) {
        if (!$iscorrect) {
            $this->expectException(Exception::class);
        }
        $encrypted_message = phpthemis_scell_seal_encrypt($key, $message, $context);
        $decrypted_message = phpthemis_scell_seal_decrypt($key."a", $encrypted_message, $context);
        $this->assertEquals($decrypted_message, $message);
    }

    public function SealWithContextProviderNoDecrypt() {
        $master_key = phpthemis_gen_sym_key();
        return array(
            array(NULL, NULL, NULL, false),
            array("", "", "", false),
            array(NULL, "This is test message", NULL, false),
            array("", "This is test message", "This is test context", false),
            array($master_key, NULL, NULL, false),
            array($master_key, "", "This is test context", false),
            array($master_key, "This is test message", "", false),
            array($master_key, "This is test message", NULL, false),
            array($master_key, "This is test message", "This is test context", false)
        );
    }

    /**
     * @dataProvider SealWithoutContextProvider
     */
    public function testSealWithoutContext($key, $message, $iscorrect) {
        if (!$iscorrect) {
            $this->expectException(Exception::class);
        }
        $encrypted_message = phpthemis_scell_seal_encrypt($key, $message);
        $decrypted_message = phpthemis_scell_seal_decrypt($key, $encrypted_message);
        $this->assertEquals($decrypted_message, $message);
    }

    public function SealWithoutContextProvider() {
        $master_key = phpthemis_gen_sym_key();
        return array(
            array(NULL, NULL, false),
            array("", "", false),
            array(NULL, "This is test message", false),
            array("", "This is test message", false),
            array($master_key, NULL, false),
            array($master_key, "", false),
            array($master_key, "This is test message", true)
        );
    }

    /**
     * @dataProvider TokenProtectWithContextProvider
     */
    public function testTokenProtectWithContext($key, $message, $context, $iscorrect) {
        if (!$iscorrect) {
            $this->expectException(Exception::class);
        }
        $encrypted_message = phpthemis_scell_token_protect_encrypt($key, $message, $context);
        $decrypted_message = phpthemis_scell_token_protect_decrypt($key, $encrypted_message['encrypted_message'], $encrypted_message['token'], $context);
        $this->assertEquals($decrypted_message, $message);
    }

    public function TokenProtectWithContextProvider() {
        $master_key = phpthemis_gen_sym_key();
        return array(
            array(NULL, NULL, NULL, false),
            array("", "", "", false),
            array(NULL, "This is test message", NULL, false),
            array("", "This is test message", "This is test context", false),
            array($master_key, NULL, NULL, false),
            array($master_key, "", "This is test context", false),
            array($master_key, "This is test message", "", true),
            array($master_key, "This is test message", NULL, true),
            array($master_key, "This is test message", "This is test context", true)
        );
    }

    /**
     * @dataProvider TokenProtectWithContextProviderNoDecrypt
     */
    public function testTokenProtectWithContextNoDecrypt($key, $message, $context, $iscorrect) {
        if (!$iscorrect) {
            $this->expectException(Exception::class);
        }
        $encrypted_message = phpthemis_scell_token_protect_encrypt($key, $message, $context);
        $decrypted_message = phpthemis_scell_token_protect_decrypt($key."a", $encrypted_message['encrypted_message'], $encrypted_message['token'], $context);
        $this->assertEquals($decrypted_message, $message);
    }

    public function TokenProtectWithContextProviderNoDecrypt() {
        $master_key = phpthemis_gen_sym_key();
        return array(
            array($master_key, "This is test message", "", false),
            array($master_key, "This is test message", NULL, false),
            array($master_key, "This is test message", "This is test context", false)
        );
    }

    /**
     * @dataProvider TokenProtectWithContextProviderNoToken
     */
    public function testTokenProtectWithContextNoToken($key, $message, $context, $iscorrect) {
        if (!$iscorrect) {
            $this->expectException(Exception::class);
        }
        $encrypted_message = phpthemis_scell_token_protect_encrypt($key, $message, $context);
        $decrypted_message = phpthemis_scell_token_protect_decrypt($key, $encrypted_message['encrypted_message']."a", $encrypted_message['token'], $context);
        $this->assertEquals($decrypted_message, $message);
    }

    public function TokenProtectWithContextProviderNoToken() {
        $master_key = phpthemis_gen_sym_key();
        return array(
            array($master_key, "This is test message", "", false),
            array($master_key, "This is test message", NULL, false),
            array($master_key, "This is test message", "This is test context", false)
        );
    }

    /**
     * @dataProvider TokenProtectWithoutContextProvider
     */
    public function testTokenProtectWithoutContext($key, $message, $iscorrect) {
        if (!$iscorrect) {
            $this->expectException(Exception::class);
        }
        $encrypted_message = phpthemis_scell_token_protect_encrypt($key, $message);
        $decrypted_message = phpthemis_scell_token_protect_decrypt($key, $encrypted_message['encrypted_message'], $encrypted_message['token']);
        $this->assertEquals($decrypted_message, $message);
    }

    public function TokenProtectWithoutContextProvider() {
        $master_key = phpthemis_gen_sym_key();
        return array(
            array(NULL, NULL, false),
            array("", "", false),
            array(NULL, "This is test message", false),
            array("", "This is test message", false),
            array($master_key, NULL, false),
            array($master_key, "", false),
            array($master_key, "This is test message", true)
        );
    }

    /**
     * @dataProvider ContextImprintProvider
     */
    public function testContextImprint($key, $message, $context, $iscorrect) {
        if (!$iscorrect) {
            $this->expectException(Exception::class);
        }
        $encrypted_message = phpthemis_scell_context_imprint_encrypt($key, $message, $context);
        $decrypted_message = phpthemis_scell_context_imprint_decrypt($key, $encrypted_message, $context);
        $this->assertEquals($decrypted_message, $message);
    }

    public function ContextImprintProvider() {
        $master_key = phpthemis_gen_sym_key();
        return array(
            array(NULL, NULL, NULL, false),
            array("", "", "", false),
            array(NULL, "This is test message", NULL, false),
            array("", "This is test message", "This is test context", false),
            array($master_key, NULL, NULL, false),
            array($master_key, "", "This is test context", false),
            array($master_key, "This is test message", "", false),
            array($master_key, "This is test message", NULL, false),
            array($master_key, "This is test message", "This is test context", true)
        );
    }

    /**
     * @dataProvider WrongInputSealWithoutContextProvider
     */
    public function testWrongInputSealDecryptWithoutContext($key, $message) {
        $this->expectException(Exception::class);
        phpthemis_scell_seal_decrypt($key, $message);
    }

    /**
     * @dataProvider WrongInputSealWithoutContextProvider
     */
    public function testWrongInputSealDecryptWithContext($key, $message) {
        $this->expectException(Exception::class);
        phpthemis_scell_seal_decrypt($key, $message, 'Context');
    }

    public function WrongInputSealWithoutContextProvider() {
        $master_key = phpthemis_gen_sym_key();
        return array(
            array(NULL, NULL),
            array("", ""),
            array(NULL, "This is test message"),
            array("", "This is test message", false),
            array($master_key, NULL),
            array($master_key, ""),
            array($master_key, "Short message"),
            # todo: next is skipped because of 'Out of memory' failure
            # array("This is test key", "This is test very loooongggg message. This is test very loooongggg message. This is test very loooongggg message. This is test very loooongggg message. This is test very loooongggg message. This is test very loooongggg message. This is test very loooongggg message. This is test very loooongggg message.")
        );
    }

    /**
     * @dataProvider WrongParamsProvider
     */
    public function testWrongParams($func, $params) {
        $this->expectException(Exception::class);
        $decrypted_message = call_user_func_array($func, $params);
    }


    public function WrongParamsProvider() {
        return array(
            array('phpthemis_scell_seal_encrypt', array('key')),
            array('phpthemis_scell_seal_decrypt', array('key'))
        );
    }

}

?>
