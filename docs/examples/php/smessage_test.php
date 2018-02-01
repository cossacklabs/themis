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

    echo '<html><body>';

    $client_pub = "\x55\x45\x43\x32\x00\x00\x00\x2d\x13\x8b\xdf\x0c\x02\x1f\x09\x88\x39\xd9\x73\x3a\x84\x8f\xa8\x50\xd9\x2b\xed\x3d\x38\xcf\x1d\xd0\xce\xf4\xae\xdb\xcf\xaf\xcb\x6b\xa5\x4a\x08\x11\x21";

    $server_priv= "\x52\x45\x43\x32\x00\x00\x00\x2d\x49\x87\x04\x6b\x00\xf2\x06\x07\x7d\xc7\x1c\x59\xa1\x8f\x39\xfc\x94\x81\x3f\x9e\xc5\xba\x70\x6f\x93\x08\x8d\xe3\x85\x82\x5b\xf8\x3f\xc6\x9f\x0b\xdf";

    $client_priv = "\x52\x45\x43\x32\x00\x00\x00\x2d\x51\xf4\xaa\x72\x00\x9f\x0f\x09\xce\xbe\x09\x33\xc2\x5e\x9a\x05\x99\x53\x9d\xb2\x32\xa2\x34\x64\x7a\xde\xde\x83\x8f\x65\xa9\x2a\x14\x6d\xaa\x90\x01";

    $server_pub  = "\x55\x45\x43\x32\x00\x00\x00\x2d\x75\x58\x33\xd4\x02\x12\xdf\x1f\xe9\xea\x48\x11\xe1\xf9\x71\x8e\x24\x11\xcb\xfd\xc0\xa3\x6e\xd6\xac\x88\xb6\x44\xc2\x9a\x24\x84\xee\x50\x4c\x3e\xa0";

    echo '<h2>encrypting message</h2>';
    $encrypted_message = phpthemis_secure_message_wrap($server_priv, $client_pub, "test message");
    echo '<b>encrypted message: </b>'; 
    echo bin2hex($encrypted_message);
    echo '<br> <b>decrypted message: </b>';
    $plain_message = phpthemis_secure_message_unwrap($client_priv, $server_pub, $encrypted_message);
    echo $plain_message;
    echo '<br>'; 

    echo '<h2>signing message</h2>';
    $signed_message = phpthemis_secure_message_wrap($server_priv, NULL, "test message");
    echo '<b>signed message: </b>'; 
    echo bin2hex($signed_message);
    echo '<br> <b>decrypted message: </b>';
    $plain_message = phpthemis_secure_message_unwrap($client_priv, $server_pub, $signed_message);
    echo $plain_message;
    echo '<br>'; 

    echo '<h2>key generation for EC</h2>';    
    $key_pair = phpthemis_gen_ec_key_pair();
    echo "private_key = ";
    echo bin2hex($key_pair['private_key']);
    echo '<br>';
    echo "public_key = ";
    echo bin2hex($key_pair['public_key']);

    echo '<h2>key generation for RSA</h2>';    
    $key_pair = phpthemis_gen_rsa_key_pair();
    echo "private_key = ";
    echo bin2hex($key_pair['private_key']);
    echo '<br>';
    echo "public_key = ";
    echo bin2hex($key_pair['public_key']);
    echo '<br></body></html>';

?>
