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
    die("Usage: <command: enc | dec > <key> <message> <context>\n");
}

$cmd = $argv[1];
$key = $argv[2];
$message = $argv[3];
$context = $argv[4];

if ($cmd == 'enc') {
    $enc_message = phpthemis_scell_context_imprint_encrypt($key, $message, $context);
    echo base64_encode($enc_message);
} elseif ($cmd == 'dec') {
    echo phpthemis_scell_context_imprint_decrypt($key, base64_decode($message), $context)."\n";
} else {
    die("Wrong command, use \"enc\" or \"dec\"\n");
}

?>

