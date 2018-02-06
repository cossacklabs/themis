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

    echo '<html><body>';

    $key = "password";
    $context = "user context";
    $message = "test message";

    $encrypted_message = phpthemis_scell_seal_encrypt($key, $message, $context);

    echo bin2hex($encrypted_message);
    echo '<br>'; 
    $plain_message = phpthemis_scell_seal_decrypt($key, $encrypted_message, $context);
    echo $plain_message;
    echo '<br>';

    $encrypted_message = phpthemis_scell_token_protect_encrypt($key, $message, $context);

    echo bin2hex($encrypted_message['encrypted_message']);
    echo bin2hex($encrypted_message['token']);
    echo '<br>'; 
    $plain_message2 = phpthemis_scell_token_protect_decrypt($key, $encrypted_message['encrypted_message'],  $encrypted_message['token'], $context);
    echo $plain_message2;
    echo '<br>';

    $encrypted_message = phpthemis_scell_context_imprint_encrypt($key, $message, $context);

    echo bin2hex($encrypted_message);
    echo '<br>'; 
    $plain_message3 = phpthemis_scell_context_imprint_decrypt($key, $encrypted_message, $context);
    echo $plain_message3;

    echo '<br></body></html>';

?>
