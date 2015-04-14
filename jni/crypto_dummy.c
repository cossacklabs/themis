/* DO NOT DELETE THIS FILE
 *
 * This file is used to compile dummy crypto library for Android.
 *
 * Android does not expose OpenSSL as part of its public NDK APIs, so to use it we do following steps:
 * 1. Use OpenSSL header files from AOSP in our sources.
 * 2. Compile and use this dummy library instead of libcrypto.so to add DT_NEEDED tag with "libcrypto.so" to our library.
 * 3. Instruct Android NDK build system to ignore unresolved symbols during link stage of our library.
 *
 * */
