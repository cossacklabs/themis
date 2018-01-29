#!/bin/bash -e

# import functions
. tests/_integration/utils.sh

#       | python | go | ruby | js | php
#python | +      | +  | +    | +  | + |
#go     | +      | +  | +    | +  | + |
#ruby   | +      | +  | +    | +  | + |
#js     | +      | +  | +    | +  | + |
#php    | +      | +  | +    | +  | + |


# RUBY <--> NODE
echo ".. testing secure message, node <--> ruby"
ruby ./tools/ruby/smessage_encryption.rb "verify" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `node ./tools/js/smessage_encryption.js "sign" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "node -> ruby secure message"`
check_result_zero
echo ".. testing secure message, ruby <--> node"
node ./tools/js/smessage_encryption.js "verify" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `ruby ./tools/ruby/smessage_encryption.rb "sign" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "ruby -> node secure message"`
check_result_zero

# NODE <--> GO
echo ".. testing secure message, node <--> go"
go run ./tools/go/smessage_encryption.go "verify" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `node ./tools/js/smessage_encryption.js "sign" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "node -> go secure message"`
check_result_zero
echo ".. testing secure message, go <--> node"
node ./tools/js/smessage_encryption.js "verify" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `go run ./tools/go/smessage_encryption.go "sign" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "go -> node secure message"`
check_result_zero

# PHP <--> NODE
echo ".. testing secure message, node <--> php"
php -f ./tools/php/smessage_encryption.php "verify" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `node ./tools/js/smessage_encryption.js "sign" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "node -> php secure message"`
check_result_zero
echo ".. testing secure message, php <--> node"
node ./tools/js/smessage_encryption.js "verify" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `php -f ./tools/php/smessage_encryption.php "sign" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "php -> node secure message"`
check_result_zero

# PYTHON <--> NODE
echo ".. testing secure message, node <--> python"
python ./tools/python/smessage_encryption.py "verify" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `node ./tools/js/smessage_encryption.js "sign" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "node -> python secure message"`
check_result_zero
echo ".. testing secure message, python <--> node"
node ./tools/js/smessage_encryption.js "verify" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `python ./tools/python/smessage_encryption.py "sign" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "python -> node secure message"`
check_result_zero

# RUBY <--> PHP
echo ".. testing secure message, php <--> ruby"
ruby ./tools/ruby/smessage_encryption.rb "verify" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `php -f ./tools/php/smessage_encryption.php "sign" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "php -> ruby secure message"`
check_result_zero
echo ".. testing secure message, ruby <--> php"
php -f ./tools/php/smessage_encryption.php "verify" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `ruby ./tools/ruby/smessage_encryption.rb "sign" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "ruby -> php secure message"`
check_result_zero

# GO <--> PHP
echo ".. testing secure message, php <--> go"
go run ./tools/go/smessage_encryption.go "verify" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `php -f ./tools/php/smessage_encryption.php "sign" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "php -> go secure message"`
check_result_zero
echo ".. testing secure message, go <--> php"
php -f ./tools/php/smessage_encryption.php "verify" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `go run ./tools/go/smessage_encryption.go "sign" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "go -> php secure message"`
check_result_zero


# GO <--> PYTHON
echo ".. testing secure message, go <--> python"
python ./tools/python/smessage_encryption.py "verify" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `go run ./tools/go/smessage_encryption.go "sign" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "go -> python secure message"`
check_result_zero
echo ".. testing secure message, python <--> go"
go run ./tools/go/smessage_encryption.go "verify" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `python ./tools/python/smessage_encryption.py "sign" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "python -> go secure message"`
check_result_zero


# RUBY <--> PYTHON
echo ".. testing secure message, python <--> ruby"
ruby ./tools/ruby/smessage_encryption.rb "verify" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `python ./tools/python/smessage_encryption.py "sign" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "python -> ruby secure message"`
check_result_zero
echo ".. testing secure message, ruby <--> python"
python ./tools/python/smessage_encryption.py "verify" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `ruby ./tools/ruby/smessage_encryption.rb "sign" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "ruby -> python secure message"`
check_result_zero


# PYTHON <--> PHP
echo ".. testing secure message, python <--> php"
php -f ./tools/php/smessage_encryption.php "verify" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `python ./tools/python/smessage_encryption.py "sign" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "python -> php secure message"`
check_result_zero
echo ".. testing secure message, php <--> python"
python ./tools/python/smessage_encryption.py "verify" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `php -f ./tools/php/smessage_encryption.php "sign" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "php -> python secure message"`
check_result_zero

# RUBY <--> GO
echo ".. testing secure message, go <--> ruby"
ruby ./tools/ruby/smessage_encryption.rb "verify" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `go run ./tools/go/smessage_encryption.go "sign" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "go -> python secure message"`
check_result_zero
echo ".. testing secure message, ruby <--> go"
go run ./tools/go/smessage_encryption.go "verify" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `ruby ./tools/ruby/smessage_encryption.rb "sign" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "ruby -> go secure message"`
check_result_zero

exit ${status}