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

#include <stdbool.h>
#include <string.h>

#include <soter/soter_rand.h>

/* Fuzz parameters */
#define MAX_KEY_SIZE 256
#define MAX_CONTEXT_SIZE 2048
#define MAX_MESSAGE_SIZE 4096

/* Keep it under 2^31 to support 32-bit systems. */
#define CORRUPTED_LENGTH 0x5AFE

#define DEFAULT_SYM_KEY_LENGTH 32

static uint8_t master_key[] =
    "\x43\x68\x75\x75\x30\x65\x6a\x6f\x68\x63\x68\x65\x65\x66\x61\x69\x39\x61\x69\x74\x33\x56\x6f\x68\x78\x65\x77\x36\x78\x6f\x68\x54";
static char message[] = "secure cell test message by Mnatsakanov Andrey from Cossack Labs";
static char user_context[] = "secure cell user context";

static bool non_zero_buffer(const uint8_t* buffer, size_t length)
{
    for (size_t i = 0; i < length; i++) {
        if (buffer[i] != 0) {
            return true;
        }
    }
    return false;
}

static bool different_buffers(const uint8_t* buffer1, size_t length1, const uint8_t* buffer2, size_t length2)
{
    size_t min = (length1 < length2) ? length1 : length2;
    if (memcmp(buffer1, buffer2, min) != 0) {
        return true;
    }
    return length1 != length2;
}

static void secure_cell_key_generation(void)
{
    themis_status_t res;
    uint8_t master_key_1[MAX_KEY_SIZE];
    uint8_t master_key_2[MAX_KEY_SIZE];
    size_t master_key_1_length = 0;
    size_t master_key_2_length = 0;

    /*
     * Normal usage of key generation API.
     */

    res = themis_gen_sym_key(NULL, &master_key_1_length);
    testsuite_fail_unless(res == THEMIS_BUFFER_TOO_SMALL, "keygen: query key size");
    testsuite_fail_unless(master_key_1_length == DEFAULT_SYM_KEY_LENGTH,
                          "keygen: recommends default key size");

    master_key_1_length = 0;
    res = themis_gen_sym_key(master_key_1, &master_key_1_length);
    testsuite_fail_unless(res == THEMIS_BUFFER_TOO_SMALL, "keygen: query key size (with buffer)");
    testsuite_fail_unless(master_key_1_length == DEFAULT_SYM_KEY_LENGTH,
                          "keygen: recommends default key size");

    size_t old_value = master_key_1_length;
    res = themis_gen_sym_key(master_key_1, &master_key_1_length);
    testsuite_fail_unless(res == THEMIS_SUCCESS, "keygen: generate recommended key");
    testsuite_fail_unless(master_key_1_length == old_value, "keygen: key length unchanged");

    master_key_1_length = MAX_KEY_SIZE;
    res = themis_gen_sym_key(master_key_1, &master_key_1_length);
    testsuite_fail_unless(res == THEMIS_SUCCESS, "keygen: generate extra-big key");
    testsuite_fail_unless(master_key_1_length == MAX_KEY_SIZE, "keygen: key length unchanged");

    master_key_1_length = 8;
    res = themis_gen_sym_key(master_key_1, &master_key_1_length);
    testsuite_fail_unless(res == THEMIS_SUCCESS, "keygen: generate small key");
    testsuite_fail_unless(master_key_1_length == 8, "keygen: key length unchanged");

    /*
     * Verify that key generation is not bogus and returns new keys.
     */

    memset(master_key_1, 0, sizeof(master_key_1));
    memset(master_key_2, 0, sizeof(master_key_2));

    master_key_1_length = MAX_KEY_SIZE;
    master_key_2_length = MAX_KEY_SIZE;

    res = themis_gen_sym_key(master_key_1, &master_key_1_length);
    testsuite_fail_unless(res == THEMIS_SUCCESS, "keygen: generates key 1");
    res = themis_gen_sym_key(master_key_2, &master_key_2_length);
    testsuite_fail_unless(res == THEMIS_SUCCESS, "keygen: generates key 2");

    testsuite_fail_unless(non_zero_buffer(master_key_1, master_key_1_length),
                          "keygen: returns nonzero key 1");
    testsuite_fail_unless(non_zero_buffer(master_key_2, master_key_2_length),
                          "keygen: returns nonzero key 2");

    testsuite_fail_unless(different_buffers(master_key_1, master_key_1_length, master_key_2, master_key_2_length),
                          "keygen: generates different keys");

    /*
     * Verify invalid input handling and error conditions.
     */

    res = themis_gen_sym_key(NULL, NULL);
    testsuite_fail_unless(res == THEMIS_INVALID_PARAMETER, "keygen: missing key length");

    res = themis_gen_sym_key(master_key_1, NULL);
    testsuite_fail_unless(res == THEMIS_INVALID_PARAMETER, "keygen: missing key length");
}

static int secure_cell_seal(void)
{
    uint8_t* encrypted_message;
    size_t encrypted_message_length = 0;
    if (themis_secure_cell_encrypt_seal(master_key,
                                        sizeof(master_key),
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
    if (themis_secure_cell_encrypt_seal(master_key,
                                        sizeof(master_key),
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

    if (themis_secure_cell_decrypt_seal(master_key,
                                        sizeof(master_key),
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
    if (themis_secure_cell_decrypt_seal(master_key,
                                        sizeof(master_key),
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

    if (themis_secure_cell_encrypt_token_protect(master_key,
                                                 sizeof(master_key),
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

    if (themis_secure_cell_encrypt_token_protect(master_key,
                                                 sizeof(master_key),
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

    if (themis_secure_cell_decrypt_token_protect(master_key,
                                                 sizeof(master_key),
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
    if (themis_secure_cell_decrypt_token_protect(master_key,
                                                 sizeof(master_key),
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

    if (themis_secure_cell_encrypt_context_imprint(master_key,
                                                   sizeof(master_key),
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

    if (themis_secure_cell_encrypt_context_imprint(master_key,
                                                   sizeof(master_key),
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

    if (themis_secure_cell_decrypt_context_imprint(master_key,
                                                   sizeof(master_key),
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
    if (themis_secure_cell_encrypt_context_imprint(master_key,
                                                   sizeof(master_key),
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
    if (!context) {
        return 0;
    }
    return ((const uint32_t*)context)[1];
}

static inline void set_context_iv_length(uint8_t* context, uint32_t value)
{
    if (!context) {
        return;
    }
    ((uint32_t*)context)[1] = value;
}

static inline uint32_t get_context_auth_tag_length(const uint8_t* context)
{
    if (!context) {
        return 0;
    }
    return ((const uint32_t*)context)[2];
}

static inline void set_context_auth_tag_length(uint8_t* context, uint32_t value)
{
    if (!context) {
        return;
    }
    ((uint32_t*)context)[2] = value;
}

static inline uint32_t get_context_message_length(const uint8_t* context)
{
    if (!context) {
        return 0;
    }
    return ((const uint32_t*)context)[3];
}

static inline void set_context_message_length(uint8_t* context, uint32_t value)
{
    if (!context) {
        return;
    }
    ((uint32_t*)context)[3] = value;
}

static themis_status_t encrypt_seal(uint8_t** encrypted_message, size_t* encrypted_message_length)
{
    themis_status_t res = THEMIS_SUCCESS;

    res = themis_secure_cell_encrypt_seal(master_key,
                                          sizeof(master_key),
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

    res = themis_secure_cell_encrypt_seal(master_key,
                                          sizeof(master_key),
                                          NULL,
                                          0,
                                          (uint8_t*)message,
                                          sizeof(message),
                                          *encrypted_message,
                                          encrypted_message_length);
    if (res != THEMIS_SUCCESS) {
        testsuite_fail_if(true, "themis_secure_cell_encrypt_seal: failed to encrypt message");
        free(*encrypted_message);
        *encrypted_message = NULL;
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

    res = themis_secure_cell_decrypt_seal(master_key,
                                          sizeof(master_key),
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

        res = themis_secure_cell_decrypt_seal(master_key,
                                              sizeof(master_key),
                                              NULL,
                                              0,
                                              encrypted_message,
                                              encrypted_message_length,
                                              decrypted_message,
                                              &decrypted_message_length);
        testsuite_fail_if(res == THEMIS_SUCCESS,
                          "themis_secure_cell_decrypt_seal detects corrupted IV length");

        free(decrypted_message);
        decrypted_message = NULL;
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

        res = themis_secure_cell_decrypt_seal(master_key,
                                              sizeof(master_key),
                                              NULL,
                                              0,
                                              encrypted_message,
                                              encrypted_message_length,
                                              decrypted_message,
                                              &decrypted_message_length);
        testsuite_fail_if(res == THEMIS_SUCCESS,
                          "themis_secure_cell_decrypt_seal detects corrupted auth tag length");

        free(decrypted_message);
        decrypted_message = NULL;
    }
    set_context_auth_tag_length(encrypted_message, old_value);

    old_value = get_context_message_length(encrypted_message);
    set_context_message_length(encrypted_message, CORRUPTED_LENGTH);
    {
        res = themis_secure_cell_decrypt_seal(master_key,
                                              sizeof(master_key),
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

    res = themis_secure_cell_encrypt_token_protect(master_key,
                                                   sizeof(master_key),
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
        *encrypted_message = NULL;
        return res;
    }

    res = themis_secure_cell_encrypt_token_protect(master_key,
                                                   sizeof(master_key),
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
        *encrypted_message = NULL;
        free(*context);
        *context = NULL;
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

    res = themis_secure_cell_decrypt_token_protect(master_key,
                                                   sizeof(master_key),
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

        res = themis_secure_cell_decrypt_token_protect(master_key,
                                                       sizeof(master_key),
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
        decrypted_message = NULL;
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

        res = themis_secure_cell_decrypt_token_protect(master_key,
                                                       sizeof(master_key),
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
        decrypted_message = NULL;
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

        res = themis_secure_cell_decrypt_token_protect(master_key,
                                                       sizeof(master_key),
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
        decrypted_message = NULL;
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

static void scell_seal_passphrase_operation(void)
{
    themis_status_t res = THEMIS_FAIL;
    const uint8_t passphrase[] = "secrets are scary";
    const uint8_t message[] = "crypto is fun";
    const uint8_t context[] = "you are carrying too much to be able to run";
    uint8_t* encrypted = NULL;
    uint8_t* decrypted = NULL;
    size_t encrypted_length = 0;
    size_t decrypted_length = 0;

    encrypted_length = 0;
    res = themis_secure_cell_encrypt_seal_with_passphrase(passphrase,
                                                          sizeof(passphrase),
                                                          context,
                                                          sizeof(context),
                                                          message,
                                                          sizeof(message),
                                                          NULL,
                                                          &encrypted_length);
    testsuite_fail_if(res != THEMIS_BUFFER_TOO_SMALL, "encrypt: compute output length");

    encrypted = malloc(encrypted_length);
    if (!encrypted) {
        testsuite_fail_if(true, "encrypt: *** failed to allocate output buffer");
        goto error;
    }

    res = themis_secure_cell_encrypt_seal_with_passphrase(passphrase,
                                                          sizeof(passphrase),
                                                          context,
                                                          sizeof(context),
                                                          message,
                                                          sizeof(message),
                                                          encrypted,
                                                          &encrypted_length);
    testsuite_fail_if(res != THEMIS_SUCCESS, "encrypt: process message");
    testsuite_fail_unless(encrypted_length > sizeof(message),
                          "encrypt: produces message with embedded auth token");

    decrypted_length = 0;
    res = themis_secure_cell_decrypt_seal_with_passphrase(passphrase,
                                                          sizeof(passphrase),
                                                          context,
                                                          sizeof(context),
                                                          encrypted,
                                                          encrypted_length,
                                                          NULL,
                                                          &decrypted_length);
    testsuite_fail_if(res != THEMIS_BUFFER_TOO_SMALL, "decrypt: compute output length");

    decrypted = malloc(decrypted_length);
    if (!decrypted) {
        testsuite_fail_if(true, "decrypt: *** failed to allocate output buffer");
        goto error;
    }

    res = themis_secure_cell_decrypt_seal_with_passphrase(passphrase,
                                                          sizeof(passphrase),
                                                          context,
                                                          sizeof(context),
                                                          encrypted,
                                                          encrypted_length,
                                                          decrypted,
                                                          &decrypted_length);
    testsuite_fail_if(res != THEMIS_SUCCESS, "decrypt: process message");

    testsuite_fail_unless(decrypted_length == sizeof(message), "validate: message length");
    testsuite_fail_unless(!memcmp(message, decrypted, decrypted_length), "validate: message content");

error:
    free(encrypted);
    free(decrypted);
}

static void scell_seal_passphrase_parameters_encrypt(void)
{
    themis_status_t res = THEMIS_FAIL;
    const uint8_t passphrase[] = "roses are red";
    const uint8_t message[] = "violets are blue";
    const uint8_t context[] = "cats are definitely superior";
    uint8_t encrypted[MAX_MESSAGE_SIZE];
    size_t encrypted_length = sizeof(encrypted);

    /* encrypt: passphrase is required and must be non-empty */

    res = themis_secure_cell_encrypt_seal_with_passphrase(NULL,
                                                          0,
                                                          context,
                                                          sizeof(context),
                                                          message,
                                                          sizeof(message),
                                                          NULL,
                                                          &encrypted_length);
    testsuite_fail_if(res != THEMIS_INVALID_PARAMETER, "encrypt: no passphrase (1)");

    res = themis_secure_cell_encrypt_seal_with_passphrase(NULL,
                                                          sizeof(passphrase),
                                                          context,
                                                          sizeof(context),
                                                          message,
                                                          sizeof(message),
                                                          NULL,
                                                          &encrypted_length);
    testsuite_fail_if(res != THEMIS_INVALID_PARAMETER, "encrypt: no passphrase (2)");

    res = themis_secure_cell_encrypt_seal_with_passphrase(passphrase,
                                                          0,
                                                          context,
                                                          sizeof(context),
                                                          message,
                                                          sizeof(message),
                                                          NULL,
                                                          &encrypted_length);
    testsuite_fail_if(res != THEMIS_INVALID_PARAMETER, "encrypt: no passphrase (3)");

    /* encrypt: context is optional, but must be non-NULL if used */

    res = themis_secure_cell_encrypt_seal_with_passphrase(passphrase,
                                                          sizeof(passphrase),
                                                          NULL,
                                                          0,
                                                          message,
                                                          sizeof(message),
                                                          NULL,
                                                          &encrypted_length);
    testsuite_fail_if(res != THEMIS_BUFFER_TOO_SMALL, "encrypt: no context (1.1)");

    res = themis_secure_cell_encrypt_seal_with_passphrase(passphrase,
                                                          sizeof(passphrase),
                                                          NULL,
                                                          0,
                                                          message,
                                                          sizeof(message),
                                                          encrypted,
                                                          &encrypted_length);
    testsuite_fail_if(res != THEMIS_SUCCESS, "encrypt: no context (1.2)");

    res = themis_secure_cell_encrypt_seal_with_passphrase(passphrase,
                                                          sizeof(passphrase),
                                                          NULL,
                                                          sizeof(context),
                                                          message,
                                                          sizeof(message),
                                                          NULL,
                                                          &encrypted_length);
    testsuite_fail_if(res != THEMIS_INVALID_PARAMETER, "encrypt: no context (2)");

    /* encrypt: message is required and must be non-empty */

    res = themis_secure_cell_encrypt_seal_with_passphrase(passphrase,
                                                          sizeof(passphrase),
                                                          context,
                                                          sizeof(context),
                                                          NULL,
                                                          sizeof(message),
                                                          NULL,
                                                          &encrypted_length);
    testsuite_fail_if(res != THEMIS_INVALID_PARAMETER, "encrypt: no message (1)");

    res = themis_secure_cell_encrypt_seal_with_passphrase(passphrase,
                                                          sizeof(passphrase),
                                                          context,
                                                          sizeof(context),
                                                          NULL,
                                                          0,
                                                          NULL,
                                                          &encrypted_length);
    testsuite_fail_if(res != THEMIS_INVALID_PARAMETER, "encrypt: no message (2)");

    res = themis_secure_cell_encrypt_seal_with_passphrase(passphrase,
                                                          sizeof(passphrase),
                                                          context,
                                                          sizeof(context),
                                                          message,
                                                          0,
                                                          NULL,
                                                          &encrypted_length);
    testsuite_fail_if(res != THEMIS_INVALID_PARAMETER, "encrypt: no message (3)");

    /* encrypt: output length is required */

    res = themis_secure_cell_encrypt_seal_with_passphrase(passphrase,
                                                          sizeof(passphrase),
                                                          context,
                                                          sizeof(context),
                                                          message,
                                                          sizeof(message),
                                                          NULL,
                                                          NULL);
    testsuite_fail_if(res != THEMIS_INVALID_PARAMETER, "encrypt: no output length");
}

static void scell_seal_passphrase_parameters_decrypt(void)
{
    themis_status_t res = THEMIS_FAIL;
    const uint8_t passphrase[] = "maigo-no maigo-no koneko-chan";
    const uint8_t message[] = "anata-no o-uchi wa doko desu ka?";
    const uint8_t context[] = "o-uchi wo kiite mo wakaranai";
    uint8_t encrypted[MAX_MESSAGE_SIZE];
    uint8_t decrypted[MAX_MESSAGE_SIZE];
    size_t encrypted_length = sizeof(encrypted);
    size_t decrypted_length = sizeof(decrypted);

    res = themis_secure_cell_encrypt_seal_with_passphrase(passphrase,
                                                          sizeof(passphrase),
                                                          context,
                                                          sizeof(context),
                                                          message,
                                                          sizeof(message),
                                                          encrypted,
                                                          &encrypted_length);
    if (res != THEMIS_SUCCESS) {
        testsuite_fail_if(true, "decrypt: *** failed to encrypt valid message");
        return;
    }

    /* decrypt: passphrase is required and must be non-empty */

    res = themis_secure_cell_decrypt_seal_with_passphrase(NULL,
                                                          0,
                                                          context,
                                                          sizeof(context),
                                                          encrypted,
                                                          encrypted_length,
                                                          decrypted,
                                                          &decrypted_length);
    testsuite_fail_if(res != THEMIS_INVALID_PARAMETER, "decrypt: no passphrase (1)");

    res = themis_secure_cell_decrypt_seal_with_passphrase(NULL,
                                                          sizeof(passphrase),
                                                          context,
                                                          sizeof(context),
                                                          encrypted,
                                                          encrypted_length,
                                                          decrypted,
                                                          &decrypted_length);
    testsuite_fail_if(res != THEMIS_INVALID_PARAMETER, "decrypt: no passphrase (2)");

    res = themis_secure_cell_decrypt_seal_with_passphrase(passphrase,
                                                          0,
                                                          context,
                                                          sizeof(context),
                                                          encrypted,
                                                          encrypted_length,
                                                          decrypted,
                                                          &decrypted_length);
    testsuite_fail_if(res != THEMIS_INVALID_PARAMETER, "decrypt: no passphrase (3)");

    /* decrypt: context is optional, but must be non-NULL if used */

    res = themis_secure_cell_decrypt_seal_with_passphrase(passphrase,
                                                          sizeof(passphrase),
                                                          NULL,
                                                          0,
                                                          encrypted,
                                                          encrypted_length,
                                                          decrypted,
                                                          &decrypted_length);
    testsuite_fail_if(res != THEMIS_FAIL, "decrypt: no context (1)");

    res = themis_secure_cell_decrypt_seal_with_passphrase(passphrase,
                                                          sizeof(passphrase),
                                                          NULL,
                                                          sizeof(context),
                                                          encrypted,
                                                          encrypted_length,
                                                          decrypted,
                                                          &decrypted_length);
    testsuite_fail_if(res != THEMIS_INVALID_PARAMETER, "decrypt: no context (2)");

    /* decrypt: message is required and must be non-empty */

    res = themis_secure_cell_decrypt_seal_with_passphrase(passphrase,
                                                          sizeof(passphrase),
                                                          context,
                                                          sizeof(context),
                                                          NULL,
                                                          encrypted_length,
                                                          decrypted,
                                                          &decrypted_length);
    testsuite_fail_if(res != THEMIS_INVALID_PARAMETER, "decrypt: no message (1)");

    res = themis_secure_cell_decrypt_seal_with_passphrase(passphrase,
                                                          sizeof(passphrase),
                                                          context,
                                                          sizeof(context),
                                                          NULL,
                                                          0,
                                                          decrypted,
                                                          &decrypted_length);
    testsuite_fail_if(res != THEMIS_INVALID_PARAMETER, "decrypt: no message (2)");

    res = themis_secure_cell_decrypt_seal_with_passphrase(passphrase,
                                                          sizeof(passphrase),
                                                          context,
                                                          sizeof(context),
                                                          encrypted,
                                                          0,
                                                          decrypted,
                                                          &decrypted_length);
    testsuite_fail_if(res != THEMIS_INVALID_PARAMETER, "decrypt: no message (3)");

    /* decrypt: output length is required */

    res = themis_secure_cell_decrypt_seal_with_passphrase(passphrase,
                                                          sizeof(passphrase),
                                                          context,
                                                          sizeof(context),
                                                          encrypted,
                                                          encrypted_length,
                                                          decrypted,
                                                          NULL);
    testsuite_fail_if(res != THEMIS_INVALID_PARAMETER, "decrypt: no output length (1)");

    res = themis_secure_cell_decrypt_seal_with_passphrase(passphrase,
                                                          sizeof(passphrase),
                                                          context,
                                                          sizeof(context),
                                                          encrypted,
                                                          encrypted_length,
                                                          NULL,
                                                          NULL);
    testsuite_fail_if(res != THEMIS_INVALID_PARAMETER, "decrypt: no output length (2)");
}

static void scell_seal_passphrase_parameters(void)
{
    scell_seal_passphrase_parameters_encrypt();
    scell_seal_passphrase_parameters_decrypt();
}

static void scell_seal_passphrase_compatibility_passphrase_to_master_key(void)
{
    themis_status_t res = THEMIS_FAIL;
    const uint8_t passphrase[] = "look to the sky, way up on high";
    const uint8_t message[] = "there in the night stars are now right";
    const uint8_t context[] = "eons have passed: now then at last";
    uint8_t encrypted[MAX_MESSAGE_SIZE];
    uint8_t decrypted[MAX_MESSAGE_SIZE];
    size_t encrypted_length = sizeof(encrypted);
    size_t decrypted_length = sizeof(decrypted);

    res = themis_secure_cell_encrypt_seal_with_passphrase(passphrase,
                                                          sizeof(passphrase),
                                                          context,
                                                          sizeof(context),
                                                          message,
                                                          sizeof(message),
                                                          NULL,
                                                          &encrypted_length);
    if (res != THEMIS_BUFFER_TOO_SMALL) {
        testsuite_fail_if(true, "encrypt(passphrase): *** failed to compute output length");
        return;
    }
    res = themis_secure_cell_encrypt_seal_with_passphrase(passphrase,
                                                          sizeof(passphrase),
                                                          context,
                                                          sizeof(context),
                                                          message,
                                                          sizeof(message),
                                                          encrypted,
                                                          &encrypted_length);
    if (res != THEMIS_SUCCESS) {
        testsuite_fail_if(true, "encrypt(passphrase): *** failed to process message");
        return;
    }

    res = themis_secure_cell_decrypt_seal(passphrase,
                                          sizeof(passphrase),
                                          context,
                                          sizeof(context),
                                          encrypted,
                                          encrypted_length,
                                          NULL,
                                          &decrypted_length);
    testsuite_fail_if(res != THEMIS_BUFFER_TOO_SMALL, "decrypt(master key): compute output length");

    res = themis_secure_cell_decrypt_seal(passphrase,
                                          sizeof(passphrase),
                                          context,
                                          sizeof(context),
                                          encrypted,
                                          encrypted_length,
                                          decrypted,
                                          &decrypted_length);
    testsuite_fail_if(res != THEMIS_FAIL, "decrypt(master key): cannot decrypt passphrase");
}

static void scell_seal_passphrase_compatibility_master_key_to_passphrase(void)
{
    themis_status_t res = THEMIS_FAIL;
    const uint8_t passphrase[] = "prison walls break, old ones awake!";
    const uint8_t message[] = "they will return, mankind will learn";
    const uint8_t context[] = "new kinds of fear when they are here";
    uint8_t encrypted[MAX_MESSAGE_SIZE];
    uint8_t decrypted[MAX_MESSAGE_SIZE];
    size_t encrypted_length = sizeof(encrypted);
    size_t decrypted_length = sizeof(decrypted);

    res = themis_secure_cell_encrypt_seal(passphrase,
                                          sizeof(passphrase),
                                          context,
                                          sizeof(context),
                                          message,
                                          sizeof(message),
                                          NULL,
                                          &encrypted_length);
    if (res != THEMIS_BUFFER_TOO_SMALL) {
        testsuite_fail_if(true, "encrypt(master key): *** failed to compute output length");
        return;
    }
    res = themis_secure_cell_encrypt_seal(passphrase,
                                          sizeof(passphrase),
                                          context,
                                          sizeof(context),
                                          message,
                                          sizeof(message),
                                          encrypted,
                                          &encrypted_length);
    if (res != THEMIS_SUCCESS) {
        testsuite_fail_if(true, "encrypt(master key): *** failed to process message");
        return;
    }

    res = themis_secure_cell_decrypt_seal_with_passphrase(passphrase,
                                                          sizeof(passphrase),
                                                          context,
                                                          sizeof(context),
                                                          encrypted,
                                                          encrypted_length,
                                                          NULL,
                                                          &decrypted_length);
    testsuite_fail_if(res != THEMIS_BUFFER_TOO_SMALL, "decrypt(passphrase): compute output length");

    res = themis_secure_cell_decrypt_seal_with_passphrase(passphrase,
                                                          sizeof(passphrase),
                                                          context,
                                                          sizeof(context),
                                                          encrypted,
                                                          encrypted_length,
                                                          decrypted,
                                                          &decrypted_length);
    testsuite_fail_if(res != THEMIS_FAIL, "decrypt(passphrase): cannot decrypt master key");
}

static void scell_seal_passphrase_compatibility(void)
{
    scell_seal_passphrase_compatibility_passphrase_to_master_key();
    scell_seal_passphrase_compatibility_master_key_to_passphrase();
}

static void scell_seal_passphrase_stability(void)
{
    themis_status_t res = THEMIS_FAIL;
    const uint8_t passphrase[] = "never gonna give you up";
    const uint8_t message[] = "never gonna let you down";
    const uint8_t context[] = "never gonna run around and desert you";
    struct test_data {
        const char* label;
        size_t length;
        const uint8_t* data;
    };
    const struct test_data encrypted_data[] = {
        {"Themis 0.13: AES-256",
         95,
         (const uint8_t*)"\x00\x01\x01\x41\x0C\x00\x00\x00\x10\x00\x00\x00\x19\x00\x00\x00\x16\x00\x00\x00\xAE\x92\x0A\x3D\x40\xF6\x62\x39\xB5\xD8\x8B\x39\x3D\x12\x38\xDB\xF0\x07\xA7\x38\xCE\x8E\xAC\x1B\x26\x42\x64\x01\x40\x0D\x03\x00\x10\x00\x4C\xC6\xC5\x69\xB1\x25\xD7\xA1\x24\x57\x17\x54\xA0\xE2\x08\xDA\xB5\xEE\xFF\xC8\x21\x47\xDE\xBD\x30\xBE\x44\xE3\x2B\x01\xE7\x73\x54\x2A\x0F\xE9\x75\x4C\x66\x71\x0B"},
        {"Themis 0.13: AES-192",
         95,
         (const uint8_t*)"\xC0\x00\x01\x41\x0C\x00\x00\x00\x10\x00\x00\x00\x19\x00\x00\x00\x16\x00\x00\x00\x6C\x78\xED\x9F\xFB\x8F\x60\x21\xDC\xA8\xD1\xD5\x5F\x8D\x90\x53\x97\x56\xB8\x50\xB2\xD9\x71\x24\xE3\x7C\x35\x73\x40\x0D\x03\x00\x10\x00\x07\x86\xCA\xAF\x50\x96\x38\xBD\x16\xB6\x50\xE6\x70\x34\xF7\xFB\xD0\x64\xE1\xFA\x99\x52\x60\x5E\x63\x59\x8B\x5C\xF7\xF2\x68\x28\x3A\x25\x47\xCD\x61\xBB\xDE\xFC\x95"},
        {"Themis 0.13: AES-128",
         95,
         (const uint8_t*)"\x80\x00\x01\x41\x0C\x00\x00\x00\x10\x00\x00\x00\x19\x00\x00\x00\x16\x00\x00\x00\x2C\x09\xD9\x09\x7A\xF6\x51\x6F\xBA\x1A\x7B\x65\x1D\x53\x3B\x1B\xC3\x7A\x5E\xFF\xD1\x96\x5E\xA9\x19\x47\x64\x3F\x40\x0D\x03\x00\x10\x00\x16\x40\x5E\x4F\xC6\x22\x30\xE8\xAF\xF8\xB2\x23\xD6\x6C\x82\x02\x9D\xDE\x21\x8F\x46\xB0\x44\x30\x81\xA8\x7F\x92\x4E\x78\x05\xA8\xA6\x25\x16\x6F\x8E\xC1\x24\x53\x8A"},
    };

    for (size_t i = 0; i < ARRAY_SIZE(encrypted_data); i++) {
        uint8_t* decrypted = NULL;
        size_t decrypted_length = 0;

        res = themis_secure_cell_decrypt_seal_with_passphrase(passphrase,
                                                              sizeof(passphrase),
                                                              context,
                                                              sizeof(context),
                                                              encrypted_data[i].data,
                                                              encrypted_data[i].length,
                                                              NULL,
                                                              &decrypted_length);
        testsuite_fail_if(res != THEMIS_BUFFER_TOO_SMALL, "decrypt: compute output length");

        decrypted = malloc(decrypted_length);
        if (!decrypted) {
            testsuite_fail_if(true, "decrypt: *** failed to allocate output buffer");
            return;
        }

        res = themis_secure_cell_decrypt_seal_with_passphrase(passphrase,
                                                              sizeof(passphrase),
                                                              context,
                                                              sizeof(context),
                                                              encrypted_data[i].data,
                                                              encrypted_data[i].length,
                                                              decrypted,
                                                              &decrypted_length);
        testsuite_fail_if(res != THEMIS_SUCCESS, "decrypt: process message");

        testsuite_fail_unless(decrypted_length == sizeof(message), "validate: message length");
        testsuite_fail_unless(!memcmp(message, decrypted, decrypted_length),
                              "validate: message content");

        testsuite_fail_if(false, encrypted_data[i].label);

        free(decrypted);
    }
}

void run_secure_cell_test(void)
{
    testsuite_enter_suite("secure cell: key generation");
    testsuite_run_test(secure_cell_key_generation);

    testsuite_enter_suite("secure cell: basic flow");
    testsuite_run_test(secure_cell_test);
    testsuite_run_test(secure_cell_test_lengths);

    testsuite_enter_suite("secure cell: api test");
    testsuite_run_test(secure_cell_api_test);

    testsuite_enter_suite("secure cell: context corruption");
    testsuite_run_test(secure_cell_context_corruption);

    testsuite_enter_suite("secure cell: seal mode: passphrases");
    testsuite_run_test(scell_seal_passphrase_operation);
    testsuite_run_test(scell_seal_passphrase_parameters);
    testsuite_run_test(scell_seal_passphrase_compatibility);
    testsuite_run_test(scell_seal_passphrase_stability);
}
