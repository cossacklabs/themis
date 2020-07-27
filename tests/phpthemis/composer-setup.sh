#!/usr/bin/env bash

set -eu

EXPECTED_SIGNATURE="$(wget -q -O - https://composer.github.io/installer.sig)"
php -r "copy('https://getcomposer.org/installer', 'composer-setup.php');"
ACTUAL_SIGNATURE="$(php -r "echo hash_file('sha384', 'composer-setup.php');")"

if [ "$EXPECTED_SIGNATURE" != "$ACTUAL_SIGNATURE" ]
then
    echo 2>&1 "Invalid installer signature"
    echo 2>&1 "expected: $EXPECTED_SIGNATURE"
    echo 2>&1 "actual:   $ACTUAL_SIGNATURE"
    php -r "unlink('composer-setup.php');"
    exit 1
fi

php composer-setup.php
RESULT=$?
php -r "unlink('composer-setup.php');"
exit $RESULT
