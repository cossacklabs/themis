/**
 * @file
 *
 * (c) CossackLabs
 */

#include <jni.h>
#include <string.h>

/*JavaVM *g_vm = NULL;*/

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
	/*g_vm = vm;*/

	return JNI_VERSION_1_6;
}

