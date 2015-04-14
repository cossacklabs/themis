LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libcrypto
LOCAL_SRC_FILES := crypto_dummy.c

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := libsoter

LOCAL_SRC_FILES := $(patsubst jni/%,%, $(wildcard $(LOCAL_PATH)/../src/soter/*.c))
LOCAL_SRC_FILES += $(patsubst jni/%,%, $(wildcard $(LOCAL_PATH)/../src/soter/openssl/*.c))

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../src $(LOCAL_PATH)
LOCAL_CFLAGS := -DOPENSSL

LOCAL_SHARED_LIBRARIES := libcrypto
LOCAL_ALLOW_UNDEFINED_SYMBOLS := true

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := libthemis

LOCAL_SRC_FILES := $(patsubst jni/%,%, $(wildcard $(LOCAL_PATH)/../src/themis/*.c))
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../src $(LOCAL_PATH)

LOCAL_SHARED_LIBRARIES := libsoter

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := soter_test

LOCAL_SRC_FILES := $(patsubst jni/%,%, $(wildcard $(LOCAL_PATH)/../tests/soter/*.c))
LOCAL_SRC_FILES += $(patsubst jni/%,%, $(wildcard $(LOCAL_PATH)/../tests/common/*.c))

LOCAL_SHARED_LIBRARIES := libsoter

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../src $(LOCAL_PATH)/../tests

LOCAL_LDFLAGS := -Wl,--allow-shlib-undefined

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_MODULE := themis_test

LOCAL_SRC_FILES := $(patsubst jni/%,%, $(wildcard $(LOCAL_PATH)/../tests/themis/*.c))
LOCAL_SRC_FILES += $(patsubst jni/%,%, $(wildcard $(LOCAL_PATH)/../tests/common/*.c))

LOCAL_SHARED_LIBRARIES := libthemis libsoter

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../src $(LOCAL_PATH)/../tests

include $(BUILD_EXECUTABLE)

