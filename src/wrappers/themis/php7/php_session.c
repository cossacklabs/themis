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

#include "php_session.h"

ssize_t send_callback(const uint8_t *data, size_t data_length, void *user_data){
    return -1;
}
ssize_t receive_callback(uint8_t *data, size_t data_length, void *user_data){
    return -1;
}

typedef struct themis_secure_session_object_t {
    secure_session_t *session;
    zend_object std;
} themis_secure_session_object;

static inline themis_secure_session_object * get_session_object(zend_object *obj) {
    return (themis_secure_session_object*)((char *)obj - XtOffsetOf(themis_secure_session_object, std));
}

#define Z_SESSION_P(zv) get_session_object(Z_OBJ_P(zv));


int get_public_key_by_id_callback(const void *id, size_t id_length, void *key_buffer, size_t key_buffer_length, void *user_data){
    // unused
    (void)(user_data);
    zval function, id_, ret_val;
    zval params[1]; // Creates array of parameters with 1 element allocated.
    ZVAL_STRING(&function, "get_pub_key_by_id");
    ZVAL_STRINGL(&id_, id, id_length);
    params[0] = id_;
    if(call_user_function(CG(function_table), NULL, &function, &ret_val, 1, params TSRMLS_CC) != SUCCESS){ return THEMIS_FAIL;}
    if(Z_TYPE_P(&ret_val)!=IS_STRING){ return THEMIS_FAIL;}
    if(key_buffer_length<Z_STRLEN_P(&ret_val)){ return THEMIS_FAIL;}
    memcpy(key_buffer, Z_STRVAL_P(&ret_val), Z_STRLEN_P(&ret_val));
    return THEMIS_SUCCESS;
}

void themis_secure_session_free_storage(void *object TSRMLS_DC)
{
    themis_secure_session_object *obj = get_session_object(object);
    secure_session_t* session = obj->session;
    secure_session_destroy(session);

    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);
    efree(object);
}

zend_object_handlers themis_secure_session_object_handlers;

zend_object* themis_secure_session_create_handler(zend_class_entry *type TSRMLS_DC)
{
    themis_secure_session_object *obj = (themis_secure_session_object *)emalloc(
            sizeof(themis_secure_session_object) + zend_object_properties_size(type));
    memset(obj, 0, sizeof(themis_secure_session_object));
    obj->std.ce = type;
    zend_object_std_init(&obj->std, type TSRMLS_CC);

    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
    themis_secure_session_object_handlers.free_obj = (zend_object_free_obj_t)themis_secure_session_free_storage;
    themis_secure_session_object_handlers.offset = (int)XtOffsetOf(themis_secure_session_object, std);
    obj->std.handlers = &themis_secure_session_object_handlers;
    return &obj->std;
}

PHP_METHOD(themis_secure_session, __construct){
    secure_session_t *session = NULL;
    char* id;
    size_t id_length;
    char* private_key;
    size_t private_key_length;

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
        pefree(callbacks_, 1);
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_secure_session in __construct secure session creation error.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    themis_secure_session_object *obj = Z_SESSION_P(getThis());
    obj->session = session;
}

PHP_METHOD(themis_secure_session, wrap){
    char* message;
    size_t message_length;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &message, &message_length) == FAILURE) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_secure_session in wrap: invalid parameters.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    themis_secure_session_object *obj = Z_SESSION_P(getThis());
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
    ZVAL_STRINGL(return_value, wrapped_message, wrapped_message_length);
    return;
}


PHP_METHOD(themis_secure_session, unwrap){
    char* message;
    size_t message_length;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &message, &message_length) == FAILURE) {
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_secure_session in uwrap: invalid parameters.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    themis_secure_session_object *obj = Z_SESSION_P(getThis());
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
    ZVAL_STRINGL(return_value, unwrapped_message, unwrapped_message_length);
    return;
}

PHP_METHOD(themis_secure_session, connect_request){
    themis_secure_session_object *obj = Z_SESSION_P(getThis());
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
    ZVAL_STRINGL(return_value, connect_request, connect_request_length);
    return;
}

PHP_METHOD(themis_secure_session, is_established){
    themis_secure_session_object *obj = Z_SESSION_P(getThis());
    if(obj->session == NULL){
        zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Error: themis_secure_session in is_established: invalid parameters.", 0 TSRMLS_CC);
        RETURN_NULL();
    }
    bool res=secure_session_is_established(obj->session);
    if(res){
        RETURN_TRUE;
    }
    RETURN_FALSE;
}