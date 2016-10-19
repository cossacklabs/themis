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

struct client_type
{
	secure_session_t* ctx;
	int sock;
};

typedef struct client_type client_t;

/* Client credentials */
static uint8_t client_priv[] = {0x52, 0x45, 0x43, 0x32, 0x00, 0x00, 0x00, 0x2d, 0x51, 0xf4, 0xaa, 0x72, 0x00, 0x9f, 0x0f, 0x09, 0xce, 0xbe, 0x09, 0x33, 0xc2, 0x5e, 0x9a, 0x05, 0x99, 0x53, 0x9d, 0xb2, 0x32, 0xa2, 0x34, 0x64, 0x7a, 0xde, 0xde, 0x83, 0x8f, 0x65, 0xa9, 0x2a, 0x14, 0x6d, 0xaa, 0x90, 0x01};

/* Server credentials */
static uint8_t server_pub[] = {0x55, 0x45, 0x43, 0x32, 0x00, 0x00, 0x00, 0x2d, 0x75, 0x58, 0x33, 0xd4, 0x02, 0x12, 0xdf, 0x1f, 0xe9, 0xea, 0x48, 0x11, 0xe1, 0xf9, 0x71, 0x8e, 0x24, 0x11, 0xcb, 0xfd, 0xc0, 0xa3, 0x6e, 0xd6, 0xac, 0x88, 0xb6, 0x44, 0xc2, 0x9a, 0x24, 0x84, 0xee, 0x50, 0x4c, 0x3e, 0xa0};

static ssize_t on_send(const uint8_t *data, size_t data_length, void *user_data)
{
	client_t *c = (client_t *)user_data;

	ssize_t bytes_sent = send(c->sock, data, data_length, 0);
	if (bytes_sent != (ssize_t)data_length)
	{
		printf("%d: %d\n", __LINE__, (int)bytes_sent);
	}
}

static ssize_t on_receive(uint8_t *data, size_t data_length, void *user_data)
{
	client_t *c = (client_t *)user_data;

	return recv(c->sock, data, data_length, 0);
}

static int on_get_pub_key(const void *id, size_t id_length, void *key_buffer, size_t key_buffer_length, void *user_data)
{
	/* Client knows only one public key - server public key. DB query may happen here */

	if (id_length == strlen(SERVER_ID))
	{
		if (!memcmp(id, SERVER_ID, id_length))
		{
			memcpy(key_buffer, server_pub, sizeof(server_pub));
			return 0;
		}
	}

	return -1;
}

/* This is client session ctx */
static client_t client = {NULL, -1};

static secure_session_user_callbacks_t clb =
{
	on_send,
	on_receive,
	NULL,
	on_get_pub_key,
	&client
};

void* run_client(void *arg)
{
	struct sockaddr_un addr = {0};
	themis_status_t status;
	int count = 10;
	ssize_t bytes_sent;

	addr.sun_family = AF_UNIX;
	memcpy(addr.sun_path, SOCKET_NAME, strlen(SOCKET_NAME) + 1);

	client.sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (-1 == client.sock)
	{
		return NULL;
	}

	//unlink(SOCKET_NAME);

	client.ctx = secure_session_create(CLIENT_ID, strlen(CLIENT_ID), client_priv, sizeof(client_priv), &clb);
	if (!client.ctx)
	{
		return NULL;
	}

	if (-1 == connect(client.sock, (const struct sockaddr *)&addr, sizeof(addr)))
	{
		return NULL;
	}

	status = secure_session_connect(client.ctx);
	if (THEMIS_SUCCESS != status)
	{
		return NULL;
	}

	while (count)
	{
		uint8_t buffer[BUF_SIZE];
		ssize_t bytes_received;
		const char *message = "This is a test message";

		bytes_sent = secure_session_send(client.ctx, message, strlen(message));
		if (bytes_sent < 0)
		{
			/* Some error, log and continue */
			printf("client send %d %d\n", __LINE__, (int)bytes_sent);
		}

		/* Wait for response */
		bytes_received = secure_session_receive(client.ctx, buffer, sizeof(buffer));
		if (bytes_received < 0)
		{
			/* Some error, log and continue */
			printf("client receive %d %d\n", __LINE__, (int)bytes_received);
			continue;
		}

		if (bytes_received > 0)
		{
			if (!memcmp(message, buffer, strlen(message)))
			{
				puts("client receive valid response");
			}
			count--;
		}
	}

	/* send "finish" message to server */
	bytes_sent = secure_session_send(client.ctx, "finish", strlen("finish"));
	if (bytes_sent < 0)
	{
		/* Some error, log and continue */
		printf("client send %d %d\n", __LINE__, (int)bytes_sent);
	}

	close(client.sock);
	return NULL;
}

