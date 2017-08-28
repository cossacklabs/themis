#
# Copyright (c) 2015 Cossack Labs Limited
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

LOCAL_PATH := $(call my-dir)

ifeq ($(BORINGSSL_PATH),)
    $(error "boringSSL path must be set")
endif

include $(CLEAR_VARS)

LOCAL_MODULE := libsoter

LOCAL_SRC_FILES := $(patsubst jni/%,%, $(wildcard $(LOCAL_PATH)/../src/soter/*.c))
LOCAL_SRC_FILES += $(patsubst jni/%,%, $(wildcard $(LOCAL_PATH)/../src/soter/boringssl/*.c))
LOCAL_SRC_FILES += $(patsubst jni/%,%, $(wildcard $(LOCAL_PATH)/../src/soter/ed25519/*.c))

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../src
LOCAL_CFLAGS := -DBORINGSSL -DCRYPTO_ENGINE_PATH=boringssl
LOCAL_EXPORT_CFLAGS := -DBORINGSSL
LOCAL_STATIC_LIBRARIES := libcrypto libdecrepit

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := libthemis

LOCAL_SRC_FILES := $(patsubst jni/%,%, $(wildcard $(LOCAL_PATH)/../src/themis/*.c))
LOCAL_CFLAGS := -DBORINGSSL -DCRYPTO_ENGINE_PATH=boringssl
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../src 
LOCAL_STATIC_LIBRARIES := libsoter libcrypto libdecrepit

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := libthemis_jni

LOCAL_SRC_FILES := themis_jni.c themis_message.c themis_keygen.c themis_cell.c themis_session.c
LOCAL_SRC_FILES += themis_compare.c
LOCAL_CFLAGS := -DBORINGSSL -DCRYPTO_ENGINE_PATH=boringssl
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../src
LOCAL_STATIC_LIBRARIES := libthemis libsoter libcrypto libdecrepit
# LOCAL_LDFLAGS := -llog

include $(BUILD_SHARED_LIBRARY)

#include $(CLEAR_VARS)

#LOCAL_MODULE := soter_test

#LOCAL_SRC_FILES := $(patsubst jni/%,%, $(wildcard $(LOCAL_PATH)/../tests/soter/*.c))
#LOCAL_SRC_FILES += $(patsubst jni/%,%, $(wildcard $(LOCAL_PATH)/../tests/common/*.c))

#LOCAL_SHARED_LIBRARIES := libthemis_jni

#LOCAL_C_INCLUDES := $(LOCAL_PATH)/../src $(LOCAL_PATH)/../tests

#LOCAL_LDFLAGS := -Wl,--allow-shlib-undefined

#include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_MODULE := themis_test

LOCAL_SRC_FILES := $(patsubst jni/%,%, $(wildcard $(LOCAL_PATH)/../tests/themis/*.c))
LOCAL_SRC_FILES += $(patsubst jni/%,%, $(wildcard $(LOCAL_PATH)/../tests/common/*.c))

LOCAL_SHARED_LIBRARIES := libthemis_jni

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../src $(LOCAL_PATH)/../tests


include $(BUILD_EXECUTABLE)

# this is needed to disable targets not present in the NDK otherwise error happens
#BUILD_HOST_EXECUTABLE := 
#BUILD_HOST_STATIC_LIBRARY := 

#include $(LOCAL_PATH)/external/openssl/Android.mk

# We are making a separate target with slightly modified script to support building libcrypto with NDK
# We put it here to make minimal changes in original Google scripts to make importing new versions easier
include $(CLEAR_VARS)

#LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libcrypto
LOCAL_SRC_FILES := $(BORINGSSL_PATH)/build-$(TARGET_ARCH_ABI)/crypto/libcrypto.a
LOCAL_EXPORT_C_INCLUDES := $(BORINGSSL_PATH)/include
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_MODULE := libdecrepit
LOCAL_SRC_FILES := $(BORINGSSL_PATH)/build-$(TARGET_ARCH_ABI)/decrepit/libdecrepit.a
include $(PREBUILT_STATIC_LIBRARY)

