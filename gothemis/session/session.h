#ifndef GOTHEMIS_SECURE_SESSION_H
#define GOTHEMIS_SECURE_SESSION_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <themis/themis_error.h>
#include <themis/secure_session.h>

struct session_with_callbacks_type
{
	secure_session_t *session;
	secure_session_user_callbacks_t callbacks;
};

size_t get_session_ctx_size(void);
struct session_with_callbacks_type* session_init(const void *id, size_t id_len, const void *key, size_t key_len);
bool session_destroy(struct session_with_callbacks_type *ctx);
bool session_connect_size(struct session_with_callbacks_type *ctx, size_t *req_len);
bool session_connect(struct session_with_callbacks_type **ctx, void *req, size_t req_len);
bool session_wrap_size(struct session_with_callbacks_type **ctx, const void *in, size_t in_len, size_t *out_len);
bool session_wrap(struct session_with_callbacks_type **ctx, const void *in, size_t in_len, void *out, size_t out_len);
int session_unwrap_size(struct session_with_callbacks_type **ctx, const void *in, size_t in_len, size_t *out_len);
int session_unwrap(struct session_with_callbacks_type **ctx, const void *in, size_t in_len, void *out, size_t out_len);

#endif /* GOTHEMIS_SECURE_SESSION_H */
