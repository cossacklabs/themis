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
#include <themis/error.h>

#include "php.h"
#include "php_themis.h"
#include "zend_exceptions.h"
#include <themis/themis.h>

ssize_t send_callback(const uint8_t *data, size_t data_length, void *user_data){
  return -1;
}
ssize_t receive_callback(uint8_t *data, size_t data_length, void *user_data){
  return -1;
}

int get_public_key_by_id_callback(const void *id, size_t id_length, void *key_buffer, size_t key_buffer_length, void *user_data){
  //  php_error_docref(NULL TSRMLS_CC, E_NOTICE, "%s - %u", id, id_length);
  zval *function, *id_, *ret_val;
  zval* params[1]; // Creates array of parameters with 1 element allocated.
  MAKE_STD_ZVAL(function);
  MAKE_STD_ZVAL(id_);
  MAKE_STD_ZVAL(ret_val);
  ZVAL_STRING(function, "get_pub_key_by_id", 1);
  ZVAL_STRINGL(id_, id, id_length, 1);
  params[0] = id_;
  if(call_user_function(CG(function_table), NULL, function, ret_val, 1, params TSRMLS_CC) != SUCCESS){ return THEMIS_FAIL;}
  if(Z_TYPE_P(ret_val)!=IS_STRING){ return THEMIS_FAIL;}
  if(key_buffer_length<Z_STRLEN_P(ret_val)){ return THEMIS_FAIL;}
  memcpy(key_buffer, Z_STRVAL_P(ret_val), Z_STRLEN_P(ret_val));
  return THEMIS_SUCCESS;
}

zend_class_entry *themis_secure_session_ce;

zend_object_handlers themis_secure_session_object_handlers;

typedef struct themis_secure_session_object_t {
    zend_object std;
    secure_session_t *session;
} themis_secure_session_object;

void themis_secure_session_free_storage(void *object TSRMLS_DC)
{
    themis_secure_session_object *obj = (themis_secure_session_object *)object;
    secure_session_destroy(obj->session);

    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);
    efree(obj);
}

zend_object_value themis_secure_session_create_handler(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;

    themis_secure_session_object *obj = (themis_secure_session_object *)emalloc(sizeof(themis_secure_session_object));
    memset(obj, 0, sizeof(themis_secure_session_object));
    obj->std.ce = type;

    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
    object_properties_init(&(obj->std), type);

    retval.handle = zend_objects_store_put(obj, NULL,
        themis_secure_session_free_storage, NULL TSRMLS_CC);
    retval.handlers = &themis_secure_session_object_handlers;
    return retval;
}

PHP_METHOD(themis_secure_session, __construct){
  secure_session_t *session = NULL;
  zval *object = getThis();
  
  char* id;
  int id_length;
  char* private_key;
  int private_key_length;
  
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &id, &id_length, &private_key, &private_key_length) == FAILURE) {
    zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_secure_session in __construct: invalid parameters.", 0 TSRMLS_CC);
    RETURN_NULL();
  }
  secure_session_user_callbacks_t* callbacks_=pemalloc(sizeof(secure_session_user_callbacks_t), 1);
  memset(callbacks_, 0, sizeof(secure_session_user_callbacks_t));
  callbacks_->send_data=send_callback;
  callbacks_->receive_data=receive_callback;
  callbacks_->get_public_key_for_id=get_public_key_by_id_callback;
  session=secure_session_create(id, id_length, private_key, private_key_length, callbacks_);
  if(session==NULL){
    zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_secure_session in __construct secure session creation error.", 0 TSRMLS_CC);
    RETURN_NULL();
  }
  themis_secure_session_object *obj = (themis_secure_session_object *)zend_object_store_get_object(object TSRMLS_CC);
  obj->session = session;
}

PHP_METHOD(themis_secure_session, wrap){
  char* message;
  int message_length;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &message, &message_length) == FAILURE) {
    zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_secure_session in wrap: invalid parameters.", 0 TSRMLS_CC);
    RETURN_NULL();
  }
  zval *object = getThis();
  themis_secure_session_object *obj = (themis_secure_session_object *)zend_object_store_get_object(object TSRMLS_CC);
  if(obj->session == NULL){
    zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_secure_session in wrap: invalid parameters.", 0 TSRMLS_CC);
    RETURN_NULL();
  }
  size_t wrapped_message_length=0;
  if(secure_session_wrap(obj->session, message, message_length, NULL, &wrapped_message_length)!=THEMIS_BUFFER_TOO_SMALL){
    zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_secure_session in wrap: wrapped length determination failed.", 0 TSRMLS_CC);
    RETURN_NULL();
  }
  char* wrapped_message=emalloc((int)wrapped_message_length);
  if(wrapped_message==NULL){
    zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_secure_session in wrap: not enough mamory.", 0 TSRMLS_CC);
    RETURN_NULL();    
  }
  if(secure_session_wrap(obj->session, message, message_length, wrapped_message, &wrapped_message_length)!=THEMIS_SUCCESS){
    efree(wrapped_message);
    zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_secure_session in wrap: wrapping failed.", 0 TSRMLS_CC);
    RETURN_NULL();
  }
  ZVAL_STRINGL(return_value, wrapped_message, wrapped_message_length, 0);
  return;
}


PHP_METHOD(themis_secure_session, unwrap){
  char* message;
  int message_length;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &message, &message_length) == FAILURE) {
    zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_secure_session in uwrap: invalid parameters.", 0 TSRMLS_CC);
    RETURN_NULL();
  }
  zval *object = getThis();
  themis_secure_session_object *obj = (themis_secure_session_object *)zend_object_store_get_object(object TSRMLS_CC);
    if(obj->session == NULL){
    zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_secure_session in unwrap: invalid parameters.", 0 TSRMLS_CC);
    RETURN_NULL();
  }
  size_t unwrapped_message_length=0;
  themis_status_t res=secure_session_unwrap(obj->session, message, message_length, NULL, &unwrapped_message_length);
  if(res==THEMIS_SUCCESS){
    RETURN_EMPTY_STRING();
  }
  if(res!=THEMIS_BUFFER_TOO_SMALL){
    zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_secure_session in unwrap: unwrapped message length determination failed.", 0 TSRMLS_CC);
    RETURN_NULL();
  }
  char* unwrapped_message=emalloc((int)unwrapped_message_length);
  if(unwrapped_message==NULL){
    zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_secure_session in unwrap: not enough memory.", 0 TSRMLS_CC);
    RETURN_NULL();
  }
  res=secure_session_unwrap(obj->session, message, message_length, unwrapped_message, &unwrapped_message_length);
  if(res!=THEMIS_SUCCESS && res!=THEMIS_SSESSION_SEND_OUTPUT_TO_PEER){
    efree(unwrapped_message);
    zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_secure_session in unwrap: unwrapping failed.", 0 TSRMLS_CC);
    RETURN_NULL();
  }
  ZVAL_STRINGL(return_value, unwrapped_message, unwrapped_message_length, 0);
  return;
}

PHP_METHOD(themis_secure_session, connect_request){
  zval *object = getThis();
  themis_secure_session_object *obj = (themis_secure_session_object *)zend_object_store_get_object(object TSRMLS_CC);
    if(obj->session == NULL){
    zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_secure_session in connect_request: invalid parameters.", 0 TSRMLS_CC);
    RETURN_NULL();
  }
  size_t connect_request_length=0;
  themis_status_t res=secure_session_generate_connect_request(obj->session, NULL, &connect_request_length);
  if(res!=THEMIS_BUFFER_TOO_SMALL){
    zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_secure_session in connect_request: request length determination failed.", 0 TSRMLS_CC);
    RETURN_NULL();
  }
  char* connect_request=emalloc((int)connect_request_length);
  if(connect_request==NULL){
    zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_secure_session in connect_request: not enough memory.", 0 TSRMLS_CC);
    RETURN_NULL();
  }
  if(secure_session_generate_connect_request(obj->session, connect_request, &connect_request_length)!=THEMIS_SUCCESS){
    efree(connect_request);
    zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_secure_session in connect_request: request construction failed.", 0 TSRMLS_CC);
    RETURN_NULL();
  }
  ZVAL_STRINGL(return_value, connect_request, connect_request_length, 0);
  return;
}

PHP_METHOD(themis_secure_session, is_established){
  zval *object = getThis();
  themis_secure_session_object *obj = (themis_secure_session_object *)zend_object_store_get_object(object TSRMLS_CC);
    if(obj->session == NULL){
    zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_secure_session in is_established: invalid parameters.", 0 TSRMLS_CC);
    RETURN_NULL();
  }
  size_t connect_request_length=0;
  bool res=secure_session_is_established(obj->session);
  if(res){
    RETURN_TRUE;
  }
  RETURN_FALSE;
}


static zend_function_entry themis_secure_session_functions[] = {
  PHP_ME(themis_secure_session, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
  PHP_ME(themis_secure_session, unwrap, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(themis_secure_session, wrap, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(themis_secure_session, connect_request, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(themis_secure_session, is_established, NULL, ZEND_ACC_PUBLIC)
  {NULL, NULL, NULL}
};

static zend_function_entry php_themis_functions[] = {
  PHP_FE(phpthemis_secure_message_wrap, NULL)
  PHP_FE(phpthemis_secure_message_unwrap, NULL)
  PHP_FE(phpthemis_gen_rsa_key_pair, NULL)
  PHP_FE(phpthemis_gen_ec_key_pair, NULL)
  PHP_FE(phpthemis_scell_seal_encrypt, NULL)
  PHP_FE(phpthemis_scell_seal_decrypt, NULL)
  PHP_FE(phpthemis_scell_token_protect_encrypt,NULL)
  PHP_FE(phpthemis_scell_token_protect_decrypt, NULL)
  PHP_FE(phpthemis_scell_context_imprint_encrypt, NULL)
  PHP_FE(phpthemis_scell_context_imprint_decrypt, NULL)
  {NULL, NULL, NULL}
};

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

PHP_FUNCTION(phpthemis_secure_message_wrap){
  char* private_key;
  int private_key_length;
  char* public_key;
  int public_key_length;
  char* message;
  int message_length;
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
  ZVAL_STRINGL(return_value, wrapped_message, wrapped_message_length, 0);
  return;
}

PHP_FUNCTION(phpthemis_secure_message_unwrap){
  char* private_key;
  int private_key_length;
  char* public_key;
  int public_key_length;
  char* message;
  int message_length;
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
  ZVAL_STRINGL(return_value, unwrapped_message, (int)unwrapped_message_length, 0);
  return;
}

PHP_FUNCTION(phpthemis_gen_rsa_key_pair){
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
  add_assoc_stringl(return_value, "private_key", private_key, private_key_length, 0);
  add_assoc_stringl(return_value, "public_key", public_key, public_key_length, 0);
}

PHP_FUNCTION(phpthemis_gen_ec_key_pair){
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
  add_assoc_stringl(return_value, "private_key", private_key, private_key_length, 0);
  add_assoc_stringl(return_value, "public_key", public_key, public_key_length, 0);
}

PHP_FUNCTION(phpthemis_scell_seal_encrypt){
  char* key;
  int key_length;
  char* message;
  int message_length;
  char* context=NULL;
  int context_length=0;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|s", &key, &key_length, &message, &message_length, &context, &context_length) == FAILURE) {
    zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: phpthemis_scell_seal_encrypt: invalid parameters.", 0 TSRMLS_CC);
    RETURN_NULL();
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
  ZVAL_STRINGL(return_value, encrypted_message, (int)encrypted_message_length, 0);
  return;
}

PHP_FUNCTION(phpthemis_scell_seal_decrypt){
  char* key;
  int key_length;
  char* message;
  int message_length;
  char* context=NULL;
  int context_length=0;
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
    zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: phpthemis_scell_seal_decrypt: decription failed.", 0 TSRMLS_CC);
    RETURN_NULL();
  }
  ZVAL_STRINGL(return_value, decrypted_message, (int)decrypted_message_length, 0);
  return;
}

PHP_FUNCTION(phpthemis_scell_token_protect_encrypt){
  char* key;
  int key_length;
  char* message;
  int message_length;
  char* context=NULL;
  int context_length=0;
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
  add_assoc_stringl(return_value, "encrypted_message", encrypted_message, encrypted_message_length, 0);
  add_assoc_stringl(return_value, "token", additional_auth_data, additional_auth_data_length, 0);
  return;
}

PHP_FUNCTION(phpthemis_scell_token_protect_decrypt){
  char* key;
  int key_length;
  char* message;
  int message_length;
  char* additional_auth_data;
  int additional_auth_data_length;
  char* context=NULL;
  int context_length=0;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss|s", &key, &key_length, &message, &message_length, &additional_auth_data, &additional_auth_data_length, &context, &context_length) == FAILURE) {
    zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: phpthemis_scell_token_protect_decrypt: invalid parameters.", 0 TSRMLS_CC);
    RETURN_NULL();
  }
  size_t decrypted_message_length=0;
  if(themis_secure_cell_decrypt_token_protect((uint8_t*)key, key_length, (uint8_t*)context, context_length, (uint8_t*)message, message_length, (uint8_t*)additional_auth_data, additional_auth_data_length, NULL, &decrypted_message_length)!=THEMIS_BUFFER_TOO_SMALL){
    zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: phpthemis_scell_token_protect_decrypt: dectipt message length determination failed.", 0 TSRMLS_CC);
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
  ZVAL_STRINGL(return_value, decrypted_message, (int)decrypted_message_length, 0);
  return;
}

PHP_FUNCTION(phpthemis_scell_context_imprint_encrypt){
  char* key;
  int key_length;
  char* message;
  int message_length;
  char* context;
  int context_length;
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
  ZVAL_STRINGL(return_value, encrypted_message, (int)encrypted_message_length, 0);
  return;
}

PHP_FUNCTION(phpthemis_scell_context_imprint_decrypt){
  char* key;
  int key_length;
  char* message;
  int message_length;
  char* context=NULL;
  int context_length=0;
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
  ZVAL_STRINGL(return_value, decrypted_message, (int)decrypted_message_length, 0);
  return;
}