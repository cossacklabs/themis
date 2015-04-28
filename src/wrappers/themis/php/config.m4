PHP_ARG_ENABLE(phpthemis, whether to enable themis support,[ --enable-phpthemis   Enable themis support])

if test "$PHP_PHPTHEMIS" = "yes"; then
  AC_DEFINE(HAVE_PHPTHEMIS, 1, [Whether you have themis])
  PHP_ADD_LIBRARY(themis, 1, PHPTHEMIS_SHARED_LIBADD)
  PHP_ADD_LIBRARY(soter, 1, PHPTHEMIS_SHARED_LIBADD)
  PHP_ADD_LIBRARY(crypto, 1, PHPTHEMIS_SHARED_LIBADD)
  PHP_NEW_EXTENSION(phpthemis, php_themis.c, $ext_shared)
fi