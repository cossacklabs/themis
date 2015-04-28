/**
 * @file
 *
 * (c) CossackLabs
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_themis.h"
#include <themis/themis.h>

static zend_function_entry php_themis_functions[] = {
  PHP_FE(hello_world, NULL)
  PHP_FE(phpthemis_secure_message_wrap, NULL)
  PHP_FE(phpthemis_secure_message_unwrap, NULL)
  PHP_FE(phpthemis_gen_rsa_key_pair, NULL)
  PHP_FE(phpthemis_gen_ec_key_pair, NULL)
  PHP_FE(phpthemis_secure_session_create, NULL)
  PHP_FE(phpthemis_secure_session_destroy, NULL)
  PHP_FE(phpthemis_secure_session_connect, NULL)
  PHP_FE(phpthemis_secure_session_generate_connect_request, NULL)
  PHP_FE(phpthemis_secure_session_wrap, NULL)
  PHP_FE(phpthemis_secure_session_unwrap, NULL)
  PHP_FE(phpthemis_secure_session_send, NULL)
  PHP_FE(phpthemis_secure_session_receive, NULL)
  PHP_FE(phpthemis_secure_session_save, NULL)
  PHP_FE(phpthemis_secure_session_load, NULL)
  {NULL, NULL, NULL}
};

zend_module_entry phpthemis_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_THEMIS_EXTNAME,
    php_themis_functions,
    NULL,
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

ssize_t send_callback(const uint8_t *data, size_t data_length, void *user_data){
  return -1;
}
ssize_t receive_callback(uint8_t *data, size_t data_length, void *user_data){
  return -1;
}

int get_public_key_by_id_callback(const void *id, size_t id_length, void *key_buffer, size_t key_buffer_length, void *user_data){
  zval params[1];
  ZVAL_STRINGL(&params[0], id, id_length, 1);
  zval* retval_ptr;
  
  zval function_name;
  ZVAL_STRING(&function_name, "get_pub_key_by_id", 1);

  if (zend_call_method( (zval*)(user_data), NULL, NULL, 
                     "get_pub_key_by_id", 
                     strlen("get_pub_key_by_id"), 
                     &retval_ptr, 1, params, 
                     NULL) == SUCCESS) {
    if(Z_TYPE_P(retval_ptr)==IS_STRING){
      if(key_buffer_length>=Z_STRLEN_P(retval_ptr)){
  	memcpy(key_buffer, Z_STRVAL_P(retval_ptr), Z_STRLEN_P(retval_ptr));
  	return 0;/*HERMES_SUCCESS*/
      }
    }
  }
  return -1;/*HERMES_FAIL*/
}

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


PHP_FUNCTION(phpthemis_secure_session_create){
  char* id;
  char* aa;
  int id_length;
  char* private_key;
  int private_key_length;
  zval* callbacks;
  
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s/s/o", &id, &id_length, &private_key, &private_key_length, &callbacks) == FAILURE) {
    RETURN_NULL();
  }
  secure_session_user_callbacks_t* callbacks_=malloc(sizeof(secure_session_user_callbacks_t));
  memset(callbacks_, 0, sizeof(secure_session_user_callbacks_t));
  callbacks_->send_data=send_callback;
  callbacks_->receive_data=receive_callback;
  callbacks_->get_public_key_for_id=get_public_key_by_id_callback;
  callbacks_->user_data=(void*)callbacks;
  secure_session_t* session_handle=secure_session_create(id, id_length, private_key, private_key_length, callbacks_);
  if(session_handle==NULL){
    RETURN_NULL();
  }
  zval tmp=session_handle;
  zval_copy_ctor(&temp);
  convert_to_long(&temp);
  //  ZVAL_LONG(return_value, session_handle);
  return Z_LVAL(temp);
}

PHP_FUNCTION(phpthemis_secure_session_destroy){
  secure_session_t* session;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", (long*)(&session)) == FAILURE) {
    RETURN_NULL();
  }
  themis_status_t res=secure_session_destroy(session);
  if(res!=0/*HERMES_SUCCESS*/){
    RETURN_FALSE;
  }
  RETURN_TRUE;
}

PHP_FUNCTION(phpthemis_secure_session_connect){
  RETURN_FALSE;
}

PHP_FUNCTION(phpthemis_secure_session_generate_connect_request){
  
}

PHP_FUNCTION(phpthemis_secure_session_wrap){

}

PHP_FUNCTION(phpthemis_secure_session_unwrap){

}

PHP_FUNCTION(phpthemis_secure_session_send){
  RETURN_FALSE;
}

PHP_FUNCTION(phpthemis_secure_session_receive){
  RETURN_FALSE;
}

PHP_FUNCTION(phpthemis_secure_session_save){

}

PHP_FUNCTION(phpthemis_secure_session_load){

}


