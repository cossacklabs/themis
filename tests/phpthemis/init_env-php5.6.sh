#!/bin/bash -e

rm -f composer.json
ln -s composer-php5.6.json composer.json
php composer.phar update
