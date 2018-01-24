#!/bin/bash -e

echo ".. testing secure cell, python <--> python"
python ./tests/_integration/scell_seal_string_echo.py "dec" "passr" `python ./tests/_integration/scell_seal_string_echo.py "enc" "passr" "python seal test"`
python ./tests/_integration/scell_seal_string_echo.py "dec" "passr" `python ./tests/_integration/scell_seal_string_echo.py "enc" "passr" "python seal with context" "somecontext"` "somecontext"

python ./tests/_integration/scell_context_string_echo.py "dec" "passw2" `python ./tests/_integration/scell_context_string_echo.py "enc" "passw2" "python context with context" "somecontext"` "somecontext"

python ./tests/_integration/scell_token_string_echo.py "dec" "passw1" `python ./tests/_integration/scell_token_string_echo.py "enc" "passw1" "python token test"`
python ./tests/_integration/scell_token_string_echo.py "dec" "passw2" `python ./tests/_integration/scell_token_string_echo.py "enc" "passw2" "python token with context" "somecontext"` "somecontext"

echo ".. testing secure message, python <--> python"
python ./tests/_integration/smessage_encryption.py "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `python ./tests/_integration/smessage_encryption.py "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "python secure message"`


# PYTHON <-> RUBY
echo ".. testing secure cell, seal mode,  python <--> ruby"

python ./tests/_integration/scell_seal_string_echo.py "dec" "passwd" `ruby ./tests/_integration/scell_seal_string_echo.rb "enc" "passwd" "ruby->python seal"`
ruby ./tests/_integration/scell_seal_string_echo.rb "dec" "passwd" `python ./tests/_integration/scell_seal_string_echo.py "enc" "passwd" "python->ruby seal"`

## with context
python ./tests/_integration/scell_seal_string_echo.py "dec" "passwd" `ruby ./tests/_integration/scell_seal_string_echo.rb "enc" "passwd" "ruby->python seal with context" "somecontext"` "somecontext"
ruby ./tests/_integration/scell_seal_string_echo.rb "dec" "passwd" `python ./tests/_integration/scell_seal_string_echo.py "enc" "passwd" "python->ruby seal with context" "somecontext"` "somecontext"

echo ".. testing secure cell, context imprint mode, python <--> ruby"
ruby ./tests/_integration/scell_context_string_echo.rb "dec" "passwd" `python ./tests/_integration/scell_context_string_echo.py "enc" "passwd" "python->ruby with context" "somecontext"` "somecontext"
python ./tests/_integration/scell_context_string_echo.py "dec" "passwd" `ruby ./tests/_integration/scell_context_string_echo.rb "enc" "passwd" "ruby->python with context" "somecontext"` "somecontext"

echo ".. testing secure cell, token protect mode, python <--> ruby"
ruby ./tests/_integration/scell_token_string_echo.rb "dec" "passwd" `python ./tests/_integration/scell_token_string_echo.py "enc" "passwd" "python->ruby token test"`
ruby ./tests/_integration/scell_token_string_echo.rb "dec" "passwd" `python ./tests/_integration/scell_token_string_echo.py "enc" "passwd" "python->ruby token test with content" "somecontext"` "somecontext"

python ./tests/_integration/scell_token_string_echo.py "dec" "passwd" `ruby ./tests/_integration/scell_token_string_echo.rb "enc" "passwd" "ruby->python with context" "somecontext"` "somecontext"
python ./tests/_integration/scell_token_string_echo.py "dec" "passwd" `ruby ./tests/_integration/scell_token_string_echo.rb "enc" "passwd" "ruby->python with context" "somecontext"` "somecontext"

echo ".. testing secure cell, token protect mode, python <--> go"
go run ./tests/_integration/scell_token_string_echo.go "dec" "passwd" `python ./tests/_integration/scell_token_string_echo.py "enc" "passwd" "python->go token"`

go run ./tests/_integration/scell_token_string_echo.go "dec" "passwd" `python ./tests/_integration/scell_token_string_echo.py "enc" "passwd" "python->go token with content" "somecontext"` "somecontext"

python ./tests/_integration/scell_token_string_echo.py "dec" "passwd" `go run ./tests/_integration/scell_token_string_echo.go "enc" "passwd" "go->python token" "somecontext"` "somecontext"
python ./tests/_integration/scell_token_string_echo.py "dec" "passwd" `go run ./tests/_integration/scell_token_string_echo.go "enc" "passwd" "go->python token with context" "somecontext"` "somecontext"

python ./tests/_integration/smessage_encryption.py "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `go run ./tests/_integration/smessage_encryption.go "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "go -> python secure message"`
go run ./tests/_integration/smessage_encryption.go "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `python ./tests/_integration/smessage_encryption.py "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "python -> go secure message"`

echo ".. testing secure cell, token protect mode, python <--> php"
php -f ./tests/_integration/scell_token_string_echo.php "dec" "passwd" `python ./tests/_integration/scell_token_string_echo.py "enc" "passwd" "php->python token"`

php -f ./tests/_integration/scell_token_string_echo.php "dec" "passwd" `python ./tests/_integration/scell_token_string_echo.py "enc" "passwd" "php->python token with content" "somecontext"` "somecontext"

python ./tests/_integration/scell_token_string_echo.py "dec" "passwd" `php -f ./tests/_integration/scell_token_string_echo.php "enc" "passwd" "python->php token" "somecontext"` "somecontext"
python ./tests/_integration/scell_token_string_echo.py "dec" "passwd" `php -f ./tests/_integration/scell_token_string_echo.php "enc" "passwd" "python->php token with context" "somecontext"` "somecontext"

echo ".. testing secure message, python <--> ruby"
ruby ./tests/_integration/smessage_encryption.rb "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `python ./tests/_integration/smessage_encryption.py "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "python -> ruby secure message"`
python ./tests/_integration/smessage_encryption.py "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `ruby ./tests/_integration/smessage_encryption.rb "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "ruby -> python secure message"`

echo ".. testing secure message, python <--> php"
php -f ./tests/_integration/smessage_encryption.php "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `python ./tests/_integration/smessage_encryption.py "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "python -> php secure message"`
python ./tests/_integration/smessage_encryption.py "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `php -f ./tests/_integration/smessage_encryption.php "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "php -> python secure message"`
