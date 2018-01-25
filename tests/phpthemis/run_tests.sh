#!/bin/bash -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR
PHPVERSION=`php -r "echo PHP_MAJOR_VERSION;"`


if [ ! -f ./composer.phar ]; then
    bash ./composer-setup.sh
fi

if [ $PHPVERSION = 7 ]; then
    bash ./init_env-php7.sh
else
    bash ./init_env-php5.6.sh
fi

echo -e "\n >>>> secure cell tests <<<<\\n"
php -c php.ini ./vendor/phpunit/phpunit/phpunit scell_test.php
echo -e "\n >>>> secure message tests <<<<\n"
php -c php.ini ./vendor/phpunit/phpunit/phpunit smessage_test.php
echo -e "\n >>>> secure token tests <<<<\n"
php -c php.ini ./vendor/phpunit/phpunit/phpunit ssession_test.php
