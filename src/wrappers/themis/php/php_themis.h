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

#ifndef _PHP_THEMIS_H_
#define _PHP_THEMIS_H_

#define PHP_THEMIS_VERSION "0.9.1.1"
#define PHP_THEMIS_EXTNAME "phpthemis"

PHP_FUNCTION(phpthemis_secure_message_wrap);
PHP_FUNCTION(phpthemis_secure_message_unwrap);
PHP_FUNCTION(phpthemis_gen_rsa_key_pair);
PHP_FUNCTION(phpthemis_gen_ec_key_pair);

PHP_FUNCTION(phpthemis_scell_seal_encrypt);
PHP_FUNCTION(phpthemis_scell_seal_decrypt);
PHP_FUNCTION(phpthemis_scell_token_protect_encrypt);
PHP_FUNCTION(phpthemis_scell_token_protect_decrypt);
PHP_FUNCTION(phpthemis_scell_context_imprint_encrypt);
PHP_FUNCTION(phpthemis_scell_context_imprint_decrypt);

extern zend_module_entry phpthemis_module_entry;
#define phpext_themis_ptr &phpthemis_module_entry


#endif /* _PHP_THEMIS_H_ */
