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

if (count($argv) != 1 && count($argv) != 3) {
    die("Usage: php -f keygen.php <private_key_path> <public_key_path>\n");
}
$private_key_path = null;
$public_key_path = null;
if (count($argv) == 1){
    $private_key_path = "key";
    $public_key_path = "key.pub";
} else if (count($argv) == 3){
    $private_key_path = $argv[1];
    $public_key_path = $argv[2];
}

$keypair = phpthemis_gen_ec_key_pair();

file_put_contents($private_key_path, $keypair['private_key']);
chmod($private_key_path, 0400);
file_put_contents($public_key_path, $keypair['public_key']);

?>