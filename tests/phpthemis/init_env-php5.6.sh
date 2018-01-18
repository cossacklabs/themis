#!/bin/bash -e

rm composer.json
ln -s composer-php5.6.json composer.json
php composer.phar update
