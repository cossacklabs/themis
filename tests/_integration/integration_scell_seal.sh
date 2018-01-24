#!/bin/bash -e

# import functions
. tests/_integration/utils.sh

#       | python | go | ruby | js | php
#python | +      | +  | +    | +  | + |
#go     | +      | +  | +    | +  | + |
#ruby   | +      | +  | +    | +  | + |
#js     | +      | +  | +    | +  | + |
#php    | +      | +  | +    | +  | + |

# GO <--> RUBY
echo ".. testing SECURE CELL, SEAL MODE, go <--> ruby"
ruby ./tests/_integration/scell_seal_string_echo.rb "dec" "passw1" `go run ./tests/_integration/scell_seal_string_echo.go "enc" "passw1" "go-ruby test"`
check_result_zero
go run ./tests/_integration/scell_seal_string_echo.go "dec" "passw3" `ruby ./tests/_integration/scell_seal_string_echo.rb "enc" "passw3" "ruby-go test"`
check_result_zero

## with context
ruby ./tests/_integration/scell_seal_string_echo.rb "dec" "passw2" `go run ./tests/_integration/scell_seal_string_echo.go "enc" "passw2" "go-ruby with context" "somecontext"` "somecontext"
check_result_zero
go run ./tests/_integration/scell_seal_string_echo.go "dec" "passw4" `ruby ./tests/_integration/scell_seal_string_echo.rb "enc" "passw4" "ruby-go with context" "somecontext"` "somecontext"
check_result_zero

# GO <--> NODE
echo ".. testing SECURE CELL, SEAL MODE, go <--> node"
node ./tests/_integration/scell_seal_string_echo.js "dec" "passw1" `go run ./tests/_integration/scell_seal_string_echo.go "enc" "passw1" "go->node test"`
check_result_zero
go run ./tests/_integration/scell_seal_string_echo.go "dec" "passw3" `node ./tests/_integration/scell_seal_string_echo.js "enc" "passw3" "node->go test"`
check_result_zero

## with context
node ./tests/_integration/scell_seal_string_echo.js "dec" "passw2" `go run ./tests/_integration/scell_seal_string_echo.go "enc" "passw2" "go->node with context" "somecontext"` "somecontext"
check_result_zero
go run ./tests/_integration/scell_seal_string_echo.go "dec" "passw4" `node ./tests/_integration/scell_seal_string_echo.js "enc" "passw4" "node->go with context" "somecontext"` "somecontext"
check_result_zero


# GO <--> PHP
echo ".. testing SECURE CELL, SEAL MODE, go <--> php"
php -f ./tests/_integration/scell_seal_string_echo.php "dec" "passw1" `go run ./tests/_integration/scell_seal_string_echo.go "enc" "passw1" "go->php test"`
check_result_zero
go run ./tests/_integration/scell_seal_string_echo.go "dec" "passw3" `php -f ./tests/_integration/scell_seal_string_echo.php "enc" "passw3" "php->go test"`
check_result_zero

## with context
php -f ./tests/_integration/scell_seal_string_echo.php "dec" "passw2" `go run ./tests/_integration/scell_seal_string_echo.go "enc" "passw2" "go->php with context" "somecontext"` "somecontext"
check_result_zero
go run ./tests/_integration/scell_seal_string_echo.go "dec" "passw4" `php -f ./tests/_integration/scell_seal_string_echo.php "enc" "passw4" "php->go with context" "somecontext"` "somecontext"
check_result_zero

# PYTHON <--> RUBY
echo ".. testing secure cell, seal mode,  python <--> ruby"

python ./tests/_integration/scell_seal_string_echo.py "dec" "passwd" `ruby ./tests/_integration/scell_seal_string_echo.rb "enc" "passwd" "ruby->python seal"`
check_result_zero
ruby ./tests/_integration/scell_seal_string_echo.rb "dec" "passwd" `python ./tests/_integration/scell_seal_string_echo.py "enc" "passwd" "python->ruby seal"`
check_result_zero

## with context
python ./tests/_integration/scell_seal_string_echo.py "dec" "passwd" `ruby ./tests/_integration/scell_seal_string_echo.rb "enc" "passwd" "ruby->python seal with context" "somecontext"` "somecontext"
check_result_zero
ruby ./tests/_integration/scell_seal_string_echo.rb "dec" "passwd" `python ./tests/_integration/scell_seal_string_echo.py "enc" "passwd" "python->ruby seal with context" "somecontext"` "somecontext"
check_result_zero

# PYTHON <--> NODE
echo ".. testing secure cell, seal mode,  python <--> node"

python ./tests/_integration/scell_seal_string_echo.py "dec" "passwd" `node ./tests/_integration/scell_seal_string_echo.js "enc" "passwd" "node->python seal"`
check_result_zero
node ./tests/_integration/scell_seal_string_echo.js "dec" "passwd" `python ./tests/_integration/scell_seal_string_echo.py "enc" "passwd" "python->node seal"`
check_result_zero

## with context
python ./tests/_integration/scell_seal_string_echo.py "dec" "passwd" `node ./tests/_integration/scell_seal_string_echo.js "enc" "passwd" "node->python seal with context" "somecontext"` "somecontext"
check_result_zero
node ./tests/_integration/scell_seal_string_echo.js "dec" "passwd" `python ./tests/_integration/scell_seal_string_echo.py "enc" "passwd" "python->node seal with context" "somecontext"` "somecontext"
check_result_zero

# PYTHON <--> PHP
echo ".. testing secure cell, seal mode,  python <--> php"

python ./tests/_integration/scell_seal_string_echo.py "dec" "passwd" `php -f ./tests/_integration/scell_seal_string_echo.php "enc" "passwd" "php->python seal"`
check_result_zero
php -f ./tests/_integration/scell_seal_string_echo.php "dec" "passwd" `python ./tests/_integration/scell_seal_string_echo.py "enc" "passwd" "python->php seal"`
check_result_zero

## with context
python ./tests/_integration/scell_seal_string_echo.py "dec" "passwd" `php -f ./tests/_integration/scell_seal_string_echo.php "enc" "passwd" "ruby->python seal with context" "somecontext"` "somecontext"
check_result_zero
php -f ./tests/_integration/scell_seal_string_echo.php "dec" "passwd" `python ./tests/_integration/scell_seal_string_echo.py "enc" "passwd" "python->ruby seal with context" "somecontext"` "somecontext"
check_result_zero

# PYTHON <--> GO
echo ".. testing secure cell, seal mode,  python <--> go"

python ./tests/_integration/scell_seal_string_echo.py "dec" "passwd" `go run ./tests/_integration/scell_seal_string_echo.go "enc" "passwd" "go->python seal"`
check_result_zero
go run ./tests/_integration/scell_seal_string_echo.go "dec" "passwd" `python ./tests/_integration/scell_seal_string_echo.py "enc" "passwd" "python->go seal"`
check_result_zero

## with context
python ./tests/_integration/scell_seal_string_echo.py "dec" "passwd" `go run ./tests/_integration/scell_seal_string_echo.go "enc" "passwd" "go->python seal with context" "somecontext"` "somecontext"
check_result_zero
go run ./tests/_integration/scell_seal_string_echo.go "dec" "passwd" `python ./tests/_integration/scell_seal_string_echo.py "enc" "passwd" "python->go seal with context" "somecontext"` "somecontext"
check_result_zero


# PHP <--> RUBY
echo ".. testing secure cell, php <--> ruby"
php -f ./tests/_integration/scell_seal_string_echo.php "dec" "passwd" `ruby ./tests/_integration/scell_seal_string_echo.rb "enc" "passwd" "ruby->php seal"`
check_result_zero
ruby ./tests/_integration/scell_seal_string_echo.rb "dec" "passwd" `php -f ./tests/_integration/scell_seal_string_echo.php "enc" "passwd" "php->ruby seal"`
check_result_zero

## with context
php -f ./tests/_integration/scell_seal_string_echo.php "dec" "passwd" `ruby ./tests/_integration/scell_seal_string_echo.rb "enc" "passwd" "ruby->php seal with context" "somecontext"` "somecontext"
check_result_zero
ruby ./tests/_integration/scell_seal_string_echo.rb "dec" "passwd" `php -f ./tests/_integration/scell_seal_string_echo.php "enc" "passwd" "php->ruby seal with context" "somecontext"` "somecontext"
check_result_zero

## NODE <--> RUBY
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


## NODE <--> PHP
echo ".. testing secure cell, seal mode,  node <--> php"

node ./tests/_integration/scell_seal_string_echo.js "dec" "passwd" `php -f ./tests/_integration/scell_seal_string_echo.php "enc" "passwd" "php->node seal"`
check_result_zero
php -f ./tests/_integration/scell_seal_string_echo.php "dec" "passwd" `node ./tests/_integration/scell_seal_string_echo.js "enc" "passwd" "node->php seal"`
check_result_zero

## with context
node ./tests/_integration/scell_seal_string_echo.js "dec" "passwd" `php -f ./tests/_integration/scell_seal_string_echo.php "enc" "passwd" "php->node seal with context" "somecontext"` "somecontext"
check_result_zero
php -f ./tests/_integration/scell_seal_string_echo.php "dec" "passwd" `node ./tests/_integration/scell_seal_string_echo.js "enc" "passwd" "node->php seal with context" "somecontext"` "somecontext"
check_result_zero


exit ${status}