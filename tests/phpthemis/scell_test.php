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

    /**
     * @dataProvider SealWithContextProvider
     */
    public function testSealWithContext($key, $message, $context, $iscorrect) {
        if (!$iscorrect) {
            $this->expectException('Exception');
        }
        $encrypted_message = phpthemis_scell_seal_encrypt($key, $message, $context);
        $decrypted_message = phpthemis_scell_seal_decrypt($key, $encrypted_message, $context);
        $this->assertEquals($decrypted_message, $message);
    }

    public function SealWithContextProvider() {
        return array(
            array(NULL, NULL, NULL, false),
            array("", "", "", false),
            array(NULL, "This is test message", NULL, false),
            array("", "This is test message", "This is test context", false),
            array("This is test key", NULL, NULL, false),
            array("This is test key", "", "This is test context", false),
            array("This is test key", "This is test message", "", true),
            array("This is test key", "This is test message", NULL, true),
            array("This is test key", "This is test message", "This is test context", true)
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
        return array(
            array(NULL, NULL, NULL, false),
            array("", "", "", false),
            array(NULL, "This is test message", NULL, false),
            array("", "This is test message", "This is test context", false),
            array("This is test key", NULL, NULL, false),
            array("This is test key", "", "This is test context", false),
            array("This is test key", "This is test message", "", false),
            array("This is test key", "This is test message", NULL, false),
            array("This is test key", "This is test message", "This is test context", false)
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
        return array(
            array(NULL, NULL, false),
            array("", "", false),
            array(NULL, "This is test message", false),
            array("", "This is test message", false),
            array("This is test key", NULL, false),
            array("This is test key", "", false),
            array("This is test key", "This is test message", true)
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
        return array(
            array(NULL, NULL, NULL, false),
            array("", "", "", false),
            array(NULL, "This is test message", NULL, false),
            array("", "This is test message", "This is test context", false),
            array("This is test key", NULL, NULL, false),
            array("This is test key", "", "This is test context", false),
            array("This is test key", "This is test message", "", true),
            array("This is test key", "This is test message", NULL, true),
            array("This is test key", "This is test message", "This is test context", true)
        );
    }

    /**
     * @dataProvider TokenProtectWithContextProviderNoDecrypt
     */
    public function testTokenProtectWithContextNoDecrypt($key, $message, $context, $iscorrect) {
        if (!$iscorrect) {
            $this->expectException('Exception');
        }
        $encrypted_message = phpthemis_scell_token_protect_encrypt($key, $message, $context);
        $decrypted_message = phpthemis_scell_token_protect_decrypt($key."a", $encrypted_message['encrypted_message'], $encrypted_message['token'], $context);
        $this->assertEquals($decrypted_message, $message);
    }

    public function TokenProtectWithContextProviderNoDecrypt() {
        return array(
            array("This is test key", "This is test message", "", false),
            array("This is test key", "This is test message", NULL, false),
            array("This is test key", "This is test message", "This is test context", false)
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
        return array(
            array("This is test key", "This is test message", "", false),
            array("This is test key", "This is test message", NULL, false),
            array("This is test key", "This is test message", "This is test context", false)
        );
    }

    /**
     * @dataProvider TokenProtectWithoutContextProvider
     */
    public function testTokenProtectWithoutContext($key, $message, $iscorrect) {
        if (!$iscorrect) {
            $this->expectException('Exception');
        }
        $encrypted_message = phpthemis_scell_token_protect_encrypt($key, $message);
        $decrypted_message = phpthemis_scell_token_protect_decrypt($key, $encrypted_message['encrypted_message'], $encrypted_message['token']);
        $this->assertEquals($decrypted_message, $message);
    }

    public function TokenProtectWithoutContextProvider() {
        return array(
            array(NULL, NULL, false),
            array("", "", false),
            array(NULL, "This is test message", false),
            array("", "This is test message", false),
            array("This is test key", NULL, false),
            array("This is test key", "", false),
            array("This is test key", "This is test message", true)
        );
    }

    /**
     * @dataProvider ContextImprintProvider
     */
    public function testContextImprint($key, $message, $context, $iscorrect) {
        if (!$iscorrect) {
            $this->expectException('Exception');
        }
        $encrypted_message = phpthemis_scell_context_imprint_encrypt($key, $message, $context);
        $decrypted_message = phpthemis_scell_context_imprint_decrypt($key, $encrypted_message, $context);
        $this->assertEquals($decrypted_message, $message);
    }

    public function ContextImprintProvider() {
        return array(
            array(NULL, NULL, NULL, false),
            array("", "", "", false),
            array(NULL, "This is test message", NULL, false),
            array("", "This is test message", "This is test context", false),
            array("This is test key", NULL, NULL, false),
            array("This is test key", "", "This is test context", false),
            array("This is test key", "This is test message", "", false),
            array("This is test key", "This is test message", NULL, false),
            array("This is test key", "This is test message", "This is test context", true)
        );
    }

    /**
     * @dataProvider WrongInputSealWithoutContextProvider
     */
    public function testWrongInputSealDecryptWithoutContext($key, $message) {
        $this->expectException('Exception');
        phpthemis_scell_seal_decrypt($key, $message);
    }

    /**
     * @dataProvider WrongInputSealWithoutContextProvider
     */
    public function testWrongInputSealDecryptWithContext($key, $message) {
        $this->expectException('Exception');
        phpthemis_scell_seal_decrypt($key, $message, 'Context');
    }

    public function WrongInputSealWithoutContextProvider() {
        return array(
            array(NULL, NULL),
            array("", ""),
            array(NULL, "This is test message"),
            array("", "This is test message", false),
            array("This is test key", NULL),
            array("This is test key", ""),
            array("This is test key", "Short message"),
            # todo: next is skipped because of 'Out of memory' failure
            # array("This is test key", "This is test very loooongggg message. This is test very loooongggg message. This is test very loooongggg message. This is test very loooongggg message. This is test very loooongggg message. This is test very loooongggg message. This is test very loooongggg message. This is test very loooongggg message.")
        );
    }

    /**
     * @dataProvider WrongParamsProvider
     */
    public function testWrongParams($func, $params) {
        $this->expectException('Exception');
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