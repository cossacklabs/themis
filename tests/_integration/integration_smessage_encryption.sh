#!/bin/bash -e

# import functions
. tests/_integration/utils.sh

#       | python | go | ruby | js | php
#python | +      | +  | +    | +  | + |
#go     | +      | +  | +    | +  | + |
#ruby   | +      | +  | +    | +  | + |
#js     | +      | +  | +    | +  | + |
#php    | +      | +  | +    | +  | + |


# NODE <--> RUBY
echo ".. testing secure message,  node <--> ruby"
ruby ./tools/ruby/smessage_encryption.rb "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `node ./tools/js/smessage_encryption.js "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "node -> ruby secure message"`
check_result_zero
echo ".. testing secure message,  ruby <--> node"
node ./tools/js/smessage_encryption.js "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `ruby ./tools/ruby/smessage_encryption.rb "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "ruby -> node secure message"`
check_result_zero

# NODE <--> GO
echo ".. testing secure message,  node <--> go"
go run ./tools/go/smessage_encryption.go "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `node ./tools/js/smessage_encryption.js "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "node -> go secure message"`
check_result_zero
echo ".. testing secure message, go  <--> node"
node ./tools/js/smessage_encryption.js "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `go run ./tools/go/smessage_encryption.go "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "go -> node secure message"`
check_result_zero

# NODE <--> PHP
echo ".. testing secure message,  node <--> php"
php -f ./tools/php/smessage_encryption.php "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `node ./tools/js/smessage_encryption.js "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "node -> php secure message"`
check_result_zero
echo ".. testing secure message,  php <--> node"
node ./tools/js/smessage_encryption.js "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `php -f ./tools/php/smessage_encryption.php "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "php -> node secure message"`
check_result_zero

# NODE <--> PYTHON
echo ".. testing secure message,  node <--> python"
python ./tools/python/smessage_encryption.py "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `node ./tools/js/smessage_encryption.js "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "node -> python secure message"`
check_result_zero
echo ".. testing secure message, python  <--> node"
node ./tools/js/smessage_encryption.js "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `python ./tools/python/smessage_encryption.py "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "python -> node secure message"`
check_result_zero

# RUBY <--> PHP
echo ".. testing secure message, php <--> ruby"
ruby ./tools/ruby/smessage_encryption.rb "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `php -f ./tools/php/smessage_encryption.php "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "php -> ruby secure message"`
check_result_zero
echo ".. testing secure message, ruby <--> php"
php -f ./tools/php/smessage_encryption.php "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `ruby ./tools/ruby/smessage_encryption.rb "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "ruby -> php secure message"`
check_result_zero

# PHP <--> GO
echo ".. testing secure message, php <--> go"
go run ./tools/go/smessage_encryption.go "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `php -f ./tools/php/smessage_encryption.php "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "php -> go secure message"`
check_result_zero
echo ".. testing secure message, go <--> php"
php -f ./tools/php/smessage_encryption.php "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `go run ./tools/go/smessage_encryption.go "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "go -> php secure message"`
check_result_zero

# PYTHON <--> GO
echo ".. testing secure message, go <--> python"
python ./tools/python/smessage_encryption.py "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `go run ./tools/go/smessage_encryption.go "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "go -> python secure message"`
check_result_zero
echo ".. testing secure message, python <--> go"
go run ./tools/go/smessage_encryption.go "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `python ./tools/python/smessage_encryption.py "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "python -> go secure message"`
check_result_zero

# PYTHON <--> RUBY
echo ".. testing secure message, python <--> ruby"
ruby ./tools/ruby/smessage_encryption.rb "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `python ./tools/python/smessage_encryption.py "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "python -> ruby secure message"`
check_result_zero
echo ".. testing secure message, ruby <--> python"
python ./tools/python/smessage_encryption.py "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `ruby ./tools/ruby/smessage_encryption.rb "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "ruby -> python secure message"`
check_result_zero

# PYTHON <--> PHP
echo ".. testing secure message, python <--> php"
php -f ./tools/php/smessage_encryption.php "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `python ./tools/python/smessage_encryption.py "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "python -> php secure message"`
check_result_zero
echo ".. testing secure message, php <--> python"
python ./tools/python/smessage_encryption.py "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `php -f ./tools/php/smessage_encryption.php "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "php -> python secure message"`
check_result_zero

# GO <--> RUBY
echo ".. testing SECURE MESSAGE, go <--> ruby"
ruby ./tools/ruby/smessage_encryption.rb "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `go run ./tools/go/smessage_encryption.go "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "go -> python secure message"`
check_result_zero
echo ".. testing SECURE MESSAGE, ruby <--> go"
go run ./tools/go/smessage_encryption.go "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `ruby ./tools/ruby/smessage_encryption.rb "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "ruby secure message"`
check_result_zero


exit ${status}