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
#define MAX_MESSAGE_SIZE 2048
#define MAX_ENCRYPTED_MESSAGE_SIZE (MAX_MESSAGE_SIZE + 1024)
#define MESSAGES_TO_SEND 3

#define MAX_KEY_SIZE 4096
#define RSA_ALG 1
#define EC_ALG 2
#define test_check(function_call, success_res, msg) \
    {                                               \
        themis_status_t res = function_call;        \
        if (res != (success_res)) {                 \
            testsuite_fail_if(true, msg);           \
            return -1;                              \
        }                                           \
    }

#define test_check_free(function_call, success_res, msg, free_condition) \
    {                                                                    \
        themis_status_t res = function_call;                             \
        if (res != (success_res)) {                                      \
            testsuite_fail_if(true, msg);                                \
            free_condition;                                              \
            return -1;                                                   \
        }                                                                \
    }

/*
 * Allow usage of deprecated Secure Message interface:
 *   - themis_secure_message_wrap()
 *   - themis_secure_message_unwrap()
 */
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

static themis_status_t themis_gen_key_pair(int alg,
                                           uint8_t* private_key,
                                           size_t* private_key_length,
                                           uint8_t* public_key,
                                           size_t* public_key_length)
{
    themis_status_t res = THEMIS_FAIL;
    if (alg == RSA_ALG) {
        res = themis_gen_rsa_key_pair(private_key, private_key_length, public_key, public_key_length);
    } else if (alg == EC_ALG) {
        res = themis_gen_ec_key_pair(private_key, private_key_length, public_key, public_key_length);
    }
    return res;
}

static int generic_themis_secure_message_encrypt_decrypt_test(int alg,
                                                              const uint8_t* message,
                                                              const size_t message_length)
{
    int res = -1;
    themis_status_t status = THEMIS_FAIL;

    uint8_t private_key[MAX_KEY_SIZE] = {0};
    uint8_t public_key[MAX_KEY_SIZE] = {0};
    size_t private_key_length = sizeof(private_key);
    size_t public_key_length = sizeof(public_key);

    uint8_t* encrypted_message = NULL;
    uint8_t* decrypted_message = NULL;
    size_t encrypted_message_length = 0;
    size_t decrypted_message_length = 0;

    status = themis_gen_key_pair(alg, private_key, &private_key_length, public_key, &public_key_length);
    if (status != THEMIS_SUCCESS) {
        testsuite_fail_if(true, "themis_gen_key_pair failed");
        goto out;
    }

    status = themis_secure_message_encrypt(private_key,
                                           private_key_length,
                                           public_key,
                                           public_key_length,
                                           message,
                                           message_length,
                                           NULL,
                                           &encrypted_message_length);
    if (status != THEMIS_BUFFER_TOO_SMALL) {
        testsuite_fail_if(true,
                          "themis_secure_message_encrypt failed to determine encrypted message length");
        goto out;
    }

    encrypted_message = malloc(encrypted_message_length);
    if (!encrypted_message) {
        testsuite_fail_if(true, "failed to allocate memory for encrypted message");
        goto out;
    }

    status = themis_secure_message_encrypt(private_key,
                                           private_key_length,
                                           public_key,
                                           public_key_length,
                                           message,
                                           message_length,
                                           encrypted_message,
                                           &encrypted_message_length);
    if (status != THEMIS_SUCCESS) {
        testsuite_fail_if(true, "themis_secure_message_encrypt failed to encrypt message");
        goto out;
    }

    status = themis_secure_message_decrypt(private_key,
                                           private_key_length,
                                           public_key,
                                           public_key_length,
                                           encrypted_message,
                                           encrypted_message_length,
                                           NULL,
                                           &decrypted_message_length);
    if (status != THEMIS_BUFFER_TOO_SMALL) {
        testsuite_fail_if(true,
                          "themis_secure_message_decrypt failed to determine decrypted message length");
        goto out;
    }

    decrypted_message = malloc(decrypted_message_length);
    if (!decrypted_message) {
        testsuite_fail_if(true, "failed to allocate memory for decrypted message");
        goto out;
    }

    status = themis_secure_message_decrypt(private_key,
                                           private_key_length,
                                           public_key,
                                           public_key_length,
                                           encrypted_message,
                                           encrypted_message_length,
                                           decrypted_message,
                                           &decrypted_message_length);
    if (status != THEMIS_SUCCESS) {
        testsuite_fail_if(true, "themis_secure_message_decrypt failed to decrypt message");
        goto out;
    }

    if (decrypted_message_length != message_length) {
        testsuite_fail_if(true,
                          "themis_secure_message_encrypt/decrypt does not preserve message length");
        goto out;
    }

    if (memcmp(decrypted_message, message, message_length) != 0) {
        testsuite_fail_if(true,
                          "themis_secure_message_encrypt/decrypt does not preserve message content");
        goto out;
    }

    res = 0;

out:
    free(decrypted_message);
    free(encrypted_message);
    return res;
}

static int generic_themis_secure_message_sign_verify_test(int alg,
                                                          const uint8_t* message,
                                                          const size_t message_length)
{
    int res = -1;
    themis_status_t status = THEMIS_FAIL;

    uint8_t private_key[MAX_KEY_SIZE] = {0};
    uint8_t public_key[MAX_KEY_SIZE] = {0};
    size_t private_key_length = sizeof(private_key);
    size_t public_key_length = sizeof(public_key);

    uint8_t* signed_message = NULL;
    uint8_t* verified_message = NULL;
    size_t signed_message_length = 0;
    size_t verified_message_length = 0;

    status = themis_gen_key_pair(alg, private_key, &private_key_length, public_key, &public_key_length);
    if (status != THEMIS_SUCCESS) {
        testsuite_fail_if(true, "themis_gen_key_pair failed");
        goto out;
    }

    status = themis_secure_message_sign(private_key,
                                        private_key_length,
                                        message,
                                        message_length,
                                        NULL,
                                        &signed_message_length);
    if (status != THEMIS_BUFFER_TOO_SMALL) {
        testsuite_fail_if(true, "themis_secure_message_sign failed to determine signed message length");
        goto out;
    }

    signed_message = malloc(signed_message_length);
    if (!signed_message) {
        testsuite_fail_if(true, "failed to allocate memory for signed message");
        goto out;
    }

    status = themis_secure_message_sign(private_key,
                                        private_key_length,
                                        message,
                                        message_length,
                                        signed_message,
                                        &signed_message_length);
    if (status != THEMIS_SUCCESS) {
        testsuite_fail_if(true, "themis_secure_message_sign failed to sign message");
        goto out;
    }

    status = themis_secure_message_verify(public_key,
                                          public_key_length,
                                          signed_message,
                                          signed_message_length,
                                          NULL,
                                          &verified_message_length);
    if (status != THEMIS_BUFFER_TOO_SMALL) {
        testsuite_fail_if(true,
                          "themis_secure_message_verify failed to determine verified message length");
        goto out;
    }

    verified_message = malloc(verified_message_length);
    if (!verified_message) {
        testsuite_fail_if(true, "failed to allocate memory for verified message");
        goto out;
    }

    status = themis_secure_message_verify(public_key,
                                          public_key_length,
                                          signed_message,
                                          signed_message_length,
                                          verified_message,
                                          &verified_message_length);
    if (status != THEMIS_SUCCESS) {
        testsuite_fail_if(true, "themis_secure_message_verify failed to verify message");
        goto out;
    }

    if (verified_message_length != message_length) {
        testsuite_fail_if(true, "themis_secure_message_sign/verify does not preserve message length");
        goto out;
    }

    if (memcmp(verified_message, message, message_length) != 0) {
        testsuite_fail_if(true, "themis_secure_message_sign/verify does not preserve message content");
        goto out;
    }

    res = 0;

out:
    free(verified_message);
    free(signed_message);
    return res;
}

static int themis_secure_signed_message_generic_test(int alg, const char* message, const size_t message_length)
{
    uint8_t private_key[10240];
    size_t private_key_length = 10240;
    uint8_t public_key[10240];
    size_t public_key_length = 10240;

    themis_status_t res;

    res = themis_gen_key_pair(alg, private_key, &private_key_length, public_key, &public_key_length);

    if (res != THEMIS_SUCCESS) {
        testsuite_fail_if(res != THEMIS_SUCCESS, "themis_gen_key_pair fail");
        return -1;
    }

    uint8_t* wrapped_message = NULL;
    size_t wrapped_message_length = 0;

    res = themis_secure_message_wrap(private_key,
                                     private_key_length + 1,
                                     NULL,
                                     0,
                                     (uint8_t*)message,
                                     message_length,
                                     NULL,
                                     &wrapped_message_length);
    if (res != THEMIS_FAIL) {
        testsuite_fail_if(res != THEMIS_BUFFER_TOO_SMALL,
                          "themis_secure_message_wrap (incorrect private key) fail");
        return -2;
    }

    res = themis_secure_message_wrap(private_key,
                                     private_key_length,
                                     NULL,
                                     0,
                                     (uint8_t*)message,
                                     message_length,
                                     NULL,
                                     &wrapped_message_length);
    if (res != THEMIS_BUFFER_TOO_SMALL) {
        testsuite_fail_if(res != THEMIS_BUFFER_TOO_SMALL,
                          "themis_secure_message_wrap (wrapped_message_length determination) fail");
        return -2;
    }

    wrapped_message = malloc(wrapped_message_length);
    if (!wrapped_message) {
        testsuite_fail_if(!wrapped_message, "malloc fail");
        return -3;
    }
    res = themis_secure_message_wrap(private_key,
                                     private_key_length,
                                     NULL,
                                     0,
                                     (uint8_t*)message,
                                     message_length,
                                     wrapped_message,
                                     &wrapped_message_length);
    if (res != THEMIS_SUCCESS) {
        free(wrapped_message);
        testsuite_fail_if(res != THEMIS_SUCCESS, "themis_secure_message_wrap fail");
        return -4;
    }

    uint8_t* unwrapped_message = NULL;
    size_t unwrapped_message_length = 0;

    res = themis_secure_message_unwrap(NULL,
                                       0,
                                       public_key,
                                       public_key_length + 1,
                                       wrapped_message,
                                       wrapped_message_length,
                                       NULL,
                                       &unwrapped_message_length);
    if (res != THEMIS_FAIL) {
        free(wrapped_message);
        testsuite_fail_if(res != THEMIS_BUFFER_TOO_SMALL,
                          "themis_secure_message_unwrap (incorrect public key) fail");
        return -5;
    }

    res = themis_secure_message_unwrap(NULL,
                                       0,
                                       public_key,
                                       public_key_length,
                                       wrapped_message,
                                       wrapped_message_length,
                                       NULL,
                                       &unwrapped_message_length);
    if (res != THEMIS_BUFFER_TOO_SMALL) {
        free(wrapped_message);
        testsuite_fail_if(res != THEMIS_BUFFER_TOO_SMALL,
                          "themis_secure_message_unwrap (unwrapped_message_length determination) fail");
        return -5;
    }
    unwrapped_message = malloc(unwrapped_message_length);
    if (!unwrapped_message) {
        free(wrapped_message);
        testsuite_fail_if(!unwrapped_message, "malloc fail");
        return -3;
    }
    res = themis_secure_message_unwrap(NULL,
                                       0,
                                       public_key,
                                       public_key_length,
                                       wrapped_message,
                                       wrapped_message_length,
                                       unwrapped_message,
                                       &unwrapped_message_length);
    if (res != THEMIS_SUCCESS) {
        free(wrapped_message);
        free(unwrapped_message);
        testsuite_fail_if(res != THEMIS_SUCCESS, "themis_secure_message_unwrap fail");
        return -2;
    }

    if ((message_length != unwrapped_message_length)
        || (memcmp(message, unwrapped_message, message_length) != 0)) {
        free(wrapped_message);
        free(unwrapped_message);
        testsuite_fail_if(true, "message not equal unwrapped_message_length");
        return -3;
    }
    free(wrapped_message);
    free(unwrapped_message);
    return 0;
}

static int themis_secure_encrypted_message_generic_test(int alg,
                                                        const char* message,
                                                        const size_t message_length)
{
    uint8_t private_key[10240];
    size_t private_key_length = 10240;
    uint8_t public_key[10240];
    size_t public_key_length = 10240;

    uint8_t peer_private_key[10240];
    size_t peer_private_key_length = 10240;
    uint8_t peer_public_key[10240];
    size_t peer_public_key_length = 10240;

    test_check(themis_gen_key_pair(alg, private_key, &private_key_length, public_key, &public_key_length),
               THEMIS_SUCCESS,
               "gen key pair fail");
    test_check(themis_gen_key_pair(alg,
                                   peer_private_key,
                                   &peer_private_key_length,
                                   peer_public_key,
                                   &peer_public_key_length),
               THEMIS_SUCCESS,
               "gen peer key pair fail");

    uint8_t* wrapped_message = NULL;
    size_t wrapped_message_length = 0;

    // themis detect something wrong at different stage with different algorithms. that why
    // different status
    if (alg == EC_ALG) {
        test_check(themis_secure_message_wrap(private_key,
                                              private_key_length + 1,
                                              peer_public_key,
                                              peer_public_key_length,
                                              (uint8_t*)message,
                                              message_length,
                                              NULL,
                                              &wrapped_message_length),
                   THEMIS_INVALID_PARAMETER,
                   "themis secure message wrap (incorrect private key) failed");
        test_check(themis_secure_message_wrap(private_key,
                                              private_key_length,
                                              peer_public_key,
                                              peer_public_key_length + 1,
                                              (uint8_t*)message,
                                              message_length,
                                              NULL,
                                              &wrapped_message_length),
                   THEMIS_INVALID_PARAMETER,
                   "themis secure message wrap (incorrect public key) failed");
    } else {
        test_check(themis_secure_message_wrap(private_key,
                                              private_key_length + 1,
                                              peer_public_key,
                                              peer_public_key_length,
                                              (uint8_t*)message,
                                              message_length,
                                              NULL,
                                              &wrapped_message_length),
                   THEMIS_BUFFER_TOO_SMALL,
                   "themis secure message wrap (incorrect private key) failed");
        test_check(themis_secure_message_wrap(private_key,
                                              private_key_length,
                                              peer_public_key,
                                              peer_public_key_length + 1,
                                              (uint8_t*)message,
                                              message_length,
                                              NULL,
                                              &wrapped_message_length),
                   THEMIS_INVALID_PARAMETER,
                   "themis secure message wrap (incorrect public key) failed");
    }

    test_check(themis_secure_message_wrap(private_key,
                                          private_key_length,
                                          peer_public_key,
                                          peer_public_key_length,
                                          (uint8_t*)message,
                                          message_length,
                                          NULL,
                                          &wrapped_message_length),
               THEMIS_BUFFER_TOO_SMALL,
               "themis secure message wrap (wrapped message length determination) failed");
    wrapped_message = malloc(wrapped_message_length);
    if (!wrapped_message) {
        testsuite_fail_if(!wrapped_message, "malloc fail (wrapped_message)");
        return -2;
    }
    test_check_free(themis_secure_message_wrap(private_key,
                                               private_key_length,
                                               peer_public_key,
                                               peer_public_key_length,
                                               (uint8_t*)message,
                                               message_length,
                                               wrapped_message,
                                               &wrapped_message_length),
                    THEMIS_SUCCESS,
                    "themis secure message wrap failed",
                    free(wrapped_message));

    uint8_t* unwrapped_message = NULL;
    size_t unwrapped_message_length = 0;

    // themis detect something wrong at different stage with different algorithms. that why
    // different status
    if (alg == EC_ALG) {
        test_check_free(themis_secure_message_unwrap(peer_private_key,
                                                     private_key_length + 1,
                                                     public_key,
                                                     public_key_length,
                                                     (uint8_t*)wrapped_message,
                                                     wrapped_message_length,
                                                     NULL,
                                                     &unwrapped_message_length),
                        THEMIS_INVALID_PARAMETER,
                        "themis secure message unwrap (incorrect private key) failed",
                        free(wrapped_message));
        test_check_free(themis_secure_message_unwrap(peer_private_key,
                                                     private_key_length,
                                                     public_key,
                                                     public_key_length + 1,
                                                     (uint8_t*)wrapped_message,
                                                     wrapped_message_length,
                                                     NULL,
                                                     &unwrapped_message_length),
                        THEMIS_INVALID_PARAMETER,
                        "themis secure message unwrap (incorrect public key) failed",
                        free(wrapped_message));
    } else {
        test_check_free(themis_secure_message_unwrap(peer_private_key,
                                                     private_key_length + 1,
                                                     public_key,
                                                     public_key_length,
                                                     (uint8_t*)wrapped_message,
                                                     wrapped_message_length,
                                                     NULL,
                                                     &unwrapped_message_length),
                        THEMIS_INVALID_PARAMETER,
                        "themis secure message unwrap (incorrect private key) failed",
                        free(wrapped_message));
        test_check_free(themis_secure_message_unwrap(peer_private_key,
                                                     private_key_length,
                                                     public_key,
                                                     public_key_length + 1,
                                                     (uint8_t*)wrapped_message,
                                                     wrapped_message_length,
                                                     NULL,
                                                     &unwrapped_message_length),
                        THEMIS_BUFFER_TOO_SMALL,
                        "themis secure message unwrap (incorrect public key) failed",
                        free(wrapped_message));
    }
    test_check_free(themis_secure_message_unwrap(peer_private_key,
                                                 private_key_length,
                                                 public_key,
                                                 public_key_length,
                                                 (uint8_t*)wrapped_message,
                                                 wrapped_message_length,
                                                 NULL,
                                                 &unwrapped_message_length),
                    THEMIS_BUFFER_TOO_SMALL,
                    "themis secure message unwrap (unwrapped message length determination) failed",
                    free(wrapped_message));
    unwrapped_message = malloc(unwrapped_message_length);
    if (!unwrapped_message) {
        testsuite_fail_if(!unwrapped_message, "malloc fail (unwrapped_message)");
        return -2;
    }
    test_check_free(themis_secure_message_unwrap(peer_private_key,
                                                 peer_private_key_length,
                                                 public_key,
                                                 public_key_length,
                                                 (uint8_t*)wrapped_message,
                                                 wrapped_message_length,
                                                 unwrapped_message,
                                                 &unwrapped_message_length),
                    THEMIS_SUCCESS,
                    "themis secure message unwrap failed",
                    (free(wrapped_message), free(unwrapped_message)));

    if ((message_length != unwrapped_message_length)
        || (memcmp(message, unwrapped_message, message_length) != 0)) {
        free(wrapped_message);
        free(unwrapped_message);
        testsuite_fail_if(true, "message not equal unwrapped_message");
        return -3;
    }
    free(wrapped_message);
    free(unwrapped_message);
    return 0;
}

static void themis_secure_message_test(void)
{
    char message[] = "Hit http://ftp.us.debian.org[1] wheezy Release.gpg"
                     "Hit http://ftp.us.debian.org[2] wheezy-updates Release.gpg"
                     "Hit http://ftp.us.debian.org[3] wheezy Release"
                     "Hit http://ftp.us.debian.org[4] wheezy-updates Release"
                     "Hit http://ftp.us.debian.org[5] wheezy/main Sources"
                     "Hit http://ftp.us.debian.org[6] wheezy/non-free Sources"
                     "Hit http://ftp.us.debian.org[7] wheezy/contrib Sources"
                     "Hit http://ftp.us.debian.org[8] wheezy/main i386 Packages"
                     "Hit http://ftp.us.debian.org[9] wheezy/non-free i386 Packages"
                     "Hit http://ftp.us.debian.org[10] wheezy/contrib i386 Packages"
                     "Hit http://ftp.us.debian.org[11] wheezy/contrib Translation-en";

    const uint8_t* message_bytes = (const uint8_t*)message;
    size_t message_length = strlen(message);

    testsuite_fail_if(generic_themis_secure_message_encrypt_decrypt_test(RSA_ALG,
                                                                         message_bytes,
                                                                         message_length),
                      "themis secure encrypt/decrypt message (RSA)");
    testsuite_fail_if(generic_themis_secure_message_encrypt_decrypt_test(EC_ALG, message_bytes, message_length),
                      "themis secure encrypt/decrypt message (EC)");
    testsuite_fail_if(generic_themis_secure_message_sign_verify_test(RSA_ALG, message_bytes, message_length),
                      "themis secure sign/verify message (RSA)");
    testsuite_fail_if(generic_themis_secure_message_sign_verify_test(EC_ALG, message_bytes, message_length),
                      "themis secure sign/verify message (EC)");

    testsuite_fail_if(themis_secure_signed_message_generic_test(RSA_ALG, message, message_length),
                      "(deprecated) themis secure signed message (RSA)");
    testsuite_fail_if(themis_secure_signed_message_generic_test(EC_ALG, message, message_length),
                      "(deprecated) themis secure signed message (EC)");
    testsuite_fail_if(themis_secure_encrypted_message_generic_test(RSA_ALG, message, message_length),
                      "(deprecated) themis secure encrypted message (RSA)");
    testsuite_fail_if(themis_secure_encrypted_message_generic_test(EC_ALG, message, message_length),
                      "(deprecated) themis secure encrypted message (EC)");
}

static void themis_secure_message_encrypt_decrypt_api_test(void)
{
    themis_status_t status = THEMIS_FAIL;

    uint8_t private_key[MAX_KEY_SIZE] = {0};
    uint8_t public_key[MAX_KEY_SIZE] = {0};
    size_t private_key_length = sizeof(private_key);
    size_t public_key_length = sizeof(public_key);

    uint8_t plaintext[MAX_MESSAGE_SIZE] = {0};
    uint8_t encrypted[MAX_ENCRYPTED_MESSAGE_SIZE] = {0};
    uint8_t decrypted[MAX_MESSAGE_SIZE] = {0};
    size_t plaintext_length = sizeof(plaintext);
    size_t encrypted_length = sizeof(encrypted);
    size_t decrypted_length = sizeof(decrypted);

    status = themis_gen_ec_key_pair(private_key, &private_key_length, public_key, &public_key_length);
    if (status != THEMIS_SUCCESS) {
        testsuite_fail_if(true, "themis_gen_ec_key_pair failed");
        return;
    }

    status = soter_rand(plaintext, plaintext_length);
    if (status != THEMIS_SUCCESS) {
        testsuite_fail_if(true, "soter_rand failed");
        return;
    }

    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_encrypt(NULL,
                                                               private_key_length,
                                                               public_key,
                                                               public_key_length,
                                                               plaintext,
                                                               plaintext_length,
                                                               encrypted,
                                                               &encrypted_length),
                          "themis_secure_message_encrypt: private key is NULL");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_encrypt(private_key,
                                                               0,
                                                               public_key,
                                                               public_key_length,
                                                               plaintext,
                                                               plaintext_length,
                                                               encrypted,
                                                               &encrypted_length),
                          "themis_secure_message_encrypt: private key is empty");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_encrypt(private_key,
                                                               private_key_length - 1,
                                                               public_key,
                                                               public_key_length,
                                                               plaintext,
                                                               plaintext_length,
                                                               encrypted,
                                                               &encrypted_length),
                          "themis_secure_message_encrypt: private key is invalid");

    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_encrypt(private_key,
                                                               private_key_length,
                                                               NULL,
                                                               public_key_length,
                                                               plaintext,
                                                               plaintext_length,
                                                               encrypted,
                                                               &encrypted_length),
                          "themis_secure_message_encrypt: public key is NULL");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_encrypt(private_key,
                                                               private_key_length,
                                                               public_key,
                                                               0,
                                                               plaintext,
                                                               plaintext_length,
                                                               encrypted,
                                                               &encrypted_length),
                          "themis_secure_message_encrypt: public key is empty");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_encrypt(private_key,
                                                               private_key_length,
                                                               public_key,
                                                               public_key_length - 1,
                                                               plaintext,
                                                               plaintext_length,
                                                               encrypted,
                                                               &encrypted_length),
                          "themis_secure_message_encrypt: public key is invalid");

    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_encrypt(public_key,
                                                               public_key_length,
                                                               private_key,
                                                               private_key_length,
                                                               plaintext,
                                                               plaintext_length,
                                                               encrypted,
                                                               &encrypted_length),
                          "themis_secure_message_encrypt: misplaced keys");

    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_encrypt(private_key,
                                                               private_key_length,
                                                               public_key,
                                                               public_key_length,
                                                               NULL,
                                                               plaintext_length,
                                                               encrypted,
                                                               &encrypted_length),
                          "themis_secure_message_encrypt: plaintext is NULL");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_encrypt(private_key,
                                                               private_key_length,
                                                               public_key,
                                                               public_key_length,
                                                               plaintext,
                                                               0,
                                                               encrypted,
                                                               &encrypted_length),
                          "themis_secure_message_encrypt: plaintext is empty");

    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_encrypt(private_key,
                                                               private_key_length,
                                                               public_key,
                                                               public_key_length,
                                                               plaintext,
                                                               plaintext_length,
                                                               NULL,
                                                               NULL),
                          "themis_secure_message_encrypt: encrypted_length is NULL");

    testsuite_fail_unless(THEMIS_BUFFER_TOO_SMALL
                              == themis_secure_message_encrypt(private_key,
                                                               private_key_length,
                                                               public_key,
                                                               public_key_length,
                                                               plaintext,
                                                               plaintext_length,
                                                               NULL,
                                                               &encrypted_length),
                          "themis_secure_message_encrypt: encrypted buffer length");
    encrypted_length -= 1;
    testsuite_fail_unless(THEMIS_BUFFER_TOO_SMALL
                              == themis_secure_message_encrypt(private_key,
                                                               private_key_length,
                                                               public_key,
                                                               public_key_length,
                                                               plaintext,
                                                               plaintext_length,
                                                               encrypted,
                                                               &encrypted_length),
                          "themis_secure_message_encrypt: encrypted buffer too small");

    status = themis_secure_message_encrypt(private_key,
                                           private_key_length,
                                           public_key,
                                           public_key_length,
                                           plaintext,
                                           plaintext_length,
                                           encrypted,
                                           &encrypted_length);
    if (status != THEMIS_SUCCESS) {
        testsuite_fail_if(true, "themis_secure_message_encrypt failed to encrypt");
        return;
    }

    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_decrypt(NULL,
                                                               private_key_length,
                                                               public_key,
                                                               public_key_length,
                                                               encrypted,
                                                               encrypted_length,
                                                               decrypted,
                                                               &decrypted_length),
                          "themis_secure_message_decrypt: private key is NULL");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_decrypt(private_key,
                                                               0,
                                                               public_key,
                                                               public_key_length,
                                                               encrypted,
                                                               encrypted_length,
                                                               decrypted,
                                                               &decrypted_length),
                          "themis_secure_message_decrypt: private key is empty");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_decrypt(private_key,
                                                               private_key_length - 1,
                                                               public_key,
                                                               public_key_length,
                                                               encrypted,
                                                               encrypted_length,
                                                               decrypted,
                                                               &decrypted_length),
                          "themis_secure_message_decrypt: private key is invalid");

    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_decrypt(private_key,
                                                               private_key_length,
                                                               NULL,
                                                               public_key_length,
                                                               encrypted,
                                                               encrypted_length,
                                                               decrypted,
                                                               &decrypted_length),
                          "themis_secure_message_decrypt: public key is NULL");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_decrypt(private_key,
                                                               private_key_length,
                                                               public_key,
                                                               0,
                                                               encrypted,
                                                               encrypted_length,
                                                               decrypted,
                                                               &decrypted_length),
                          "themis_secure_message_decrypt: public key is empty");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_decrypt(private_key,
                                                               private_key_length,
                                                               public_key,
                                                               public_key_length - 1,
                                                               encrypted,
                                                               encrypted_length,
                                                               decrypted,
                                                               &decrypted_length),
                          "themis_secure_message_decrypt: public key is invalid");

    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_decrypt(public_key,
                                                               public_key_length,
                                                               private_key,
                                                               private_key_length,
                                                               encrypted,
                                                               encrypted_length,
                                                               decrypted,
                                                               &decrypted_length),
                          "themis_secure_message_decrypt: misplaced keys");

    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_decrypt(private_key,
                                                               private_key_length,
                                                               public_key,
                                                               public_key_length,
                                                               NULL,
                                                               encrypted_length,
                                                               decrypted,
                                                               &decrypted_length),
                          "themis_secure_message_decrypt: encrypted is NULL");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_decrypt(private_key,
                                                               private_key_length,
                                                               public_key,
                                                               public_key_length,
                                                               encrypted,
                                                               0,
                                                               decrypted,
                                                               &decrypted_length),
                          "themis_secure_message_decrypt: encrypted is empty");

    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_decrypt(private_key,
                                                               private_key_length,
                                                               public_key,
                                                               public_key_length,
                                                               encrypted,
                                                               encrypted_length,
                                                               NULL,
                                                               NULL),
                          "themis_secure_message_decrypt: decrypted_length is NULL");

    testsuite_fail_unless(THEMIS_BUFFER_TOO_SMALL
                              == themis_secure_message_decrypt(private_key,
                                                               private_key_length,
                                                               public_key,
                                                               public_key_length,
                                                               encrypted,
                                                               encrypted_length,
                                                               NULL,
                                                               &decrypted_length),
                          "themis_secure_message_decrypt: decrypted buffer length");
    decrypted_length -= 1;
    testsuite_fail_unless(THEMIS_BUFFER_TOO_SMALL
                              == themis_secure_message_decrypt(private_key,
                                                               private_key_length,
                                                               public_key,
                                                               public_key_length,
                                                               encrypted,
                                                               encrypted_length,
                                                               decrypted,
                                                               &decrypted_length),
                          "themis_secure_message_decrypt: decrypted buffer too small");

    status = themis_secure_message_decrypt(private_key,
                                           private_key_length,
                                           public_key,
                                           public_key_length,
                                           encrypted,
                                           encrypted_length,
                                           decrypted,
                                           &decrypted_length);
    if (status != THEMIS_SUCCESS) {
        testsuite_fail_if(true, "themis_secure_message_decrypt failed to decrypt");
        return;
    }
}

static void themis_secure_message_sign_verify_api_test(void)
{
    themis_status_t status = THEMIS_FAIL;

    uint8_t private_key[MAX_KEY_SIZE] = {0};
    uint8_t public_key[MAX_KEY_SIZE] = {0};
    size_t private_key_length = sizeof(private_key);
    size_t public_key_length = sizeof(public_key);

    uint8_t plaintext[MAX_MESSAGE_SIZE] = {0};
    uint8_t signed_msg[MAX_ENCRYPTED_MESSAGE_SIZE] = {0};
    uint8_t verified[MAX_MESSAGE_SIZE] = {0};
    size_t plaintext_length = sizeof(plaintext);
    size_t signed_msg_length = sizeof(signed_msg);
    size_t verified_length = sizeof(verified);

    status = themis_gen_ec_key_pair(private_key, &private_key_length, public_key, &public_key_length);
    if (status != THEMIS_SUCCESS) {
        testsuite_fail_if(true, "themis_gen_ec_key_pair failed");
        return;
    }

    status = soter_rand(plaintext, plaintext_length);
    if (status != THEMIS_SUCCESS) {
        testsuite_fail_if(true, "soter_rand failed");
        return;
    }

    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_sign(NULL,
                                                            private_key_length,
                                                            plaintext,
                                                            plaintext_length,
                                                            signed_msg,
                                                            &signed_msg_length),
                          "themis_secure_message_sign: private key is NULL");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_sign(private_key,
                                                            0,
                                                            plaintext,
                                                            plaintext_length,
                                                            signed_msg,
                                                            &signed_msg_length),
                          "themis_secure_message_sign: private key is empty");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_sign(private_key,
                                                            private_key_length - 1,
                                                            plaintext,
                                                            plaintext_length,
                                                            signed_msg,
                                                            &signed_msg_length),
                          "themis_secure_message_sign: private key is invalid");

    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_sign(public_key,
                                                            public_key_length,
                                                            plaintext,
                                                            plaintext_length,
                                                            signed_msg,
                                                            &signed_msg_length),
                          "themis_secure_message_sign: using public key");

    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_sign(private_key,
                                                            private_key_length,
                                                            NULL,
                                                            plaintext_length,
                                                            signed_msg,
                                                            &signed_msg_length),
                          "themis_secure_message_sign: plaintext is NULL");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_sign(private_key,
                                                            private_key_length,
                                                            plaintext,
                                                            0,
                                                            signed_msg,
                                                            &signed_msg_length),
                          "themis_secure_message_sign: plaintext is empty");

    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_sign(private_key,
                                                            private_key_length,
                                                            plaintext,
                                                            plaintext_length,
                                                            NULL,
                                                            NULL),
                          "themis_secure_message_sign: signed_msg_length is NULL");

    testsuite_fail_unless(THEMIS_BUFFER_TOO_SMALL
                              == themis_secure_message_sign(private_key,
                                                            private_key_length,
                                                            plaintext,
                                                            plaintext_length,
                                                            NULL,
                                                            &signed_msg_length),
                          "themis_secure_message_sign: signed buffer length");
    signed_msg_length -= 1;
    testsuite_fail_unless(THEMIS_BUFFER_TOO_SMALL
                              == themis_secure_message_sign(private_key,
                                                            private_key_length,
                                                            plaintext,
                                                            plaintext_length,
                                                            signed_msg,
                                                            &signed_msg_length),
                          "themis_secure_message_sign: signed buffer too small");

    status = themis_secure_message_sign(private_key,
                                        private_key_length,
                                        plaintext,
                                        plaintext_length,
                                        signed_msg,
                                        &signed_msg_length);
    if (status != THEMIS_SUCCESS) {
        testsuite_fail_if(true, "themis_secure_message_sign failed to sign");
        return;
    }

    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_verify(NULL,
                                                              public_key_length,
                                                              signed_msg,
                                                              signed_msg_length,
                                                              verified,
                                                              &verified_length),
                          "themis_secure_message_verify: public key is NULL");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_verify(public_key,
                                                              0,
                                                              signed_msg,
                                                              signed_msg_length,
                                                              verified,
                                                              &verified_length),
                          "themis_secure_message_verify: public key is empty");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_verify(public_key,
                                                              public_key_length - 1,
                                                              signed_msg,
                                                              signed_msg_length,
                                                              verified,
                                                              &verified_length),
                          "themis_secure_message_verify: public key is invalid");

    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_verify(private_key,
                                                              private_key_length,
                                                              signed_msg,
                                                              signed_msg_length,
                                                              verified,
                                                              &verified_length),
                          "themis_secure_message_verify: using private key");

    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_verify(public_key,
                                                              public_key_length,
                                                              NULL,
                                                              signed_msg_length,
                                                              verified,
                                                              &verified_length),
                          "themis_secure_message_verify: signed_msg is NULL");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_verify(public_key,
                                                              public_key_length,
                                                              signed_msg,
                                                              0,
                                                              verified,
                                                              &verified_length),
                          "themis_secure_message_verify: signed_msg is empty");

    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_verify(public_key,
                                                              public_key_length,
                                                              signed_msg,
                                                              signed_msg_length,
                                                              NULL,
                                                              NULL),
                          "themis_secure_message_verify: verified_length is NULL");

    testsuite_fail_unless(THEMIS_BUFFER_TOO_SMALL
                              == themis_secure_message_verify(public_key,
                                                              public_key_length,
                                                              signed_msg,
                                                              signed_msg_length,
                                                              NULL,
                                                              &verified_length),
                          "themis_secure_message_verify: verified buffer length");
    verified_length -= 1;
    testsuite_fail_unless(THEMIS_BUFFER_TOO_SMALL
                              == themis_secure_message_verify(public_key,
                                                              public_key_length,
                                                              signed_msg,
                                                              signed_msg_length,
                                                              verified,
                                                              &verified_length),
                          "themis_secure_message_verify: verified buffer too small");

    status = themis_secure_message_verify(public_key,
                                          public_key_length,
                                          signed_msg,
                                          signed_msg_length,
                                          verified,
                                          &verified_length);
    if (status != THEMIS_SUCCESS) {
        testsuite_fail_if(true, "themis_secure_message_verify failed to verify");
        return;
    }
}

static void secure_message_old_api_test(void)
{
    uint8_t plaintext[MAX_MESSAGE_SIZE];
    size_t plaintext_length = 2048; // rand_int(MAX_MESSAGE_SIZE);

    uint8_t ciphertext[MAX_MESSAGE_SIZE + 52]; // chipther text allwais bigger then plain text (24 bytes
                                               // of header + 12 bytes of iv + 16 bytes of auth_tag)
    size_t ciphertext_length = sizeof(ciphertext);

    uint8_t decryptext[MAX_MESSAGE_SIZE];
    size_t decryptext_length = sizeof(decryptext);

    uint8_t priv[MAX_MESSAGE_SIZE];
    size_t priv_length = sizeof(priv);

    uint8_t pub[MAX_MESSAGE_SIZE];
    size_t pub_length = sizeof(pub);

    uint8_t peer_priv[MAX_MESSAGE_SIZE];
    size_t peer_priv_length = sizeof(peer_priv);

    uint8_t peer_pub[MAX_MESSAGE_SIZE];
    size_t peer_pub_length = sizeof(peer_pub);

    themis_status_t res;

    testsuite_fail_unless(THEMIS_BUFFER_TOO_SMALL
                              == themis_gen_ec_key_pair(NULL, &priv_length, pub, &pub_length),
                          "themis_gen_ec_key_pair: get output size (NULL out buffer for private key)");
    priv_length--;
    testsuite_fail_unless(THEMIS_BUFFER_TOO_SMALL
                              == themis_gen_ec_key_pair(priv, &priv_length, pub, &pub_length),
                          "themis_gen_ec_key_pair: get output size (small out buffer for private key)");

    testsuite_fail_unless(THEMIS_BUFFER_TOO_SMALL
                              == themis_gen_ec_key_pair(peer_priv, &priv_length, NULL, &pub_length),
                          "themis_gen_ec_key_pair: get output size (NULL out buffer for public key)");
    pub_length--;
    testsuite_fail_unless(THEMIS_BUFFER_TOO_SMALL
                              == themis_gen_ec_key_pair(peer_priv, &priv_length, pub, &pub_length),
                          "themis_gen_ec_key_pair: get output size (small out buffer for public key)");

    res = themis_gen_ec_key_pair(priv, &priv_length, pub, &pub_length);
    if (THEMIS_SUCCESS != res) {
        testsuite_fail_if(true, "themis_gen_ec_key_pair fail");
        return;
    }

    res = themis_gen_ec_key_pair(peer_priv, &peer_priv_length, peer_pub, &peer_pub_length);
    if (THEMIS_SUCCESS != res) {
        testsuite_fail_if(true, "themis_gen_ec_key_pair fail");
        return;
    }

    if (THEMIS_SUCCESS != soter_rand(plaintext, plaintext_length)) {
        testsuite_fail_if(true, "soter_rand fail");
        return;
    }

    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_wrap(NULL,
                                                            priv_length,
                                                            peer_pub,
                                                            peer_pub_length,
                                                            plaintext,
                                                            plaintext_length,
                                                            ciphertext,
                                                            &ciphertext_length),
                          "themis_secure_message_wrap: invalid private key");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_wrap(priv,
                                                            priv_length - 1,
                                                            peer_pub,
                                                            peer_pub_length,
                                                            plaintext,
                                                            plaintext_length,
                                                            ciphertext,
                                                            &ciphertext_length),
                          "themis_secure_message_wrap: invalid private key length");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_wrap(priv,
                                                            priv_length,
                                                            peer_pub,
                                                            peer_pub_length - 1,
                                                            plaintext,
                                                            plaintext_length,
                                                            ciphertext,
                                                            &ciphertext_length),
                          "themis_secure_message_wrap: invalid peer public key length");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_wrap(priv,
                                                            priv_length,
                                                            peer_pub,
                                                            peer_pub_length,
                                                            NULL,
                                                            plaintext_length,
                                                            ciphertext,
                                                            &ciphertext_length),
                          "themis_secure_message_wrap: invalid plaintext");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_wrap(priv,
                                                            priv_length,
                                                            peer_pub,
                                                            peer_pub_length,
                                                            plaintext,
                                                            0,
                                                            ciphertext,
                                                            &ciphertext_length),
                          "themis_secure_message_wrap: invalid plaintext length");
    testsuite_fail_unless(THEMIS_BUFFER_TOO_SMALL
                              == themis_secure_message_wrap(priv,
                                                            priv_length,
                                                            pub,
                                                            pub_length,
                                                            plaintext,
                                                            plaintext_length,
                                                            NULL,
                                                            &ciphertext_length),
                          "themis_secure_message_wrap: get output size (NULL out buffer)");
    ciphertext_length--;
    testsuite_fail_unless(THEMIS_BUFFER_TOO_SMALL
                              == themis_secure_message_wrap(priv,
                                                            priv_length,
                                                            peer_pub,
                                                            peer_pub_length,
                                                            plaintext,
                                                            plaintext_length,
                                                            ciphertext,
                                                            &ciphertext_length),
                          "themis_secure_message_wrap: get output size (small out buffer)");

    res = themis_secure_message_wrap(priv,
                                     priv_length,
                                     peer_pub,
                                     peer_pub_length,
                                     plaintext,
                                     plaintext_length,
                                     ciphertext,
                                     &ciphertext_length);
    if (THEMIS_SUCCESS != res) {
        testsuite_fail_if(true, "themis_secure_message_wrap fail");
        return;
    }

    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_unwrap(NULL,
                                                              peer_priv_length,
                                                              pub,
                                                              pub_length,
                                                              ciphertext,
                                                              ciphertext_length,
                                                              decryptext,
                                                              &decryptext_length),
                          "themis_secure_message_unwrap: invalid private key");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_unwrap(peer_priv,
                                                              peer_priv_length - 1,
                                                              pub,
                                                              pub_length,
                                                              ciphertext,
                                                              ciphertext_length,
                                                              decryptext,
                                                              &decryptext_length),
                          "themis_secure_message_unwrap: invalid private key length");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_unwrap(peer_priv,
                                                              peer_priv_length,
                                                              pub,
                                                              pub_length - 1,
                                                              ciphertext,
                                                              ciphertext_length,
                                                              decryptext,
                                                              &decryptext_length),
                          "themis_secure_message_unwrap: invalid peer public key length");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_unwrap(peer_priv,
                                                              peer_priv_length,
                                                              pub,
                                                              pub_length,
                                                              NULL,
                                                              ciphertext_length,
                                                              decryptext,
                                                              &decryptext_length),
                          "themis_secure_message_unwrap: invalid ciphertext");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_unwrap(peer_priv,
                                                              peer_priv_length,
                                                              pub,
                                                              pub_length,
                                                              ciphertext,
                                                              0,
                                                              decryptext,
                                                              &decryptext_length),
                          "themis_secure_message_unwrap: invalid ciphertext length");
    testsuite_fail_unless(THEMIS_BUFFER_TOO_SMALL
                              == themis_secure_message_unwrap(peer_priv,
                                                              peer_priv_length,
                                                              pub,
                                                              pub_length,
                                                              ciphertext,
                                                              ciphertext_length,
                                                              NULL,
                                                              &decryptext_length),
                          "themis_secure_message_unwrap: get output size (NULL out buffer)");
    decryptext_length--;
    testsuite_fail_unless(THEMIS_BUFFER_TOO_SMALL
                              == themis_secure_message_unwrap(peer_priv,
                                                              peer_priv_length,
                                                              pub,
                                                              pub_length,
                                                              ciphertext,
                                                              ciphertext_length,
                                                              decryptext,
                                                              &decryptext_length),
                          "themis_secure_message_unwrap: get output size (small out buffer)");

    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_unwrap(peer_priv,
                                                              peer_priv_length,
                                                              NULL,
                                                              pub_length,
                                                              ciphertext,
                                                              ciphertext_length,
                                                              decryptext,
                                                              &decryptext_length),
                          "themis_secure_message_unwrap: treating encrypted message as signed (NULL peer public "
                          "key)");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_secure_message_unwrap(peer_priv,
                                                              peer_priv_length,
                                                              pub,
                                                              0,
                                                              ciphertext,
                                                              ciphertext_length,
                                                              decryptext,
                                                              &decryptext_length),
                          "themis_secure_message_unwrap: treating encrypted message as signed "
                          "(zero peer public key length)");

    res = themis_secure_message_unwrap(peer_priv,
                                       peer_priv_length,
                                       pub,
                                       pub_length,
                                       ciphertext,
                                       ciphertext_length,
                                       decryptext,
                                       &decryptext_length);
    if (THEMIS_SUCCESS != res) {
        testsuite_fail_if(true, "themis_secure_message_unwrap fail");
        return;
    }

    testsuite_fail_unless((decryptext_length == plaintext_length),
                          "generic secure message: normal flow");
    testsuite_fail_unless((!memcmp(plaintext, decryptext, plaintext_length)),
                          "generic secure message: normal flow 2");
}

static void key_validation_test(void)
{
    themis_status_t status = THEMIS_FAIL;

    uint8_t ec_private_key[MAX_KEY_SIZE] = {0};
    uint8_t ec_public_key[MAX_KEY_SIZE] = {0};
    uint8_t rsa_private_key[MAX_KEY_SIZE] = {0};
    uint8_t rsa_public_key[MAX_KEY_SIZE] = {0};
    size_t ec_private_key_length = sizeof(ec_private_key);
    size_t ec_public_key_length = sizeof(ec_public_key);
    size_t rsa_private_key_length = sizeof(rsa_private_key);
    size_t rsa_public_key_length = sizeof(rsa_public_key);

    testsuite_fail_unless(THEMIS_INVALID_PARAMETER == themis_gen_ec_key_pair(NULL, NULL, NULL, NULL),
                          "themis_gen_ec_key_pair: null pointers");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER == themis_gen_rsa_key_pair(NULL, NULL, NULL, NULL),
                          "themis_gen_rsa_key_pair: null pointers");

    testsuite_fail_unless(THEMIS_BUFFER_TOO_SMALL
                              == themis_gen_ec_key_pair(NULL, &ec_private_key_length, NULL, &ec_public_key_length),
                          "themis_gen_ec_key_pair: check length");
    testsuite_fail_unless(THEMIS_BUFFER_TOO_SMALL
                              == themis_gen_rsa_key_pair(NULL,
                                                         &rsa_private_key_length,
                                                         NULL,
                                                         &rsa_public_key_length),
                          "   themis_gen_rsa_key_pair: check length");

    ec_private_key_length -= 1;
    ec_public_key_length -= 1;
    rsa_private_key_length -= 1;
    rsa_public_key_length -= 1;
    testsuite_fail_unless(THEMIS_BUFFER_TOO_SMALL
                              == themis_gen_ec_key_pair(ec_private_key,
                                                        &ec_private_key_length,
                                                        ec_public_key,
                                                        &ec_public_key_length),
                          "themis_gen_ec_key_pair: small buffer");
    testsuite_fail_unless(THEMIS_BUFFER_TOO_SMALL
                              == themis_gen_rsa_key_pair(rsa_private_key,
                                                         &rsa_private_key_length,
                                                         rsa_public_key,
                                                         &rsa_public_key_length),
                          "themis_gen_rsa_key_pair: small buffer");

    status = themis_gen_ec_key_pair(ec_private_key,
                                    &ec_private_key_length,
                                    ec_public_key,
                                    &ec_public_key_length);
    if (status != THEMIS_SUCCESS) {
        testsuite_fail_if(true, "themis_gen_ec_key_pair failed");
        return;
    }

    status = themis_gen_rsa_key_pair(rsa_private_key,
                                     &rsa_private_key_length,
                                     rsa_public_key,
                                     &rsa_public_key_length);
    if (status != THEMIS_SUCCESS) {
        testsuite_fail_if(true, "themis_gen_rsa_key_pair failed");
        return;
    }

    testsuite_fail_unless(THEMIS_KEY_EC_PRIVATE
                              == themis_get_asym_key_kind(ec_private_key, ec_private_key_length),
                          "themis_get_asym_key_kind: EC private");
    testsuite_fail_unless(THEMIS_KEY_EC_PUBLIC
                              == themis_get_asym_key_kind(ec_public_key, ec_public_key_length),
                          "themis_get_asym_key_kind: EC public");

    testsuite_fail_unless(THEMIS_KEY_RSA_PRIVATE
                              == themis_get_asym_key_kind(rsa_private_key, rsa_private_key_length),
                          "themis_get_asym_key_kind: RSA private");
    testsuite_fail_unless(THEMIS_KEY_RSA_PUBLIC
                              == themis_get_asym_key_kind(rsa_public_key, rsa_public_key_length),
                          "themis_get_asym_key_kind: RSA public");

    testsuite_fail_unless(THEMIS_SUCCESS
                              == themis_is_valid_asym_key(ec_private_key, ec_private_key_length),
                          "themis_is_valid_asym_key: EC private");
    testsuite_fail_unless(THEMIS_SUCCESS == themis_is_valid_asym_key(ec_public_key, ec_public_key_length),
                          "themis_is_valid_asym_key: EC public");
    testsuite_fail_unless(THEMIS_SUCCESS
                              == themis_is_valid_asym_key(rsa_private_key, rsa_private_key_length),
                          "themis_is_valid_asym_key: RSA private");
    testsuite_fail_unless(THEMIS_SUCCESS
                              == themis_is_valid_asym_key(rsa_public_key, rsa_public_key_length),
                          "themis_is_valid_asym_key: RSA public");

    testsuite_fail_unless(THEMIS_INVALID_PARAMETER == themis_is_valid_asym_key(NULL, 0),
                          "themis_is_valid_asym_key: invalid arguments");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_is_valid_asym_key(rsa_public_key, rsa_public_key_length - 1),
                          "themis_is_valid_asym_key: truncated RSA buffer");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_is_valid_asym_key(ec_private_key, ec_private_key_length - 1),
                          "themis_is_valid_asym_key: truncated EC buffer");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_is_valid_asym_key(rsa_private_key, rsa_private_key_length + 1),
                          "themis_is_valid_asym_key: extended RSA buffer");
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_is_valid_asym_key(ec_public_key, ec_public_key_length + 1),
                          "themis_is_valid_asym_key: extended EC buffer");

    testsuite_fail_unless(THEMIS_KEY_INVALID == themis_get_asym_key_kind(NULL, 0),
                          "themis_get_asym_key_kind: invalid arguments");
    testsuite_fail_unless(THEMIS_KEY_INVALID == themis_get_asym_key_kind(ec_private_key, 2),
                          "themis_get_asym_key_kind: truncated buffer");

    const char* input = "definitely not a valid key";
    testsuite_fail_unless(THEMIS_INVALID_PARAMETER
                              == themis_is_valid_asym_key((const uint8_t*)input, strlen(input)),
                          "themis_is_valid_asym_key: garbage input");
    testsuite_fail_unless(THEMIS_KEY_INVALID
                              == themis_get_asym_key_kind((const uint8_t*)input, strlen(input)),
                          "themis_get_asym_key_kind: garbage input");
}

static void keygen_parameters_ec(void)
{
    themis_status_t status;
    uint8_t ec_private_key[MAX_KEY_SIZE] = {0};
    uint8_t ec_public_key[MAX_KEY_SIZE] = {0};
    size_t ec_private_key_length = sizeof(ec_private_key);
    size_t ec_public_key_length = sizeof(ec_public_key);

    status = themis_gen_ec_key_pair(ec_private_key, &ec_private_key_length, NULL, NULL);
    testsuite_fail_unless(status == THEMIS_INVALID_PARAMETER,
                          "themis_gen_ec_key_pair: only private key requested");

    status = themis_gen_ec_key_pair(NULL, NULL, ec_public_key, &ec_public_key_length);
    testsuite_fail_unless(status == THEMIS_INVALID_PARAMETER,
                          "themis_gen_ec_key_pair: only public key requested");
}

static void keygen_parameters_rsa(void)
{
    themis_status_t status;
    uint8_t rsa_private_key[MAX_KEY_SIZE] = {0};
    uint8_t rsa_public_key[MAX_KEY_SIZE] = {0};
    size_t rsa_private_key_length = sizeof(rsa_private_key);
    size_t rsa_public_key_length = sizeof(rsa_public_key);

    status = themis_gen_rsa_key_pair(rsa_private_key, &rsa_private_key_length, NULL, NULL);
    testsuite_fail_unless(status == THEMIS_INVALID_PARAMETER,
                          "themis_gen_rsa_key_pair: only private key requested");

    status = themis_gen_rsa_key_pair(NULL, NULL, rsa_public_key, &rsa_public_key_length);
    testsuite_fail_unless(status == THEMIS_INVALID_PARAMETER,
                          "themis_gen_rsa_key_pair: only public key requested");
}

void run_secure_message_test(void)
{
    testsuite_enter_suite("generic secure message");
    testsuite_run_test(themis_secure_message_test);

    testsuite_enter_suite("generic secure message: api test");
    testsuite_run_test(themis_secure_message_encrypt_decrypt_api_test);
    testsuite_run_test(themis_secure_message_sign_verify_api_test);
    testsuite_run_test(secure_message_old_api_test);

    testsuite_enter_suite("key generation and validation");
    testsuite_run_test(key_validation_test);
    testsuite_run_test(keygen_parameters_ec);
    testsuite_run_test(keygen_parameters_rsa);
}
