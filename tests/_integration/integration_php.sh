#!/bin/bash -e

# PHP <-> PHP
echo ".. testing secure cell, php <--> php"
php -f ./tests/_integration/scell_seal_string_echo.php "dec" "passwd" \
`php -f ./tests/_integration/scell_seal_string_echo.php "enc" "passwd" "php->php seal"`
# with context
php -f ./tests/_integration/scell_seal_string_echo.php "dec" "passwd"  `php -f ./tests/_integration/scell_seal_string_echo.php "enc" "passwd" "php->php seal with context" "context"` "context"
# with required context
php -f ./tests/_integration/scell_context_string_echo.php "dec" "passwd" `php -f ./tests/_integration/scell_context_string_echo.php "enc" "passwd" "php->php seal context message" "somecontext"` "somecontext"
# with token
php -f ./tests/_integration/scell_token_string_echo.php "dec" "passwd" \
`php -f ./tests/_integration/scell_token_string_echo.php "enc" "passwd" "php->php token"`
# with token+context
php -f ./tests/_integration/scell_token_string_echo.php "dec" "passwd"  `php -f ./tests/_integration/scell_token_string_echo.php "enc" "passwd" "php->php token with context" "context"` "context"

# PHP <-> RUBY
echo ".. testing secure cell, php <--> ruby"
php -f ./tests/_integration/scell_seal_string_echo.php "dec" "passwd" `ruby ./tests/_integration/scell_seal_string_echo.rb "enc" "passwd" "ruby->php seal"`

ruby ./tests/_integration/scell_seal_string_echo.rb "dec" "passwd" `php -f ./tests/_integration/scell_seal_string_echo.php "enc" "passwd" "php->ruby seal"`
## with context
php -f ./tests/_integration/scell_seal_string_echo.php "dec" "passwd" `ruby ./tests/_integration/scell_seal_string_echo.rb "enc" "passwd" "ruby->php seal with context" "somecontext"` "somecontext"

ruby ./tests/_integration/scell_seal_string_echo.rb "dec" "passwd" `php -f ./tests/_integration/scell_seal_string_echo.php "enc" "passwd" "php->ruby seal with context" "somecontext"` "somecontext"

echo ".. testing secure cell, context imprint mode, php <--> ruby"
ruby ./tests/_integration/scell_context_string_echo.rb "dec" "passwd" `php -f ./tests/_integration/scell_context_string_echo.php "enc" "passwd" "php->ruby with context" "somecontext"` "somecontext"
php -f ./tests/_integration/scell_context_string_echo.php "dec" "passwd" `ruby ./tests/_integration/scell_context_string_echo.rb "enc" "passwd" "ruby->php with context" "somecontext"` "somecontext"

echo ".. testing secure cell, token protect mode, php <--> ruby"
ruby ./tests/_integration/scell_token_string_echo.rb "dec" "passwd" `php -f ./tests/_integration/scell_token_string_echo.php "enc" "passwd" "php->ruby token test"`

ruby ./tests/_integration/scell_token_string_echo.rb "dec" "passwd" `php -f ./tests/_integration/scell_token_string_echo.php "enc" "passwd" "php->ruby token test with content" "somecontext"` "somecontext"

php -f ./tests/_integration/scell_token_string_echo.php "dec" "passwd" `ruby ./tests/_integration/scell_token_string_echo.rb "enc" "passwd" "ruby->php with context" "somecontext"` "somecontext"
php -f ./tests/_integration/scell_token_string_echo.php "dec" "passwd" `ruby ./tests/_integration/scell_token_string_echo.rb "enc" "passwd" "ruby->php with context" "somecontext"` "somecontext"

echo ".. testing secure cell, token protect mode, php <--> go"
go run ./tests/_integration/scell_token_string_echo.go "dec" "passwd" `php -f ./tests/_integration/scell_token_string_echo.php "enc" "passwd" "php->go token"`

go run ./tests/_integration/scell_token_string_echo.go "dec" "passwd" `php -f ./tests/_integration/scell_token_string_echo.php "enc" "passwd" "php->go token with content" "somecontext"` "somecontext"

php -f ./tests/_integration/scell_token_string_echo.php "dec" "passwd" `go run ./tests/_integration/scell_token_string_echo.go "enc" "passwd" "go->php token" "somecontext"` "somecontext"
php -f ./tests/_integration/scell_token_string_echo.php "dec" "passwd" `go run ./tests/_integration/scell_token_string_echo.go "enc" "passwd" "go->php token with context" "somecontext"` "somecontext"
