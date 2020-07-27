#!/usr/bin/env bash

set -eu

rm -f composer.json

PHP_MINOR_VERSION=`php -r "echo PHP_MINOR_VERSION;"`
if [ ${PHP_MINOR_VERSION} -lt 2 ]; then
    ln -s composer-php7.json composer.json
else
    ln -s composer-php7.2.json composer.json
fi
php composer.phar update
