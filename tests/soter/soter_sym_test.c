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

#include "soter/soter_test.h"

#include <stdio.h>
#include <string.h>

struct test_vector_type {
    uint32_t alg;
    char* key;
    char* iv;
    char* plaintext;
    char* ciphertext;
};

typedef struct test_vector_type test_vector_t;

struct test_vector_aead_type {
    uint32_t alg;
    char* key;
    char* iv;
    char* aad;
    char* plaintext;
    char* ciphertext;
    char* authtag;
};

typedef struct test_vector_aead_type test_vector_aead_t;

/* taken from NIST SP800-38A */
static test_vector_t vectors[] =
    {{SOTER_SYM_AES_ECB_PKCS7 | SOTER_SYM_128_KEY_LENGTH,
      "2b7e151628aed2a6abf7158809cf4f3c",
      "",
      "6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411e5fbc1191a0a52eff69f2445df4f9b17ad2b417be66c3710",
      "3ad77bb40d7a3660a89ecaf32466ef97f5d3d58503b9699de785895a96fdbaaf43b1cd7f598ece23881b00e3ed0306887b0c785e27e8ad3f8223207104725dd4a254be88e037ddd9d79fb6411c3f9df8"},
     {SOTER_SYM_AES_ECB_PKCS7 | SOTER_SYM_256_KEY_LENGTH,
      "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4",
      "",
      "6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411e5fbc1191a0a52eff69f2445df4f9b17ad2b417be66c3710",
      "f3eed1bdb5d2a03c064b5a7e3db181f8591ccb10d410ed26dc5ba74a31362870b6ed21b99ca6f4f9f153e7b1beafed1d23304b7a39f9f3ff067d8d8f9e24ecc74c45dfb3b3b484ec35b0512dc8c1c4d6"},
     {SOTER_SYM_AES_CTR | SOTER_SYM_128_KEY_LENGTH,
      "2b7e151628aed2a6abf7158809cf4f3c",
      "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff",
      "6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411e5fbc1191a0a52eff69f2445df4f9b17ad2b417be66c3710",
      "874d6191b620e3261bef6864990db6ce9806f66b7970fdff8617187bb9fffdff5ae4df3edbd5d35e5b4f09020db03eab1e031dda2fbe03d1792170a0f3009cee"},
     {SOTER_SYM_AES_CTR | SOTER_SYM_256_KEY_LENGTH,
      "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4",
      "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff",
      "6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411e5fbc1191a0a52eff69f2445df4f9b17ad2b417be66c3710",
      "601ec313775789a5b7a7f504bbf3d228f443e3ca4d62b59aca84e990cacaf5c52b0930daa23de94ce87017ba2d84988ddfc9c58db67aada613c2dd08457941a6"}};

/* GCM - taken from The Galois/Counter Mode of Operation (GCM) by David A. McGrew and John Viega */
static test_vector_aead_t vectors_aead[] =
    {{SOTER_SYM_AES_GCM | SOTER_SYM_128_KEY_LENGTH,
      "feffe9928665731c6d6a8f9467308308",
      "cafebabefacedbaddecaf888",
      "",
      "d9313225f88406e5a55909c5aff5269a86a7a9531534f7da2e4c303d8a318a721c3c0c95956809532fcf0e2449a6b525b16aedf5aa0de657ba637b391aafd255",
      "42831ec2217774244b7221b784d0d49ce3aa212f2c02a4e035c17e2329aca12e21d514b25466931c7d8f6a5aac84aa051ba30b396a0aac973d58e091473f5985",
      "4d5c2af327cd64a62cf35abd2ba6fab4"},
     {SOTER_SYM_AES_GCM | SOTER_SYM_128_KEY_LENGTH,
      "feffe9928665731c6d6a8f9467308308",
      "cafebabefacedbaddecaf888",
      "feedfacedeadbeeffeedfacedeadbeefabaddad2",
      "d9313225f88406e5a55909c5aff5269a86a7a9531534f7da2e4c303d8a318a721c3c0c95956809532fcf0e2449a6b525b16aedf5aa0de657ba637b39",
      "42831ec2217774244b7221b784d0d49ce3aa212f2c02a4e035c17e2329aca12e21d514b25466931c7d8f6a5aac84aa051ba30b396a0aac973d58e091",
      "5bc94fbc3221a5db94fae95ae7121a47"},
     {SOTER_SYM_AES_GCM | SOTER_SYM_256_KEY_LENGTH,
      "feffe9928665731c6d6a8f9467308308feffe9928665731c6d6a8f9467308308",
      "cafebabefacedbaddecaf888",
      "",
      "d9313225f88406e5a55909c5aff5269a86a7a9531534f7da2e4c303d8a318a721c3c0c95956809532fcf0e2449a6b525b16aedf5aa0de657ba637b391aafd255",
      "522dc1f099567d07f47f37a32a84427d643a8cdcbfe5c0c97598a2bd2555d1aa8cb08e48590dbb3da7b08b1056828838c5f61e6393ba7a0abcc9f662898015ad",
      "b094dac5d93471bdec1a502270e3cc6c"},
     {SOTER_SYM_AES_GCM | SOTER_SYM_256_KEY_LENGTH,
      "feffe9928665731c6d6a8f9467308308feffe9928665731c6d6a8f9467308308",
      "cafebabefacedbaddecaf888",
      "feedfacedeadbeeffeedfacedeadbeefabaddad2",
      "d9313225f88406e5a55909c5aff5269a86a7a9531534f7da2e4c303d8a318a721c3c0c95956809532fcf0e2449a6b525b16aedf5aa0de657ba637b39",
      "522dc1f099567d07f47f37a32a84427d643a8cdcbfe5c0c97598a2bd2555d1aa8cb08e48590dbb3da7b08b1056828838c5f61e6393ba7a0abcc9f662",
      "76fc6ece0f4e1768cddf8853bb2d551b"}};

#define MAX_KEY_LENGTH 32
#define MAX_DATA_LENGTH 1024
#define MAX_IV_LENGTH 32

static void test_known_values(void)
{
    soter_sym_ctx_t* ctx;
    int i;

    uint8_t key[MAX_KEY_LENGTH];
    uint8_t iv[MAX_IV_LENGTH];
    uint8_t plaintext[MAX_DATA_LENGTH];
    uint8_t ciphertext[MAX_DATA_LENGTH];

    uint8_t computed[MAX_DATA_LENGTH];
    size_t computed_length;
    uint8_t computed2[MAX_DATA_LENGTH];
    size_t computed_length2 = MAX_DATA_LENGTH;

    uint8_t pad[MAX_IV_LENGTH];
    size_t pad_length = sizeof(pad);

    soter_status_t res;

    for (i = 0; i < (int)(sizeof(vectors) / sizeof(test_vector_t)); i++) {
        res = string_to_bytes(vectors[i].key, key, sizeof(key));
        if (SOTER_SUCCESS != res) {
            testsuite_fail_if(res, "string_to_bytes");
            continue;
        }

        res = string_to_bytes(vectors[i].iv, iv, sizeof(iv));
        if (SOTER_SUCCESS != res) {
            testsuite_fail_if(res, "string_to_bytes");
            continue;
        }

        res = string_to_bytes(vectors[i].plaintext, plaintext, sizeof(plaintext));
        if (SOTER_SUCCESS != res) {
            testsuite_fail_if(res, "string_to_bytes");
            continue;
        }

        res = string_to_bytes(vectors[i].ciphertext, ciphertext, sizeof(ciphertext));
        if (SOTER_SUCCESS != res) {
            testsuite_fail_if(res, "string_to_bytes");
            continue;
        }

        /* Encryption */
        if (strlen(vectors[i].iv) == 0) {
            ctx = soter_sym_encrypt_create(vectors[i].alg, key, strlen(vectors[i].key) / 2, NULL, 0, NULL, 0);
        } else {
            ctx = soter_sym_encrypt_create(vectors[i].alg,
                                           key,
                                           strlen(vectors[i].key) / 2,
                                           NULL,
                                           0,
                                           iv,
                                           strlen(vectors[i].iv) / 2);
        }
        if (NULL == ctx) {
            testsuite_fail_unless(ctx, "soter_sym_create");
            continue;
        }

        computed_length = sizeof(computed);
        res = soter_sym_encrypt_update(ctx,
                                       plaintext,
                                       strlen(vectors[i].plaintext) / 2,
                                       computed,
                                       &computed_length);
        if (SOTER_SUCCESS != res) {
            testsuite_fail_if(SOTER_SUCCESS != res, "soter_sym_update");
            soter_sym_encrypt_destroy(ctx);
            continue;
        }

        pad_length = sizeof(pad);
        res = soter_sym_encrypt_final(ctx, computed + computed_length, &pad_length);
        if (SOTER_SUCCESS != res) {
            testsuite_fail_if(SOTER_SUCCESS != res, "soter_sym_final");
            soter_sym_encrypt_destroy(ctx);
            continue;
        }
        computed_length += pad_length;
        res = soter_sym_encrypt_destroy(ctx);
        if (SOTER_SUCCESS != res) {
            testsuite_fail_if(SOTER_SUCCESS != res, "soter_sym_destroy");
        }

        testsuite_fail_if(memcmp(computed, ciphertext, computed_length) != 0, "known encryption");

        /* Decryption */
        if (strlen(vectors[i].iv) == 0) {
            ctx = soter_sym_decrypt_create(vectors[i].alg, key, strlen(vectors[i].key) / 2, NULL, 0, NULL, 0);
        } else {
            ctx = soter_sym_decrypt_create(vectors[i].alg,
                                           key,
                                           strlen(vectors[i].key) / 2,
                                           NULL,
                                           0,
                                           iv,
                                           strlen(vectors[i].iv) / 2);
        }
        if (NULL == ctx) {
            testsuite_fail_unless(ctx, "soter_sym_create");
            continue;
        }
        computed_length2 = MAX_DATA_LENGTH;
        res = soter_sym_decrypt_update(ctx, computed, computed_length, computed2, &computed_length2);
        if (SOTER_SUCCESS != res) {
            testsuite_fail_if(res, "soter_sym_update");
            soter_sym_decrypt_destroy(ctx);
            continue;
        }
        pad_length = sizeof(pad);
        res = soter_sym_decrypt_final(ctx, computed2 + computed_length2, &pad_length);
        if (SOTER_SUCCESS != res) {
            testsuite_fail_if(res, "soter_sym_final");
            soter_sym_decrypt_destroy(ctx);
            continue;
        }
        computed_length2 += pad_length;
        res = soter_sym_decrypt_destroy(ctx);
        if (SOTER_SUCCESS != res) {
            testsuite_fail_if(res, "soter_sym_destroy");
            soter_sym_decrypt_destroy(ctx);
            continue;
        }
        testsuite_fail_if(memcmp(computed2, plaintext, computed_length2) != 0, "known decryption");
    }
}

#define MAX_AAD_LENGTH 1024
#define MAX_AUTH_TAG_LENGTH 16

static void test_known_values_gcm(void)
{
    soter_sym_ctx_t* ctx;
    int i;

    uint8_t key[MAX_KEY_LENGTH];
    uint8_t iv[MAX_IV_LENGTH];
    uint8_t plaintext[MAX_DATA_LENGTH];
    uint8_t ciphertext[MAX_DATA_LENGTH];

    uint8_t computed[MAX_DATA_LENGTH];
    size_t computed_length;

    uint8_t computed2[MAX_DATA_LENGTH];
    size_t computed_length2;

    uint8_t aad[MAX_AAD_LENGTH];

    uint8_t auth_tag[MAX_AUTH_TAG_LENGTH];
    uint8_t expected_auth_tag[MAX_AUTH_TAG_LENGTH];
    size_t auth_tag_length = sizeof(auth_tag);
    soter_status_t res;

    for (i = 0; i < (int)(sizeof(vectors_aead) / sizeof(test_vector_aead_t)); i++) {
        res = string_to_bytes(vectors_aead[i].key, key, sizeof(key));
        if (SOTER_SUCCESS != res) {
            testsuite_fail_if(res, "string_to_bytes");
            continue;
        }

        res = string_to_bytes(vectors_aead[i].iv, iv, sizeof(iv));
        if (SOTER_SUCCESS != res) {
            testsuite_fail_if(res, "string_to_bytes");
            continue;
        }

        res = string_to_bytes(vectors_aead[i].plaintext, plaintext, sizeof(plaintext));
        if (SOTER_SUCCESS != res) {
            testsuite_fail_if(res, "string_to_bytes");
            continue;
        }

        res = string_to_bytes(vectors_aead[i].ciphertext, ciphertext, sizeof(ciphertext));
        if (SOTER_SUCCESS != res) {
            testsuite_fail_if(res, "string_to_bytes");
            continue;
        }

        res = string_to_bytes(vectors_aead[i].aad, aad, sizeof(aad));
        if (SOTER_SUCCESS != res) {
            testsuite_fail_if(res, "string_to_bytes");
            continue;
        }

        res = string_to_bytes(vectors_aead[i].authtag, expected_auth_tag, MAX_AUTH_TAG_LENGTH);
        if (SOTER_SUCCESS != res) {
            testsuite_fail_if(res, "string_to_bytes");
            continue;
        }

        /* Encryption */
        ctx = soter_sym_aead_encrypt_create(vectors_aead[i].alg,
                                            key,
                                            strlen(vectors_aead[i].key) / 2,
                                            NULL,
                                            0,
                                            iv,
                                            strlen(vectors_aead[i].iv) / 2);
        if (NULL == ctx) {
            testsuite_fail_unless(ctx, "soter_sym_create");
            continue;
        }
        if (strlen(vectors_aead[i].aad) != 0) {
            computed_length = sizeof(computed);
            res = soter_sym_aead_encrypt_aad(ctx, aad, strlen(vectors_aead[i].aad) / 2);
            if (SOTER_SUCCESS != res) {
                testsuite_fail_if(res, "soter_sym_update aad");
                soter_sym_encrypt_destroy(ctx);
                continue;
            }
        }

        computed_length = sizeof(computed);

        res = soter_sym_aead_encrypt_update(ctx,
                                            plaintext,
                                            strlen(vectors_aead[i].plaintext) / 2,
                                            computed,
                                            &computed_length);
        if (SOTER_SUCCESS != res) {
            testsuite_fail_if(res, "soter_sym_update");
            soter_sym_aead_encrypt_destroy(ctx);
            continue;
        }

        res = soter_sym_aead_encrypt_final(ctx, auth_tag, &auth_tag_length);
        if (SOTER_SUCCESS != res) {
            testsuite_fail_if(res, "soter_sym_final");
            soter_sym_aead_encrypt_destroy(ctx);
            continue;
        }

        res = soter_sym_aead_encrypt_destroy(ctx);
        if (SOTER_SUCCESS != res) {
            testsuite_fail_if(res, "soter_sym_destroy");
        }

        testsuite_fail_if(memcmp(computed, ciphertext, computed_length) != 0, "known encryption");

        testsuite_fail_if(memcmp(auth_tag, expected_auth_tag, auth_tag_length) != 0,
                          "known encryption (auth tag value)");

        /* Decryption */
        ctx = soter_sym_aead_decrypt_create(vectors_aead[i].alg,
                                            key,
                                            strlen(vectors_aead[i].key) / 2,
                                            NULL,
                                            0,
                                            iv,
                                            strlen(vectors_aead[i].iv) / 2);
        if (NULL == ctx) {
            testsuite_fail_unless(ctx, "soter_sym_create");
            continue;
        }

        if (strlen(vectors_aead[i].aad) != 0) {
            computed_length2 = sizeof(computed2);
            res = soter_sym_aead_decrypt_aad(ctx, aad, strlen(vectors_aead[i].aad) / 2);
            if (SOTER_SUCCESS != res) {
                testsuite_fail_if(res, "soter_sym_update aad");
                soter_sym_aead_encrypt_destroy(ctx);
                continue;
            }
        }

        computed_length2 = sizeof(computed2);
        res = soter_sym_aead_decrypt_update(ctx, computed, computed_length, computed2, &computed_length2);
        if (SOTER_SUCCESS != res) {
            testsuite_fail_if(res, "soter_sym_decrypt_update");
            soter_sym_aead_decrypt_destroy(ctx);
            continue;
        }

        res = soter_sym_aead_decrypt_final(ctx, auth_tag, auth_tag_length);
        if (SOTER_SUCCESS != res) {
            testsuite_fail_if(res, "soter_sym_final");
            soter_sym_aead_decrypt_destroy(ctx);
            continue;
        }
        res = soter_sym_aead_decrypt_destroy(ctx);
        if (SOTER_SUCCESS != res) {
            testsuite_fail_if(res, "soter_sym_destroy");
            soter_sym_aead_decrypt_destroy(ctx);
            continue;
        }

        testsuite_fail_if(memcmp(computed2, plaintext, computed_length2) != 0, "known decryption");
    }
}

static void test_auth_tag(void)
{
    uint8_t key[MAX_KEY_LENGTH];
    uint8_t iv[MAX_IV_LENGTH];
    uint8_t plaintext[MAX_DATA_LENGTH];
    uint8_t aad[MAX_KEY_LENGTH];
    uint8_t ciphertext[MAX_DATA_LENGTH];
    uint8_t decrypted[MAX_DATA_LENGTH];
    size_t bytes_processed = sizeof(ciphertext);

    uint8_t tag[MAX_AUTH_TAG_LENGTH];
    size_t tag_length = sizeof(tag);

    soter_sym_ctx_t* ctx;
    soter_status_t res;

    res = soter_rand(key, sizeof(key));
    if (SOTER_SUCCESS != res) {
        testsuite_fail_if(res, "key gen");
        return;
    }

    res = soter_rand(iv, sizeof(iv));
    if (SOTER_SUCCESS != res) {
        testsuite_fail_if(res, "iv gen");
        return;
    }

    res = soter_rand(plaintext, sizeof(plaintext));
    if (SOTER_SUCCESS != res) {
        testsuite_fail_if(res, "plaintext gen");
        return;
    }

    res = soter_rand(aad, sizeof(aad));
    if (SOTER_SUCCESS != res) {
        testsuite_fail_if(res, "aad gen");
        return;
    }

    ctx = soter_sym_aead_encrypt_create(SOTER_SYM_AES_GCM | SOTER_SYM_256_KEY_LENGTH,
                                        key,
                                        sizeof(key),
                                        NULL,
                                        0,
                                        iv,
                                        sizeof(iv));
    if (NULL == ctx) {
        testsuite_fail_if(NULL == ctx, "soter_sym_aead_encrypt_create");
        return;
    }

    res = soter_sym_aead_encrypt_aad(ctx, aad, sizeof(aad));
    if (SOTER_SUCCESS != res) {
        testsuite_fail_if(res, "soter_sym_aead_encrypt_aad");
        soter_sym_aead_encrypt_destroy(ctx);
        return;
    }

    res = soter_sym_aead_encrypt_update(ctx, plaintext, sizeof(plaintext), ciphertext, &bytes_processed);
    if (SOTER_SUCCESS != res) {
        testsuite_fail_if(res, "soter_sym_aead_encrypt_update");
        soter_sym_aead_encrypt_destroy(ctx);
        return;
    }

    res = soter_sym_aead_encrypt_final(ctx, NULL, NULL);
    testsuite_fail_if(res != SOTER_INVALID_PARAMETER, "soter_sym_aead_encrypt_final: NULL length");

    res = soter_sym_aead_encrypt_final(ctx, NULL, &tag_length);
    testsuite_fail_if(res != SOTER_BUFFER_TOO_SMALL, "soter_sym_aead_encrypt_final: query size");

    res = soter_sym_aead_encrypt_final(ctx, tag, &tag_length);
    if (SOTER_SUCCESS != res) {
        testsuite_fail_if(res, "soter_sym_aead_encrypt_final");
        soter_sym_aead_encrypt_destroy(ctx);
        return;
    }

    soter_sym_aead_encrypt_destroy(ctx);

    ctx = soter_sym_aead_decrypt_create(SOTER_SYM_AES_GCM | SOTER_SYM_256_KEY_LENGTH,
                                        key,
                                        sizeof(key),
                                        NULL,
                                        0,
                                        iv,
                                        sizeof(iv));
    if (NULL == ctx) {
        testsuite_fail_if(NULL == ctx, "soter_sym_aead_decrypt_create");
        return;
    }

    /* Tamper with AAD */
    aad[2] ^= 0xff;

    res = soter_sym_aead_decrypt_aad(ctx, aad, sizeof(aad));
    if (SOTER_SUCCESS != res) {
        testsuite_fail_if(res, "soter_sym_aead_decrypt_aad");
        soter_sym_aead_decrypt_destroy(ctx);
        return;
    }

    res = soter_sym_aead_decrypt_update(ctx, ciphertext, sizeof(ciphertext), decrypted, &bytes_processed);
    if (SOTER_SUCCESS != res) {
        testsuite_fail_if(res, "soter_sym_aead_decrypt_update");
        soter_sym_aead_decrypt_destroy(ctx);
        return;
    }

    res = soter_sym_aead_decrypt_final(ctx, tag, tag_length);

    soter_sym_aead_decrypt_destroy(ctx);

    if (memcmp(plaintext, decrypted, sizeof(plaintext)) != 0) {
        testsuite_fail_if(true, "decrypted date does not match plaintext");
        return;
    }

    testsuite_fail_unless(SOTER_FAIL == res, "detect data tamper");
}

static void test_invalid_params(void)
{
    soter_sym_ctx_t* ctx = NULL;
    uint8_t key[MAX_KEY_LENGTH] = {0};
    uint8_t iv[MAX_IV_LENGTH] = {0};

    ctx = soter_sym_encrypt_create(0xFFFFFFFF, key, sizeof(key), NULL, 0, iv, sizeof(iv));
    testsuite_fail_if(ctx != NULL, "encrypt_create: invalid algorithm");
    if (ctx != NULL) {
        soter_sym_encrypt_destroy(ctx);
    }

    ctx = soter_sym_encrypt_create(SOTER_SYM_AES_GCM | SOTER_SYM_256_KEY_LENGTH,
                                   key,
                                   0,
                                   NULL,
                                   0,
                                   iv,
                                   sizeof(iv));
    testsuite_fail_if(ctx != NULL, "encrypt_create: invalid key length");
    if (ctx != NULL) {
        soter_sym_encrypt_destroy(ctx);
    }

    ctx = soter_sym_encrypt_create(SOTER_SYM_AES_GCM | SOTER_SYM_256_KEY_LENGTH | SOTER_SYM_PBKDF2,
                                   key,
                                   sizeof(key),
                                   NULL,
                                   1,
                                   NULL,
                                   0);
    testsuite_fail_if(ctx != NULL, "encrypt_create: invalid salt length");
    if (ctx != NULL) {
        soter_sym_encrypt_destroy(ctx);
    }

    ctx = soter_sym_encrypt_create(SOTER_SYM_AES_GCM | SOTER_SYM_256_KEY_LENGTH,
                                   key,
                                   sizeof(key),
                                   NULL,
                                   0,
                                   iv,
                                   0);
    testsuite_fail_if(ctx != NULL, "encrypt_create: invalid IV length");
    if (ctx != NULL) {
        soter_sym_encrypt_destroy(ctx);
    }

    ctx = soter_sym_aead_encrypt_create(0xFFFFFFFF, key, sizeof(key), NULL, 0, iv, sizeof(iv));
    testsuite_fail_if(ctx != NULL, "aead_encrypt_create: invalid algorithm");
    if (ctx != NULL) {
        soter_sym_aead_encrypt_destroy(ctx);
    }

    ctx = soter_sym_aead_encrypt_create(SOTER_SYM_AES_GCM | SOTER_SYM_256_KEY_LENGTH,
                                        key,
                                        0,
                                        NULL,
                                        0,
                                        iv,
                                        sizeof(iv));
    testsuite_fail_if(ctx != NULL, "aead_encrypt_create: invalid key length");
    if (ctx != NULL) {
        soter_sym_aead_encrypt_destroy(ctx);
    }

    ctx = soter_sym_aead_encrypt_create(SOTER_SYM_AES_GCM | SOTER_SYM_256_KEY_LENGTH | SOTER_SYM_PBKDF2,
                                        key,
                                        sizeof(key),
                                        NULL,
                                        1,
                                        NULL,
                                        0);
    testsuite_fail_if(ctx != NULL, "aead_encrypt_create: invalid salt length");
    if (ctx != NULL) {
        soter_sym_aead_encrypt_destroy(ctx);
    }

    ctx = soter_sym_aead_encrypt_create(SOTER_SYM_AES_GCM | SOTER_SYM_256_KEY_LENGTH,
                                        key,
                                        sizeof(key),
                                        NULL,
                                        0,
                                        iv,
                                        0);
    testsuite_fail_if(ctx != NULL, "aead_encrypt_create: invalid IV length");
    if (ctx != NULL) {
        soter_sym_aead_encrypt_destroy(ctx);
    }
}

void run_soter_sym_test()
{
    testsuite_enter_suite("soter sym");
    //  testsuite_run_test(soter_sym_test);
    testsuite_run_test(test_known_values);
    testsuite_run_test(test_known_values_gcm);
    testsuite_run_test(test_auth_tag);
    testsuite_run_test(test_invalid_params);
    //  soter_sym_test();
}
