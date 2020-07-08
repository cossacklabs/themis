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
#include <themis/secure_comparator.h>
#include <themis/themis_error.h>

#define COMPARE_CTX_FIELD_NAME "nativeCtx"
#define COMPARE_CTX_FIELD_SIG "J"

static secure_comparator_t* get_native_ctx(JNIEnv* env, jobject compare_obj)
{
    jfieldID compare_ctx_field_id = (*env)->GetFieldID(env,
                                                       (*env)->GetObjectClass(env, compare_obj),
                                                       COMPARE_CTX_FIELD_NAME,
                                                       COMPARE_CTX_FIELD_SIG);
    intptr_t compare_ctx_ptr;

    if (!compare_ctx_field_id) {
        return NULL;
    }

    compare_ctx_ptr = (intptr_t)((*env)->GetLongField(env, compare_obj, compare_ctx_field_id));

    return (secure_comparator_t*)compare_ctx_ptr;
}

JNIEXPORT jint JNICALL Java_com_cossacklabs_themis_SecureCompare_jniAppend(JNIEnv* env,
                                                                           jobject thiz,
                                                                           jbyteArray secret)
{
    secure_comparator_t* ctx = get_native_ctx(env, thiz);
    size_t secret_length = (*env)->GetArrayLength(env, secret);
    jbyte* secret_buf;

    themis_status_t res;

    if (NULL == ctx) {
        return THEMIS_FAIL;
    }

    secret_buf = (*env)->GetByteArrayElements(env, secret, NULL);
    if (!secret_buf) {
        return THEMIS_FAIL;
    }

    res = secure_comparator_append_secret(ctx, secret_buf, secret_length);

    (*env)->ReleaseByteArrayElements(env, secret, secret_buf, 0);

    return (jint)res;
}

JNIEXPORT jbyteArray JNICALL Java_com_cossacklabs_themis_SecureCompare_jniBegin(JNIEnv* env, jobject thiz)
{
    secure_comparator_t* ctx = get_native_ctx(env, thiz);
    size_t compare_data_length = 0;

    jbyteArray compare_data;
    jbyte* compare_data_buf;

    themis_status_t res;

    if (NULL == ctx) {
        return NULL;
    }

    res = secure_comparator_begin_compare(ctx, NULL, &compare_data_length);
    if (THEMIS_BUFFER_TOO_SMALL != res) {
        return NULL;
    }

    /*
     * Normally the messages should not be this big, but just in case, avoid
     * integer overflow here. JVM cannot allocate more than 2 GB in one chunk.
     */
    if (compare_data_length > INT32_MAX) {
        res = THEMIS_NO_MEMORY;
        return NULL;
    }

    compare_data = (*env)->NewByteArray(env, compare_data_length);
    if (!compare_data) {
        return NULL;
    }

    compare_data_buf = (*env)->GetByteArrayElements(env, compare_data, NULL);
    if (!compare_data_buf) {
        return NULL;
    }

    res = secure_comparator_begin_compare(ctx, compare_data_buf, &compare_data_length);

    (*env)->ReleaseByteArrayElements(env, compare_data, compare_data_buf, 0);

    if (THEMIS_SCOMPARE_SEND_OUTPUT_TO_PEER == res) {
        return compare_data;
    }

    return NULL;
}

static void throwSecureCompareException(JNIEnv* env)
{
    jclass exception_class = (*env)->FindClass(env, "Lcom/cossacklabs/themis/SecureCompareException");
    if (exception_class) {
        (*env)->ThrowNew(env, exception_class, "");
    }
}

JNIEXPORT jbyteArray JNICALL Java_com_cossacklabs_themis_SecureCompare_jniProceed(JNIEnv* env,
                                                                                  jobject thiz,
                                                                                  jbyteArray compare_data)
{
    secure_comparator_t* ctx = get_native_ctx(env, thiz);
    size_t compare_data_length = (*env)->GetArrayLength(env, compare_data);

    jbyte* compare_data_buf;

    themis_status_t res = THEMIS_FAIL;
    size_t output_length = 0;
    jbyteArray output = NULL;
    jbyte* output_buf = NULL;

    if (NULL == ctx) {
        throwSecureCompareException(env);
        return NULL;
    }

    compare_data_buf = (*env)->GetByteArrayElements(env, compare_data, NULL);
    if (NULL == compare_data_buf) {
        throwSecureCompareException(env);
        return NULL;
    }

    res = secure_comparator_proceed_compare(ctx, compare_data_buf, compare_data_length, NULL, &output_length);
    if (THEMIS_BUFFER_TOO_SMALL != res) {
        goto err;
    }

    /*
     * Normally the messages should not be this big, but just in case, avoid
     * integer overflow here. JVM cannot allocate more than 2 GB in one chunk.
     */
    if (output_length > INT32_MAX) {
        res = THEMIS_NO_MEMORY;
        goto err;
    }

    output = (*env)->NewByteArray(env, output_length);
    if (!output) {
        goto err;
    }

    output_buf = (*env)->GetByteArrayElements(env, output, NULL);
    if (!output_buf) {
        goto err;
    }

    res = secure_comparator_proceed_compare(ctx,
                                            compare_data_buf,
                                            compare_data_length,
                                            output_buf,
                                            &output_length);

err:

    if (output_buf) {
        (*env)->ReleaseByteArrayElements(env, output, output_buf, 0);
    }

    if (compare_data_buf) {
        (*env)->ReleaseByteArrayElements(env, compare_data, compare_data_buf, 0);
    }

    if (THEMIS_SCOMPARE_SEND_OUTPUT_TO_PEER == res) {
        return output;
    }

    if (THEMIS_SUCCESS != res) {
        throwSecureCompareException(env);
    }

    return NULL;
}

JNIEXPORT jint JNICALL Java_com_cossacklabs_themis_SecureCompare_jniGetResult(JNIEnv* env, jobject thiz)
{
    return (jint)secure_comparator_get_result(get_native_ctx(env, thiz));
}

JNIEXPORT jlong JNICALL Java_com_cossacklabs_themis_SecureCompare_create(JNIEnv* env, jobject thiz)
{
    UNUSED(env);
    UNUSED(thiz);
    return (intptr_t)secure_comparator_create();
}

JNIEXPORT void JNICALL Java_com_cossacklabs_themis_SecureCompare_destroy(JNIEnv* env, jobject thiz)
{
    jfieldID compare_ctx_field_id = (*env)->GetFieldID(env,
                                                       (*env)->GetObjectClass(env, thiz),
                                                       COMPARE_CTX_FIELD_NAME,
                                                       COMPARE_CTX_FIELD_SIG);
    intptr_t compare_ctx_ptr;

    if (!compare_ctx_field_id) {
        return;
    }

    compare_ctx_ptr = (intptr_t)((*env)->GetLongField(env, thiz, compare_ctx_field_id));

    if (compare_ctx_ptr) {
        secure_comparator_destroy((secure_comparator_t*)compare_ctx_ptr);
        (*env)->SetLongField(env, thiz, compare_ctx_field_id, 0);
    }
}

JNIEXPORT jint JNICALL Java_com_cossacklabs_themis_SecureCompare_scompareMatch(JNIEnv* env, jobject thiz)
{
    UNUSED(env);
    UNUSED(thiz);
    return THEMIS_SCOMPARE_MATCH;
}

JNIEXPORT jint JNICALL Java_com_cossacklabs_themis_SecureCompare_scompareNoMatch(JNIEnv* env, jobject thiz)
{
    UNUSED(env);
    UNUSED(thiz);
    return THEMIS_SCOMPARE_NO_MATCH;
}

JNIEXPORT jint JNICALL Java_com_cossacklabs_themis_SecureCompare_scompareNotReady(JNIEnv* env,
                                                                                  jobject thiz)
{
    UNUSED(env);
    UNUSED(thiz);
    return THEMIS_SCOMPARE_NOT_READY;
}
