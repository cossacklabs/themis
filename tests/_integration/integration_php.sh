#!/bin/bash -e

# import functions
. tests/_integration/utils.sh

# PHP <-> PHP
echo ".. testing secure cell seal, php <--> php"
php -f ./tools/php/scell_seal_string_echo.php "dec" "passwd" `php -f ./tools/php/scell_seal_string_echo.php "enc" "passwd" "php->php seal"`
check_result_zero
echo ".. testing secure cell seal context, php <--> php"
php -f ./tools/php/scell_seal_string_echo.php "dec" "passwd"  `php -f ./tools/php/scell_seal_string_echo.php "enc" "passwd" "php->php seal with context" "context"` "context"
check_result_zero

echo ".. testing secure cell context imprint, php <--> php"
php -f ./tools/php/scell_context_string_echo.php "dec" "passwd" `php -f ./tools/php/scell_context_string_echo.php "enc" "passwd" "php->php seal context message" "somecontext"` "somecontext"
check_result_zero

echo ".. testing secure cell token protect, php <--> php"
php -f ./tools/php/scell_token_string_echo.php "dec" "passwd" `php -f ./tools/php/scell_token_string_echo.php "enc" "passwd" "php->php token"`
check_result_zero
echo ".. testing secure cell token protect context, php <--> php"
php -f ./tools/php/scell_token_string_echo.php "dec" "passwd"  `php -f ./tools/php/scell_token_string_echo.php "enc" "passwd" "php->php token with context" "context"` "context"
check_result_zero

echo ".. testing secure message encryption, php <--> php"
php -f ./tools/php/smessage_encryption.php "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `php -f ./tools/php/smessage_encryption.php "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "php secure message"`
check_result_zero

echo ".. testing secure message signing, php <--> php"
php -f ./tools/php/smessage_encryption.php "verify" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `php -f ./tools/php/smessage_encryption.php "sign" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "php secure message"`
check_result_zero

exit ${status}