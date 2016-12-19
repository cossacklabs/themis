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

<?php

    $client_pub = "\x55\x45\x43\x32\x00\x00\x00\x2d\x13\x8b\xdf\x0c\x02\x1f\x09\x88\x39\xd9\x73\x3a\x84\x8f\xa8\x50\xd9\x2b\xed\x3d\x38\xcf\x1d\xd0\xce\xf4\xae\xdb\xcf\xaf\xcb\x6b\xa5\x4a\x08\x11\x21";

    $server_priv= "\x52\x45\x43\x32\x00\x00\x00\x2d\x49\x87\x04\x6b\x00\xf2\x06\x07\x7d\xc7\x1c\x59\xa1\x8f\x39\xfc\x94\x81\x3f\x9e\xc5\xba\x70\x6f\x93\x08\x8d\xe3\x85\x82\x5b\xf8\x3f\xc6\x9f\x0b\xdf";

    $client_priv = "\x52\x45\x43\x32\x00\x00\x00\x2d\x51\xf4\xaa\x72\x00\x9f\x0f\x09\xce\xbe\x09\x33\xc2\x5e\x9a\x05\x99\x53\x9d\xb2\x32\xa2\x34\x64\x7a\xde\xde\x83\x8f\x65\xa9\x2a\x14\x6d\xaa\x90\x01";

    $server_pub  = "\x55\x45\x43\x32\x00\x00\x00\x2d\x75\x58\x33\xd4\x02\x12\xdf\x1f\xe9\xea\x48\x11\xe1\xf9\x71\x8e\x24\x11\xcb\xfd\xc0\xa3\x6e\xd6\xac\x88\xb6\x44\xc2\x9a\x24\x84\xee\x50\x4c\x3e\xa0";

    function get_pub_key_by_id($id){
	$key_array = [
	    "client" => "\x55\x45\x43\x32\x00\x00\x00\x2d\x13\x8b\xdf\x0c\x02\x1f\x09\x88\x39\xd9\x73\x3a\x84\x8f\xa8\x50\xd9\x2b\xed\x3d\x38\xcf\x1d\xd0\xce\xf4\xae\xdb\xcf\xaf\xcb\x6b\xa5\x4a\x08\x11\x21",
	];
	return $key_array[$id];
    }

error_reporting(1);
set_time_limit(0);
ob_implicit_flush();

$address = '0.0.0.0';
$port = 26026;

$sock = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
socket_bind($sock, $address, $port);
socket_listen($sock, 5);

try{
    do {
	if(($msgsock = socket_accept($sock))===false){break;}
	$session = new themis_secure_session("server", $server_priv);

	while (! $session->is_established()) {
	    if(false==socket_recv($msgsock, $buf, 2048, 0)){break 2;}
	    $unwrapped_message=$session->unwrap($buf);
	    if(FALSE === socket_send($msgsock, $unwrapped_message, strlen($unwrapped_message), MSG_EOR)){echo socket_strerror(socket_last_error($sock))."\n";exit;}
	};
	$buf = socket_read($msgsock, 2048, PHP_BINARY_READ);
	$msg=$session->unwrap($buf);
	echo $msg."\n";
	$wrapped_reply = $session->wrap($msg." reply");
	socket_write($msgsock, $wrapped_reply, strlen($wrapped_reply));
	socket_close($msgsock);
    } while (true);
}
catch (Exception $e){
    echo $e->getMessage();
}
socket_close($sock);
?>
