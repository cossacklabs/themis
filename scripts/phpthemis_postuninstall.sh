#/bin/bash -e

CONFCONT="extension=phpthemis.so"
SEARCH="Scan this dir for additional .ini files => "
PHPCONFD=`php -i | grep "$SEARCH" | sed "s/$SEARCH//"`;
if [ -d $PHPCONFD ]; then
    rm $PHPCONFD/20-phpthemis.ini;
fi
