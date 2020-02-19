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
?>
<!doctype html>
<html>
<head><title>Themis Secure Cell Example</title></head>
<body>
<p>All binary values are base64-encoded on this page.</p>
<h1>Parameters</h1>
<?php
    $passphrase = "password";
    $key = phpthemis_gen_sym_key();
    $context = "user context";
    $message = "test message";
?>
<p>Passphrase: <code><?php echo $passphrase; ?></code></p>
<p>Master key: <code><?php echo base64_encode($key); ?></code></p>
<p>Context:    <code><?php echo $context; ?></code></p>
<p>Message:    <code><?php echo $message; ?></code></p>
<h1>Secure Cell Modes</h1>
<h2>Seal Mode</h2>
<h3>Master key API</h3>
<?php
    $encrypted = phpthemis_scell_seal_encrypt($key, $message, $context);
    $decrypted = phpthemis_scell_seal_decrypt($key, $encrypted, $context);
?>
<p>Encrypted message: <code><?php echo base64_encode($encrypted); ?></code></p>
<p>Decrypted message: <code><?php echo $decrypted; ?></code></p>
<h3>Passphrase API</h3>
<?php
    $encrypted = phpthemis_scell_seal_encrypt_with_passphrase($passphrase, $message, $context);
    $decrypted = phpthemis_scell_seal_decrypt_with_passphrase($passphrase, $encrypted, $context);
?>
<p>Encrypted message: <code><?php echo base64_encode($encrypted); ?></code></p>
<p>Decrypted message: <code><?php echo $decrypted; ?></code></p>
<h2>Token Protect Mode</h2>
<?php
    $encrypted = phpthemis_scell_token_protect_encrypt($key, $message, $context);
    $encrypted_message = $encrypted['encrypted_message'];
    $authentication_token = $encrypted['token'];
    $decrypted = phpthemis_scell_token_protect_decrypt($key, $encrypted_message, $authentication_token, $context);
?>
<p>Message (binary):     <code><?php echo base64_encode($message); ?></code></p>
<p>Encrypted message:    <code><?php echo base64_encode($encrypted_message); ?></code></p>
<p>Authentication token: <code><?php echo base64_encode($authentication_token); ?></code></p>
<p>Decrypted message:    <code><?php echo $decrypted; ?></code></p>
<h2>Context Imprint Mode</h2>
<?php
    $encrypted = phpthemis_scell_context_imprint_encrypt($key, $message, $context);
    $decrypted = phpthemis_scell_context_imprint_decrypt($key, $encrypted, $context);
?>
<p>Message (binary):  <code><?php echo base64_encode($message); ?></code></p>
<p>Encrypted message: <code><?php echo base64_encode($encrypted); ?></code></p>
<p>Decrypted message: <code><?php echo $decrypted; ?></code></p>
</body>
</html>
