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
#include "php_session.h"

static zend_function_entry php_themis_functions[] = {
  ZEND_FE(phpthemis_secure_message_wrap, NULL)
  ZEND_FE(phpthemis_secure_message_unwrap, NULL)
  ZEND_FE(phpthemis_gen_rsa_key_pair, NULL)
  ZEND_FE(phpthemis_gen_ec_key_pair, NULL)
  ZEND_FE(phpthemis_gen_sym_key, NULL)
  ZEND_FE(phpthemis_scell_seal_encrypt, NULL)
  ZEND_FE(phpthemis_scell_seal_decrypt, NULL)
  ZEND_FE(phpthemis_scell_seal_encrypt_with_passphrase, NULL)
  ZEND_FE(phpthemis_scell_seal_decrypt_with_passphrase, NULL)
  ZEND_FE(phpthemis_scell_token_protect_encrypt, NULL)
  ZEND_FE(phpthemis_scell_token_protect_decrypt, NULL)
  ZEND_FE(phpthemis_scell_context_imprint_encrypt, NULL)
  ZEND_FE(phpthemis_scell_context_imprint_decrypt, NULL)
  ZEND_FE_END
};

static zend_function_entry themis_secure_session_functions[] = {
        ZEND_ME(themis_secure_session, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
        ZEND_ME(themis_secure_session, unwrap, NULL, ZEND_ACC_PUBLIC)
        ZEND_ME(themis_secure_session, wrap, NULL, ZEND_ACC_PUBLIC)
        ZEND_ME(themis_secure_session, connect_request, NULL, ZEND_ACC_PUBLIC)
        ZEND_ME(themis_secure_session, is_established, NULL, ZEND_ACC_PUBLIC)
        ZEND_FE_END
};

zend_class_entry *themis_secure_session_ce;
PHP_MINIT_FUNCTION(phpthemis)
{
    zend_class_entry tmp_ce;
    INIT_CLASS_ENTRY(tmp_ce, "themis_secure_session", themis_secure_session_functions);
    themis_secure_session_ce = zend_register_internal_class(&tmp_ce TSRMLS_CC);
    themis_secure_session_ce->create_object = themis_secure_session_create_handler;
    memcpy(&themis_secure_session_object_handlers,
           zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    themis_secure_session_object_handlers.clone_obj = NULL;
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


