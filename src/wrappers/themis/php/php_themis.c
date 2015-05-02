/*
* Copyright (C) 2015 CossackLabs
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

#include "php.h"
#include "php_themis.h"
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
  //  php_error_docref(NULL TSRMLS_CC, E_NOTICE, "cccc");
  if(call_user_function(CG(function_table), NULL, function, ret_val, 1, params TSRMLS_CC) != SUCCESS){ return -1;/*HERMES_FAIL*/}
  //  php_error_docref(NULL TSRMLS_CC, E_NOTICE, "ffff");
  if(Z_TYPE_P(ret_val)!=IS_STRING){ return -1; /*HERMES_FAIL*/}
  if(key_buffer_length<Z_STRLEN_P(ret_val)){ return -1; /*HERMES_FAIL*/}
  memcpy(key_buffer, Z_STRVAL_P(ret_val), Z_STRLEN_P(ret_val));
  //  php_error_docref(NULL TSRMLS_CC, E_NOTICE, "gggg");
  return 0;/*HERMES_SUCCESS*/
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
    RETURN_NULL();
  }
  secure_session_user_callbacks_t* callbacks_=pemalloc(sizeof(secure_session_user_callbacks_t), 1);
  memset(callbacks_, 0, sizeof(secure_session_user_callbacks_t));
  callbacks_->send_data=send_callback;
  callbacks_->receive_data=receive_callback;
  callbacks_->get_public_key_for_id=get_public_key_by_id_callback;
  session=secure_session_create(id, id_length, private_key, private_key_length, callbacks_);
  if(session==NULL){
    RETURN_NULL();
  }
  themis_secure_session_object *obj = (themis_secure_session_object *)zend_object_store_get_object(object TSRMLS_CC);
  obj->session = session;
}

PHP_METHOD(themis_secure_session, wrap){
  char* message;
  int message_length;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &message, &message_length) == FAILURE) {
    RETURN_NULL();
  }
  zval *object = getThis();
  themis_secure_session_object *obj = (themis_secure_session_object *)zend_object_store_get_object(object TSRMLS_CC);
  if(obj->session == NULL){
    RETURN_NULL();
  }
  size_t wrapped_message_length=0;
  if(secure_session_wrap(obj->session, message, message_length, NULL, &wrapped_message_length)!=-4/*THEMIS_BUFFER_TOO_SMALL*/){
    RETURN_NULL();
  }
  char* wrapped_message=emalloc((int)wrapped_message_length);
  if(wrapped_message==NULL){
    RETURN_NULL();    
  }
  if(secure_session_wrap(obj->session, message, message_length, wrapped_message, &wrapped_message_length)!=0/*HERMES_SUCCESS*/){
    efree(wrapped_message);
    RETURN_EMPTY_STRING();
  }
  ZVAL_STRINGL(return_value, wrapped_message, wrapped_message_length, 0);
  return;
}

PHP_METHOD(themis_secure_session, unwrap){
  char* message;
  int message_length;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &message, &message_length) == FAILURE) {
    RETURN_NULL();
  }
  zval *object = getThis();
  themis_secure_session_object *obj = (themis_secure_session_object *)zend_object_store_get_object(object TSRMLS_CC);
    if(obj->session == NULL){
    RETURN_NULL();
  }
  size_t unwrapped_message_length=0;
  themis_status_t res=secure_session_unwrap(obj->session, message, message_length, NULL, &unwrapped_message_length);
  if(res!=-4/*THEMIS_BUFFER_TOO_SMALL*/){
    RETURN_NULL();
  }
  char* unwrapped_message=emalloc((int)unwrapped_message_length);
  if(unwrapped_message==NULL){
    RETURN_NULL();    
  }
  if(secure_session_unwrap(obj->session, message, message_length, unwrapped_message, &unwrapped_message_length)<0/*HERMES_SUCCESS*/){
    efree(unwrapped_message);
    RETURN_EMPTY_STRING();
  }
  ZVAL_STRINGL(return_value, unwrapped_message, unwrapped_message_length, 0);
  return;
}

static zend_function_entry themis_secure_session_functions[] = {
  PHP_ME(themis_secure_session, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
  PHP_ME(themis_secure_session, unwrap, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(themis_secure_session, wrap, NULL, ZEND_ACC_PUBLIC)
  {NULL, NULL, NULL}
};

static zend_function_entry php_themis_functions[] = {
  PHP_FE(hello_world, NULL)
  PHP_FE(phpthemis_secure_message_wrap, NULL)
  PHP_FE(phpthemis_secure_message_unwrap, NULL)
  PHP_FE(phpthemis_gen_rsa_key_pair, NULL)
  PHP_FE(phpthemis_gen_ec_key_pair, NULL)
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

PHP_FUNCTION(hello_world)
{
    RETURN_STRING("Hello World", 1);
}

PHP_FUNCTION(phpthemis_secure_message_wrap){
  char* private_key;
  int private_key_length;
  char* public_key;
  int public_key_length;
  char* message;
  int message_length;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss", &private_key, &private_key_length, &public_key, &public_key_length, &message, &message_length) == FAILURE) {
    RETURN_NULL();
  }
  size_t wrapped_message_length=0;
  if(themis_secure_message_wrap(private_key, private_key_length, public_key, public_key_length, message, message_length, NULL, &wrapped_message_length)!=-4/*HERMES_BUFFER_TOO_SMALL*/){
    RETURN_EMPTY_STRING();
  }
  char* wrapped_message=emalloc((int)wrapped_message_length);
  if(wrapped_message==NULL){
    RETURN_EMPTY_STRING();    
  }
  if(themis_secure_message_wrap(private_key, private_key_length, public_key, public_key_length, message, message_length, wrapped_message, &wrapped_message_length)!=0/*HERMES_SUCCESS*/){
    efree(wrapped_message);
    RETURN_EMPTY_STRING();
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
    RETURN_NULL();
  }
  size_t unwrapped_message_length=0;
  if(themis_secure_message_unwrap(private_key, private_key_length, public_key, public_key_length, message, message_length, NULL, &unwrapped_message_length)!=-4/*HERMES_BUFFER_TOO_SMALL*/){
    RETURN_EMPTY_STRING();
  }
  char* unwrapped_message=emalloc((int)unwrapped_message_length);
  if(unwrapped_message==NULL){
    RETURN_EMPTY_STRING();    
  }
  if(themis_secure_message_unwrap(private_key, private_key_length, public_key, public_key_length, message, message_length, unwrapped_message, &unwrapped_message_length)!=0/*HERMES_SUCCESS*/){
    efree(unwrapped_message);
    RETURN_EMPTY_STRING();
  }
  //  RETURN_STRINGL(unwrapped_message, unwrapped_message_length, 0);
  ZVAL_STRINGL(return_value, unwrapped_message, (int)unwrapped_message_length, 0);
  return;
}

PHP_FUNCTION(phpthemis_gen_rsa_key_pair){
  size_t private_key_length;
  size_t public_key_length;
  if(themis_gen_rsa_key_pair(NULL, &private_key_length, NULL, &public_key_length)!=-4/*HERMES_BUFFER_TOO_SMALL*/){
    RETURN_NULL();
  }
  char* private_key=emalloc(private_key_length);
  if(private_key==NULL){
    RETURN_NULL();
  }
  char* public_key=emalloc(public_key_length);
  if(public_key==NULL){
    RETURN_NULL();
  }
  if(themis_gen_rsa_key_pair(private_key, &private_key_length, public_key, &public_key_length)!=0/*HERMES_SUCCESS*/){
    RETURN_NULL();
  }
  array_init(return_value);
  add_assoc_stringl(return_value, "private_key", private_key, private_key_length, 0);
  add_assoc_stringl(return_value, "public_key", public_key, public_key_length, 0);
}

PHP_FUNCTION(phpthemis_gen_ec_key_pair){
  size_t private_key_length;
  size_t public_key_length;
  if(themis_gen_ec_key_pair(NULL, &private_key_length, NULL, &public_key_length)!=-4/*HERMES_BUFFER_TOO_SMALL*/){
    RETURN_NULL();
  }
  char* private_key=emalloc(private_key_length);
  if(private_key==NULL){
    RETURN_NULL();
  }
  char* public_key=emalloc(public_key_length);
  if(public_key==NULL){
    RETURN_NULL();
  }
  if(themis_gen_ec_key_pair(private_key, &private_key_length, public_key, &public_key_length)!=0/*HERMES_SUCCESS*/){
    RETURN_NULL();
  }
  array_init(return_value);
  add_assoc_stringl(return_value, "private_key", private_key, private_key_length, 0);
  add_assoc_stringl(return_value, "public_key", public_key, public_key_length, 0);
}





