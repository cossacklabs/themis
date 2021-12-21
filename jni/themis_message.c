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
#include <string.h>
#include <themis/secure_message.h>
#include <themis/themis_error.h>

#define SECURE_MESSAGE_ENCRYPT 1
#define SECURE_MESSAGE_DECRYPT 2
#define SECURE_MESSAGE_SIGN 3
#define SECURE_MESSAGE_VERIFY 4

JNIEXPORT jbyteArray JNICALL Java_com_cossacklabs_themis_SecureMessage_process(JNIEnv* env,
                                                                               jobject thiz,
                                                                               jbyteArray private_key,
                                                                               jbyteArray public_key,
                                                                               jbyteArray message,
                                                                               jint action)
{
    UNUSED(thiz);

    size_t private_key_length = 0;
    size_t public_key_length = 0;
    size_t message_length = (*env)->GetArrayLength(env, message);
    size_t output_length = 0;

    jbyte* priv_buf = NULL;
    jbyte* pub_buf = NULL;
    jbyte* message_buf = NULL;
    jbyte* output_bytes = NULL;
    uint8_t* output_buffer = NULL;

    themis_status_t res = THEMIS_FAIL;
    jbyteArray output = NULL;

    if (private_key) {
        private_key_length = (*env)->GetArrayLength(env, private_key);
    }

    if (public_key) {
        public_key_length = (*env)->GetArrayLength(env, public_key);
    }

    if (private_key) {
        priv_buf = (*env)->GetByteArrayElements(env, private_key, NULL);
        if (!priv_buf) {
            return NULL;
        }
    }

    if (public_key) {
        pub_buf = (*env)->GetByteArrayElements(env, public_key, NULL);
        if (!pub_buf) {
            goto err;
        }
    }

    message_buf = (*env)->GetByteArrayElements(env, message, NULL);
    if (!message_buf) {
        goto err;
    }

    switch (action) {
    case SECURE_MESSAGE_ENCRYPT:
        res = themis_secure_message_encrypt((uint8_t*)priv_buf,
                                            private_key_length,
                                            (uint8_t*)pub_buf,
                                            public_key_length,
                                            (uint8_t*)message_buf,
                                            message_length,
                                            NULL,
                                            &output_length);
        break;
    case SECURE_MESSAGE_DECRYPT:
        res = themis_secure_message_decrypt((uint8_t*)priv_buf,
                                            private_key_length,
                                            (uint8_t*)pub_buf,
                                            public_key_length,
                                            (uint8_t*)message_buf,
                                            message_length,
                                            NULL,
                                            &output_length);
        break;
    case SECURE_MESSAGE_SIGN:
        res = themis_secure_message_sign((uint8_t*)priv_buf,
                                         private_key_length,
                                         (uint8_t*)message_buf,
                                         message_length,
                                         NULL,
                                         &output_length);
        break;
    case SECURE_MESSAGE_VERIFY:
        res = themis_secure_message_verify((uint8_t*)pub_buf,
                                           public_key_length,
                                           (uint8_t*)message_buf,
                                           message_length,
                                           NULL,
                                           &output_length);
        break;
    default:
        res = THEMIS_NOT_SUPPORTED;
        break;
    }

    if (THEMIS_BUFFER_TOO_SMALL != res) {
        goto err;
    }

    /*
     * Secure Message data format can store messages up to 4 GB long, but JVM
     * cannot allocate more than 2 GB in one chunk. Avoid overflows here.
     */
    if (output_length > INT32_MAX) {
        res = THEMIS_NO_MEMORY;
        goto err;
    }

    output_buffer = malloc(output_length);
    if (!output_buffer) {
        res = THEMIS_NO_MEMORY;
        goto err;
    }

    switch (action) {
    case SECURE_MESSAGE_ENCRYPT:
        res = themis_secure_message_encrypt((uint8_t*)priv_buf,
                                            private_key_length,
                                            (uint8_t*)pub_buf,
                                            public_key_length,
                                            (uint8_t*)message_buf,
                                            message_length,
                                            output_buffer,
                                            &output_length);
        break;
    case SECURE_MESSAGE_DECRYPT:
        res = themis_secure_message_decrypt((uint8_t*)priv_buf,
                                            private_key_length,
                                            (uint8_t*)pub_buf,
                                            public_key_length,
                                            (uint8_t*)message_buf,
                                            message_length,
                                            output_buffer,
                                            &output_length);
        break;
    case SECURE_MESSAGE_SIGN:
        res = themis_secure_message_sign((uint8_t*)priv_buf,
                                         private_key_length,
                                         (uint8_t*)message_buf,
                                         message_length,
                                         output_buffer,
                                         &output_length);
        break;
    case SECURE_MESSAGE_VERIFY:
        res = themis_secure_message_verify((uint8_t*)pub_buf,
                                           public_key_length,
                                           (uint8_t*)message_buf,
                                           message_length,
                                           output_buffer,
                                           &output_length);
        break;
    default:
        res = THEMIS_NOT_SUPPORTED;
        break;
    }

    if (res != THEMIS_SUCCESS) {
        goto err;
    }

    output = (*env)->NewByteArray(env, output_length);
    if (!output) {
        res = THEMIS_NO_MEMORY;
        goto err;
    }

    output_bytes = (*env)->GetPrimitiveArrayCritical(env, output, NULL);
    if (!output_bytes) {
        res = THEMIS_FAIL;
        goto err;
    }
    memmove(output_bytes, output_buffer, output_length);
    (*env)->ReleasePrimitiveArrayCritical(env, output, output_bytes, 0);

err:
    free(output_buffer);

    if (message_buf) {
        (*env)->ReleaseByteArrayElements(env, message, message_buf, 0);
    }

    if (pub_buf) {
        (*env)->ReleaseByteArrayElements(env, public_key, pub_buf, 0);
    }

    if (priv_buf) {
        (*env)->ReleaseByteArrayElements(env, private_key, priv_buf, 0);
    }

    if (THEMIS_SUCCESS == res) {
        return output;
    }

    return NULL;
}
