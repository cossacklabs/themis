#
# Copyright (c) 2015 Cossack Labs Limited
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

PHP_ARG_ENABLE(phpthemis, whether to enable themis support,[ --enable-phpthemis   Enable themis support])

if test "$PHP_PHPTHEMIS" = "yes"; then
  AC_DEFINE(HAVE_PHPTHEMIS, 1, [Whether you have themis])
  for i in /usr/local /usr; do
      if test -r $i/lib/libthemis.a && test -r $i/lib/libsoter.a; then
        THEMIS_DIR=$i
        AC_MSG_RESULT(themis found in $i)
      fi
  done
  
  if test -z "$THEMIS_DIR"; then
    AC_MSG_RESULT(not found)
    AC_MSG_ERROR(Please reinstall the libthemis distribution)
  fi
  PHP_ADD_LIBRARY_WITH_PATH(themis, $THEMIS_DIR/lib, PHPTHEMIS_SHARED_LIBADD)
  PHP_SUBST(PHPTHEMIS_SHARED_LIBADD)
  PHP_NEW_EXTENSION(phpthemis, php_themis.c, $ext_shared)
fi