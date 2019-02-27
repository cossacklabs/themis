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
#include <themis/themis_error.h>
#include <themis/secure_keygen.h>

/* Should be same as in AsymmetricKey.java */
#define KEYTYPE_EC 0
#define KEYTYPE_RSA 1

JNIEXPORT jobjectArray JNICALL Java_com_cossacklabs_themis_KeypairGenerator_generateKeys(JNIEnv *env, jobject thiz, jint key_type)
{
	size_t private_length = 0;
	size_t public_length = 0;

	jbyteArray private;
	jbyteArray public;

	jbyte *priv_buf;
	jbyte *pub_buf;

	jobjectArray keys;

	themis_status_t res;

	switch (key_type)
	{
	case KEYTYPE_EC:
		res = themis_gen_ec_key_pair(NULL, &private_length, NULL, &public_length);
		break;
	case KEYTYPE_RSA:
		res = themis_gen_rsa_key_pair(NULL, &private_length, NULL, &public_length);
		break;
	default:
		return NULL;
	}

	if (THEMIS_BUFFER_TOO_SMALL != res)
	{
		return NULL;
	}

	private = (*env)->NewByteArray(env, private_length);
	if (!private)
	{
		return NULL;
	}

	public = (*env)->NewByteArray(env, public_length);
	if (!public)
	{
		return NULL;
	}

	priv_buf = (*env)->GetByteArrayElements(env, private, NULL);
	if (!priv_buf)
	{
		return NULL;
	}

	pub_buf = (*env)->GetByteArrayElements(env, public, NULL);
	if (!pub_buf)
	{
		(*env)->ReleaseByteArrayElements(env, private, priv_buf, 0);
		return NULL;
	}

	switch (key_type)
	{
	case KEYTYPE_EC:
		res = themis_gen_ec_key_pair((uint8_t *)priv_buf, &private_length, (uint8_t *)pub_buf, &public_length);
		break;
	case KEYTYPE_RSA:
		res = themis_gen_rsa_key_pair((uint8_t *)priv_buf, &private_length, (uint8_t *)pub_buf, &public_length);
		break;
	default:
		(*env)->ReleaseByteArrayElements(env, public, pub_buf, 0);
		(*env)->ReleaseByteArrayElements(env, private, priv_buf, 0);
		return NULL;
	}

	(*env)->ReleaseByteArrayElements(env, public, pub_buf, 0);
	(*env)->ReleaseByteArrayElements(env, private, priv_buf, 0);

	if (THEMIS_SUCCESS != res)
	{
		return NULL;
	}

	keys = (*env)->NewObjectArray(env, 2, (*env)->GetObjectClass(env, private), NULL);
	if (!keys)
	{
		return NULL;
	}

	(*env)->SetObjectArrayElement(env, keys, 0, private);
	(*env)->SetObjectArrayElement(env, keys, 1, public);

	return keys;
}
