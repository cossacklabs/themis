#!/bin/bash -e

echo -e "\n >>>> secure cell tests <<<<\\n"
./vendor/phpunit/phpunit/phpunit scell_test.php
echo -e "\n >>>> secure message tests <<<<\n"
./vendor/phpunit/phpunit/phpunit smessage_test.php
echo -e "\n >>>> secure token tests <<<<\n"
# todo:
./vendor/phpunit/phpunit/phpunit ssession_test.php
