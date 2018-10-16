#!/bin/bash -e

php -r "copy('https://getcomposer.org/installer', 'composer-setup.php');"

php -r "if (hash_file('SHA384', 'composer-setup.php') === '93b54496392c062774670ac18b134c3b3a95e5a5e5c8f1a9f115f203b75bf9a129d5daa8ba6a13e2cc8a1da0806388a8') 
  { echo 'Valid signature, continue installation'; } 
else 
  { echo 'ERROR: Invalid installer signature, probably installer was updated'; unlink('composer-setup.php'); exit(1); } 
echo PHP_EOL;"

echo "If installation fails, means installer signature is invalid"
php composer-setup.php
RESULT=$?
php -r "unlink('composer-setup.php');"