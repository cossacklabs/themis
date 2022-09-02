# CMake project configuration for Android.
project(AndroidThemis C)
cmake_minimum_required(VERSION 3.22.1)

# BoringSSL
# =========

add_subdirectory(../third_party/boringssl/src boringssl)

# Soter
# =====

file(GLOB SOTER_SOURCES
        ../src/soter/*.c
        ../src/soter/ed25519/*.c
        ../src/soter/boringssl/*.c
)
add_library(soter STATIC ${SOTER_SOURCES})
target_compile_definitions(soter
    PRIVATE
        -DBORINGSSL
        -DCRYPTO_ENGINE_PATH=boringssl
)
target_include_directories(soter
    PUBLIC
        ../include
    PRIVATE
        ../src
        ../third_party/boringssl/src/include
)

# Themis
# ======

file(GLOB THEMIS_SOURCES ../src/themis/*.c)
add_library(themis STATIC ${THEMIS_SOURCES})
target_compile_definitions(themis
    PRIVATE
        -DBORINGSSL
        -DCRYPTO_ENGINE_PATH=boringssl
)
target_include_directories(themis
    PUBLIC
        ../include
    PRIVATE
        ../src
        ../third_party/boringssl/src/include
)

# JNI library
# ===========

file(GLOB THEMIS_JNI_SOURCES *.c)
add_library(themis_jni SHARED ${THEMIS_JNI_SOURCES})
target_compile_definitions(themis_jni
    PRIVATE
        -DBORINGSSL
        -DCRYPTO_ENGINE_PATH=boringssl
)
target_include_directories(themis_jni
    PRIVATE
        ../include
        ../src
        ../third_party/boringssl/src/include
)
target_link_libraries(themis_jni PRIVATE themis soter crypto decrepit)
