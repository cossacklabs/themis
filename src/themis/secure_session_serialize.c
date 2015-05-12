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

#include <themis/secure_session_utils.h>
#include <themis/secure_session_t.h>
#include <themis/error.h>

#include <soter/soter_container.h>

#include <string.h>

#include <arpa/inet.h>

#define THEMIS_SESSION_CONTEXT_TAG "TSSC"

#define SESSION_CTX_SERIZALIZED_SIZE(_CTX_) (sizeof(_CTX_->session_id) + sizeof(_CTX_->is_client) + sizeof(_CTX_->session_master_key) + sizeof(_CTX_->out_seq) + sizeof(_CTX_->in_seq))

themis_status_t secure_session_save(const secure_session_t *session_ctx, void *out, size_t *out_length)
{
	soter_container_hdr_t *hdr = out;
	uint32_t *curr;

	if ((!session_ctx) || (!out_length))
	{
		return THEMIS_INVALID_PARAMETER;
	}

	if (session_ctx->state_handler)
	{
		/* Key agreement is not complete. We cannot save session state at this stage. */
		return THEMIS_INVALID_PARAMETER;
	}

	/* | session_id | is_client | master_key | out_seq | in_seq | */

	if ((!out) || (*out_length < (sizeof(soter_container_hdr_t) + SESSION_CTX_SERIZALIZED_SIZE(session_ctx))))
	{
		*out_length = (sizeof(soter_container_hdr_t) + SESSION_CTX_SERIZALIZED_SIZE(session_ctx));
		return THEMIS_BUFFER_TOO_SMALL;
	}

	*out_length = (sizeof(soter_container_hdr_t) + SESSION_CTX_SERIZALIZED_SIZE(session_ctx));

	soter_container_set_data_size(hdr, SESSION_CTX_SERIZALIZED_SIZE(session_ctx));
	memcpy(hdr->tag, THEMIS_SESSION_CONTEXT_TAG, SOTER_CONTAINER_TAG_LENGTH);

	curr = (uint32_t*)soter_container_data(hdr);

	/* session_id */
	*curr = htonl(session_ctx->session_id);
	curr++;

	/* is_client */
	*curr = htonl(session_ctx->is_client);
	curr++;

	/* master_key */
	memcpy(curr, session_ctx->session_master_key, sizeof(session_ctx->session_master_key));
	curr += sizeof(session_ctx->session_master_key) / sizeof(uint32_t);

	/* out_seq */
	*curr = htonl(session_ctx->out_seq);
	curr++;

	/* in_seq */
	*curr = htonl(session_ctx->in_seq);

	soter_update_container_checksum(hdr);

	return THEMIS_SUCCESS;
}

themis_status_t secure_session_load(secure_session_t *session_ctx, const void *in, size_t in_length, const secure_session_user_callbacks_t *user_callbacks)
{
	const soter_container_hdr_t *hdr = in;
	soter_status_t soter_res;
	themis_status_t res;
	const uint32_t *curr;

	if ((!session_ctx) || (!in))
	{
		return THEMIS_INVALID_PARAMETER;
	}

	if (in_length < sizeof(soter_container_hdr_t))
	{
		return THEMIS_INVALID_PARAMETER;
	}

	if (in_length < (sizeof(soter_container_hdr_t) + soter_container_data_size(hdr)))
	{
		return THEMIS_INVALID_PARAMETER;
	}

	soter_res = soter_verify_container_checksum(hdr);
	if (THEMIS_SUCCESS != soter_res)
	{
		return (themis_status_t)soter_res;
	}

	memset(session_ctx, 0, sizeof(secure_session_t)); //Правильно ли
	curr = (const uint32_t *)soter_container_const_data(hdr);

	session_ctx->session_id = ntohl(*curr);
	curr++;

	session_ctx->is_client = ntohl(*curr);
	curr++;

	memcpy(session_ctx->session_master_key, curr, sizeof(session_ctx->session_master_key));
	curr += sizeof(session_ctx->session_master_key) / sizeof(uint32_t);

	/* We have to derive session keys before extracting sequence numbers, because this function overwrites them */
	res = secure_session_derive_message_keys(session_ctx);
	if (THEMIS_SUCCESS != res)
	{
		return res;
	}

	session_ctx->out_seq = ntohl(*curr);
	curr++;

	session_ctx->in_seq = ntohl(*curr);

	session_ctx->user_callbacks = user_callbacks;

	return THEMIS_SUCCESS;
}
