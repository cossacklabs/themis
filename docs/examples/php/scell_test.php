<?php

    echo '<html><body>';

    $key = "password";
    $context = "user context";
    $message = "test message";

    $encrypted_message = phpthemis_scell_full_encrypt($key, $message, $context);

    echo bin2hex($encrypted_message);
    echo '<br>'; 
    $plain_message = phpthemis_scell_full_decrypt($key, $encrypted_message, $context);
    echo $plain_message;
    echo '<br>';

    $encrypted_message = phpthemis_scell_auto_split_encrypt($key, $message, $context);

    echo bin2hex($encrypted_message['encrypted_message']);
    echo bin2hex($encrypted_message['additional_auth_data']);
    echo '<br>'; 
    $plain_message2 = phpthemis_scell_auto_split_decrypt($key, $encrypted_message['encrypted_message'],  $encrypted_message['additional_auth_data'], $context);
    echo $plain_message2;
    echo '<br>';

    $encrypted_message = phpthemis_scell_user_split_encrypt($key, $message, $context);

    echo bin2hex($encrypted_message);
    echo '<br>'; 
    $plain_message3 = phpthemis_scell_user_split_decrypt($key, $encrypted_message, $context);
    echo $plain_message3;

    echo '<br></body></html>';

?>