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

#include "themis/themis_test.h"

#include <assert.h>
#include <string.h>

#include "themis_secure_session_test_keys.h"

/* Fuzz parameters */
#define MAX_MESSAGE_SIZE 2048
#define MESSAGES_TO_SEND 3

/* return codes for scheduler */
#define TEST_CONTINUE 1
#define TEST_STOP_SUCCESS 0
#define TEST_STOP_ERROR (-1)

/* Test keys can be found in themis_secure_session_test_keys.h */

typedef struct client_info_type client_info_t;

struct client_info_type {
    const char* id;

    const uint8_t* priv;
    size_t priv_length;

    const uint8_t* pub;
    size_t pub_length;

    secure_session_t* session;
    secure_session_user_callbacks_t transport;
};

static client_info_t client = {
    "client",
    client_priv,
    sizeof(client_priv),
    client_pub,
    sizeof(client_pub),
    NULL,
    {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
    },
};
static client_info_t server = {
    "server",
    server_priv,
    sizeof(server_priv),
    server_pub,
    sizeof(server_pub),
    NULL,
    {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
    },
};

static client_info_t client_rsa = {
    "client",
    client_priv_rsa,
    sizeof(client_priv_rsa),
    client_pub_rsa,
    sizeof(client_pub_rsa),
    NULL,
    {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
    },
};

/* Peers will communicate using shared memory */
static uint8_t shared_mem[4096];
static size_t current_length = 0;

static int on_get_public_key(
    const void* id, size_t id_length, void* key_buffer, size_t key_buffer_length, void* user_data)
{
    client_info_t* info = user_data;
    client_info_t* peer;

    if (info == &client) {
        /* The client should request server's public key */
        peer = &server;
    } else if (info == &server) {
        /* The server should request client's public key */
        peer = &client;
    } else {
        return -1;
    }

    if (memcmp(peer->id, id, id_length) != 0) {
        return -1;
    }

    if (peer->pub_length > key_buffer_length) {
        return -1;
    }

    memcpy(key_buffer, peer->pub, peer->pub_length);
    return 0;
}

static ssize_t on_send_data(const uint8_t* data, size_t data_length, void* user_data)
{
    UNUSED(user_data);
    memcpy(shared_mem, data, data_length);
    current_length = data_length;
    return (ssize_t)data_length;
}

static ssize_t on_receive_data(uint8_t* data, size_t data_length, void* user_data)
{
    UNUSED(user_data);

    if (data_length < current_length) {
        return -1;
    }

    memcpy(data, shared_mem, current_length);

    return current_length;
}

static void on_state_changed(int event, void* user_data)
{
    UNUSED(event);
    UNUSED(user_data);
    /* TODO: implement */
}

static secure_session_user_callbacks_t transport = {on_send_data,
                                                    on_receive_data,
                                                    on_state_changed,
                                                    on_get_public_key,
                                                    NULL};

static int client_function(void)
{
    static bool connected = false;
    themis_status_t res;
    uint8_t recv_buf[2048];
    ssize_t bytes_received;
    ssize_t bytes_sent;

    /* Client is not connected yet. Initiate key agreement */
    if (!connected) {
        res = secure_session_connect((client.session));
        if (THEMIS_SUCCESS == res) {
            connected = true;
            return TEST_CONTINUE;
        }

        testsuite_fail_if(res, "secure_session_connect failed");
        return TEST_STOP_ERROR;
    }
    if (secure_session_is_established(client.session)) {
        size_t remote_id_length = 0;
        if (THEMIS_BUFFER_TOO_SMALL
            != secure_session_get_remote_id(client.session, NULL, &remote_id_length)) {
            testsuite_fail_if(true, "remote id getting failed (length_determination)");
            return TEST_STOP_ERROR;
        }
        uint8_t* remote_id = malloc(remote_id_length);
        assert(remote_id);
        if (THEMIS_SUCCESS
            != secure_session_get_remote_id(client.session, remote_id, &remote_id_length)) {
            testsuite_fail_if(true, "remote id getting failed");
            free(remote_id);
            return TEST_STOP_ERROR;
        }
        testsuite_fail_unless(remote_id_length == strlen(server.id)
                                  && 0 == memcmp(remote_id, server.id, strlen(server.id)),
                              "secure_session remote id getting");
        free(remote_id);

        static int messages_to_send = MESSAGES_TO_SEND;

        /* Connection is already established. */
        static uint8_t data_to_send[MAX_MESSAGE_SIZE];
        static size_t length_to_send;

        /* If there is anything to receive, receive it */
        if (current_length) {
            bytes_received = secure_session_receive(client.session, recv_buf, sizeof(recv_buf));
            if (bytes_received > 0) {
                /* The server should echo our previously sent data */
                testsuite_fail_unless((length_to_send == (size_t)bytes_received)
                                          && (!memcmp(recv_buf, data_to_send, bytes_received)),
                                      "secure_session message send/receive");
                messages_to_send--;

                if (!messages_to_send) {
                    return TEST_STOP_SUCCESS;
                }
            } else {
                /* We shoud receive something. */
                testsuite_fail_if(bytes_received, "secure_session_receive failed");
                return TEST_STOP_ERROR;
            }
        }

        length_to_send = rand_int(MAX_MESSAGE_SIZE);

        if (THEMIS_SUCCESS != soter_rand(data_to_send, length_to_send)) {
            testsuite_fail_if(true, "soter_rand failed");
            return TEST_STOP_ERROR;
        }

        bytes_sent = secure_session_send((client.session), data_to_send, length_to_send);
        if (bytes_sent > 0) {
            /* Check whether data was indeed encrypted (it should not be the same as in
             * data_to_send) */
            testsuite_fail_if((length_to_send == current_length)
                                  || (!memcmp(data_to_send, shared_mem, length_to_send)),
                              "secure_session client message wrap");
            return TEST_CONTINUE;
        }

        testsuite_fail_if(true, "secure_session_send failed");
        return TEST_STOP_ERROR;
    }
    /* Connection is not established. We should receive some key agreement data. */

    bytes_received = secure_session_receive((client.session), recv_buf, sizeof(recv_buf));
    /* When key agreement data is received and processed client gets 0 in return value (no data
     * for client is received) */
    if (bytes_received) {
        testsuite_fail_if(bytes_received, "secure_session_receive failed");
        return TEST_STOP_ERROR;
    }

    if (secure_session_is_established((client.session))) {
        /* Negotiation completed. Clear the shared memory. */
        current_length = 0;
    }

    return TEST_CONTINUE;
}

static int client_function_no_transport(void)
{
    static bool connected = false;
    themis_status_t res;
    uint8_t recv_buf[2048];
    ssize_t bytes_received = 0;
    uint8_t processing_buf[2048];
    size_t processing_buf_size = 0;

    /* Client is not connected yet. Initiate key agreement */
    if (!connected) {
        res = secure_session_generate_connect_request((client.session),
                                                      processing_buf,
                                                      (size_t*)(&processing_buf_size));
        if (THEMIS_BUFFER_TOO_SMALL != res) {
            testsuite_fail_if(res, "secure_session_generate_connect_request failed");
            return TEST_STOP_ERROR;
        }

        res = secure_session_generate_connect_request((client.session),
                                                      processing_buf,
                                                      (size_t*)(&processing_buf_size));
        if (THEMIS_SUCCESS == res) {
            /* This test-send function never fails, so we do not check for error here */
            on_send_data(processing_buf, processing_buf_size, NULL);
            connected = true;
            return TEST_CONTINUE;
        }

        testsuite_fail_if(res, "secure_session_generate_connect_request failed");
        return TEST_STOP_ERROR;
    }

    if (secure_session_is_established((client.session))) {
        size_t remote_id_length = 0;
        if (THEMIS_BUFFER_TOO_SMALL
            != secure_session_get_remote_id(client.session, NULL, &remote_id_length)) {
            testsuite_fail_if(true, "remote id getting failed (length_determination)");
            return TEST_STOP_ERROR;
        }
        uint8_t* remote_id = malloc(remote_id_length);
        assert(remote_id);
        if (THEMIS_SUCCESS
            != secure_session_get_remote_id(client.session, remote_id, &remote_id_length)) {
            testsuite_fail_if(true, "remote id getting failed");
            free(remote_id);
            return TEST_STOP_ERROR;
        }
        testsuite_fail_unless(remote_id_length == strlen(server.id)
                                  && 0 == memcmp(remote_id, server.id, strlen(server.id)),
                              "secure_session remote id getting");
        free(remote_id);

        static int messages_to_send = MESSAGES_TO_SEND;

        /* Connection is already established. */
        static uint8_t data_to_send[MAX_MESSAGE_SIZE];
        static size_t length_to_send;
        processing_buf_size = sizeof(processing_buf);

        /* If there is anything to receive, receive it */
        if (current_length) {
            bytes_received = on_receive_data(recv_buf, sizeof(recv_buf), NULL);
            if (bytes_received > 0) {
                res = secure_session_unwrap((client.session),
                                            recv_buf,
                                            (size_t)bytes_received,
                                            processing_buf,
                                            (size_t*)(&processing_buf_size));
                if (THEMIS_SUCCESS != res) {
                    testsuite_fail_if(res, "secure_session_unwrap failed");
                    return TEST_STOP_ERROR;
                }

                /* The server should echo our previously sent data */
                testsuite_fail_if(current_length == (size_t)processing_buf_size,
                                  "secure_session message send/receive");
                messages_to_send--;

                if (!messages_to_send) {
                    return TEST_STOP_SUCCESS;
                }
            } else {
                /* We shoud receive something. */
                testsuite_fail_if(bytes_received, "secure_session_receive failed");
                return TEST_STOP_ERROR;
            }
        }

        length_to_send = rand_int(MAX_MESSAGE_SIZE - 64);

        if (THEMIS_SUCCESS != soter_rand(data_to_send, length_to_send)) {
            testsuite_fail_if(true, "soter_rand failed");
            return TEST_STOP_ERROR;
        }

        processing_buf_size = sizeof(processing_buf);
        res = secure_session_wrap((client.session),
                                  data_to_send,
                                  length_to_send,
                                  processing_buf,
                                  (size_t*)(&processing_buf_size));
        if (THEMIS_SUCCESS != res) {
            testsuite_fail_if(res, "secure_session_wrap failed");
            return TEST_STOP_ERROR;
        }

        /* This test-send function never fails, so we do not check for error here */
        on_send_data(processing_buf, processing_buf_size, NULL);
        testsuite_fail_if(length_to_send == current_length, "secure_session client message wrap");
        return TEST_CONTINUE;
    }

    /* Connection is not established. We should receive some key agreement data. */

    bytes_received = on_receive_data(recv_buf, sizeof(recv_buf), NULL);
    if (bytes_received <= 0) {
        testsuite_fail_if(bytes_received, "secure_session_receive failed");
        return TEST_STOP_ERROR;
    }

    res = secure_session_unwrap((client.session),
                                recv_buf,
                                bytes_received,
                                processing_buf,
                                (size_t*)(&processing_buf_size));
    if (THEMIS_SUCCESS == res) {
        if (secure_session_is_established((client.session))) {
            /* Negotiation completed. Clear the shared memory. */
            current_length = 0;
            return TEST_CONTINUE;
        }

        testsuite_fail_if(true, "secure_session_unwrap failed");
        return TEST_STOP_ERROR;
    }
    if (THEMIS_BUFFER_TOO_SMALL != res) {
        testsuite_fail_if(true, "secure_session_unwrap failed");
        return TEST_STOP_ERROR;
    }

    res = secure_session_unwrap((client.session),
                                recv_buf,
                                bytes_received,
                                processing_buf,
                                (size_t*)(&processing_buf_size));
    if ((THEMIS_SSESSION_SEND_OUTPUT_TO_PEER == res) && (processing_buf_size > 0)) {
        /* This test-send function never fails, so we do not check for error here */
        on_send_data(processing_buf, processing_buf_size, NULL);
        return TEST_CONTINUE;
    }

    testsuite_fail_if(true, "secure_session_unwrap failed");
    return TEST_STOP_ERROR;
}

static void server_function(void)
{
    uint8_t recv_buf[2048];
    ssize_t bytes_received;

    if (current_length > 0) {
        bytes_received = secure_session_receive((server.session), recv_buf, sizeof(recv_buf));
    } else {
        /* Nothing to receive. Do nothing */
        return;
    }

    if (bytes_received < 0) {
        testsuite_fail_if(bytes_received, "secure_session_receive failed");
        return;
    }

    size_t remote_id_length = 0;
    if (THEMIS_BUFFER_TOO_SMALL
        != secure_session_get_remote_id(server.session, NULL, &remote_id_length)) {
        testsuite_fail_if(true, "remote id getting failed (length_determination)");
        return;
    }
    uint8_t* remote_id = malloc(remote_id_length);
    assert(remote_id);
    if (THEMIS_SUCCESS != secure_session_get_remote_id(server.session, remote_id, &remote_id_length)) {
        testsuite_fail_if(true, "remote id getting failed");
        free(remote_id);
        return;
    }
    testsuite_fail_unless(remote_id_length == strlen(client.id)
                              && 0 == memcmp(remote_id, client.id, strlen(client.id)),
                          "secure_session remote id getting");
    free(remote_id);

    if (0 == bytes_received) {
        /* This was a key agreement packet. Nothing to do */
        return;
    }

    if (bytes_received > 0) {
        /* We received some data. Echo it back to the client. */
        ssize_t bytes_sent = secure_session_send((server.session), recv_buf, (size_t)bytes_received);

        if (bytes_sent == bytes_received) {
            /* Check whether data was indeed encrypted (it should not be the same as in
             * data_to_send) */
            testsuite_fail_if(((size_t)bytes_sent == current_length)
                                  || (!memcmp(recv_buf, shared_mem, bytes_sent)),
                              "secure_session server message wrap");
        } else {
            testsuite_fail_if(true, "secure_session_send failed");
        }
    }
}

static void server_function_no_transport(void)
{
    uint8_t recv_buf[2048];
    ssize_t bytes_received;
    uint8_t processing_buf[2048];
    ssize_t processing_buf_size = sizeof(processing_buf);
    themis_status_t res;
    if (current_length > 0) {
        bytes_received = on_receive_data(recv_buf, sizeof(recv_buf), NULL);
        res = secure_session_unwrap((server.session),
                                    recv_buf,
                                    bytes_received,
                                    processing_buf,
                                    (size_t*)(&processing_buf_size));
    } else {
        /* Nothing to receive. Do nothing */
        return;
    }

    size_t remote_id_length = 0;
    if (THEMIS_BUFFER_TOO_SMALL
        != secure_session_get_remote_id(server.session, NULL, &remote_id_length)) {
        testsuite_fail_if(true, "remote id getting failed (length_determination)");
        return;
    }
    uint8_t* remote_id = malloc(remote_id_length);
    assert(remote_id);
    if (THEMIS_SUCCESS != secure_session_get_remote_id(server.session, remote_id, &remote_id_length)) {
        testsuite_fail_if(true, "remote id getting failed");
        free(remote_id);
        return;
    }
    testsuite_fail_unless(remote_id_length == strlen(client.id)
                              && 0 == memcmp(remote_id, client.id, strlen(client.id)),
                          "secure_session remote id getting");
    free(remote_id);

    if ((THEMIS_SSESSION_SEND_OUTPUT_TO_PEER == res) && (processing_buf_size > 0)) {
        /* This is key agreement data. Return response to the client. */
        on_send_data(processing_buf, processing_buf_size, NULL);
        return;
    }
    if ((THEMIS_SUCCESS == res) && (processing_buf_size > 0)) {
        ssize_t bytes_sent = 0;

        /* This is actual data. Echo it to the client. */
        if (!secure_session_is_established((server.session))) {
            /* Should not happen */
            testsuite_fail_if(true, "secure_session_unwrap failed");
            return;
        }
        memcpy(recv_buf, processing_buf, (size_t)processing_buf_size);
        res = secure_session_wrap((server.session),
                                  recv_buf,
                                  processing_buf_size,
                                  processing_buf,
                                  (size_t*)(&bytes_sent));
        if (THEMIS_BUFFER_TOO_SMALL != res) {
            testsuite_fail_if(true, "secure_session_wrap failed");
            return;
        }

        res = secure_session_wrap((server.session),
                                  recv_buf,
                                  processing_buf_size,
                                  processing_buf,
                                  (size_t*)(&bytes_sent));
        if (THEMIS_SUCCESS != res) {
            testsuite_fail_if(true, "secure_session_wrap failed");
            return;
        }

        testsuite_fail_if(processing_buf_size == bytes_sent, "secure_session server message wrap");
        on_send_data(processing_buf, bytes_sent, NULL);
        return;
    }
    testsuite_fail_if(true, "secure_session_unwrap failed");
}

static void schedule(void)
{
    int res = client_function();

    while (TEST_CONTINUE == res) {
        server_function();
        res = client_function();
    }

    testsuite_fail_if(res, "secure session: basic flow");
}

static void schedule_no_transport(void)
{
    int res = client_function_no_transport();

    while (TEST_CONTINUE == res) {
        server_function_no_transport();
        res = client_function_no_transport();
    }

    testsuite_fail_if(res, "secure session: basic flow (no transport)");
}

static void test_basic_flow(void)
{
    themis_status_t res;

    memcpy(&(client.transport), &transport, sizeof(secure_session_user_callbacks_t));
    client.transport.user_data = &client;

    memcpy(&(server.transport), &transport, sizeof(secure_session_user_callbacks_t));
    server.transport.user_data = &server;

    client.session = secure_session_create(client.id,
                                           strlen(client.id),
                                           client.priv,
                                           client.priv_length,
                                           &(client.transport));
    if (client.session == NULL) {
        testsuite_fail_if(false, "secure_session_init failed");
        return;
    }

    server.session = secure_session_create(server.id,
                                           strlen(server.id),
                                           server.priv,
                                           server.priv_length,
                                           &(server.transport));
    if (server.session == NULL) {
        testsuite_fail_if(false, "secure_session_init failed");
        secure_session_destroy((client.session));
        return;
    }

    schedule();

    res = secure_session_destroy((server.session));
    if (res) {
        testsuite_fail_if(res, "secure_session_destroy failed");
    }

    res = secure_session_destroy((client.session));
    if (res) {
        testsuite_fail_if(res, "secure_session_destroy failed");
    }
}

static void test_basic_flow_no_transport(void)
{
    themis_status_t res;

    memcpy(&(client.transport), &transport, sizeof(secure_session_user_callbacks_t));
    client.transport.user_data = &client;

    memcpy(&(server.transport), &transport, sizeof(secure_session_user_callbacks_t));
    server.transport.user_data = &server;

    client.session = secure_session_create(client.id,
                                           strlen(client.id),
                                           client.priv,
                                           client.priv_length,
                                           &(client.transport));
    if (client.session == NULL) {
        testsuite_fail_if(false, "secure_session_init failed");
        return;
    }

    server.session = secure_session_create(server.id,
                                           strlen(server.id),
                                           server.priv,
                                           server.priv_length,
                                           &(server.transport));
    if (server.session == NULL) {
        testsuite_fail_if(false, "secure_session_init failed");
        secure_session_destroy((client.session));
        return;
    }

    schedule_no_transport();

    res = secure_session_destroy((server.session));
    if (res) {
        testsuite_fail_if(res, "secure_session_destroy failed");
    }

    res = secure_session_destroy((client.session));
    if (res) {
        testsuite_fail_if(res, "secure_session_destroy failed");
    }
}

static void test_empty_peer_id(void)
{
    memcpy(&(client.transport), &transport, sizeof(secure_session_user_callbacks_t));
    client.transport.user_data = &client;

    client.session = secure_session_create(NULL, 9001, client.priv, client.priv_length, &(client.transport));

    testsuite_fail_if(client.session != NULL, "secure_session_create must fail with peer id == NULL");

    client.session =
        secure_session_create(client.id, 0, client.priv, client.priv_length, &(client.transport));

    testsuite_fail_if(client.session != NULL, "secure_session_create must fail with peer id_len == 0");
}

static void test_create_session_with_ec_pubkey(void)
{
    memcpy(&(client_rsa.transport), &transport, sizeof(secure_session_user_callbacks_t));
    client_rsa.transport.user_data = &client_rsa;

    /* Public keys are not the ones secure_session_create() expects */

    client.session = secure_session_create(client.id,
                                           strlen(client.id),
                                           client.pub,
                                           client.pub_length,
                                           &(client.transport));

    testsuite_fail_if(client.session != NULL, "secure_session_create must fail with EC pub key");
}

static void test_create_session_with_rsa_privkey(void)
{
    memcpy(&(client.transport), &transport, sizeof(secure_session_user_callbacks_t));
    client.transport.user_data = &client_rsa;

    /* RSA keys are not currently supported */

    client_rsa.session = secure_session_create(client_rsa.id,
                                               strlen(client_rsa.id),
                                               client_rsa.priv,
                                               client_rsa.priv_length,
                                               &(client_rsa.transport));

    testsuite_fail_if(client_rsa.session != NULL, "secure_session_create must fail with RSA key");
}

static void test_create_session_with_rsa_pubkey(void)
{
    memcpy(&(client.transport), &transport, sizeof(secure_session_user_callbacks_t));
    client.transport.user_data = &client_rsa;

    /* Public keys are not expected at all */

    client_rsa.session = secure_session_create(client_rsa.id,
                                               strlen(client_rsa.id),
                                               client_rsa.pub,
                                               client_rsa.pub_length,
                                               &(client_rsa.transport));

    testsuite_fail_if(client_rsa.session != NULL, "secure_session_create must fail with RSA pub key");
}

void run_secure_session_test(void)
{
    testsuite_enter_suite("secure session: basic flow");
    testsuite_run_test(test_basic_flow);

    testsuite_enter_suite("secure session: basic flow (no transport)");
    testsuite_run_test(test_basic_flow_no_transport);

    testsuite_enter_suite("secure session: invalid parameters");
    testsuite_run_test(test_empty_peer_id);
    testsuite_run_test(test_create_session_with_ec_pubkey);
    testsuite_run_test(test_create_session_with_rsa_privkey);
    testsuite_run_test(test_create_session_with_rsa_pubkey);
}
