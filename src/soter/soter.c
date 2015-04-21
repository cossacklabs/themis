#include "soter.h"

#ifdef OPENSSL
#include <openssl/crypto.h>
#endif

#ifdef LIBRESSL
#include <openssl/crypto.h>
#endif

#include <stdio.h>
#include <string.h>

#define SOTER_VERSION_TEXT "0.2.0 with "
#define SOTER_VERSION_TEXT_LENGTH sizeof(SOTER_VERSION_TEXT)

char soter_version_text[256]=SOTER_VERSION_TEXT;

const char* soter_version(){
#ifdef OPENSSL
    memcpy(soter_version_text+SOTER_VERSION_TEXT_LENGTH-1, SSLeay_version(SSLEAY_VERSION), strlen(SSLeay_version(SSLEAY_VERSION))+1);
#endif
#ifdef LIBRESSL
    memcpy(soter_version_text+SOTER_VERSION_TEXT_LENGTH-1, SSLeay_version(SSLEAY_VERSION), strlen(SSLeay_version(SSLEAY_VERSION))+1);
#endif
    return soter_version_text;
}