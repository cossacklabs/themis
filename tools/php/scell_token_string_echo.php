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

if (count($argv) < 3 || count($argv) > 5) {
    die("Usage: <command: enc | dec > <key> <message> <context>\n");
}

$dump_file_name = 'scell_token_string.dat';
$cmd = $argv[1];
$key = $argv[2];
if ($cmd == 'enc') {
    $message = $argv[3];
    $token = null;
} else {
    list($message, $token) = explode(",", $argv[3]);
}
$context = count($argv) > 4 ? $argv[4] : null;
if ($cmd == 'enc') {
    $enc_data = phpthemis_scell_token_protect_encrypt($key, $message, $context);
    echo implode(',', [base64_encode($enc_data['encrypted_message']), base64_encode($enc_data['token'])]);
} elseif ($cmd == 'dec') {
    echo phpthemis_scell_token_protect_decrypt($key, base64_decode($message), base64_decode($token), $context)."\n";
} else {
    die("Wrong command, use \"enc\" or \"dec\"\n");
}

?>
