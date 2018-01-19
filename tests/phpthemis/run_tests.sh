#!/bin/bash -e

PHPVERSION=`php -r "echo PHP_MAJOR_VERSION;"`
if [ $PHPVERSION = 7 ]; then
    bash ./init_env-php7.sh
else
    bash ./init_env-php5.6.sh
fi

echo -e "\n >>>> secure cell tests <<<<\\n"
./vendor/phpunit/phpunit/phpunit scell_test.php
echo -e "\n >>>> secure message tests <<<<\n"
./vendor/phpunit/phpunit/phpunit smessage_test.php
echo -e "\n >>>> secure token tests <<<<\n"
# todo:
./vendor/phpunit/phpunit/phpunit ssession_test.php
