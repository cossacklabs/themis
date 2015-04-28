/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef _PHP_THEMIS_H_
#define _PHP_THEMIS_H_

#define PHP_THEMIS_VERSION "0.1.0"
#define PHP_THEMIS_EXTNAME "phpthemis"

PHP_FUNCTION(hello_world);

PHP_FUNCTION(phpthemis_secure_message_wrap);
PHP_FUNCTION(phpthemis_secure_message_unwrap);
PHP_FUNCTION(phpthemis_gen_rsa_key_pair);
PHP_FUNCTION(phpthemis_gen_ec_key_pair);

PHP_FUNCTION(phpthemis_secure_session_create);
PHP_FUNCTION(phpthemis_secure_session_destroy);
PHP_FUNCTION(phpthemis_secure_session_connect);
PHP_FUNCTION(phpthemis_secure_session_generate_connect_request);
PHP_FUNCTION(phpthemis_secure_session_wrap);
PHP_FUNCTION(phpthemis_secure_session_unwrap);
PHP_FUNCTION(phpthemis_secure_session_send);
PHP_FUNCTION(phpthemis_secure_session_receive);
PHP_FUNCTION(phpthemis_secure_session_save);
PHP_FUNCTION(phpthemis_secure_session_load);

extern zend_module_entry phpthemis_module_entry;
#define phpext_themis_ptr &phpthemis_module_entry


#endif /* _PHP_THEMIS_H_ */
