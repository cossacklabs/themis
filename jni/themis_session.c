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
#include <themis/secure_session.h>
#include <themis/secure_session_t.h>
#include <themis/themis_error.h>
/*extern JavaVM *g_vm;*/

#define SESSION_BUFFER_FIELD_NAME "sessionPtr"
#define SESSION_BUFFER_FIELD_SIG "J"

#define SESSION_GET_PUB_KEY_NAME "getPublicKeyForId"
#define SESSION_GET_PUB_KEY_SIG "([B)[B"

#define SESSION_STATE_CHANGED_NAME "stateChanged"
#define SESSION_STATE_CHANGED_SIG "(I)V"

/* These should correspond to SessionDataType enum in SecureSession.java */
#define NO_DATA 0
#define PROTOCOL_DATA 1
#define USER_DATA 2

typedef struct session_with_callbacks_type session_with_callbacks_t;

struct session_with_callbacks_type {
    secure_session_t session;
    secure_session_user_callbacks_t callbacks;
    JNIEnv* env;
    jobject thiz;
};

/*static JNIEnv* get_env(void)
{
        JNIEnv* res = NULL;

        if (JNI_OK == (*g_vm)->GetEnv(g_vm, (void **)&res, JNI_VERSION_1_6))
        {
                return res;
        }
        else
        {
                return NULL;
        }
}*/

static session_with_callbacks_t* get_native_session(JNIEnv* env, jobject session_obj)
{
    jfieldID session_field_id = (*env)->GetFieldID(env,
                                                   (*env)->GetObjectClass(env, session_obj),
                                                   SESSION_BUFFER_FIELD_NAME,
                                                   SESSION_BUFFER_FIELD_SIG);
    intptr_t session_ptr;

    if (!session_field_id) {
        return NULL;
    }

    session_ptr = (intptr_t)((*env)->GetLongField(env, session_obj, session_field_id));

    return (session_with_callbacks_t*)session_ptr;
}

/*JNIEXPORT jint JNICALL Java_com_cossacklabs_themis_SecureSession_getSessionContextSize(JNIEnv *env, jobject thiz)
{
        return (jint)sizeof(session_with_callbacks_t);
}*/

/* Make sure this method is called in the context of Java thread */
static void on_state_changed(int event, void* user_data)
{
    session_with_callbacks_t* ctx = (session_with_callbacks_t*)user_data;
    jmethodID state_changed_method = NULL;

    if (!ctx) {
        return;
    }

    state_changed_method = (*(ctx->env))
                               ->GetMethodID(ctx->env,
                                             (*(ctx->env))->GetObjectClass(ctx->env, ctx->thiz),
                                             SESSION_STATE_CHANGED_NAME,
                                             SESSION_STATE_CHANGED_SIG);
    if (!state_changed_method) {
        return;
    }

    (*(ctx->env))->CallVoidMethod(ctx->env, ctx->thiz, state_changed_method, (jint)event);
}

/* Make sure this method is called in the context of Java thread */
static int on_get_public_key_for_id(
    const void* id, size_t id_length, void* key_buffer, size_t key_buffer_length, void* user_data)
{
    session_with_callbacks_t* ctx = (session_with_callbacks_t*)user_data;
    jmethodID get_key_method = NULL;
    jbyteArray public_key = NULL;
    jbyteArray peer_id = NULL;

    jbyte* peer_id_buf = NULL;

    jsize public_key_length = 0;

    if (!ctx) {
        return THEMIS_FAIL;
    }

    if (!(ctx->env) || !(ctx->thiz)) {
        return THEMIS_FAIL;
    }

    get_key_method = (*(ctx->env))
                         ->GetMethodID(ctx->env,
                                       (*(ctx->env))->GetObjectClass(ctx->env, ctx->thiz),
                                       SESSION_GET_PUB_KEY_NAME,
                                       SESSION_GET_PUB_KEY_SIG);
    if (!get_key_method) {
        return THEMIS_FAIL;
    }

    /*
     * Normally peer IDs should not be this big, but just in case, avoid
     * integer overflow here. JVM cannot allocate more than 2 GB in one chunk.
     */
    if (id_length > INT32_MAX) {
        return THEMIS_NO_MEMORY;
    }

    peer_id = (*(ctx->env))->NewByteArray(ctx->env, id_length);
    if (!peer_id) {
        return THEMIS_FAIL;
    }

    peer_id_buf = (*(ctx->env))->GetByteArrayElements(ctx->env, peer_id, NULL);
    if (!peer_id_buf) {
        return THEMIS_FAIL;
    }

    memcpy(peer_id_buf, id, id_length);
    (*(ctx->env))->ReleaseByteArrayElements(ctx->env, peer_id, peer_id_buf, 0);

    public_key = (*(ctx->env))->CallObjectMethod(ctx->env, ctx->thiz, get_key_method, peer_id);
    if (!public_key) {
        return THEMIS_FAIL;
    }

    public_key_length = (*(ctx->env))->GetArrayLength(ctx->env, public_key);
    if ((size_t)public_key_length > key_buffer_length) {
        return THEMIS_BUFFER_TOO_SMALL;
    }

    (*(ctx->env))->GetByteArrayRegion(ctx->env, public_key, 0, public_key_length, key_buffer);
    return THEMIS_SUCCESS;
}

JNIEXPORT jlong JNICALL Java_com_cossacklabs_themis_SecureSession_jniLoad(JNIEnv* env,
                                                                          jobject thiz,
                                                                          jbyteArray state)
{
    UNUSED(thiz);

    size_t state_length = (*env)->GetArrayLength(env, state);

    themis_status_t themis_status;
    session_with_callbacks_t* ctx = NULL;

    jbyte* state_buf = (*env)->GetByteArrayElements(env, state, NULL);
    if (!state_buf) {
        return 0;
    }

    ctx = malloc(sizeof(session_with_callbacks_t));
    if (!ctx) {
        goto err;
    }

    memset(ctx, 0, sizeof(session_with_callbacks_t));

    ctx->callbacks.get_public_key_for_id = on_get_public_key_for_id;
    ctx->callbacks.state_changed = on_state_changed;
    ctx->callbacks.user_data = ctx;

    themis_status = secure_session_load(&(ctx->session), state_buf, state_length, &(ctx->callbacks));
    if (THEMIS_SUCCESS != themis_status) {
        free(ctx);
        ctx = NULL;
        goto err;
    }

err:

    if (state_buf) {
        (*env)->ReleaseByteArrayElements(env, state, state_buf, 0);
    }

    return (intptr_t)ctx;
}

JNIEXPORT jbyteArray JNICALL Java_com_cossacklabs_themis_SecureSession_jniSave(JNIEnv* env, jobject thiz)
{
    session_with_callbacks_t* ctx = get_native_session(env, thiz);

    size_t state_length = 0;
    jbyteArray state = NULL;
    jbyte* state_buf = NULL;

    themis_status_t res;

    if (!ctx) {
        return NULL;
    }

    res = secure_session_save(&(ctx->session), NULL, &state_length);
    if (THEMIS_BUFFER_TOO_SMALL != res) {
        return NULL;
    }

    /*
     * Normally serialized Secure Session should not be this big, but just in case,
     * avoid integer overflow here. JVM cannot allocate more than 2 GB in one chunk.
     */
    if (state_length > INT32_MAX) {
        res = THEMIS_NO_MEMORY;
        return NULL;
    }

    state = (*env)->NewByteArray(env, state_length);
    if (!state) {
        return NULL;
    }

    state_buf = (*env)->GetByteArrayElements(env, state, NULL);
    if (!state_buf) {
        return NULL;
    }

    res = secure_session_save(&(ctx->session), state_buf, &state_length);
    (*env)->ReleaseByteArrayElements(env, state, state_buf, 0);

    if (THEMIS_SUCCESS == res) {
        return state;
    }

    return NULL;
}

JNIEXPORT jboolean JNICALL Java_com_cossacklabs_themis_SecureSession_jniIsEstablished(JNIEnv* env,
                                                                                      jobject thiz)
{
    session_with_callbacks_t* ctx = get_native_session(env, thiz);

    if (!ctx) {
        return JNI_FALSE;
    }

    if (secure_session_is_established(&(ctx->session))) {
        return JNI_TRUE;
    }

    return JNI_FALSE;
}

JNIEXPORT jbyteArray JNICALL Java_com_cossacklabs_themis_SecureSession_jniWrap(JNIEnv* env,
                                                                               jobject thiz,
                                                                               jbyteArray data)
{
    size_t data_length = (*env)->GetArrayLength(env, data);
    session_with_callbacks_t* ctx = get_native_session(env, thiz);

    size_t wrapped_data_length = 0;

    jbyte* data_buf = NULL;
    jbyte* wrapped_data_buf = NULL;

    jbyteArray output = NULL;
    jbyteArray wrapped_data = NULL;

    themis_status_t res;

    if (!ctx) {
        return NULL;
    }

    data_buf = (*env)->GetByteArrayElements(env, data, NULL);
    if (!data_buf) {
        return NULL;
    }

    ctx->env = env;
    ctx->thiz = thiz;

    res = secure_session_wrap(&(ctx->session), data_buf, data_length, NULL, &wrapped_data_length);
    if (THEMIS_BUFFER_TOO_SMALL != res) {
        goto err;
    }

    /*
     * Secure Session protocol can handle messages up to 4 GB, but JVM does not
     * support byte arrays bigger that 2 GB. We just cannot allocate that much.
     */
    if (wrapped_data_length > INT32_MAX) {
        res = THEMIS_NO_MEMORY;
        goto err;
    }

    wrapped_data = (*env)->NewByteArray(env, wrapped_data_length);
    if (!wrapped_data) {
        goto err;
    }

    wrapped_data_buf = (*env)->GetByteArrayElements(env, wrapped_data, NULL);
    if (!wrapped_data_buf) {
        goto err;
    }

    res = secure_session_wrap(&(ctx->session), data_buf, data_length, wrapped_data_buf, &wrapped_data_length);
    if (THEMIS_SUCCESS != res) {
        goto err;
    }

    output = wrapped_data;

err:

    if (wrapped_data_buf) {
        (*env)->ReleaseByteArrayElements(env, wrapped_data, wrapped_data_buf, 0);
    }

    ctx->env = NULL;
    ctx->thiz = NULL;

    if (data_buf) {
        (*env)->ReleaseByteArrayElements(env, data, data_buf, 0);
    }

    return output;
}

JNIEXPORT jobjectArray JNICALL Java_com_cossacklabs_themis_SecureSession_jniUnwrap(JNIEnv* env,
                                                                                   jobject thiz,
                                                                                   jbyteArray wrapped_data)
{
    size_t wrapped_data_length = (*env)->GetArrayLength(env, wrapped_data);
    session_with_callbacks_t* ctx = get_native_session(env, thiz);

    size_t data_length;

    themis_status_t res;

    jbyte data_type;
    jbyteArray data_type_array = NULL;
    jbyteArray data_array = NULL;

    jbyte* wrapped_data_buf = NULL;
    jbyte* data_buf = NULL;

    jobjectArray unwrapped_data = NULL;
    jobjectArray output = NULL;

    if (!ctx) {
        return NULL;
    }

    wrapped_data_buf = (*env)->GetByteArrayElements(env, wrapped_data, NULL);
    if (!wrapped_data_buf) {
        return NULL;
    }

    unwrapped_data = (*env)->NewObjectArray(env, 2, (*env)->GetObjectClass(env, wrapped_data), NULL);
    if (!unwrapped_data) {
        goto err;
    }

    data_type_array = (*env)->NewByteArray(env, 1);
    if (!data_type_array) {
        goto err;
    }

    ctx->env = env;
    ctx->thiz = thiz;

    res = secure_session_unwrap(&(ctx->session), wrapped_data_buf, wrapped_data_length, NULL, &data_length);
    if ((THEMIS_SUCCESS == res) && (0 == data_length)) {
        /* This is the end of negotiation. No return data */
        data_type = NO_DATA;
        (*env)->SetByteArrayRegion(env, data_type_array, 0, 1, &data_type);
        (*env)->SetObjectArrayElement(env, unwrapped_data, 0, data_type_array);
        output = unwrapped_data;
        goto err;
    } else if (THEMIS_BUFFER_TOO_SMALL != res) {
        goto err;
    }

    /*
     * Secure Session protocol can handle messages up to 4 GB, but JVM does not
     * support byte arrays bigger that 2 GB. We just cannot allocate that much.
     */
    if (data_length > INT32_MAX) {
        res = THEMIS_NO_MEMORY;
        goto err;
    }

    data_array = (*env)->NewByteArray(env, data_length);
    if (!data_array) {
        goto err;
    }

    data_buf = (*env)->GetByteArrayElements(env, data_array, NULL);
    if (!data_buf) {
        goto err;
    }

    res = secure_session_unwrap(&(ctx->session), wrapped_data_buf, wrapped_data_length, data_buf, &data_length);
    if ((THEMIS_SUCCESS == res) && (0 == data_length)) {
        /* This is the end of negotiation. No return data */
        data_type = NO_DATA;
        (*env)->SetByteArrayRegion(env, data_type_array, 0, 1, &data_type);
        (*env)->SetObjectArrayElement(env, unwrapped_data, 0, data_type_array);
        output = unwrapped_data;
        goto err;
    } else if ((THEMIS_SSESSION_SEND_OUTPUT_TO_PEER == res) && (data_length > 0)) {
        /* Negotiation continues. Send output to peer */
        data_type = PROTOCOL_DATA;
        (*env)->SetByteArrayRegion(env, data_type_array, 0, 1, &data_type);
        (*env)->SetObjectArrayElement(env, unwrapped_data, 0, data_type_array);
        (*env)->SetObjectArrayElement(env, unwrapped_data, 1, data_array);
        output = unwrapped_data;
        goto err;
    } else if ((THEMIS_SUCCESS == res) && (data_length > 0)) {
        /* This is user data */
        data_type = USER_DATA;
        (*env)->SetByteArrayRegion(env, data_type_array, 0, 1, &data_type);
        (*env)->SetObjectArrayElement(env, unwrapped_data, 0, data_type_array);
        (*env)->SetObjectArrayElement(env, unwrapped_data, 1, data_array);
        output = unwrapped_data;
        goto err;
    }

err:

    if (data_buf) {
        (*env)->ReleaseByteArrayElements(env, data_array, data_buf, 0);
    }

    ctx->env = NULL;
    ctx->thiz = NULL;

    if (wrapped_data_buf) {
        (*env)->ReleaseByteArrayElements(env, wrapped_data, wrapped_data_buf, 0);
    }

    return output;
}

JNIEXPORT jbyteArray JNICALL Java_com_cossacklabs_themis_SecureSession_jniGenerateConnect(JNIEnv* env,
                                                                                          jobject thiz)
{
    size_t request_length = 0;
    session_with_callbacks_t* ctx = get_native_session(env, thiz);
    jbyteArray output = NULL;
    jbyteArray request = NULL;

    jbyte* request_buf = NULL;

    themis_status_t res;

    if (NULL == ctx) {
        return NULL;
    }

    ctx->env = env;
    ctx->thiz = thiz;

    res = secure_session_generate_connect_request(&(ctx->session), NULL, &request_length);
    if (THEMIS_BUFFER_TOO_SMALL != res) {
        goto err;
    }

    /*
     * Normally the connection request should not be this big, but just in case,
     * avoid integer overflow here. JVM cannot allocate more than 2 GB at once.
     */
    if (request_length > INT32_MAX) {
        res = THEMIS_NO_MEMORY;
        goto err;
    }

    request = (*env)->NewByteArray(env, request_length);
    if (!request) {
        goto err;
    }

    request_buf = (*env)->GetByteArrayElements(env, request, NULL);
    if (!request_buf) {
        goto err;
    }

    res = secure_session_generate_connect_request(&(ctx->session), request_buf, &request_length);
    if (THEMIS_SUCCESS != res) {
        goto err;
    }

    output = request;

err:

    if (request_buf) {
        (*env)->ReleaseByteArrayElements(env, request, request_buf, 0);
    }

    ctx->env = NULL;
    ctx->thiz = NULL;

    return output;
}
JNIEXPORT jbyteArray JNICALL Java_com_cossacklabs_themis_SecureSession_jniGenerateConntect(JNIEnv* env,
                                                                                           jobject thiz)
{
    return Java_com_cossacklabs_themis_SecureSession_jniGenerateConnect(env, thiz);
}

JNIEXPORT jlong JNICALL Java_com_cossacklabs_themis_SecureSession_create(JNIEnv* env,
                                                                         jobject thiz,
                                                                         jbyteArray id,
                                                                         jbyteArray sign_key)
{
    UNUSED(thiz);

    size_t id_length = (*env)->GetArrayLength(env, id);
    size_t sign_key_length = (*env)->GetArrayLength(env, sign_key);

    jbyte* id_buf = NULL;
    jbyte* sign_key_buf = NULL;

    themis_status_t themis_status;

    session_with_callbacks_t* ctx = NULL;

    id_buf = (*env)->GetByteArrayElements(env, id, NULL);
    if (!id_buf) {
        return 0;
    }

    sign_key_buf = (*env)->GetByteArrayElements(env, sign_key, NULL);
    if (!sign_key_buf) {
        goto err;
    }

    ctx = malloc(sizeof(session_with_callbacks_t));
    if (!ctx) {
        goto err;
    }

    memset(ctx, 0, sizeof(session_with_callbacks_t));

    /*ctx->thiz = (*env)->NewGlobalRef(env, thiz);
        if (!(ctx->thiz))
        {
                free(ctx);
                ctx = NULL;
                goto err;
        }*/

    ctx->callbacks.get_public_key_for_id = on_get_public_key_for_id;
    ctx->callbacks.state_changed = on_state_changed;
    ctx->callbacks.user_data = ctx;

    themis_status = secure_session_init(&(ctx->session),
                                        id_buf,
                                        id_length,
                                        sign_key_buf,
                                        sign_key_length,
                                        &(ctx->callbacks));
    if (THEMIS_SUCCESS != themis_status) {
        free(ctx);
        ctx = NULL;
        goto err;
    }

err:

    if (sign_key_buf) {
        (*env)->ReleaseByteArrayElements(env, sign_key, sign_key_buf, 0);
    }

    if (id_buf) {
        (*env)->ReleaseByteArrayElements(env, id, id_buf, 0);
    }

    return (intptr_t)ctx;
}

JNIEXPORT void JNICALL Java_com_cossacklabs_themis_SecureSession_destroy(JNIEnv* env, jobject thiz)
{
    jfieldID session_field_id = (*env)->GetFieldID(env,
                                                   (*env)->GetObjectClass(env, thiz),
                                                   SESSION_BUFFER_FIELD_NAME,
                                                   SESSION_BUFFER_FIELD_SIG);
    intptr_t session_ptr;
    session_with_callbacks_t* ctx;

    if (!session_field_id) {
        /* TODO: Maybe throw RuntimeException? */
        return;
    }

    session_ptr = (intptr_t)((*env)->GetLongField(env, thiz, session_field_id));
    ctx = (session_with_callbacks_t*)session_ptr;

    secure_session_cleanup(&(ctx->session));
    (*env)->SetLongField(env, thiz, session_field_id, 0);

    /*(*env)->DeleteGlobalRef(env, ctx->thiz);*/
    free(ctx);
}
