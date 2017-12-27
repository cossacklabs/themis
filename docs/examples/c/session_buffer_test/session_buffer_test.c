#include "session_buffer_test.h"

typedef struct callback_key_type {
    uint8_t *key;
    size_t key_length;
} callback_key_t;

static int
on_get_pub_key(const void *id, size_t id_length, void *key_buffer, size_t key_buffer_length, void *user_data) {
    callback_key_t *cb = (callback_key_t *) user_data;
    memcpy(key_buffer, cb->key, cb->key_length);
    return THEMIS_SUCCESS;
}

callback_key_t client_callback_data = {server_pub, sizeof(server_pub)};
callback_key_t server_callback_data = {client_pub, sizeof(client_pub)};

int main(int argc, char *argv[]) {
    secure_session_user_callbacks_t client_callback = {NULL, NULL, NULL, on_get_pub_key, &client_callback_data};
    secure_session_user_callbacks_t server_callback = {NULL, NULL, NULL, on_get_pub_key, &server_callback_data};

    secure_session_t *server_session = secure_session_create(SERVER_ID, strlen(SERVER_ID), server_priv,
                                                             sizeof(server_priv), &server_callback);
    secure_session_t *client_session = secure_session_create(CLIENT_ID, strlen(CLIENT_ID), client_priv,
                                                             sizeof(client_priv), &client_callback);

    uint8_t client_buf[2048];
    size_t client_buf_length = sizeof(client_buf);

    uint8_t server_buf[2048];
    size_t server_buf_length = sizeof(server_buf);
    if (secure_session_generate_connect_request(client_session, client_buf, &client_buf_length) != THEMIS_SUCCESS) {
        fprintf(stdout, "connection request error\n");
        return 1;
    };
    themis_status_t status;
    while (!secure_session_is_established(client_session) && !secure_session_is_established(server_session)) {
        if (!secure_session_is_established(server_session)) {
            status = secure_session_unwrap(server_session, client_buf, client_buf_length, server_buf, &server_buf_length);
            if (!(status == THEMIS_SSESSION_SEND_OUTPUT_TO_PEER || status == THEMIS_SUCCESS)) {
                fprintf(stdout, "server unwrap error\n");
                return 1;
            }
        }
        if (!secure_session_is_established(client_session)) {
            status = secure_session_unwrap(client_session, server_buf, server_buf_length, client_buf,
                                           &client_buf_length);
            if (!(status == THEMIS_SSESSION_SEND_OUTPUT_TO_PEER || status == THEMIS_SUCCESS)) {
                fprintf(stdout, "client unwrap error\n");
                return 1;
            }
        }
    }
    fprintf(stdout, "session established\n");
    char message[10];
    size_t message_size;
    for (int i = 0; i < 10; i++) {
        client_buf_length = sizeof(client_buf);
        server_buf_length = sizeof(server_buf);
        message_size = (size_t)sprintf((char*)message, "%d", i);

        if(secure_session_wrap(client_session, message, message_size, client_buf, &client_buf_length) != THEMIS_SUCCESS){
            fprintf(stdout, "client wrap error\n");
            return 1;
        };

        if(secure_session_unwrap(server_session, client_buf, client_buf_length, server_buf, &server_buf_length) != THEMIS_SUCCESS){
            fprintf(stdout, "client wrap error\n");
            return 1;
        };
        if(memcmp(server_buf, message, message_size) != 0){
            fprintf(stdout, "incorrect decrypted message\n");
        };
    }
    fprintf(stdout, "finished\n");
}
