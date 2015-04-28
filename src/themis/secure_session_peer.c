/**
 * @file
 *
 * (c) CossackLabs
 */

#include <themis/secure_session_peer.h>
#include <common/error.h>

#include <string.h>

void secure_session_peer_cleanup(secure_session_peer_t *peer)
{
	if (peer->id)
	{
		free(peer->id);
	}

	memset(peer, 0, sizeof(secure_session_peer_t));
}

themis_status_t secure_session_peer_init(secure_session_peer_t *peer, const void *id, size_t id_len, const void *ecdh_key, size_t ecdh_key_len, const void *sign_key, size_t sign_key_len)
{
	size_t total_len = id_len + sign_key_len;

	if (ecdh_key)
	{
		total_len += ecdh_key_len;
	}
	peer->id = malloc(total_len);
	if (NULL == peer->id)
	{
		return HERMES_NO_MEMORY;
	}

	peer->id_length = id_len;
	memcpy(peer->id, id, id_len);

	peer->sign_key = peer->id + id_len;
	peer->sign_key_length = sign_key_len;
	memcpy(peer->sign_key, sign_key, sign_key_len);

	if (ecdh_key)
	{
		peer->ecdh_key = peer->sign_key + sign_key_len;
		peer->ecdh_key_length = ecdh_key_len;
		memcpy(peer->ecdh_key, ecdh_key, ecdh_key_len);
	}

	return HERMES_SUCCESS;
}
