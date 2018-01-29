#!/bin/bash -e

# import functions
. tests/_integration/utils.sh

#       | python | go | ruby | js | php
#python | +      | +  | +    | +  | + |
#go     | +      | +  | +    | +  | + |
#ruby   | +      | +  | +    | +  | + |
#js     | +      | +  | +    | +  | + |
#php    | +      | +  | +    | +  | + |


# NODE <-> RUBY

echo ".. testing secure cell, context imprint mode, node <--> ruby"
ruby ./tools/ruby/scell_context_string_echo.rb "dec" "passwd" `node ./tools/js/scell_context_string_echo.js "enc" "passwd" "node->ruby with context" "somecontext"` "somecontext"
check_result_zero
echo ".. testing secure cell, context imprint mode, ruby <--> node"
node ./tools/js/scell_context_string_echo.js "dec" "passwd" `ruby ./tools/ruby/scell_context_string_echo.rb "enc" "passwd" "ruby->node with context" "somecontext"` "somecontext"
check_result_zero

# PHP <--> RUBY
echo ".. testing secure cell, context imprint mode, php <--> ruby"
ruby ./tools/ruby/scell_context_string_echo.rb "dec" "passwd" `php -f ./tools/php/scell_context_string_echo.php "enc" "passwd" "php->ruby with context" "somecontext"` "somecontext"
check_result_zero
echo ".. testing secure cell, context imprint mode, ruby <--> php"
php -f ./tools/php/scell_context_string_echo.php "dec" "passwd" `ruby ./tools/ruby/scell_context_string_echo.rb "enc" "passwd" "ruby->php with context" "somecontext"` "somecontext"
check_result_zero

# PYTHON <--> RUBY
echo ".. testing secure cell, context imprint mode, python <--> ruby"
ruby ./tools/ruby/scell_context_string_echo.rb "dec" "passwd" `python ./tools/python/scell_context_string_echo.py "enc" "passwd" "python->ruby with context" "somecontext"` "somecontext"
check_result_zero
echo ".. testing secure cell, context imprint mode, ruby <--> python"
python ./tools/python/scell_context_string_echo.py "dec" "passwd" `ruby ./tools/ruby/scell_context_string_echo.rb "enc" "passwd" "ruby->python with context" "somecontext"` "somecontext"
check_result_zero

# PYTHON <--> NODE
echo ".. testing secure cell, context imprint mode, python <--> node"
node ./tools/js/scell_context_string_echo.js "dec" "passwd" `python ./tools/python/scell_context_string_echo.py "enc" "passwd" "python->node with context" "somecontext"` "somecontext"
check_result_zero
echo ".. testing secure cell, context imprint mode, node <--> python"
python ./tools/python/scell_context_string_echo.py "dec" "passwd" `node ./tools/js/scell_context_string_echo.js "enc" "passwd" "node->python with context" "somecontext"` "somecontext"
check_result_zero

# PYTHON <--> PHP
echo ".. testing secure cell, context imprint mode, python <--> php"
php -f ./tools/php/scell_context_string_echo.php "dec" "passwd" `python ./tools/python/scell_context_string_echo.py "enc" "passwd" "python->php with context" "somecontext"` "somecontext"
check_result_zero
echo ".. testing secure cell, context imprint mode, php <--> python"
python ./tools/python/scell_context_string_echo.py "dec" "passwd" `php -f ./tools/php/scell_context_string_echo.php "enc" "passwd" "php->python with context" "somecontext"` "somecontext"
check_result_zero

# PYTHON <--> GO
echo ".. testing secure cell, context imprint mode, python <--> go"
go run ./tools/go/scell_context_string_echo.go "dec" "passwd" `python ./tools/python/scell_context_string_echo.py "enc" "passwd" "python->go with context" "somecontext"` "somecontext"
check_result_zero
echo ".. testing secure cell, context imprint mode, go <--> python"
python ./tools/python/scell_context_string_echo.py "dec" "passwd" `go run ./tools/go/scell_context_string_echo.go "enc" "passwd" "go->python with context" "somecontext"` "somecontext"
check_result_zero

# NODE <--> GO
echo ".. testing secure cell, context imprint mode, node <--> go"
go run ./tools/go/scell_context_string_echo.go "dec" "passwd" `node ./tools/js/scell_context_string_echo.js "enc" "passwd" "node->go with context" "somecontext"` "somecontext"
check_result_zero
echo ".. testing secure cell, context imprint mode, go <--> node"
node ./tools/js/scell_context_string_echo.js "dec" "passwd" `go run ./tools/go/scell_context_string_echo.go "enc" "passwd" "go->node with context" "somecontext"` "somecontext"
check_result_zero

# NODE <--> GO
echo ".. testing secure cell, context imprint mode, node <--> php"
php -f ./tools/php/scell_context_string_echo.php "dec" "passwd" `node ./tools/js/scell_context_string_echo.js "enc" "passwd" "node->php with context" "somecontext"` "somecontext"
check_result_zero
echo ".. testing secure cell, context imprint mode, php <--> node"
node ./tools/js/scell_context_string_echo.js "dec" "passwd" `php -f ./tools/php/scell_context_string_echo.php "enc" "passwd" "php->node with context" "somecontext"` "somecontext"
check_result_zero

# RUBY <--> GO
echo ".. testing SECURE CELL, CONTEXT IMPRINT MODE, go <--> ruby"
ruby ./tools/ruby/scell_context_string_echo.rb "dec" "passw2" `go run ./tools/go/scell_context_string_echo.go "enc" "passw2" "go-ruby with context" "somecontext"` "somecontext"
check_result_zero
echo ".. testing SECURE CELL, CONTEXT IMPRINT MODE, ruby <--> go"
go run ./tools/go/scell_context_string_echo.go "dec" "passw4" `ruby ./tools/ruby/scell_context_string_echo.rb "enc" "passw4" "ruby-go with context" "somecontext"` "somecontext"
check_result_zero

# PHP <--> GO
echo ".. testing SECURE CELL, CONTEXT IMPRINT MODE, go <--> php"
php -f ./tools/php/scell_context_string_echo.php "dec" "passw2" `go run ./tools/go/scell_context_string_echo.go "enc" "passw2" "go-php with context" "somecontext"` "somecontext"
check_result_zero
echo ".. testing SECURE CELL, CONTEXT IMPRINT MODE, php <--> go"
go run ./tools/go/scell_context_string_echo.go "dec" "passw4" `php -f ./tools/php/scell_context_string_echo.php "enc" "passw4" "php-go with context" "somecontext"` "somecontext"
check_result_zero

exit ${status}