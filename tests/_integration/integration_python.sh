#!/bin/bash -e

echo ".. testing secure cell, python <--> python"
python ./tests/_integration/scell_seal_string_echo.py "dec" "passr" `python ./tests/_integration/scell_seal_string_echo.py "enc" "passr" "python seal test"`
python ./tests/_integration/scell_seal_string_echo.py "dec" "passr" `python ./tests/_integration/scell_seal_string_echo.py "enc" "passr" "python seal with context" "somecontext"` "somecontext"

python ./tests/_integration/scell_context_string_echo.py "dec" "passw2" `python ./tests/_integration/scell_context_string_echo.py "enc" "passw2" "python context with context" "somecontext"` "somecontext"

python ./tests/_integration/scell_token_string_echo.py "dec" "passw1" `python ./tests/_integration/scell_token_string_echo.py "enc" "passw1" "python token test"`
python ./tests/_integration/scell_token_string_echo.py "dec" "passw2" `python ./tests/_integration/scell_token_string_echo.py "enc" "passw2" "python token with context" "somecontext"` "somecontext"


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

echo ".. testing secure cell, token protect mode, python <--> php"
php -f ./tests/_integration/scell_token_string_echo.php "dec" "passwd" `python ./tests/_integration/scell_token_string_echo.py "enc" "passwd" "php->python token"`

php -f ./tests/_integration/scell_token_string_echo.php "dec" "passwd" `python ./tests/_integration/scell_token_string_echo.py "enc" "passwd" "php->python token with content" "somecontext"` "somecontext"

python ./tests/_integration/scell_token_string_echo.py "dec" "passwd" `php -f ./tests/_integration/scell_token_string_echo.php "enc" "passwd" "python->php token" "somecontext"` "somecontext"
python ./tests/_integration/scell_token_string_echo.py "dec" "passwd" `php -f ./tests/_integration/scell_token_string_echo.php "enc" "passwd" "python->php token with context" "somecontext"` "somecontext"
