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

#ifndef THEMIS_SECURE_COMPARATOR_H
#define THEMIS_SECURE_COMPARATOR_H

#ifdef SECURE_COMPARATOR_ENABLED
#include <themis/themis.h>

#define THEMIS_SCOMPARE_MATCH 0xf0f0f0f0
#define THEMIS_SCOMPARE_NO_MATCH THEMIS_FAIL
#define THEMIS_SCOMPARE_NOT_READY 0

#ifdef __cplusplus
extern "C"{
#endif

typedef struct secure_comparator_type secure_comparator_t;

secure_comparator_t* secure_comparator_create(void);
themis_status_t secure_comparator_destroy(secure_comparator_t *comp_ctx);

themis_status_t secure_comparator_append_secret(secure_comparator_t *comp_ctx, const void *secret_data, size_t secret_data_length);

themis_status_t secure_comparator_begin_compare(secure_comparator_t *comp_ctx, void *compare_data, size_t *compare_data_length);
themis_status_t secure_comparator_proceed_compare(secure_comparator_t *comp_ctx, const void *peer_compare_data, size_t peer_compare_data_length, void *compare_data, size_t *compare_data_length);

themis_status_t secure_comparator_get_result(const secure_comparator_t *comp_ctx);

#ifdef __cplusplus
}
#endif
#endif
#endif /* THEMIS_SECURE_COMPARATOR_H */
