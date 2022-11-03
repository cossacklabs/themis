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
#include "php_cell.h"
#include "zend_exceptions.h"
#include <themis/themis.h>


ZEND_FUNCTION(phpthemis_scell_seal_encrypt){
    char* key = NULL;
    size_t key_length = 0;
    char* message = NULL;
    size_t message_length = 0;
    char* context=NULL;
    size_t context_length=0;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|s", &key, &key_length, &message, &message_length, &context, &context_length) == FAILURE) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: phpthemis_scell_seal_encrypt: invalid parameters.", 0 TSRMLS_CC);
        return;
    }
    size_t encrypted_message_length=0;
    if(themis_secure_cell_encrypt_seal((uint8_t*)key, key_length, (uint8_t*)context, context_length, (uint8_t*)message, message_length, NULL, &encrypted_message_length)!=THEMIS_BUFFER_TOO_SMALL){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: phpthemis_scell_seal_encrypt: encrypted message length determination failed.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    char* encrypted_message=emalloc((int)encrypted_message_length);
    if(encrypted_message==NULL){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: phpthemis_scell_seal_encrypt: not enough memory.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    if(themis_secure_cell_encrypt_seal((uint8_t*)key, key_length, (uint8_t*)context, context_length, (uint8_t*)message, message_length, (uint8_t*)encrypted_message, &encrypted_message_length)!=THEMIS_SUCCESS){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: phpthemis_scell_seal_encrypt: encryption failed.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    ZVAL_STRINGL(return_value, encrypted_message, (int)encrypted_message_length);
    return;
}

ZEND_FUNCTION(phpthemis_scell_seal_decrypt){
    char* key;
    size_t key_length;
    char* message;
    size_t message_length;
    char* context=NULL;
    size_t context_length=0;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|s", &key, &key_length, &message, &message_length, &context, &context_length) == FAILURE) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: phpthemis_scell_seal_decrypt: invalid parameters.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    size_t decrypted_message_length=0;
    if(themis_secure_cell_decrypt_seal((uint8_t*)key, key_length, (uint8_t*)context, context_length, (uint8_t*)message, message_length, NULL, &decrypted_message_length)!=THEMIS_BUFFER_TOO_SMALL){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: phpthemis_scell_seal_decrypt: decrypted message length determination failed.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    char* decrypted_message=emalloc((int)decrypted_message_length);
    if(decrypted_message==NULL){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: phpthemis_scell_seal_decrypt: not enough memory.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    if(themis_secure_cell_decrypt_seal((uint8_t*)key, key_length, (uint8_t*)context, context_length, (uint8_t*)message, message_length, (uint8_t*)decrypted_message, &decrypted_message_length)!=THEMIS_SUCCESS){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: phpthemis_scell_seal_decrypt: decryption failed.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    ZVAL_STRINGL(return_value, decrypted_message, (int)decrypted_message_length);
    return;
}

ZEND_FUNCTION(phpthemis_scell_seal_encrypt_with_passphrase){
    char* passphrase = NULL;
    size_t passphrase_length = 0;
    char* message = NULL;
    size_t message_length = 0;
    char* context=NULL;
    size_t context_length=0;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|s", &passphrase, &passphrase_length, &message, &message_length, &context, &context_length) == FAILURE) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: phpthemis_scell_seal_encrypt_with_passphrase: invalid parameters.", 0 TSRMLS_CC);
        return;
    }
    size_t encrypted_message_length=0;
    if(themis_secure_cell_encrypt_seal_with_passphrase((uint8_t*)passphrase, passphrase_length, (uint8_t*)context, context_length, (uint8_t*)message, message_length, NULL, &encrypted_message_length)!=THEMIS_BUFFER_TOO_SMALL){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: phpthemis_scell_seal_encrypt_with_passphrase: encrypted message length determination failed.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    char* encrypted_message=emalloc((int)encrypted_message_length);
    if(encrypted_message==NULL){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: phpthemis_scell_seal_encrypt_with_passphrase: not enough memory.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    if(themis_secure_cell_encrypt_seal_with_passphrase((uint8_t*)passphrase, passphrase_length, (uint8_t*)context, context_length, (uint8_t*)message, message_length, (uint8_t*)encrypted_message, &encrypted_message_length)!=THEMIS_SUCCESS){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: phpthemis_scell_seal_encrypt_with_passphrase: encryption failed.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    ZVAL_STRINGL(return_value, encrypted_message, (int)encrypted_message_length);
    return;
}

ZEND_FUNCTION(phpthemis_scell_seal_decrypt_with_passphrase){
    char* passphrase;
    size_t passphrase_length;
    char* message;
    size_t message_length;
    char* context=NULL;
    size_t context_length=0;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|s", &passphrase, &passphrase_length, &message, &message_length, &context, &context_length) == FAILURE) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: phpthemis_scell_seal_decrypt_with_passphrase: invalid parameters.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    size_t decrypted_message_length=0;
    if(themis_secure_cell_decrypt_seal_with_passphrase((uint8_t*)passphrase, passphrase_length, (uint8_t*)context, context_length, (uint8_t*)message, message_length, NULL, &decrypted_message_length)!=THEMIS_BUFFER_TOO_SMALL){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: phpthemis_scell_seal_decrypt_with_passphrase: decrypted message length determination failed.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    char* decrypted_message=emalloc((int)decrypted_message_length);
    if(decrypted_message==NULL){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: phpthemis_scell_seal_decrypt_with_passphrase: not enough memory.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    if(themis_secure_cell_decrypt_seal_with_passphrase((uint8_t*)passphrase, passphrase_length, (uint8_t*)context, context_length, (uint8_t*)message, message_length, (uint8_t*)decrypted_message, &decrypted_message_length)!=THEMIS_SUCCESS){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: phpthemis_scell_seal_decrypt_with_passphrase: decryption failed.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    ZVAL_STRINGL(return_value, decrypted_message, (int)decrypted_message_length);
    return;
}

ZEND_FUNCTION(phpthemis_scell_token_protect_encrypt){
    //zend_string* key;
    char* key;
    size_t key_length;
    char* message;
    size_t message_length;
    char* context=NULL;
    size_t context_length=0;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|s", &key, &key_length, &message, &message_length, &context, &context_length) == FAILURE) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: phpthemis_scell_token_protect_encrypt: invalid parameters.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    size_t encrypted_message_length=0;
    size_t additional_auth_data_length=0;
    if(themis_secure_cell_encrypt_token_protect((uint8_t*)key, key_length, (uint8_t*)context, context_length, (uint8_t*)message, message_length, NULL, &additional_auth_data_length, NULL, &encrypted_message_length)!=THEMIS_BUFFER_TOO_SMALL){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: phpthemis_scell_token_protect_encrypt: encrypted message length determination failed.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    char* encrypted_message=emalloc((int)encrypted_message_length);
    if(encrypted_message==NULL){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: phpthemis_scell_token_protect_encrypt: not enough memory.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    char* additional_auth_data=emalloc((int)additional_auth_data_length);
    if(additional_auth_data==NULL){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: phpthemis_scell_token_protect_encrypt: not enough memory.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    if(themis_secure_cell_encrypt_token_protect((uint8_t*)key, key_length, (uint8_t*)context, context_length, (uint8_t*)message, message_length, (uint8_t*)additional_auth_data, &additional_auth_data_length, (uint8_t*)encrypted_message, &encrypted_message_length)!=THEMIS_SUCCESS){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: phpthemis_scell_token_protect_encrypt: encryption failed.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    array_init(return_value);
    add_assoc_stringl(return_value, "encrypted_message", encrypted_message, encrypted_message_length);
    add_assoc_stringl(return_value, "token", additional_auth_data, additional_auth_data_length);
    return;
}

ZEND_FUNCTION(phpthemis_scell_token_protect_decrypt){
    char* key;
    size_t key_length;
    char* message;
    size_t message_length;
    char* additional_auth_data;
    size_t additional_auth_data_length;
    char* context=NULL;
    size_t context_length=0;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss|s", &key, &key_length, &message, &message_length, &additional_auth_data, &additional_auth_data_length, &context, &context_length) == FAILURE) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: phpthemis_scell_token_protect_decrypt: invalid parameters.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    size_t decrypted_message_length=0;
    if(themis_secure_cell_decrypt_token_protect((uint8_t*)key, key_length, (uint8_t*)context, context_length, (uint8_t*)message, message_length, (uint8_t*)additional_auth_data, additional_auth_data_length, NULL, &decrypted_message_length)!=THEMIS_BUFFER_TOO_SMALL){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: phpthemis_scell_token_protect_decrypt: decrypt message length determination failed.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    char* decrypted_message=emalloc((int)decrypted_message_length);
    if(decrypted_message==NULL){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: phpthemis_scell_token_protect_decrypt: not enough memory.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    if(themis_secure_cell_decrypt_token_protect((uint8_t*)key, key_length, (uint8_t*)context, context_length, (uint8_t*)message, message_length, (uint8_t*)additional_auth_data, additional_auth_data_length, (uint8_t*)decrypted_message, &decrypted_message_length)!=THEMIS_SUCCESS){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: phpthemis_scell_token_protect_decrypt: decryption failed.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    ZVAL_STRINGL(return_value, decrypted_message, (int)decrypted_message_length);
    return;
}

ZEND_FUNCTION(phpthemis_scell_context_imprint_encrypt){
    char* key;
    size_t key_length;
    char* message;
    size_t message_length;
    char* context;
    size_t context_length;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss", &key, &key_length, &message, &message_length, &context, &context_length) == FAILURE) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: phpthemis_scell_context_imprint_encrypt: invalid parameters.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    size_t encrypted_message_length=0;
    if(themis_secure_cell_encrypt_context_imprint((uint8_t*)key, key_length, (uint8_t*)message, message_length, (uint8_t*)context, context_length, NULL, &encrypted_message_length)!=THEMIS_BUFFER_TOO_SMALL){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: phpthemis_scell_context_imprint_encrypt: encrypt message length determination failed.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    char* encrypted_message=emalloc((int)encrypted_message_length);
    if(encrypted_message==NULL){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: phpthemis_scell_context_imprint_encrypt: not enough memory.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    if(themis_secure_cell_encrypt_context_imprint((uint8_t*)key, key_length, (uint8_t*)message, message_length, (uint8_t*)context, context_length, (uint8_t*)encrypted_message, &encrypted_message_length)!=THEMIS_SUCCESS){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: phpthemis_scell_context_imprint_encrypt: encryption failed.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    ZVAL_STRINGL(return_value, encrypted_message, (int)encrypted_message_length);
    return;
}

ZEND_FUNCTION(phpthemis_scell_context_imprint_decrypt){
    char* key;
    size_t key_length;
    char* message;
    size_t message_length;
    char* context=NULL;
    size_t context_length=0;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss", &key, &key_length, &message, &message_length, &context, &context_length) == FAILURE) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: phpthemis_scell_context_imprint_decrypt: invalid parameters.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    size_t decrypted_message_length=0;
    if(themis_secure_cell_decrypt_context_imprint((uint8_t*)key, key_length, (uint8_t*)message, message_length, (uint8_t*)context, context_length, NULL, &decrypted_message_length)!=THEMIS_BUFFER_TOO_SMALL){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: phpthemis_scell_context_imprint_decrypt: decrypt message length determination failed.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    char* decrypted_message=emalloc((int)decrypted_message_length);
    if(decrypted_message==NULL){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: phpthemis_scell_context_imprint_decrypt: not enough memory.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    if(themis_secure_cell_decrypt_context_imprint((uint8_t*)key, key_length, (uint8_t*)message, message_length, (uint8_t*)context, context_length, (uint8_t*)decrypted_message, &decrypted_message_length)!=THEMIS_SUCCESS){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: phpthemis_scell_context_imprint_decrypt: decrypting failure.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    ZVAL_STRINGL(return_value, decrypted_message, (int)decrypted_message_length);
    return;
}
