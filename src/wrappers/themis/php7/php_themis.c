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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <themis/themis_error.h>

#include "php.h"
#include "php_themis.h"
#include "php_cell.h"
#include "php_key_generator.h"
#include "php_message.h"

static zend_function_entry php_themis_functions[] = {
  ZEND_FE(phpthemis_secure_message_wrap, NULL)
  ZEND_FE(phpthemis_secure_message_unwrap, NULL)
  ZEND_FE(phpthemis_gen_rsa_key_pair, NULL)
  ZEND_FE(phpthemis_gen_ec_key_pair, NULL)
  ZEND_FE(phpthemis_scell_seal_encrypt, NULL)
  ZEND_FE(phpthemis_scell_seal_decrypt, NULL)
  ZEND_FE(phpthemis_scell_token_protect_encrypt, NULL)
  ZEND_FE(phpthemis_scell_token_protect_decrypt, NULL)
  ZEND_FE(phpthemis_scell_context_imprint_encrypt, NULL)
  ZEND_FE(phpthemis_scell_context_imprint_decrypt, NULL)
  ZEND_FE_END
};

PHP_MINIT_FUNCTION(phpthemis)
{
    return SUCCESS;
}

zend_module_entry phpthemis_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_THEMIS_EXTNAME,
    php_themis_functions,
    PHP_MINIT(phpthemis),
    NULL,
    NULL,
    NULL,
    NULL,
#if ZEND_MODULE_API_NO >= 20010901
    PHP_THEMIS_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_PHPTHEMIS
ZEND_GET_MODULE(phpthemis)
#endif


