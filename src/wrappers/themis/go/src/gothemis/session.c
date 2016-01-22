#include "session.h"

size_t get_session_ctx_size(void)
{
	return sizeof(struct session_with_callbacks_type);
}

static int on_get_public_key_for_id(const void *id, size_t id_len, void *key, size_t key_len, void *user_data)
{
	return onPublicKeyForId(user_data, id, id_len, key, key_len);
}

static void on_state_changed(int event, void *user_data)
{
	onStateChanged(user_data, event);
}

bool session_init(void *goctx, struct session_with_callbacks_type *ctx, const void *id, size_t id_len, const void *key, size_t key_len)
{
	themis_status_t themis_status;

	ctx->callbacks.get_public_key_for_id = on_get_public_key_for_id;
	ctx->callbacks.state_changed = on_state_changed;
	ctx->callbacks.user_data = goctx;

	themis_status = secure_session_init(&(ctx->session), id, id_len, key, key_len, &(ctx->callbacks));
	return THEMIS_SUCCESS == themis_status;
}

bool session_connect_size(struct session_with_callbacks_type *ctx, size_t *req_len)
{
	return THEMIS_BUFFER_TOO_SMALL == secure_session_generate_connect_request(&(ctx->session), NULL, req_len);
}

bool session_connect(struct session_with_callbacks_type *ctx, void *req, size_t req_len)
{
	return THEMIS_SUCCESS == secure_session_generate_connect_request(&(ctx->session), req, &req_len);
}

bool session_wrap_size(struct session_with_callbacks_type *ctx, const void *in, size_t in_len, size_t *out_len)
{
	return THEMIS_BUFFER_TOO_SMALL == secure_session_wrap(&(ctx->session), in, in_len, NULL, out_len);
}

bool session_wrap(struct session_with_callbacks_type *ctx, const void *in, size_t in_len, void *out, size_t out_len)
{
	return THEMIS_SUCCESS == secure_session_wrap(&(ctx->session), in, in_len, out, &out_len);
}

int session_unwrap_size(struct session_with_callbacks_type *ctx, const void *in, size_t in_len, size_t *out_len)
{
	return secure_session_unwrap(&(ctx->session), in, in_len, NULL, out_len);
}

int session_unwrap(struct session_with_callbacks_type *ctx, const void *in, size_t in_len, void *out, size_t out_len)
{
	return secure_session_unwrap(&(ctx->session), in, in_len, out, &out_len);
}

