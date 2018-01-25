#!/bin/bash -e

rm -f composer.json
ln -s composer-php7.json composer.json
php composer.phar update
