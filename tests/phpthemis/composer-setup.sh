#!/bin/bash -e

php -r "copy('https://getcomposer.org/installer', 'composer-setup.php');"
php -r "echo 'Installed hash = '; echo hash_file('SHA384', 'composer-setup.php');"

php -r "if (hash_file('SHA384', 'composer-setup.php') === '48e3236262b34d30969dca3c37281b3b4bbe3221bda826ac6a9a62d6444cdb0dcd0615698a5cbe587c3f0fe57a54d8f5') 
  { echo 'Valid signature, continue installation'; } 
else 
  { echo 'ERROR: Invalid installer signature, probably installer was updated, fix in /tests/phpthemis/composer-setup.sh'; unlink('composer-setup.php'); exit(1); } 
echo PHP_EOL;"

echo "If installation fails, means installer signature is invalid"
php composer-setup.php
RESULT=$?
php -r "unlink('composer-setup.php');"