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

#include "common.h"

#include <themis/themis.h>

#define SERVER_ID "server"
#define CLIENT_ID "client"

struct server_type
{
	secure_session_t* ctx;
	int listen_socket;
	int client_socket;
};

typedef struct server_type server_t;

/* Client credentials */
static uint8_t client_pub[] = {0x55, 0x45, 0x43, 0x32, 0x00, 0x00, 0x00, 0x2d, 0x13, 0x8b, 0xdf, 0x0c, 0x02, 0x1f, 0x09, 0x88, 0x39, 0xd9, 0x73, 0x3a, 0x84, 0x8f, 0xa8, 0x50, 0xd9, 0x2b, 0xed, 0x3d, 0x38, 0xcf, 0x1d, 0xd0, 0xce, 0xf4, 0xae, 0xdb, 0xcf, 0xaf, 0xcb, 0x6b, 0xa5, 0x4a, 0x08, 0x11, 0x21};

/* Server credentials */
static uint8_t server_priv[] = {0x52, 0x45, 0x43, 0x32, 0x00, 0x00, 0x00, 0x2d, 0x49, 0x87, 0x04, 0x6b, 0x00, 0xf2, 0x06, 0x07, 0x7d, 0xc7, 0x1c, 0x59, 0xa1, 0x8f, 0x39, 0xfc, 0x94, 0x81, 0x3f, 0x9e, 0xc5, 0xba, 0x70, 0x6f, 0x93, 0x08, 0x8d, 0xe3, 0x85, 0x82, 0x5b, 0xf8, 0x3f, 0xc6, 0x9f, 0x0b, 0xdf};

static ssize_t on_send(const uint8_t *data, size_t data_length, void *user_data)
{
	server_t *s = (server_t *)user_data;

	ssize_t bytes_sent = send(s->client_socket, data, data_length, 0);
	if (bytes_sent != (ssize_t)data_length)
	{
		printf("%d: %d\n", __LINE__, (int)bytes_sent);
	}
}

static ssize_t on_receive(uint8_t *data, size_t data_length, void *user_data)
{
	server_t *s = (server_t *)user_data;

	return recv(s->client_socket, data, data_length, 0);
}

static int on_get_pub_key(const void *id, size_t id_length, void *key_buffer, size_t key_buffer_length, void *user_data)
{
	/* Server knows only one public key - client public key. DB query may happen here */

	if (id_length == strlen(CLIENT_ID))
	{
		if (!memcmp(id, CLIENT_ID, id_length))
		{
			memcpy(key_buffer, client_pub, sizeof(client_pub));
			return 0;
		}
	}

	return -1;
}

/* This is server session ctx */
static server_t server = {NULL, -1, -1};

static secure_session_user_callbacks_t clb =
{
	on_send,
	on_receive,
	NULL,
	on_get_pub_key,
	&server
};

void* run_server(void *arg)
{
	struct sockaddr_un addr = {0};
	themis_status_t status;

	addr.sun_family = AF_UNIX;
	memcpy(addr.sun_path, SOCKET_NAME, strlen(SOCKET_NAME) + 1);

	server.listen_socket = socket(AF_UNIX, SOCK_STREAM, 0);
	if (-1 == server.listen_socket)
	{
		return NULL;
	}

	//unlink(SOCKET_NAME);

	if (-1 == bind(server.listen_socket, (const struct sockaddr *)&addr, sizeof(addr)))
	{
		close(server.listen_socket);
		return NULL;
	}

	if (-1 == listen(server.listen_socket, 5))
	{
		close(server.listen_socket);
		return NULL;
	}

	server.ctx = secure_session_create(SERVER_ID, strlen(SERVER_ID), server_priv, sizeof(server_priv), &clb);
	if (!server.ctx)
	{
		return NULL;
	}

	server.client_socket = accept(server.listen_socket, NULL, NULL);
	if (-1 == server.client_socket)
	{
		close(server.listen_socket);
		return NULL;
	}

	while (1)
	{
		uint8_t buffer[BUF_SIZE];
		ssize_t bytes_received = secure_session_receive(server.ctx, buffer, sizeof(buffer));
		ssize_t bytes_sent;

		if (bytes_received < 0)
		{
			/* Some error, log and continue */
			printf("server receive %d %d\n", __LINE__, (int)bytes_received);
			continue;
		}

		if (bytes_received > 0)
		{
			/* If we received "finish", then client disconnected */

			if (!memcmp(buffer, "finish", bytes_received < strlen("finish") ? bytes_received : strlen("finish")))
			{
				puts("server receive finish");
				break;
			}

			/* For other messages, just echo them back */
			bytes_sent = secure_session_send(server.ctx, buffer, bytes_received);
			if (bytes_sent < 0)
			{
				/* Some error, log and continue */
				printf("server send %d %d\n", __LINE__, (int)bytes_sent);
			}
		}
	}

	close(server.client_socket);
	close(server.listen_socket);

	return NULL;
}
