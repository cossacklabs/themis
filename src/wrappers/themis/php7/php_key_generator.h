/*
* Copyright (c) 2015 Cossack Labs Limited
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#ifndef THEMIS_KEY_GENERATOR_H
#define THEMIS_KEY_GENERATOR_H

#include <themis/themis_error.h>

#include "php.h"
#include "zend_exceptions.h"
#include <themis/themis.h>

ZEND_FUNCTION(phpthemis_gen_rsa_key_pair);
ZEND_FUNCTION(phpthemis_gen_ec_key_pair);

ZEND_FUNCTION(phpthemis_gen_sym_key);

#endif //THEMIS_KEY_GENERATOR_H
