/**
 * @file
 *
 * (c) CossackLabs
 */

#include <jni.h>
#include <common/error.h>
#include <themis/secure_message.h>

JNIEXPORT jbyteArray JNICALL Java_com_cossacklabs_themis_SecureMessage_process(JNIEnv *env, jclass thiz, jbyteArray private, jbyteArray public, jbyteArray message, jboolean is_wrap)
{
	size_t private_length = (*env)->GetArrayLength(env, private);
	size_t public_length = (*env)->GetArrayLength(env, public);
	size_t message_length = (*env)->GetArrayLength(env, message);
	size_t output_length = 0;

	jbyte *priv_buf = NULL;
	jbyte *pub_buf = NULL;
	jbyte *message_buf = NULL;
	jbyte *output_buf = NULL;

	themis_status_t res;
	jbyteArray output = NULL;

	priv_buf = (*env)->GetByteArrayElements(env, private, NULL);
	if (!priv_buf)
	{
		return NULL;
	}

	pub_buf = (*env)->GetByteArrayElements(env, public, NULL);
	if (!pub_buf)
	{
		goto err;
	}

	message_buf = (*env)->GetByteArrayElements(env, message, NULL);
	if (!message_buf)
	{
		goto err;
	}

	if (is_wrap)
	{
		res = themis_secure_message_wrap(priv_buf, private_length, pub_buf, public_length, message_buf, message_length, NULL, &output_length);
	}
	else
	{
		res = themis_secure_message_unwrap(priv_buf, private_length, pub_buf, public_length, message_buf, message_length, NULL, &output_length);
	}

	if (HERMES_BUFFER_TOO_SMALL != res)
	{
		goto err;
	}

	output = (*env)->NewByteArray(env, output_length);
	if (!output)
	{
		goto err;
	}

	output_buf = (*env)->GetByteArrayElements(env, output, NULL);
	if (!output_buf)
	{
		output = NULL;
		goto err;
	}

	if (is_wrap)
	{
		res = themis_secure_message_wrap(priv_buf, private_length, pub_buf, public_length, message_buf, message_length, output_buf, &output_length);
	}
	else
	{
		res = themis_secure_message_unwrap(priv_buf, private_length, pub_buf, public_length, message_buf, message_length, output_buf, &output_length);
	}

	if (HERMES_SUCCESS != res)
	{
		output = NULL;
	}

err:

	if (output_buf)
	{
		(*env)->ReleaseByteArrayElements(env, output, output_buf, 0);
	}

	if (message_buf)
	{
		(*env)->ReleaseByteArrayElements(env, message, message_buf, 0);
	}

	if (pub_buf)
	{
		(*env)->ReleaseByteArrayElements(env, public, pub_buf, 0);
	}

	if (priv_buf)
	{
		(*env)->ReleaseByteArrayElements(env, private, priv_buf, 0);
	}

	return output;
}
