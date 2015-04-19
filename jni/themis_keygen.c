/**
 * @file
 *
 * (c) CossackLabs
 */

#include <jni.h>
#include <common/error.h>
#include <themis/secure_message.h>

JNIEXPORT jobjectArray JNICALL Java_com_cossacklabs_themis_KeypairGenerator_generateKeys(JNIEnv *env, jclass thiz)
{
	size_t private_length = 0;
	size_t public_length = 0;

	jbyteArray private;
	jbyteArray public;

	jbyte *priv_buf;
	jbyte *pub_buf;

	jobjectArray keys;

	themis_status_t res = themis_gen_ec_key_pair(NULL, &private_length, NULL, &public_length);
	if (HERMES_BUFFER_TOO_SMALL != res)
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

	res = themis_gen_ec_key_pair(priv_buf, &private_length, pub_buf, &public_length);

	(*env)->ReleaseByteArrayElements(env, public, pub_buf, 0);
	(*env)->ReleaseByteArrayElements(env, private, priv_buf, 0);

	if (HERMES_SUCCESS != res)
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
