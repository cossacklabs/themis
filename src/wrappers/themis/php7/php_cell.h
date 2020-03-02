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
#ifndef THEMIS_PHP_CELL_H
#define THEMIS_PHP_CELL_H

ZEND_FUNCTION(phpthemis_scell_seal_encrypt);
ZEND_FUNCTION(phpthemis_scell_seal_decrypt);
ZEND_FUNCTION(phpthemis_scell_seal_encrypt_with_passphrase);
ZEND_FUNCTION(phpthemis_scell_seal_decrypt_with_passphrase);
ZEND_FUNCTION(phpthemis_scell_token_protect_encrypt);
ZEND_FUNCTION(phpthemis_scell_token_protect_decrypt);
ZEND_FUNCTION(phpthemis_scell_context_imprint_encrypt);
ZEND_FUNCTION(phpthemis_scell_context_imprint_decrypt);

#endif //THEMIS_PHP_CELL_H
