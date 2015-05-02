#
# Copyright (C) 2015 CossackLabs
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
  PHP_ADD_LIBRARY(themis, 1, PHPTHEMIS_SHARED_LIBADD)
  PHP_ADD_LIBRARY(soter, 1, PHPTHEMIS_SHARED_LIBADD)
  PHP_ADD_LIBRARY(crypto, 1, PHPTHEMIS_SHARED_LIBADD)
  PHP_NEW_EXTENSION(phpthemis, php_themis.c, $ext_shared)
fi