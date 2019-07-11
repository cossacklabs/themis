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

#include <string.h>

#include <soter/soter_rand.h>

/* Fuzz parameters */
#define MAX_KEY_SIZE 256
#define MAX_CONTEXT_SIZE 2048
#define MAX_MESSAGE_SIZE 4096

/* Keep it under 2^31 to support 32-bit systems. */
#define CORRUPTED_LENGTH 0x5AFE

static char passwd[] = "password";
static char message[] = "secure cell test message by Mnatsakanov Andrey from Cossack Labs";
static char user_context[] = "secure cell user context";

static int secure_cell_seal(void)
{
    uint8_t* encrypted_message;
    size_t encrypted_message_length = 0;
    if (themis_secure_cell_encrypt_seal((uint8_t*)passwd,
                                        sizeof(passwd),
                                        NULL,
                                        0,
                                        (uint8_t*)message,
                                        sizeof(message),
                                        NULL,
                                        &encrypted_message_length)
            != THEMIS_BUFFER_TOO_SMALL
        || encrypted_message_length == 0) {
        testsuite_fail_if(true,
                          "themis_secure_cell_encrypt_seal (encrypted_message_length determination) fail");
        return -1;
    }
    encrypted_message = malloc(encrypted_message_length);
    if (encrypted_message == NULL) {
        testsuite_fail_if(true, "encrypted_message malloc fail");
        return -2;
    }
    if (themis_secure_cell_encrypt_seal((uint8_t*)passwd,
                                        sizeof(passwd),
                                        NULL,
                                        0,
                                        (uint8_t*)message,
                                        sizeof(message),
                                        encrypted_message,
                                        &encrypted_message_length)
        != THEMIS_SUCCESS) {
        testsuite_fail_if(true, "themis_secure_cell_encrypt_seal fail");
        free(encrypted_message);
        return -3;
    }

    uint8_t* decrypted_message;
    size_t decrypted_message_length = 0;

    if (themis_secure_cell_decrypt_seal((uint8_t*)passwd,
                                        sizeof(passwd),
                                        NULL,
                                        0,
                                        encrypted_message,
                                        encrypted_message_length,
                                        NULL,
                                        &decrypted_message_length)
            != THEMIS_BUFFER_TOO_SMALL
        || decrypted_message_length == 0) {
        testsuite_fail_if(true,
                          "themis_secure_cell_decrypt_seal (decrypted_message_length determination) fail");
        free(encrypted_message);
        return -4;
    }
    decrypted_message = malloc(decrypted_message_length);
    if (decrypted_message == NULL) {
        testsuite_fail_if(true, "decrypted_message malloc fail");
        free(encrypted_message);
        return -5;
    }
    if (themis_secure_cell_decrypt_seal((uint8_t*)passwd,
                                        sizeof(passwd),
                                        NULL,
                                        0,
                                        encrypted_message,
                                        encrypted_message_length,
                                        decrypted_message,
                                        &decrypted_message_length)
        != THEMIS_SUCCESS) {
        testsuite_fail_if(true, "themis_secure_cell_decrypt_seal fail");
        free(encrypted_message);
        free(decrypted_message);
        return -6;
    }

    if (sizeof(message) != decrypted_message_length
        || memcmp((uint8_t*)message, decrypted_message, decrypted_message_length) != 0) {
        testsuite_fail_if(true, "message and decrypted_message not equal");
        free(encrypted_message);
        free(decrypted_message);
        return -6;
    }
    free(encrypted_message);
    free(decrypted_message);
    return 0;
}

static int secure_cell_token_protect(void)
{
    uint8_t* encrypted_message;
    size_t encrypted_message_length = 0;
    uint8_t* context;
    size_t context_length = 0;

    if (themis_secure_cell_encrypt_token_protect((uint8_t*)passwd,
                                                 sizeof(passwd),
                                                 NULL,
                                                 0,
                                                 (uint8_t*)message,
                                                 sizeof(message),
                                                 NULL,
                                                 &context_length,
                                                 NULL,
                                                 &encrypted_message_length)
            != THEMIS_BUFFER_TOO_SMALL
        || encrypted_message_length == 0) {
        testsuite_fail_if(true,
                          "themis_secure_cell_encrypt_token_protect "
                          "(encrypted_message_length determination) fail");
        return -1;
    }
    if (encrypted_message_length != sizeof(message)) {
        testsuite_fail_if(true,
                          "themis_secure_cell_encrypt_token_protect "
                          "(encrypted_message_length != message length) fail");
        return -1;
    }
    encrypted_message = malloc(encrypted_message_length);
    if (encrypted_message == NULL) {
        testsuite_fail_if(true, "encrypted_message malloc fail");
        return -2;
    }
    context = malloc(context_length);
    if (context == NULL) {
        testsuite_fail_if(true, "context malloc fail");
        free(encrypted_message);
        return -2;
    }

    if (themis_secure_cell_encrypt_token_protect((uint8_t*)passwd,
                                                 sizeof(passwd),
                                                 NULL,
                                                 0,
                                                 (uint8_t*)message,
                                                 sizeof(message),
                                                 context,
                                                 &context_length,
                                                 encrypted_message,
                                                 &encrypted_message_length)
        != THEMIS_SUCCESS) {
        testsuite_fail_if(true, "themis_secure_cell_encrypt_token_protect fail");
        free(encrypted_message);
        free(context);
        return -3;
    }
    if (encrypted_message_length != sizeof(message)) {
        testsuite_fail_if(true,
                          "themis_secure_cell_encrypt_token_protect "
                          "(encrypted_message_length != message length) fail");
        free(encrypted_message);
        free(context);
        return -3;
    }
    uint8_t* decrypted_message;
    size_t decrypted_message_length = 0;

    if (themis_secure_cell_decrypt_token_protect((uint8_t*)passwd,
                                                 sizeof(passwd),
                                                 NULL,
                                                 0,
                                                 encrypted_message,
                                                 encrypted_message_length,
                                                 context,
                                                 context_length,
                                                 NULL,
                                                 &decrypted_message_length)
            != THEMIS_BUFFER_TOO_SMALL
        || decrypted_message_length == 0) {
        testsuite_fail_if(true,
                          "themis_secure_cell_decrypt_token_protect "
                          "(decrypted_message_length determination) fail");
        free(encrypted_message);
        free(context);
        return -4;
    }
    decrypted_message = malloc(decrypted_message_length);
    if (decrypted_message == NULL) {
        testsuite_fail_if(true, "decrypted_message malloc fail");
        free(encrypted_message);
        free(context);
        return -5;
    }
    if (themis_secure_cell_decrypt_token_protect((uint8_t*)passwd,
                                                 sizeof(passwd),
                                                 NULL,
                                                 0,
                                                 encrypted_message,
                                                 encrypted_message_length,
                                                 context,
                                                 context_length,
                                                 decrypted_message,
                                                 &decrypted_message_length)
        != THEMIS_SUCCESS) {
        testsuite_fail_if(true, "themis_secure_cell_decrypt_token_protect fail");
        free(encrypted_message);
        free(decrypted_message);
        free(context);
        return -6;
    }

    if (sizeof(message) != decrypted_message_length
        || memcmp((uint8_t*)message, decrypted_message, decrypted_message_length) != 0) {
        testsuite_fail_if(true, "message and decrypted_message not equal");
        free(encrypted_message);
        free(decrypted_message);
        free(context);
        return -7;
    }
    free(encrypted_message);
    free(decrypted_message);
    free(context);
    return 0;
}

static int secure_cell_context_imprint(void)
{
    uint8_t* encrypted_message;
    size_t encrypted_message_length = 0;

    if (themis_secure_cell_encrypt_context_imprint((uint8_t*)passwd,
                                                   sizeof(passwd),
                                                   (uint8_t*)message,
                                                   sizeof(message),
                                                   (uint8_t*)user_context,
                                                   strlen(user_context),
                                                   NULL,
                                                   &encrypted_message_length)
            != THEMIS_BUFFER_TOO_SMALL
        || encrypted_message_length == 0) {
        testsuite_fail_if(true,
                          "themis_secure_cell_encrypt_context_imprint "
                          "(encrypted_message_length determination) fail");
        return -1;
    }
    encrypted_message = malloc(encrypted_message_length);
    if (encrypted_message == NULL) {
        testsuite_fail_if(true, "encrypted_message malloc fail");
        return -2;
    }

    if (themis_secure_cell_encrypt_context_imprint((uint8_t*)passwd,
                                                   sizeof(passwd),
                                                   (uint8_t*)message,
                                                   sizeof(message),
                                                   (uint8_t*)user_context,
                                                   strlen(user_context),
                                                   encrypted_message,
                                                   &encrypted_message_length)
        != THEMIS_SUCCESS) {
        testsuite_fail_if(true, "themis_secure_cell_encrypt_context_imprint fail");
        free(encrypted_message);
        return -3;
    }

    uint8_t* decrypted_message;
    size_t decrypted_message_length = 0;

    if (themis_secure_cell_decrypt_context_imprint((uint8_t*)passwd,
                                                   sizeof(passwd),
                                                   encrypted_message,
                                                   encrypted_message_length,
                                                   (uint8_t*)user_context,
                                                   strlen(user_context),
                                                   NULL,
                                                   &decrypted_message_length)
            != THEMIS_BUFFER_TOO_SMALL
        || decrypted_message_length == 0) {
        testsuite_fail_if(true,
                          "themis_secure_cell_decrypt_context_imprint "
                          "(decrypted_message_length determination) fail");
        free(encrypted_message);
        return -4;
    }
    decrypted_message = malloc(decrypted_message_length);
    if (decrypted_message == NULL) {
        testsuite_fail_if(true, "decrypted_message malloc fail");
        free(encrypted_message);
        return -5;
    }
    if (themis_secure_cell_encrypt_context_imprint((uint8_t*)passwd,
                                                   sizeof(passwd),
                                                   encrypted_message,
                                                   encrypted_message_length,
                                                   (uint8_t*)user_context,
                                                   strlen(user_context),
                                                   decrypted_message,
                                                   &decrypted_message_length)
        != THEMIS_SUCCESS) {
        testsuite_fail_if(true, "themis_secure_cell_decrypt_context_imprint fail");
        free(encrypted_message);
        free(decrypted_message);
        return -6;
    }

    if (sizeof(message) != decrypted_message_length
        || memcmp((uint8_t*)message, decrypted_message, decrypted_message_length) != 0) {
        testsuite_fail_if(true, "message and decrypted_message not equal");
        free(encrypted_message);
        free(decrypted_message);
        return -7;
    }
    free(encrypted_message);
    free(decrypted_message);
    return 0;
}

static void secure_cell_test(void)
{
    testsuite_fail_if(secure_cell_seal(), "secure cell seal mode");
    testsuite_fail_if(secure_cell_token_protect(), "secure cell token protect mode");
    testsuite_fail_if(secure_cell_context_imprint(), "secure cell context imprint mode");
}

/*
 * Make sure that we always return correct and consistent lengths, regardless of input.
 */
static void secure_cell_test_lengths(void)
{
    themis_status_t res;
    uint8_t encrypted_buffer[MAX_MESSAGE_SIZE];
    uint8_t authtoken_buffer[MAX_MESSAGE_SIZE];
    size_t encrypted_length;
    size_t encrypted_length_measured;
    size_t authtoken_length;
    size_t authtoken_length_measured;

    /* Seal mode */

    encrypted_length_measured = sizeof(encrypted_buffer);
    res = themis_secure_cell_encrypt_seal((const uint8_t*)"master key",
                                          strlen("master key"),
                                          (const uint8_t*)"my context",
                                          strlen("my context"),
                                          (const uint8_t*)"a message!",
                                          strlen("a message!"),
                                          NULL,
                                          &encrypted_length_measured);
    testsuite_fail_unless(res == THEMIS_BUFFER_TOO_SMALL, "seal: measuring length");

    encrypted_length = sizeof(encrypted_buffer);
    res = themis_secure_cell_encrypt_seal((const uint8_t*)"master key",
                                          strlen("master key"),
                                          (const uint8_t*)"my context",
                                          strlen("my context"),
                                          (const uint8_t*)"a message!",
                                          strlen("a message!"),
                                          encrypted_buffer,
                                          &encrypted_length);
    testsuite_fail_unless(res == THEMIS_SUCCESS, "seal: encrypting buffer");

    testsuite_fail_unless(encrypted_length == encrypted_length_measured,
                          "seal: returned message length");

    /* Token protect mode */

    authtoken_length_measured = sizeof(authtoken_buffer);
    encrypted_length_measured = sizeof(encrypted_buffer);
    res = themis_secure_cell_encrypt_token_protect((const uint8_t*)"master key",
                                                   strlen("master key"),
                                                   (const uint8_t*)"my context",
                                                   strlen("my context"),
                                                   (const uint8_t*)"a message!",
                                                   strlen("a message!"),
                                                   NULL,
                                                   &authtoken_length_measured,
                                                   NULL,
                                                   &encrypted_length_measured);
    testsuite_fail_unless(res == THEMIS_BUFFER_TOO_SMALL, "token protect: measuring length");

    authtoken_length = sizeof(authtoken_buffer);
    encrypted_length = sizeof(encrypted_buffer);
    res = themis_secure_cell_encrypt_token_protect((const uint8_t*)"master key",
                                                   strlen("master key"),
                                                   (const uint8_t*)"my context",
                                                   strlen("my context"),
                                                   (const uint8_t*)"a message!",
                                                   strlen("a message!"),
                                                   authtoken_buffer,
                                                   &authtoken_length,
                                                   encrypted_buffer,
                                                   &encrypted_length);
    testsuite_fail_unless(res == THEMIS_SUCCESS, "token protect: encrypting buffer");

    testsuite_fail_unless(authtoken_length == authtoken_length_measured,
                          "token protect: returned auth token length");
    testsuite_fail_unless(encrypted_length == encrypted_length_measured,
                          "token protect: returned message length");

    /* Context imprint mode */

    encrypted_length_measured = sizeof(encrypted_buffer);
    res = themis_secure_cell_encrypt_context_imprint((const uint8_t*)"master key",
                                                     strlen("master key"),
                                                     (const uint8_t*)"a message!",
                                                     strlen("a message!"),
                                                     (const uint8_t*)"my context",
                                                     strlen("my context"),
                                                     NULL,
                                                     &encrypted_length_measured);
    testsuite_fail_unless(res == THEMIS_BUFFER_TOO_SMALL, "context imprint: measuring length");

    encrypted_length = sizeof(encrypted_buffer);
    res = themis_secure_cell_encrypt_context_imprint((const uint8_t*)"master key",
                                                     strlen("master key"),
                                                     (const uint8_t*)"a message!",
                                                     strlen("a message!"),
                                                     (const uint8_t*)"my context",
                                                     strlen("my context"),
                                                     encrypted_buffer,
                                                     &encrypted_length);
    testsuite_fail_unless(res == THEMIS_SUCCESS, "context imprint: encrypting buffer");

    testsuite_fail_unless(encrypted_length == encrypted_length_measured,
                          "context imprint: returned message length");
}

static void secure_cell_api_test_seal(void)
{
    uint8_t key[MAX_KEY_SIZE];
    uint8_t message[MAX_MESSAGE_SIZE];

    size_t key_length = rand_int(MAX_KEY_SIZE);
    size_t message_length = rand_int(MAX_MESSAGE_SIZE);

    uint8_t* encrypted = NULL;
    size_t encrypted_length;

    uint8_t* decrypted = NULL;
    size_t decrypted_length;

    if (THEMIS_SUCCESS != soter_rand(key, key_length)) {
        testsuite_fail_if(true, "soter_rand fail");
        return;
    }

    if (THEMIS_SUCCESS != soter_rand(message, message_length)) {
        testsuite_fail_if(true, "soter_rand fail");
        return;
    }

    testsuite_fail_unless(THEMIS_BUFFER_TOO_SMALL
                              == themis_secure_cell_encrypt_seal(key,
                                                                 key_length,
                                                                 NULL,
                                                                 0,
                                                                 message,
                                                                 message_length,
                                                                 NULL,
                                                                 &encrypted_length),
                          "themis_secure_cell_encrypt_seal: get output size (NULL out buffer)");
    encrypted = malloc(encrypted_length);
    if (!encrypted) {
        testsuite_fail_if(true, "malloc fail");
        return;
    }

    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_cell_encrypt_seal(NULL,
                                                                 key_length,
                                                                 NULL,
                                                                 0,
                                                                 message,
                                                                 message_length,
                                                                 encrypted,
                                                                 &encrypted_length),
                          "themis_secure_cell_encrypt_seal: invalid key");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_cell_encrypt_seal(key,
                                                                 0,
                                                                 NULL,
                                                                 0,
                                                                 message,
                                                                 message_length,
                                                                 encrypted,
                                                                 &encrypted_length),
                          "themis_secure_cell_encrypt_seal: invalid key length");

    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_cell_encrypt_seal(key,
                                                                 key_length,
                                                                 NULL,
                                                                 0,
                                                                 NULL,
                                                                 message_length,
                                                                 encrypted,
                                                                 &encrypted_length),
                          "themis_secure_cell_encrypt_seal: invalid message");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_cell_encrypt_seal(key,
                                                                 key_length,
                                                                 NULL,
                                                                 0,
                                                                 message,
                                                                 0,
                                                                 encrypted,
                                                                 &encrypted_length),
                          "themis_secure_cell_encrypt_seal: invalid message length");

    encrypted_length--;
    testsuite_fail_unless(THEMIS_BUFFER_TOO_SMALL
                              == themis_secure_cell_encrypt_seal(key,
                                                                 key_length,
                                                                 NULL,
                                                                 0,
                                                                 message,
                                                                 message_length,
                                                                 encrypted,
                                                                 &encrypted_length),
                          "themis_secure_cell_encrypt_seal: get output size (small out buffer)");

    testsuite_fail_unless(THEMIS_SUCCESS
                              == themis_secure_cell_encrypt_seal(key,
                                                                 key_length,
                                                                 NULL,
                                                                 0,
                                                                 message,
                                                                 message_length,
                                                                 encrypted,
                                                                 &encrypted_length),
                          "themis_secure_cell_encrypt_seal: normal flow");

    testsuite_fail_unless(THEMIS_BUFFER_TOO_SMALL
                              == themis_secure_cell_decrypt_seal(key,
                                                                 key_length,
                                                                 NULL,
                                                                 0,
                                                                 encrypted,
                                                                 encrypted_length,
                                                                 NULL,
                                                                 &decrypted_length),
                          "themis_secure_cell_decrypt_seal: get output size (NULL out buffer)");

    decrypted = malloc(decrypted_length);
    if (!decrypted) {
        testsuite_fail_if(true, "malloc fail");
        free(encrypted);
        return;
    }

    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_cell_decrypt_seal(NULL,
                                                                 key_length,
                                                                 NULL,
                                                                 0,
                                                                 encrypted,
                                                                 encrypted_length,
                                                                 decrypted,
                                                                 &decrypted_length),
                          "themis_secure_cell_decrypt_seal: invalid key");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_cell_decrypt_seal(key,
                                                                 0,
                                                                 NULL,
                                                                 0,
                                                                 encrypted,
                                                                 encrypted_length,
                                                                 decrypted,
                                                                 &decrypted_length),
                          "themis_secure_cell_decrypt_seal: invalid key length");

    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_cell_decrypt_seal(key,
                                                                 key_length,
                                                                 NULL,
                                                                 0,
                                                                 NULL,
                                                                 encrypted_length,
                                                                 decrypted,
                                                                 &decrypted_length),
                          "themis_secure_cell_decrypt_seal: invalid message");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_cell_decrypt_seal(key,
                                                                 key_length,
                                                                 NULL,
                                                                 0,
                                                                 encrypted,
                                                                 0,
                                                                 decrypted,
                                                                 &decrypted_length),
                          "themis_secure_cell_decrypt_seal: invalid message length");

    decrypted_length--;
    testsuite_fail_unless(THEMIS_BUFFER_TOO_SMALL
                              == themis_secure_cell_decrypt_seal(key,
                                                                 key_length,
                                                                 NULL,
                                                                 0,
                                                                 encrypted,
                                                                 encrypted_length,
                                                                 decrypted,
                                                                 &decrypted_length),
                          "themis_secure_cell_decrypt_seal: get output size (small out buffer)");

    encrypted[0]++;
    testsuite_fail_unless(THEMIS_FAIL
                              == themis_secure_cell_decrypt_seal(key,
                                                                 key_length,
                                                                 NULL,
                                                                 0,
                                                                 encrypted,
                                                                 encrypted_length,
                                                                 decrypted,
                                                                 &decrypted_length),
                          "themis_secure_cell_decrypt_seal: header corrupt");
    encrypted[0]--;

    encrypted[encrypted_length / 2]++;
    testsuite_fail_unless(THEMIS_FAIL
                              == themis_secure_cell_decrypt_seal(key,
                                                                 key_length,
                                                                 NULL,
                                                                 0,
                                                                 encrypted,
                                                                 encrypted_length,
                                                                 decrypted,
                                                                 &decrypted_length),
                          "themis_secure_cell_decrypt_seal: message corrupt");
    encrypted[encrypted_length / 2]--;

    encrypted[encrypted_length - 1]++;
    testsuite_fail_unless(THEMIS_FAIL
                              == themis_secure_cell_decrypt_seal(key,
                                                                 key_length,
                                                                 NULL,
                                                                 0,
                                                                 encrypted,
                                                                 encrypted_length,
                                                                 decrypted,
                                                                 &decrypted_length),
                          "themis_secure_cell_decrypt_seal: tag corrupt");
    encrypted[encrypted_length - 1]--;

    testsuite_fail_unless(THEMIS_SUCCESS
                                  == themis_secure_cell_decrypt_seal(key,
                                                                     key_length,
                                                                     NULL,
                                                                     0,
                                                                     encrypted,
                                                                     encrypted_length,
                                                                     decrypted,
                                                                     &decrypted_length)
                              && (message_length == decrypted_length)
                              && !memcmp(message, decrypted, message_length),
                          "themis_secure_cell_decrypt_seal: normal flow");

    free(decrypted);
    free(encrypted);
}

static void secure_cell_api_test_context_imprint(void)
{
    uint8_t key[MAX_KEY_SIZE];
    uint8_t context[MAX_CONTEXT_SIZE];
    uint8_t message[MAX_MESSAGE_SIZE];

    size_t key_length = rand_int(MAX_KEY_SIZE);
    size_t context_length = rand_int(MAX_CONTEXT_SIZE);
    size_t message_length = rand_int(MAX_MESSAGE_SIZE);

    uint8_t encrypted[MAX_MESSAGE_SIZE];
    size_t encrypted_length = message_length;

    uint8_t decrypted[MAX_MESSAGE_SIZE];
    size_t decrypted_length = message_length;

    if (THEMIS_SUCCESS != soter_rand(key, key_length)) {
        testsuite_fail_if(true, "soter_rand fail");
        return;
    }

    if (THEMIS_SUCCESS != soter_rand(context, context_length)) {
        testsuite_fail_if(true, "soter_rand fail");
        return;
    }

    if (THEMIS_SUCCESS != soter_rand(message, message_length)) {
        testsuite_fail_if(true, "soter_rand fail");
        return;
    }

    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_cell_encrypt_context_imprint(NULL,
                                                                            key_length,
                                                                            message,
                                                                            message_length,
                                                                            context,
                                                                            context_length,
                                                                            encrypted,
                                                                            &encrypted_length),
                          "themis_secure_cell_encrypt_context_imprint: invalid key");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_cell_encrypt_context_imprint(key,
                                                                            0,
                                                                            message,
                                                                            message_length,
                                                                            context,
                                                                            context_length,
                                                                            encrypted,
                                                                            &encrypted_length),
                          "themis_secure_cell_encrypt_context_imprint: invalid key length");

    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_cell_encrypt_context_imprint(key,
                                                                            key_length,
                                                                            NULL,
                                                                            message_length,
                                                                            context,
                                                                            context_length,
                                                                            encrypted,
                                                                            &encrypted_length),
                          "themis_secure_cell_encrypt_context_imprint: invalid message");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_cell_encrypt_context_imprint(key,
                                                                            key_length,
                                                                            message,
                                                                            0,
                                                                            context,
                                                                            context_length,
                                                                            encrypted,
                                                                            &encrypted_length),
                          "themis_secure_cell_encrypt_context_imprint: invalid message length");

    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_cell_encrypt_context_imprint(key,
                                                                            key_length,
                                                                            message,
                                                                            message_length,
                                                                            NULL,
                                                                            context_length,
                                                                            encrypted,
                                                                            &encrypted_length),
                          "themis_secure_cell_encrypt_context_imprint: invalid context");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_cell_encrypt_context_imprint(key,
                                                                            key_length,
                                                                            message,
                                                                            message_length,
                                                                            context,
                                                                            0,
                                                                            encrypted,
                                                                            &encrypted_length),
                          "themis_secure_cell_encrypt_context_imprint: invalid context length");

    testsuite_fail_unless((THEMIS_BUFFER_TOO_SMALL
                           == themis_secure_cell_encrypt_context_imprint(key,
                                                                         key_length,
                                                                         message,
                                                                         message_length,
                                                                         context,
                                                                         context_length,
                                                                         NULL,
                                                                         &encrypted_length))
                              && (encrypted_length == message_length),
                          "themis_secure_cell_encrypt_context_imprint: get output size (NULL out buffer)");

    encrypted_length--;
    testsuite_fail_unless((THEMIS_BUFFER_TOO_SMALL
                           == themis_secure_cell_encrypt_context_imprint(key,
                                                                         key_length,
                                                                         message,
                                                                         message_length,
                                                                         context,
                                                                         context_length,
                                                                         encrypted,
                                                                         &encrypted_length))
                              && (encrypted_length == message_length),
                          "themis_secure_cell_encrypt_context_imprint: get output size (small out buffer)");

    testsuite_fail_unless((THEMIS_SUCCESS
                           == themis_secure_cell_encrypt_context_imprint(key,
                                                                         key_length,
                                                                         message,
                                                                         message_length,
                                                                         context,
                                                                         context_length,
                                                                         encrypted,
                                                                         &encrypted_length))
                              && (encrypted_length == message_length),
                          "themis_secure_cell_encrypt_context_imprint: normal flow");

    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_cell_decrypt_context_imprint(NULL,
                                                                            key_length,
                                                                            encrypted,
                                                                            encrypted_length,
                                                                            context,
                                                                            context_length,
                                                                            decrypted,
                                                                            &decrypted_length),
                          "themis_secure_cell_decrypt_context_imprint: invalid key");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_cell_decrypt_context_imprint(key,
                                                                            0,
                                                                            encrypted,
                                                                            encrypted_length,
                                                                            context,
                                                                            context_length,
                                                                            decrypted,
                                                                            &decrypted_length),
                          "themis_secure_cell_decrypt_context_imprint: invalid key length");

    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_cell_decrypt_context_imprint(key,
                                                                            key_length,
                                                                            NULL,
                                                                            encrypted_length,
                                                                            context,
                                                                            context_length,
                                                                            decrypted,
                                                                            &decrypted_length),
                          "themis_secure_cell_decrypt_context_imprint: invalid message");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_cell_decrypt_context_imprint(key,
                                                                            key_length,
                                                                            encrypted,
                                                                            0,
                                                                            context,
                                                                            context_length,
                                                                            decrypted,
                                                                            &decrypted_length),
                          "themis_secure_cell_decrypt_context_imprint: invalid message length");

    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_cell_decrypt_context_imprint(key,
                                                                            key_length,
                                                                            encrypted,
                                                                            encrypted_length,
                                                                            NULL,
                                                                            context_length,
                                                                            decrypted,
                                                                            &decrypted_length),
                          "themis_secure_cell_decrypt_context_imprint: invalid context");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_cell_decrypt_context_imprint(key,
                                                                            key_length,
                                                                            encrypted,
                                                                            encrypted_length,
                                                                            context,
                                                                            0,
                                                                            decrypted,
                                                                            &decrypted_length),
                          "themis_secure_cell_decrypt_context_imprint: invalid context length");

    testsuite_fail_unless((THEMIS_BUFFER_TOO_SMALL
                           == themis_secure_cell_decrypt_context_imprint(key,
                                                                         key_length,
                                                                         encrypted,
                                                                         encrypted_length,
                                                                         context,
                                                                         context_length,
                                                                         NULL,
                                                                         &decrypted_length))
                              && (decrypted_length == encrypted_length),
                          "themis_secure_cell_decrypt_context_imprint: get output size (NULL out buffer)");

    decrypted_length--;
    testsuite_fail_unless((THEMIS_BUFFER_TOO_SMALL
                           == themis_secure_cell_decrypt_context_imprint(key,
                                                                         key_length,
                                                                         encrypted,
                                                                         encrypted_length,
                                                                         context,
                                                                         context_length,
                                                                         decrypted,
                                                                         &decrypted_length))
                              && (decrypted_length == encrypted_length),
                          "themis_secure_cell_decrypt_context_imprint: get output size (small out buffer)");

    testsuite_fail_unless(THEMIS_SUCCESS
                                  == themis_secure_cell_decrypt_context_imprint(key,
                                                                                key_length,
                                                                                encrypted,
                                                                                encrypted_length,
                                                                                context,
                                                                                context_length,
                                                                                decrypted,
                                                                                &decrypted_length)
                              && (message_length == decrypted_length)
                              && (!memcmp(message, decrypted, message_length)),
                          "themis_secure_cell_decrypt_context_imprint: normal flow");
}

static void secure_cell_api_test(void)
{
    secure_cell_api_test_seal();
    secure_cell_api_test_context_imprint();
}

/*
 * See themis_auth_sym_message_hdr_t definition.
 */

static inline uint32_t get_context_iv_length(const uint8_t* context)
{
    return ((const uint32_t*)context)[1];
}

static inline void set_context_iv_length(uint8_t* context, uint32_t value)
{
    ((uint32_t*)context)[1] = value;
}

static inline uint32_t get_context_auth_tag_length(const uint8_t* context)
{
    return ((const uint32_t*)context)[2];
}

static inline void set_context_auth_tag_length(uint8_t* context, uint32_t value)
{
    ((uint32_t*)context)[2] = value;
}

static inline uint32_t get_context_message_length(const uint8_t* context)
{
    return ((const uint32_t*)context)[3];
}

static inline void set_context_message_length(uint8_t* context, uint32_t value)
{
    ((uint32_t*)context)[3] = value;
}

static themis_status_t encrypt_seal(uint8_t** encrypted_message, size_t* encrypted_message_length)
{
    themis_status_t res = THEMIS_SUCCESS;

    res = themis_secure_cell_encrypt_seal((uint8_t*)passwd,
                                          sizeof(passwd),
                                          NULL,
                                          0,
                                          (uint8_t*)message,
                                          sizeof(message),
                                          NULL,
                                          encrypted_message_length);
    if (res != THEMIS_BUFFER_TOO_SMALL) {
        testsuite_fail_if(true,
                          "themis_secure_cell_encrypt_seal: failed to determine encrypted message length");
        return res;
    }

    *encrypted_message = malloc(*encrypted_message_length);
    if (*encrypted_message == NULL) {
        testsuite_fail_if(true,
                          "themis_secure_cell_encrypt_seal: failed to allocate memory for encrypted message");
        return THEMIS_NO_MEMORY;
    }

    res = themis_secure_cell_encrypt_seal((uint8_t*)passwd,
                                          sizeof(passwd),
                                          NULL,
                                          0,
                                          (uint8_t*)message,
                                          sizeof(message),
                                          *encrypted_message,
                                          encrypted_message_length);
    if (res != THEMIS_SUCCESS) {
        testsuite_fail_if(true, "themis_secure_cell_encrypt_seal: failed to encrypt message");
        free(encrypted_message);
        return res;
    }

    return THEMIS_SUCCESS;
}

static themis_status_t prepare_decrypt_seal(uint8_t* encrypted_message,
                                            size_t encrypted_message_length,
                                            uint8_t** decrypted_message,
                                            size_t* decrypted_message_length)
{
    themis_status_t res = THEMIS_SUCCESS;

    res = themis_secure_cell_decrypt_seal((uint8_t*)passwd,
                                          sizeof(passwd),
                                          NULL,
                                          0,
                                          encrypted_message,
                                          encrypted_message_length,
                                          NULL,
                                          decrypted_message_length);
    if (res != THEMIS_BUFFER_TOO_SMALL) {
        testsuite_fail_if(true,
                          "themis_secure_cell_decrypt_seal: failed to determine decrypted message length");
        return res;
    }

    *decrypted_message = malloc(*decrypted_message_length);
    if (*decrypted_message == NULL) {
        testsuite_fail_if(true,
                          "themis_secure_cell_decrypt_seal: failed to allocate memory for decrypted message");
        return THEMIS_NO_MEMORY;
    }

    return THEMIS_SUCCESS;
}

static void secure_cell_context_corruption_seal(void)
{
    themis_status_t res = THEMIS_SUCCESS;

    uint8_t* encrypted_message = NULL;
    size_t encrypted_message_length = 0;

    res = encrypt_seal(&encrypted_message, &encrypted_message_length);
    if (res != THEMIS_SUCCESS) {
        goto out;
    }

    uint32_t old_value = 0;

    uint8_t* decrypted_message = NULL;
    size_t decrypted_message_length = 0;

    old_value = get_context_iv_length(encrypted_message);
    set_context_iv_length(encrypted_message, CORRUPTED_LENGTH);
    {
        res = prepare_decrypt_seal(encrypted_message,
                                   encrypted_message_length,
                                   &decrypted_message,
                                   &decrypted_message_length);
        if (res != THEMIS_SUCCESS) {
            goto out;
        }

        res = themis_secure_cell_decrypt_seal((uint8_t*)passwd,
                                              sizeof(passwd),
                                              NULL,
                                              0,
                                              encrypted_message,
                                              encrypted_message_length,
                                              decrypted_message,
                                              &decrypted_message_length);
        testsuite_fail_if(res == THEMIS_SUCCESS,
                          "themis_secure_cell_decrypt_seal detects corrupted IV length");

        free(decrypted_message);
    }
    set_context_iv_length(encrypted_message, old_value);

    old_value = get_context_auth_tag_length(encrypted_message);
    set_context_auth_tag_length(encrypted_message, CORRUPTED_LENGTH);
    {
        res = prepare_decrypt_seal(encrypted_message,
                                   encrypted_message_length,
                                   &decrypted_message,
                                   &decrypted_message_length);
        if (res != THEMIS_SUCCESS) {
            goto out;
        }

        res = themis_secure_cell_decrypt_seal((uint8_t*)passwd,
                                              sizeof(passwd),
                                              NULL,
                                              0,
                                              encrypted_message,
                                              encrypted_message_length,
                                              decrypted_message,
                                              &decrypted_message_length);
        testsuite_fail_if(res == THEMIS_SUCCESS,
                          "themis_secure_cell_decrypt_seal detects corrupted auth tag length");

        free(decrypted_message);
    }
    set_context_auth_tag_length(encrypted_message, old_value);

    old_value = get_context_message_length(encrypted_message);
    set_context_message_length(encrypted_message, CORRUPTED_LENGTH);
    {
        res = themis_secure_cell_decrypt_seal((uint8_t*)passwd,
                                              sizeof(passwd),
                                              NULL,
                                              0,
                                              encrypted_message,
                                              encrypted_message_length,
                                              NULL,
                                              &decrypted_message_length);
        testsuite_fail_if(res == THEMIS_BUFFER_TOO_SMALL,
                          "themis_secure_cell_decrypt_seal detects corrupted message length");
    }
    set_context_message_length(encrypted_message, old_value);

out:
    free(encrypted_message);
}

static themis_status_t encrypt_token_protect(uint8_t** encrypted_message,
                                             size_t* encrypted_message_length,
                                             uint8_t** context,
                                             size_t* context_length)
{
    themis_status_t res = THEMIS_SUCCESS;

    res = themis_secure_cell_encrypt_token_protect((uint8_t*)passwd,
                                                   sizeof(passwd),
                                                   NULL,
                                                   0,
                                                   (uint8_t*)message,
                                                   sizeof(message),
                                                   NULL,
                                                   context_length,
                                                   NULL,
                                                   encrypted_message_length);
    if (res != THEMIS_BUFFER_TOO_SMALL || context_length == 0 || encrypted_message_length == 0) {
        testsuite_fail_if(true,
                          "themis_secure_cell_encrypt_token_protect: failed to determine "
                          "encrypted message length");
        return res;
    }

    *encrypted_message = malloc(*encrypted_message_length);
    if (*encrypted_message == NULL) {
        testsuite_fail_if(true,
                          "themis_secure_cell_encrypt_token_protect: failed to allocate "
                          "memory for encrypted message");
        return res;
    }

    *context = malloc(*context_length);
    if (*context == NULL) {
        testsuite_fail_if(true,
                          "themis_secure_cell_encrypt_token_protect: failed to allocate memory for auth context");
        free(*encrypted_message);
        return res;
    }

    res = themis_secure_cell_encrypt_token_protect((uint8_t*)passwd,
                                                   sizeof(passwd),
                                                   NULL,
                                                   0,
                                                   (uint8_t*)message,
                                                   sizeof(message),
                                                   *context,
                                                   context_length,
                                                   *encrypted_message,
                                                   encrypted_message_length);
    if (res != THEMIS_SUCCESS) {
        testsuite_fail_if(true, "themis_secure_cell_encrypt_token_protect: failed to encrypt message");
        free(*encrypted_message);
        free(*context);
        return res;
    }

    return THEMIS_SUCCESS;
}

static themis_status_t prepare_decrypt_token_protect(uint8_t* encrypted_message,
                                                     size_t encrypted_message_length,
                                                     uint8_t* context,
                                                     size_t context_length,
                                                     uint8_t** decrypted_message,
                                                     size_t* decrypted_message_length)
{
    themis_status_t res = THEMIS_SUCCESS;

    res = themis_secure_cell_decrypt_token_protect((uint8_t*)passwd,
                                                   sizeof(passwd),
                                                   NULL,
                                                   0,
                                                   encrypted_message,
                                                   encrypted_message_length,
                                                   context,
                                                   context_length,
                                                   NULL,
                                                   decrypted_message_length);
    if (res != THEMIS_BUFFER_TOO_SMALL || decrypted_message_length == 0) {
        testsuite_fail_if(true,
                          "themis_secure_cell_decrypt_token_protect: failed to determine "
                          "decrypted message length");
        return res;
    }

    *decrypted_message = malloc(*decrypted_message_length);
    if (*decrypted_message == NULL) {
        testsuite_fail_if(true,
                          "themis_secure_cell_decrypt_token_protect: failed to allocate "
                          "memory for decrypted message");
        return THEMIS_NO_MEMORY;
    }

    return THEMIS_SUCCESS;
}

static void secure_cell_context_corruption_token_protect(void)
{
    themis_status_t res = THEMIS_SUCCESS;

    uint8_t* encrypted_message = NULL;
    size_t encrypted_message_length = 0;
    uint8_t* context = NULL;
    size_t context_length = 0;

    res = encrypt_token_protect(&encrypted_message, &encrypted_message_length, &context, &context_length);
    if (res != THEMIS_SUCCESS) {
        goto out;
    }

    uint32_t old_value = 0;

    uint8_t* decrypted_message = NULL;
    size_t decrypted_message_length = 0;

    old_value = get_context_iv_length(context);
    set_context_iv_length(context, CORRUPTED_LENGTH);
    {
        res = prepare_decrypt_token_protect(encrypted_message,
                                            encrypted_message_length,
                                            context,
                                            context_length,
                                            &decrypted_message,
                                            &decrypted_message_length);
        if (res != THEMIS_SUCCESS) {
            goto out;
        }

        res = themis_secure_cell_decrypt_token_protect((uint8_t*)passwd,
                                                       sizeof(passwd),
                                                       NULL,
                                                       0,
                                                       encrypted_message,
                                                       encrypted_message_length,
                                                       context,
                                                       context_length,
                                                       decrypted_message,
                                                       &decrypted_message_length);
        testsuite_fail_if(res == THEMIS_SUCCESS,
                          "themis_secure_cell_decrypt_token_protect detects corrupted IV length");

        free(decrypted_message);
    }
    set_context_iv_length(context, old_value);

    old_value = get_context_auth_tag_length(context);
    set_context_auth_tag_length(context, CORRUPTED_LENGTH);
    {
        res = prepare_decrypt_token_protect(encrypted_message,
                                            encrypted_message_length,
                                            context,
                                            context_length,
                                            &decrypted_message,
                                            &decrypted_message_length);
        if (res != THEMIS_SUCCESS) {
            goto out;
        }

        res = themis_secure_cell_decrypt_token_protect((uint8_t*)passwd,
                                                       sizeof(passwd),
                                                       NULL,
                                                       0,
                                                       encrypted_message,
                                                       encrypted_message_length,
                                                       context,
                                                       context_length,
                                                       decrypted_message,
                                                       &decrypted_message_length);
        testsuite_fail_if(res == THEMIS_SUCCESS,
                          "themis_secure_cell_decrypt_token_protect detects corrupted auth tag length");

        free(decrypted_message);
    }
    set_context_auth_tag_length(context, old_value);

    old_value = get_context_message_length(context);
    set_context_message_length(context, CORRUPTED_LENGTH);
    {
        res = prepare_decrypt_token_protect(encrypted_message,
                                            encrypted_message_length,
                                            context,
                                            context_length,
                                            &decrypted_message,
                                            &decrypted_message_length);
        if (res != THEMIS_SUCCESS) {
            goto out;
        }

        res = themis_secure_cell_decrypt_token_protect((uint8_t*)passwd,
                                                       sizeof(passwd),
                                                       NULL,
                                                       0,
                                                       encrypted_message,
                                                       encrypted_message_length,
                                                       context,
                                                       context_length,
                                                       decrypted_message,
                                                       &decrypted_message_length);
        testsuite_fail_if(res == THEMIS_SUCCESS,
                          "themis_secure_cell_decrypt_token_protect detects corrupted message length");

        free(decrypted_message);
    }
    set_context_message_length(context, old_value);

out:
    free(encrypted_message);
    free(context);
}

static void secure_cell_context_corruption(void)
{
    secure_cell_context_corruption_seal();
    secure_cell_context_corruption_token_protect();
}

void run_secure_cell_test(void)
{
    testsuite_enter_suite("secure cell: basic flow");
    testsuite_run_test(secure_cell_test);
    testsuite_run_test(secure_cell_test_lengths);

    testsuite_enter_suite("secure cell: api test");
    testsuite_run_test(secure_cell_api_test);

    testsuite_enter_suite("secure cell: context corruption");
    testsuite_run_test(secure_cell_context_corruption);
}
