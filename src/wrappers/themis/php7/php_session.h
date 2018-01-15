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

#ifndef THEMIS_SESSION_H
#define THEMIS_SESSION_H

#include <themis/themis_error.h>

#include "php.h"
#include "zend_exceptions.h"
#include "zend_API.h"
#include <themis/themis.h>

ssize_t send_callback(const uint8_t *data, size_t data_length, void *user_data);
ssize_t receive_callback(uint8_t *data, size_t data_length, void *user_data);
int get_public_key_by_id_callback(const void *id, size_t id_length, void *key_buffer, size_t key_buffer_length, void *user_data);

void themis_secure_session_free_storage(void *object TSRMLS_DC);
zend_object* themis_secure_session_create_handler(zend_class_entry *type TSRMLS_DC);
PHP_METHOD(themis_secure_session, __construct);
PHP_METHOD(themis_secure_session, wrap);
PHP_METHOD(themis_secure_session, unwrap);
PHP_METHOD(themis_secure_session, connect_request);
PHP_METHOD(themis_secure_session, is_established);

#endif //THEMIS_SESSION_H
