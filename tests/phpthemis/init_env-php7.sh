#!/bin/bash -e

rm composer.json
ln -s composer-php7.json composer.json
php composer.phar update
