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


if (!extension_loaded('phpthemis')) die("no phpthemis extention found!\n");

if (count($argv) != 5) {
    die("Usage: <command: enc | dec | sign | verify> <send_private_key> <recipient_public_key> <message>\n");
}

$cmd = $argv[1];
$private_key = file_get_contents($argv[2]);
$public_key = file_get_contents($argv[3]);
$message = $argv[4];

if ($cmd == 'enc') {
    $enc_message = phpthemis_secure_message_wrap($private_key, $public_key, $message);
    echo base64_encode($enc_message);
} elseif ($cmd == 'dec') {
    echo phpthemis_secure_message_unwrap($private_key, $public_key, base64_decode($message));
} elseif ($cmd == 'sign') {
    $enc_message = phpthemis_secure_message_wrap($private_key, NULL, $message);
    echo base64_encode($enc_message);
} elseif ($cmd == 'verify') {
    echo phpthemis_secure_message_unwrap(NULL, $public_key, base64_decode($message));
} else {
    die("Wrong command, use <enc | dec | sign | verify>\n");
}

?>

