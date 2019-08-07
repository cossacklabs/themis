#!/bin/bash -e

php -r "copy('https://getcomposer.org/installer', 'composer-setup.php');"
php -r "echo 'Installed hash = '; echo hash_file('SHA384', 'composer-setup.php');"

php -r "if (hash_file('SHA384', 'composer-setup.php') === 'a5c698ffe4b8e849a443b120cd5ba38043260d5c4023dbf93e1558871f1f07f58274fc6f4c93bcfd858c6bd0775cd8d1') 
  { echo 'Valid signature, continue installation'; } 
else 
  { echo '\nERROR: Invalid installer signature, probably installer was updated, fix in /tests/phpthemis/composer-setup.sh'; unlink('composer-setup.php'); exit(1); } 
echo PHP_EOL;"

echo "If installation fails, means installer signature is invalid"
php composer-setup.php
RESULT=$?
php -r "unlink('composer-setup.php');"