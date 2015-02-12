/**
 * @file
 *
 * (c) CossackLabs
 */

#include <themis/secure_session.h>
#include <stdio.h>
#include <string.h>
#include "themis_test.h"

static uint8_t client_priv[] = {0x52, 0x45, 0x43, 0x32, 0x00, 0x00, 0x00, 0x2d, 0x51, 0xf4, 0xaa, 0x72, 0x00, 0x9f, 0x0f, 0x09, 0xce, 0xbe, 0x09, 0x33, 0xc2, 0x5e, 0x9a, 0x05, 0x99, 0x53, 0x9d, 0xb2, 0x32, 0xa2, 0x34, 0x64, 0x7a, 0xde, 0xde, 0x83, 0x8f, 0x65, 0xa9, 0x2a, 0x14, 0x6d, 0xaa, 0x90, 0x01};
static uint8_t client_pub[] = {0x55, 0x45, 0x43, 0x32, 0x00, 0x00, 0x00, 0x2d, 0x13, 0x8b, 0xdf, 0x0c, 0x02, 0x1f, 0x09, 0x88, 0x39, 0xd9, 0x73, 0x3a, 0x84, 0x8f, 0xa8, 0x50, 0xd9, 0x2b, 0xed, 0x3d, 0x38, 0xcf, 0x1d, 0xd0, 0xce, 0xf4, 0xae, 0xdb, 0xcf, 0xaf, 0xcb, 0x6b, 0xa5, 0x4a, 0x08, 0x11, 0x21};

static uint8_t server_priv[] = {0x52, 0x45, 0x43, 0x32, 0x00, 0x00, 0x00, 0x2d, 0x49, 0x87, 0x04, 0x6b, 0x00, 0xf2, 0x06, 0x07, 0x7d, 0xc7, 0x1c, 0x59, 0xa1, 0x8f, 0x39, 0xfc, 0x94, 0x81, 0x3f, 0x9e, 0xc5, 0xba, 0x70, 0x6f, 0x93, 0x08, 0x8d, 0xe3, 0x85, 0x82, 0x5b, 0xf8, 0x3f, 0xc6, 0x9f, 0x0b, 0xdf};
static uint8_t server_pub[] = {0x55, 0x45, 0x43, 0x32, 0x00, 0x00, 0x00, 0x2d, 0x75, 0x58, 0x33, 0xd4, 0x02, 0x12, 0xdf, 0x1f, 0xe9, 0xea, 0x48, 0x11, 0xe1, 0xf9, 0x71, 0x8e, 0x24, 0x11, 0xcb, 0xfd, 0xc0, 0xa3, 0x6e, 0xd6, 0xac, 0x88, 0xb6, 0x44, 0xc2, 0x9a, 0x24, 0x84, 0xee, 0x50, 0x4c, 0x3e, 0xa0};

typedef struct client_info_type client_info_t;

struct client_info_type
{
	const char *id;

	const uint8_t *priv;
	size_t priv_length;

	const uint8_t *pub;
	size_t pub_length;

	secure_session_t session;
	secure_session_user_callbacks_t transport;
};

static client_info_t clients[2] =
{
	{"client", client_priv, sizeof(client_priv), client_pub, sizeof(client_pub), {0} },
	{"server", server_priv, sizeof(server_priv), server_pub, sizeof(server_pub), {0} }
};

/* Peers will communicate using shared memory */
static uint8_t shared_mem[4096];
static size_t current_length = 0;

static int on_get_public_key(const void *id, size_t id_length, void *key_buffer, size_t key_buffer_length, void *user_data)
{
	client_info_t *info = user_data;
	int i;

	if (info == clients)
	{
		/* The client should request server's public key */
		i = 1;
	}
	else if (info == &(clients[1]))
	{
		/* The server should request client's public key */
		i = 0;
	}
	else
	{
		return -1;
	}

	if (memcmp(clients[i].id, id, id_length))
	{
		return -1;
	}

	if (clients[i].pub_length > key_buffer_length)
	{
		return -1;
	}

	memcpy(key_buffer, clients[i].pub, clients[i].pub_length);
	return 0;
}

static ssize_t on_send_data(const uint8_t *data, size_t data_length, void *user_data)
{
	memcpy(shared_mem, data, data_length);
	current_length = data_length;

	return (ssize_t)data_length;
}

static ssize_t on_receive_data(uint8_t *data, size_t data_length, void *user_data)
{
	if (data_length < current_length)
	{
		return -1;
	}

	memcpy(data, shared_mem, current_length);
	return current_length;
}

static void on_state_changed(int event, void *user_data)
{
	/* TODO: implement */
}

static secure_session_user_callbacks_t transport =
{
	on_send_data,
	on_receive_data,
	on_state_changed,
	on_get_public_key,
	NULL
};

static void test_basic_flow(void)
{
	size_t i;
	themis_status_t res;

	uint8_t dummy[128];
	size_t dummy_length = sizeof(dummy);

	memcpy(&(clients[0].transport), &transport, sizeof(secure_session_user_callbacks_t));
	clients[0].transport.user_data = &(clients[0]);

	memcpy(&(clients[1].transport), &transport, sizeof(secure_session_user_callbacks_t));
	clients[1].transport.user_data = &(clients[1]);

	for (i = 0; i < 2; i++)
	{
		res = secure_session_init(&(clients[i].session), clients[i].id, strlen(clients[i].id), clients[i].priv, clients[i].priv_length, &(clients[i].transport));
		if (res)
		{
			testsuite_fail_if(res, "secure_session_init failed");
			return;
		}
	}

	res = secure_session_connect(&(clients[0].session));
	if (res)
	{
		testsuite_fail_if(res, "secure_session_connect failed");
		return;
	}

	res = secure_session_receive(&(clients[1].session), dummy, sizeof(dummy));
	if (res)
	{
		testsuite_fail_if(res, "secure_session_receive failed");
		return;
	}

	res = secure_session_receive(&(clients[0].session), dummy, sizeof(dummy));
	if (res)
	{
		testsuite_fail_if(res, "secure_session_receive failed");
		return;
	}

	res = secure_session_receive(&(clients[1].session), dummy, sizeof(dummy));
	if (res)
	{
		testsuite_fail_if(res, "secure_session_receive failed");
		return;
	}

	res = secure_session_receive(&(clients[0].session), dummy, sizeof(dummy));
	if (res)
	{
		testsuite_fail_if(res, "secure_session_receive failed");
		return;
	}

	res = secure_session_wrap(&(clients[0].session), "abc", sizeof("abc"), dummy, &dummy_length);
	if (res)
	{
		testsuite_fail_if(res, "secure_session_wrap failed");
		return;
	}

	res = secure_session_unwrap(&(clients[1].session), dummy, dummy_length, dummy, &dummy_length);
	if (res)
	{
		testsuite_fail_if(res, "secure_session_unwrap failed");
		return;
	}

	testsuite_fail_if(memcmp("abc", dummy, dummy_length), "secure_session message send/receive");

err:

	for (i = 0; i < 2; i++)
	{
		res = secure_session_cleanup(&(clients[i].session));
		if (res)
		{
			testsuite_fail_if(res, "secure_session_cleanup failed");
		}
	}
}

/*static void print_bytes(uint8_t *bytes, size_t len)
{
	size_t i;

	for (i = 0; i < len; i++)
		printf("%02x", bytes[i]);
	puts("");
}

static void init_bytes(uint8_t *bytes, size_t len)
{
	size_t i;

	for (i = 0; i < len; i++)
		bytes[i] = i;
}

#include <themis/secure_session_utils.h>
static void test_func(void)
{
	uint8_t in[] = {'a', 'b', 'c'};
	uint8_t out[64];

	uint8_t key[32];
	uint8_t iv[16];

	memset (out, 0, sizeof(out));
	init_bytes(key, sizeof(key));
	init_bytes(iv, sizeof(iv));

	printf("%d\n", encrypt_gcm(key, sizeof(key), iv, sizeof(iv), in, sizeof(in), out, sizeof(out)));

	print_bytes(out, sizeof(out));
}*/

void run_secure_session_test(void)
{
	testsuite_enter_suite("secure session: basic flow");
	testsuite_run_test(test_basic_flow);

	//test_func();
}
