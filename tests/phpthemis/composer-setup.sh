#!/bin/bash -e

php -r "copy('https://getcomposer.org/installer', 'composer-setup.php');"
php -r "echo 'Installed hash = '; echo hash_file('SHA384', 'composer-setup.php');"

php -r "if (hash_file('SHA384', 'composer-setup.php') === '106d3d32cc30011325228b9272424c1941ad75207cb5244bee161e5f9906b0edf07ab2a733e8a1c945173eb9b1966197')
  { echo 'Valid signature, continue installation'; } 
else 
  { echo '\nERROR: Invalid installer signature, probably installer was updated, fix in /tests/phpthemis/composer-setup.sh'; unlink('composer-setup.php'); exit(1); } 
echo PHP_EOL;"

echo "If installation fails, means installer signature is invalid"
php composer-setup.php
RESULT=$?
php -r "unlink('composer-setup.php');"
