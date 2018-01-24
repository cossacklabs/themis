#!/bin/bash -e

# import functions
. tests/_integration/utils.sh

#       | python | go | ruby | js | php
#python | +      | +  | +    | +  | + |
#go     | +      | +  | +    | +  | + |
#ruby   | +      | +  | +    | +  | + |
#js     | +      | +  | +    | +  | + |
#php    | +      | +  | +    | +  | + |

## NODE <--> RUBY
echo ".. testing secure cell, token protect mode, node <--> ruby"
ruby ./tests/_integration/scell_token_string_echo.rb "dec" "passwd" `node ./tests/_integration/scell_token_string_echo.js "enc" "passwd" "node->ruby token test"`
check_result_zero
node ./tests/_integration/scell_token_string_echo.js "dec" "passwd" `ruby ./tests/_integration/scell_token_string_echo.rb "enc" "passwd" "ruby->node token test"`
check_result_zero
## with context
ruby ./tests/_integration/scell_token_string_echo.rb "dec" "passwd" `node ./tests/_integration/scell_token_string_echo.js "enc" "passwd" "node->ruby token with context" "somecontext"` "somecontext"
check_result_zero
node ./tests/_integration/scell_token_string_echo.js "dec" "passwd" `ruby ./tests/_integration/scell_token_string_echo.rb "enc" "passwd" "ruby->node token with context" "somecontext"` "somecontext"
check_result_zero

## GO <--> RUBY
echo ".. testing secure cell, token protect mode, go <--> ruby"
ruby ./tests/_integration/scell_token_string_echo.rb "dec" "passwd" `go run ./tests/_integration/scell_token_string_echo.go "enc" "passwd" "go->ruby token test"`
check_result_zero
go run ./tests/_integration/scell_token_string_echo.go "dec" "passwd" `ruby ./tests/_integration/scell_token_string_echo.rb "enc" "passwd" "ruby->go token test"`
check_result_zero
## with context
ruby ./tests/_integration/scell_token_string_echo.rb "dec" "passwd" `go run ./tests/_integration/scell_token_string_echo.go "enc" "passwd" "go->ruby token with context" "somecontext"` "somecontext"
check_result_zero
go run ./tests/_integration/scell_token_string_echo.go "dec" "passwd" `ruby ./tests/_integration/scell_token_string_echo.rb "enc" "passwd" "ruby->go token with context" "somecontext"` "somecontext"
check_result_zero


## GO <--> NODE
echo ".. testing secure cell, token protect mode, node <--> go"
go run ./tests/_integration/scell_token_string_echo.go "dec" "passwd" `node ./tests/_integration/scell_token_string_echo.js "enc" "passwd" "node->go token"`
check_result_zero
node ./tests/_integration/scell_token_string_echo.js "dec" "passwd" `go run ./tests/_integration/scell_token_string_echo.go "enc" "passwd" "go->node token"`
check_result_zero
## with context
go run ./tests/_integration/scell_token_string_echo.go "dec" "passwd" `node ./tests/_integration/scell_token_string_echo.js "enc" "passwd" "node->go token with context" "somecontext"` "somecontext"
check_result_zero
node ./tests/_integration/scell_token_string_echo.js "dec" "passwd" `go run ./tests/_integration/scell_token_string_echo.go "enc" "passwd" "go->node token with context" "somecontext"` "somecontext"
check_result_zero


## NODE <--> PHP
echo ".. testing secure cell, token protect mode, node <--> php"
php -f ./tests/_integration/scell_token_string_echo.php "dec" "passwd" `node ./tests/_integration/scell_token_string_echo.js "enc" "passwd" "php->node token"`
check_result_zero
node ./tests/_integration/scell_token_string_echo.js "dec" "passwd" `php -f ./tests/_integration/scell_token_string_echo.php "enc" "passwd" "node->php token"`
check_result_zero
## with context
php -f ./tests/_integration/scell_token_string_echo.php "dec" "passwd" `node ./tests/_integration/scell_token_string_echo.js "enc" "passwd" "php->node token with context" "somecontext"` "somecontext"
check_result_zero
node ./tests/_integration/scell_token_string_echo.js "dec" "passwd" `php -f ./tests/_integration/scell_token_string_echo.php "enc" "passwd" "node->php token with context" "somecontext"` "somecontext"
check_result_zero

## NODE <--> PYTHON
echo ".. testing secure cell, token protect mode, node <--> python"
python ./tests/_integration/scell_token_string_echo.py "dec" "passwd" `node ./tests/_integration/scell_token_string_echo.js "enc" "passwd" "python->node token"`
check_result_zero
node ./tests/_integration/scell_token_string_echo.js "dec" "passwd" `python ./tests/_integration/scell_token_string_echo.py "enc" "passwd" "node->python token"`
check_result_zero
## with context
python ./tests/_integration/scell_token_string_echo.py "dec" "passwd" `node ./tests/_integration/scell_token_string_echo.js "enc" "passwd" "python->node token with context" "somecontext"` "somecontext"
check_result_zero
node ./tests/_integration/scell_token_string_echo.js "dec" "passwd" `python ./tests/_integration/scell_token_string_echo.py "enc" "passwd" "node->python token with context" "somecontext"` "somecontext"
check_result_zero

## PHP <--> RUBY
echo ".. testing secure cell, token protect mode, php <--> ruby"
ruby ./tests/_integration/scell_token_string_echo.rb "dec" "passwd" `php -f ./tests/_integration/scell_token_string_echo.php "enc" "passwd" "php->ruby token test"`
check_result_zero
php -f ./tests/_integration/scell_token_string_echo.php "dec" "passwd" `ruby ./tests/_integration/scell_token_string_echo.rb "enc" "passwd" "ruby->php token test"`
check_result_zero
## with context
ruby ./tests/_integration/scell_token_string_echo.rb "dec" "passwd" `php -f ./tests/_integration/scell_token_string_echo.php "enc" "passwd" "php->ruby token test with content" "somecontext"` "somecontext"
check_result_zero
php -f ./tests/_integration/scell_token_string_echo.php "dec" "passwd" `ruby ./tests/_integration/scell_token_string_echo.rb "enc" "passwd" "ruby->php token test with context" "somecontext"` "somecontext"
check_result_zero

## PHP <--> GO

echo ".. testing secure cell, token protect mode, php <--> go"
go run ./tests/_integration/scell_token_string_echo.go "dec" "passwd" `php -f ./tests/_integration/scell_token_string_echo.php "enc" "passwd" "php->go token"`
check_result_zero
php -f ./tests/_integration/scell_token_string_echo.php "dec" "passwd" `go run ./tests/_integration/scell_token_string_echo.go "enc" "passwd" "go->php token"`
check_result_zero
## with context
go run ./tests/_integration/scell_token_string_echo.go "dec" "passwd" `php -f ./tests/_integration/scell_token_string_echo.php "enc" "passwd" "php->go token with context" "somecontext"` "somecontext"
check_result_zero
php -f ./tests/_integration/scell_token_string_echo.php "dec" "passwd" `go run ./tests/_integration/scell_token_string_echo.go "enc" "passwd" "go->php token with context" "somecontext"` "somecontext"
check_result_zero

## PYTHON <--> RUBY
echo ".. testing secure cell, token protect mode, python <--> ruby"
ruby ./tests/_integration/scell_token_string_echo.rb "dec" "passwd" `python ./tests/_integration/scell_token_string_echo.py "enc" "passwd" "python->ruby token test"`
check_result_zero
python ./tests/_integration/scell_token_string_echo.py "dec" "passwd" `ruby ./tests/_integration/scell_token_string_echo.rb "enc" "passwd" "ruby->python token test"`
check_result_zero
## with context
ruby ./tests/_integration/scell_token_string_echo.rb "dec" "passwd" `python ./tests/_integration/scell_token_string_echo.py "enc" "passwd" "python->ruby with context" "somecontext"` "somecontext"
check_result_zero
python ./tests/_integration/scell_token_string_echo.py "dec" "passwd" `ruby ./tests/_integration/scell_token_string_echo.rb "enc" "passwd" "ruby->python with context" "somecontext"` "somecontext"
check_result_zero

## PYTHON <--> GO
echo ".. testing secure cell, token protect mode, python <--> go"
go run ./tests/_integration/scell_token_string_echo.go "dec" "passwd" `python ./tests/_integration/scell_token_string_echo.py "enc" "passwd" "python->go token"`
check_result_zero
python ./tests/_integration/scell_token_string_echo.py "dec" "passwd" `go run ./tests/_integration/scell_token_string_echo.go "enc" "passwd" "go->python token"`
check_result_zero
## with context
go run ./tests/_integration/scell_token_string_echo.go "dec" "passwd" `python ./tests/_integration/scell_token_string_echo.py "enc" "passwd" "python->go token with context" "somecontext"` "somecontext"
check_result_zero
python ./tests/_integration/scell_token_string_echo.py "dec" "passwd" `go run ./tests/_integration/scell_token_string_echo.go "enc" "passwd" "go->python token with context" "somecontext"` "somecontext"
check_result_zero

## PYTHON <--> PHP
echo ".. testing secure cell, token protect mode, python <--> php"
php -f ./tests/_integration/scell_token_string_echo.php "dec" "passwd" `python ./tests/_integration/scell_token_string_echo.py "enc" "passwd" "php->python token"`
check_result_zero
python ./tests/_integration/scell_token_string_echo.py "dec" "passwd" `php -f ./tests/_integration/scell_token_string_echo.php "enc" "passwd" "python->php token"`
check_result_zero
## with context
php -f ./tests/_integration/scell_token_string_echo.php "dec" "passwd" `python ./tests/_integration/scell_token_string_echo.py "enc" "passwd" "php->python token with context" "somecontext"` "somecontext"
check_result_zero
python ./tests/_integration/scell_token_string_echo.py "dec" "passwd" `php -f ./tests/_integration/scell_token_string_echo.php "enc" "passwd" "python->php token with context" "somecontext"` "somecontext"
check_result_zero


exit ${status}