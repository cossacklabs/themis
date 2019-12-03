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
#include "php_key_generator.h"

ZEND_FUNCTION(phpthemis_gen_rsa_key_pair){
    size_t private_key_length;
    size_t public_key_length;
    if(themis_gen_rsa_key_pair(NULL, &private_key_length, NULL, &public_key_length)!=THEMIS_BUFFER_TOO_SMALL){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_gen_rsa_key_pair: invalid parameters.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    char* private_key=emalloc(private_key_length);
    if(private_key==NULL){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_gen_rsa_key_pair: not enough memory.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    char* public_key=emalloc(public_key_length);
    if(public_key==NULL){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_gen_rsa_key_pair: not enough memory.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    if(themis_gen_rsa_key_pair((uint8_t*)private_key, &private_key_length, (uint8_t*)public_key, &public_key_length)!=THEMIS_SUCCESS){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_gen_rsa_key_pair: generation failed.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    array_init(return_value);
    add_assoc_stringl(return_value, "private_key", private_key, private_key_length);
    add_assoc_stringl(return_value, "public_key", public_key, public_key_length);
}

ZEND_FUNCTION(phpthemis_gen_ec_key_pair){
    size_t private_key_length;
    size_t public_key_length;
    if(themis_gen_ec_key_pair(NULL, &private_key_length, NULL, &public_key_length)!=THEMIS_BUFFER_TOO_SMALL){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_gen_ec_key_pair: invalid parameters.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    char* private_key=emalloc(private_key_length);
    if(private_key==NULL){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_gen_ec_key_pair: not enough memory.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    char* public_key=emalloc(public_key_length);
    if(public_key==NULL){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_gen_ec_key_pair: not enough memory.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    if(themis_gen_ec_key_pair((uint8_t*)private_key, &private_key_length, (uint8_t*)public_key, &public_key_length)!=THEMIS_SUCCESS){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_gen_ec_key_pair: generation failed.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    array_init(return_value);
    add_assoc_stringl(return_value, "private_key", private_key, private_key_length);
    add_assoc_stringl(return_value, "public_key", public_key, public_key_length);
}

ZEND_FUNCTION(phpthemis_gen_sym_key){
    size_t key_length;
    if(themis_gen_sym_key(NULL, &key_length)!=THEMIS_BUFFER_TOO_SMALL){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_gen_sym_key: invalid parameters.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    uint8_t* key=emalloc(key_length);
    if(key==NULL){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_gen_sym_key: not enough memory.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    if(themis_gen_sym_key(key, &key_length)!=THEMIS_SUCCESS){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_gen_sym_key: generation failed.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    ZVAL_STRINGL(return_value, key, (int)key_length);
    return;
}
