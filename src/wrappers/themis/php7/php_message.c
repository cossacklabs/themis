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
#include "php_message.h"
ZEND_FUNCTION(phpthemis_secure_message_wrap){
    char* private_key;
    size_t private_key_length;
    char* public_key;
    size_t public_key_length;
    char* message;
    size_t message_length;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss", &private_key, &private_key_length, &public_key, &public_key_length, &message, &message_length) == FAILURE) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_secure_message in wrap: invalid parameters.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    if(!public_key_length){
        public_key=NULL;
    }
    size_t wrapped_message_length=0;
    if(themis_secure_message_wrap((uint8_t*)private_key, private_key_length, (uint8_t*)public_key, public_key_length, (uint8_t*)message, message_length, NULL, &wrapped_message_length)!=THEMIS_BUFFER_TOO_SMALL){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_secure_message in wrap: wrapped message length determination failed.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    char* wrapped_message=emalloc((int)wrapped_message_length);
    if(wrapped_message==NULL){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_secure_message in wrap: not enough memory.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    if(themis_secure_message_wrap((uint8_t*)private_key, private_key_length, (uint8_t*)public_key, public_key_length, (uint8_t*)message, message_length, (uint8_t*)wrapped_message, &wrapped_message_length)!=THEMIS_SUCCESS){
        efree(wrapped_message);
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_secure_message in wrap: wrapping failed.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    ZVAL_STRINGL(return_value, wrapped_message, wrapped_message_length);
    return;
}

ZEND_FUNCTION(phpthemis_secure_message_unwrap){
    char* private_key;
    size_t private_key_length;
    char* public_key;
    size_t public_key_length;
    char* message;
    size_t message_length;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss", &private_key, &private_key_length, &public_key, &public_key_length, &message, &message_length) == FAILURE) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_secure_message in unwrap: invalid parameters.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    size_t unwrapped_message_length=0;
    if(themis_secure_message_unwrap((uint8_t*)private_key, private_key_length, (uint8_t*)public_key, public_key_length, (uint8_t*)message, message_length, NULL, &unwrapped_message_length)!=THEMIS_BUFFER_TOO_SMALL){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_secure_message in unwrap: unwrapped message length determination failed.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    char* unwrapped_message=emalloc((int)unwrapped_message_length);
    if(unwrapped_message==NULL){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_secure_message in unwrap: not enough memory.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    if(themis_secure_message_unwrap((uint8_t*)private_key, private_key_length, (uint8_t*)public_key, public_key_length, (uint8_t*)message, message_length, (uint8_t*)unwrapped_message, &unwrapped_message_length)!=THEMIS_SUCCESS){
        efree(unwrapped_message);
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_secure_message in unwrap: unwrapping failed.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    ZVAL_STRINGL(return_value, unwrapped_message, (int)unwrapped_message_length);
    return;
}