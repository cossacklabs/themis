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

#include <jni.h>
#include <themis/secure_cell.h>
#include <themis/themis_error.h>

/* These definitions should correspond to the ones in SecureCell.java */
#define MODE_SEAL 0
#define MODE_TOKEN_PROTECT 1
#define MODE_CONTEXT_IMPRINT 2
#define MODE_SEAL_PASSPHRASE 3

JNIEXPORT jobjectArray JNICALL Java_com_cossacklabs_themis_SecureCell_encrypt(
    JNIEnv* env, jobject thiz, jbyteArray key, jbyteArray context, jbyteArray data, jint mode)
{
    UNUSED(thiz);

    size_t key_length = (*env)->GetArrayLength(env, key);
    size_t data_length = (*env)->GetArrayLength(env, data);
    size_t context_length = 0;

    size_t encrypted_data_length = 0;
    size_t additional_data_length = 0;

    jbyte* key_buf = NULL;
    jbyte* data_buf = NULL;
    jbyte* context_buf = NULL;

    jobjectArray protected_data = NULL;
    jbyteArray encrypted_data = NULL;
    jbyteArray additional_data = NULL;

    jbyte* encrypted_data_buf = NULL;
    jbyte* additional_data_buf = NULL;

    themis_status_t res;

    if (context) {
        context_length = (*env)->GetArrayLength(env, context);
    }

    key_buf = (*env)->GetByteArrayElements(env, key, NULL);
    if (!key_buf) {
        return NULL;
    }

    data_buf = (*env)->GetByteArrayElements(env, data, NULL);
    if (!data_buf) {
        goto err;
    }

    if (context) {
        context_buf = (*env)->GetByteArrayElements(env, context, NULL);
        if (!context_buf) {
            goto err;
        }
    }

    switch (mode) {
    case MODE_SEAL:
        res = themis_secure_cell_encrypt_seal((uint8_t*)key_buf,
                                              key_length,
                                              (uint8_t*)context_buf,
                                              context_length,
                                              (uint8_t*)data_buf,
                                              data_length,
                                              NULL,
                                              &encrypted_data_length);
        break;
    case MODE_SEAL_PASSPHRASE:
        /* Passphrase bytes passed as key */
        res = themis_secure_cell_encrypt_seal_with_passphrase((uint8_t*)key_buf,
                                                              key_length,
                                                              (uint8_t*)context_buf,
                                                              context_length,
                                                              (uint8_t*)data_buf,
                                                              data_length,
                                                              NULL,
                                                              &encrypted_data_length);
        break;
    case MODE_TOKEN_PROTECT:
        res = themis_secure_cell_encrypt_token_protect((uint8_t*)key_buf,
                                                       key_length,
                                                       (uint8_t*)context_buf,
                                                       context_length,
                                                       (uint8_t*)data_buf,
                                                       data_length,
                                                       NULL,
                                                       &additional_data_length,
                                                       NULL,
                                                       &encrypted_data_length);
        break;
    case MODE_CONTEXT_IMPRINT:
        if (!context) {
            /* Context is mandatory for this mode */
            goto err;
        }

        res = themis_secure_cell_encrypt_context_imprint((uint8_t*)key_buf,
                                                         key_length,
                                                         (uint8_t*)data_buf,
                                                         data_length,
                                                         (uint8_t*)context_buf,
                                                         context_length,
                                                         NULL,
                                                         &encrypted_data_length);
        break;
    default:
        goto err;
    }

    if (THEMIS_BUFFER_TOO_SMALL != res) {
        goto err;
    }

    /*
     * Secure Cell can contain up to 4 GB of data but JVM does not support
     * byte arrays bigger that 2 GB. We just cannot allocate that much.
     */
    if (encrypted_data_length > INT32_MAX || additional_data_length > INT32_MAX) {
        res = THEMIS_NO_MEMORY;
        goto err;
    }

    encrypted_data = (*env)->NewByteArray(env, encrypted_data_length);
    if (!encrypted_data) {
        goto err;
    }

    if (additional_data_length) {
        additional_data = (*env)->NewByteArray(env, additional_data_length);
        if (!additional_data) {
            goto err;
        }
    }

    encrypted_data_buf = (*env)->GetByteArrayElements(env, encrypted_data, NULL);
    if (!encrypted_data_buf) {
        goto err;
    }

    if (additional_data_length) {
        additional_data_buf = (*env)->GetByteArrayElements(env, additional_data, NULL);
        if (!additional_data_buf) {
            goto err;
        }
    }

    switch (mode) {
    case MODE_SEAL:
        res = themis_secure_cell_encrypt_seal((uint8_t*)key_buf,
                                              key_length,
                                              (uint8_t*)context_buf,
                                              context_length,
                                              (uint8_t*)data_buf,
                                              data_length,
                                              (uint8_t*)encrypted_data_buf,
                                              &encrypted_data_length);
        break;
    case MODE_SEAL_PASSPHRASE:
        /* Passphrase bytes passed as key */
        res = themis_secure_cell_encrypt_seal_with_passphrase((uint8_t*)key_buf,
                                                              key_length,
                                                              (uint8_t*)context_buf,
                                                              context_length,
                                                              (uint8_t*)data_buf,
                                                              data_length,
                                                              (uint8_t*)encrypted_data_buf,
                                                              &encrypted_data_length);
        break;
    case MODE_TOKEN_PROTECT:
        res = themis_secure_cell_encrypt_token_protect((uint8_t*)key_buf,
                                                       key_length,
                                                       (uint8_t*)context_buf,
                                                       context_length,
                                                       (uint8_t*)data_buf,
                                                       data_length,
                                                       (uint8_t*)additional_data_buf,
                                                       &additional_data_length,
                                                       (uint8_t*)encrypted_data_buf,
                                                       &encrypted_data_length);
        break;
    case MODE_CONTEXT_IMPRINT:
        if (!context) {
            /* Context is mandatory for this mode */
            goto err;
        }

        res = themis_secure_cell_encrypt_context_imprint((uint8_t*)key_buf,
                                                         key_length,
                                                         (uint8_t*)data_buf,
                                                         data_length,
                                                         (uint8_t*)context_buf,
                                                         context_length,
                                                         (uint8_t*)encrypted_data_buf,
                                                         &encrypted_data_length);
        break;
    default:
        goto err;
    }

    if (THEMIS_SUCCESS != res) {
        goto err;
    }

    protected_data = (*env)->NewObjectArray(env, 2, (*env)->GetObjectClass(env, data), NULL);
    if (!protected_data) {
        goto err;
    }

    (*env)->SetObjectArrayElement(env, protected_data, 0, encrypted_data);

    if (additional_data_length) {
        (*env)->SetObjectArrayElement(env, protected_data, 1, additional_data);
    }

err:

    if (additional_data_buf) {
        (*env)->ReleaseByteArrayElements(env, additional_data, additional_data_buf, 0);
    }

    if (encrypted_data_buf) {
        (*env)->ReleaseByteArrayElements(env, encrypted_data, encrypted_data_buf, 0);
    }

    if (context_buf) {
        (*env)->ReleaseByteArrayElements(env, context, context_buf, 0);
    }

    if (data_buf) {
        (*env)->ReleaseByteArrayElements(env, data, data_buf, 0);
    }

    if (key_buf) {
        (*env)->ReleaseByteArrayElements(env, key, key_buf, 0);
    }

    return protected_data;
}

JNIEXPORT jbyteArray JNICALL Java_com_cossacklabs_themis_SecureCell_decrypt(
    JNIEnv* env, jobject thiz, jbyteArray key, jbyteArray context, jobjectArray protected_data, jint mode)
{
    UNUSED(thiz);

    size_t key_length = (*env)->GetArrayLength(env, key);
    size_t data_length = 0;
    size_t context_length = 0;

    size_t encrypted_data_length = 0;
    size_t additional_data_length = 0;

    jbyte* key_buf = NULL;
    jbyte* data_buf = NULL;
    jbyte* context_buf = NULL;

    jbyteArray encrypted_data = NULL;
    jbyteArray additional_data = NULL;
    jbyteArray data = NULL;
    jbyteArray output = NULL;

    jbyte* encrypted_data_buf = NULL;
    jbyte* additional_data_buf = NULL;

    themis_status_t res;

    if (context) {
        context_length = (*env)->GetArrayLength(env, context);
    }

    encrypted_data = (*env)->GetObjectArrayElement(env, protected_data, 0);
    additional_data = (*env)->GetObjectArrayElement(env, protected_data, 1);
    if (!encrypted_data) {
        return NULL;
    }

    encrypted_data_length = (*env)->GetArrayLength(env, encrypted_data);
    if (additional_data) {
        additional_data_length = (*env)->GetArrayLength(env, additional_data);
    }

    key_buf = (*env)->GetByteArrayElements(env, key, NULL);
    if (!key_buf) {
        return NULL;
    }

    encrypted_data_buf = (*env)->GetByteArrayElements(env, encrypted_data, NULL);
    if (!encrypted_data_buf) {
        goto err;
    }

    if (context) {
        context_buf = (*env)->GetByteArrayElements(env, context, NULL);
        if (!context_buf) {
            goto err;
        }
    }

    if (additional_data) {
        additional_data_buf = (*env)->GetByteArrayElements(env, additional_data, NULL);
        if (!additional_data_buf) {
            goto err;
        }
    }

    switch (mode) {
    case MODE_SEAL:
        res = themis_secure_cell_decrypt_seal((uint8_t*)key_buf,
                                              key_length,
                                              (uint8_t*)context_buf,
                                              context_length,
                                              (uint8_t*)encrypted_data_buf,
                                              encrypted_data_length,
                                              NULL,
                                              &data_length);
        break;
    case MODE_SEAL_PASSPHRASE:
        /* Passphrase bytes passed as key */
        res = themis_secure_cell_decrypt_seal_with_passphrase((uint8_t*)key_buf,
                                                              key_length,
                                                              (uint8_t*)context_buf,
                                                              context_length,
                                                              (uint8_t*)encrypted_data_buf,
                                                              encrypted_data_length,
                                                              NULL,
                                                              &data_length);
        break;
    case MODE_TOKEN_PROTECT:
        if (!additional_data_buf) {
            /* Additional data is mandatory for this mode */
            goto err;
        }

        res = themis_secure_cell_decrypt_token_protect((uint8_t*)key_buf,
                                                       key_length,
                                                       (uint8_t*)context_buf,
                                                       context_length,
                                                       (uint8_t*)encrypted_data_buf,
                                                       encrypted_data_length,
                                                       (uint8_t*)additional_data_buf,
                                                       additional_data_length,
                                                       NULL,
                                                       &data_length);
        break;
    case MODE_CONTEXT_IMPRINT:
        if (!context) {
            /* Context is mandatory for this mode */
            goto err;
        }

        res = themis_secure_cell_encrypt_context_imprint((uint8_t*)key_buf,
                                                         key_length,
                                                         (uint8_t*)encrypted_data_buf,
                                                         encrypted_data_length,
                                                         (uint8_t*)context_buf,
                                                         context_length,
                                                         NULL,
                                                         &data_length);
        break;
    default:
        goto err;
    }

    if (THEMIS_BUFFER_TOO_SMALL != res) {
        goto err;
    }

    /*
     * Secure Cell can contain up to 4 GB of data but JVM does not support
     * byte arrays bigger that 2 GB. We just cannot allocate that much.
     */
    if (data_length > INT32_MAX) {
        res = THEMIS_NO_MEMORY;
        goto err;
    }

    data = (*env)->NewByteArray(env, data_length);
    if (!data) {
        goto err;
    }

    data_buf = (*env)->GetByteArrayElements(env, data, NULL);
    if (!data_buf) {
        goto err;
    }

    switch (mode) {
    case MODE_SEAL:
        res = themis_secure_cell_decrypt_seal((uint8_t*)key_buf,
                                              key_length,
                                              (uint8_t*)context_buf,
                                              context_length,
                                              (uint8_t*)encrypted_data_buf,
                                              encrypted_data_length,
                                              (uint8_t*)data_buf,
                                              &data_length);
        break;
    case MODE_SEAL_PASSPHRASE:
        /* Passphrase bytes passed as key */
        res = themis_secure_cell_decrypt_seal_with_passphrase((uint8_t*)key_buf,
                                                              key_length,
                                                              (uint8_t*)context_buf,
                                                              context_length,
                                                              (uint8_t*)encrypted_data_buf,
                                                              encrypted_data_length,
                                                              (uint8_t*)data_buf,
                                                              &data_length);
        break;
    case MODE_TOKEN_PROTECT:
        if (!additional_data_buf) {
            /* Additional data is mandatory for this mode */
            goto err;
        }

        res = themis_secure_cell_decrypt_token_protect((uint8_t*)key_buf,
                                                       key_length,
                                                       (uint8_t*)context_buf,
                                                       context_length,
                                                       (uint8_t*)encrypted_data_buf,
                                                       encrypted_data_length,
                                                       (uint8_t*)additional_data_buf,
                                                       additional_data_length,
                                                       (uint8_t*)data_buf,
                                                       &data_length);
        break;
    case MODE_CONTEXT_IMPRINT:
        if (!context) {
            /* Context is mandatory for this mode */
            goto err;
        }

        res = themis_secure_cell_encrypt_context_imprint((uint8_t*)key_buf,
                                                         key_length,
                                                         (uint8_t*)encrypted_data_buf,
                                                         encrypted_data_length,
                                                         (uint8_t*)context_buf,
                                                         context_length,
                                                         (uint8_t*)data_buf,
                                                         &data_length);
        break;
    default:
        goto err;
    }

    if (THEMIS_SUCCESS != res) {
        goto err;
    }

    output = data;

err:

    if (additional_data_buf) {
        (*env)->ReleaseByteArrayElements(env, additional_data, additional_data_buf, 0);
    }

    if (encrypted_data_buf) {
        (*env)->ReleaseByteArrayElements(env, encrypted_data, encrypted_data_buf, 0);
    }

    if (context_buf) {
        (*env)->ReleaseByteArrayElements(env, context, context_buf, 0);
    }

    if (data_buf) {
        (*env)->ReleaseByteArrayElements(env, data, data_buf, 0);
    }

    if (key_buf) {
        (*env)->ReleaseByteArrayElements(env, key, key_buf, 0);
    }

    return output;
}
