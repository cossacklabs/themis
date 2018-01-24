#!/bin/bash -e

# import functions
. tests/_integration/utils.sh

echo ".. testing secure cell, node <--> node"
node ./tests/_integration/scell_seal_string_echo.js "dec" "passr" `node ./tests/_integration/scell_seal_string_echo.js "enc" "passr" "node seal test"`
check_result_zero
node ./tests/_integration/scell_seal_string_echo.js "dec" "passr" `node ./tests/_integration/scell_seal_string_echo.js "enc" "passr" "node seal with context" "somecontext"` "somecontext"
check_result_zero

node ./tests/_integration/scell_context_string_echo.js "dec" "passw2" `node ./tests/_integration/scell_context_string_echo.js "enc" "passw2" "node context with context" "somecontext"` "somecontext"
check_result_zero

node ./tests/_integration/scell_token_string_echo.js "dec" "passw1" `node ./tests/_integration/scell_token_string_echo.js "enc" "passw1" "node token test"`
check_result_zero
node ./tests/_integration/scell_token_string_echo.js "dec" "passw2" `node ./tests/_integration/scell_token_string_echo.js "enc" "passw2" "node token with context" "somecontext"` "somecontext"
check_result_zero

echo ".. testing secure message, node <--> node"
node ./tests/_integration/smessage_encryption.js "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `node ./tests/_integration/smessage_encryption.js "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "node secure message"`
check_result_zero

node ./tests/_integration/smessage_encryption.js "verify" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `node ./tests/_integration/smessage_encryption.js "sign" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "node secure message"`
check_result_zero

###
# NODE <-> RUBY
echo ".. testing secure cell, seal mode,  node <--> ruby"

node ./tests/_integration/scell_seal_string_echo.js "dec" "passwd" `ruby ./tests/_integration/scell_seal_string_echo.rb "enc" "passwd" "ruby->node seal"`
check_result_zero
ruby ./tests/_integration/scell_seal_string_echo.rb "dec" "passwd" `node ./tests/_integration/scell_seal_string_echo.js "enc" "passwd" "node->ruby seal"`
check_result_zero

## with context
node ./tests/_integration/scell_seal_string_echo.js "dec" "passwd" `ruby ./tests/_integration/scell_seal_string_echo.rb "enc" "passwd" "ruby->node seal with context" "somecontext"` "somecontext"
check_result_zero
ruby ./tests/_integration/scell_seal_string_echo.rb "dec" "passwd" `node ./tests/_integration/scell_seal_string_echo.js "enc" "passwd" "node->ruby seal with context" "somecontext"` "somecontext"
check_result_zero

echo ".. testing secure cell, context imprint mode, node <--> ruby"
ruby ./tests/_integration/scell_context_string_echo.rb "dec" "passwd" `node ./tests/_integration/scell_context_string_echo.js "enc" "passwd" "node->ruby with context" "somecontext"` "somecontext"
check_result_zero
node ./tests/_integration/scell_context_string_echo.js "dec" "passwd" `ruby ./tests/_integration/scell_context_string_echo.rb "enc" "passwd" "ruby->node with context" "somecontext"` "somecontext"
check_result_zero

echo ".. testing secure cell, token protect mode, node <--> ruby"
ruby ./tests/_integration/scell_token_string_echo.rb "dec" "passwd" `node ./tests/_integration/scell_token_string_echo.js "enc" "passwd" "node->ruby token test"`
check_result_zero
ruby ./tests/_integration/scell_token_string_echo.rb "dec" "passwd" `node ./tests/_integration/scell_token_string_echo.js "enc" "passwd" "node->ruby token test with content" "somecontext"` "somecontext"
check_result_zero

node ./tests/_integration/scell_token_string_echo.js "dec" "passwd" `ruby ./tests/_integration/scell_token_string_echo.rb "enc" "passwd" "ruby->node with context" "somecontext"` "somecontext"
check_result_zero
node ./tests/_integration/scell_token_string_echo.js "dec" "passwd" `ruby ./tests/_integration/scell_token_string_echo.rb "enc" "passwd" "ruby->node with context" "somecontext"` "somecontext"
check_result_zero

echo ".. testing secure message,  node <--> ruby"
ruby ./tests/_integration/smessage_encryption.rb "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `node ./tests/_integration/smessage_encryption.js "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "node -> ruby secure message"`
check_result_zero
node ./tests/_integration/smessage_encryption.js "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `ruby ./tests/_integration/smessage_encryption.rb "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "ruby -> node secure message"`
check_result_zero

ruby ./tests/_integration/smessage_encryption.rb "verify" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `node ./tests/_integration/smessage_encryption.js "sign" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "node -> ruby secure message"`
check_result_zero
node ./tests/_integration/smessage_encryption.js "verify" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `ruby ./tests/_integration/smessage_encryption.rb "sign" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "ruby -> node secure message"`
check_result_zero

echo ".. testing secure cell, token protect mode, node <--> go"
go run ./tests/_integration/scell_token_string_echo.go "dec" "passwd" `node ./tests/_integration/scell_token_string_echo.js "enc" "passwd" "node->go token"`
check_result_zero

go run ./tests/_integration/scell_token_string_echo.go "dec" "passwd" `node ./tests/_integration/scell_token_string_echo.js "enc" "passwd" "node->go token with content" "somecontext"` "somecontext"
check_result_zero

node ./tests/_integration/scell_token_string_echo.js "dec" "passwd" `go run ./tests/_integration/scell_token_string_echo.go "enc" "passwd" "go->node token" "somecontext"` "somecontext"
check_result_zero
node ./tests/_integration/scell_token_string_echo.js "dec" "passwd" `go run ./tests/_integration/scell_token_string_echo.go "enc" "passwd" "go->node token with context" "somecontext"` "somecontext"
check_result_zero

echo ".. testing secure message,  node <--> go"
go run ./tests/_integration/smessage_encryption.go "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `node ./tests/_integration/smessage_encryption.js "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "node -> go secure message"`
check_result_zero
node ./tests/_integration/smessage_encryption.js "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `go run ./tests/_integration/smessage_encryption.go "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "go -> node secure message"`
check_result_zero

go run ./tests/_integration/smessage_encryption.go "verify" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `node ./tests/_integration/smessage_encryption.js "sign" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "node -> go secure message"`
check_result_zero
node ./tests/_integration/smessage_encryption.js "verify" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `go run ./tests/_integration/smessage_encryption.go "sign" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "go -> node secure message"`
check_result_zero

echo ".. testing secure cell, token protect mode, node <--> php"
php -f ./tests/_integration/scell_token_string_echo.php "dec" "passwd" `node ./tests/_integration/scell_token_string_echo.js "enc" "passwd" "php->node token"`
check_result_zero

php -f ./tests/_integration/scell_token_string_echo.php "dec" "passwd" `node ./tests/_integration/scell_token_string_echo.js "enc" "passwd" "php->node token with content" "somecontext"` "somecontext"
check_result_zero

node ./tests/_integration/scell_token_string_echo.js "dec" "passwd" `php -f ./tests/_integration/scell_token_string_echo.php "enc" "passwd" "node->php token" "somecontext"` "somecontext"
check_result_zero
node ./tests/_integration/scell_token_string_echo.js "dec" "passwd" `php -f ./tests/_integration/scell_token_string_echo.php "enc" "passwd" "node->php token with context" "somecontext"` "somecontext"
check_result_zero

echo ".. testing secure message,  node <--> php"
php -f ./tests/_integration/smessage_encryption.php "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `node ./tests/_integration/smessage_encryption.js "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "node -> php secure message"`
check_result_zero
node ./tests/_integration/smessage_encryption.js "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `php -f ./tests/_integration/smessage_encryption.php "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "php -> node secure message"`
check_result_zero

php -f ./tests/_integration/smessage_encryption.php "verify" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `node ./tests/_integration/smessage_encryption.js "sign" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "node -> php secure message"`
check_result_zero
node ./tests/_integration/smessage_encryption.js "verify" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `php -f ./tests/_integration/smessage_encryption.php "sign" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "php -> node secure message"`
check_result_zero

echo ".. testing secure cell, token protect mode, node <--> python"
python ./tests/_integration/scell_token_string_echo.py "dec" "passwd" `node ./tests/_integration/scell_token_string_echo.js "enc" "passwd" "php->node token"`
check_result_zero

python ./tests/_integration/scell_token_string_echo.py "dec" "passwd" `node ./tests/_integration/scell_token_string_echo.js "enc" "passwd" "php->node token with content" "somecontext"` "somecontext"
check_result_zero

node ./tests/_integration/scell_token_string_echo.js "dec" "passwd" `python ./tests/_integration/scell_token_string_echo.py "enc" "passwd" "node->php token" "somecontext"` "somecontext"
check_result_zero
node ./tests/_integration/scell_token_string_echo.js "dec" "passwd" `python ./tests/_integration/scell_token_string_echo.py "enc" "passwd" "node->php token with context" "somecontext"` "somecontext"
check_result_zero

echo ".. testing secure message,  node <--> python"
python ./tests/_integration/smessage_encryption.py "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `node ./tests/_integration/smessage_encryption.js "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "node -> python secure message"`
check_result_zero
node ./tests/_integration/smessage_encryption.js "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `python ./tests/_integration/smessage_encryption.py "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "python -> node secure message"`
check_result_zero

python ./tests/_integration/smessage_encryption.py "verify" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `node ./tests/_integration/smessage_encryption.js "sign" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "node -> python secure message"`
check_result_zero
node ./tests/_integration/smessage_encryption.js "verify" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `python ./tests/_integration/smessage_encryption.py "sign" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "python -> node secure message"`
check_result_zero

exit ${status}