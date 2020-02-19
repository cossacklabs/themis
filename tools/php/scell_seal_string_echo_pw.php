<?php
#
# Copyright (c) 2020 Cossack Labs Limited
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

if (!extension_loaded('phpthemis')) {
    die("phpthemis extension not loaded!\n");
}

if (count($argv) < 3 || count($argv) > 5) {
    die("usage: {enc|dec} <passphrase> <message> [context]\n");
}

$command = $argv[1];
$passphrase = $argv[2];
$message = isset($argv[3]) ? $argv[3] : null;
$context = count($argv) > 4 ? $argv[4] : null;

switch ($command) {
case 'enc':
    $encrypted = phpthemis_scell_seal_encrypt_with_passphrase($passphrase, $message, $context);
    echo base64_encode($encrypted)."\n";
    break;
case 'dec':
    $message = base64_decode($message);
    $decrypted = phpthemis_scell_seal_decrypt_with_passphrase($passphrase, $message, $context);
    echo $decrypted."\n";
    break;
default:
    die("unknown command: \"$command\", use \"enc\" or \"dec\"\n");
}

?>
