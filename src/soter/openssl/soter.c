#include <soter/soter.h>

#include <openssl/crypto.h>

#include <stdio.h>
#include <string.h>

#define SOTER_VERSION_TEXT "0.2.0 with "
#define SOTER_VERSION_TEXT_LENGTH sizeof(SOTER_VERSION_TEXT)

char soter_version_text[256]=SOTER_VERSION_TEXT;

const char* soter_version(){
    memcpy(soter_version_text+SOTER_VERSION_TEXT_LENGTH-1, SSLeay_version(SSLEAY_VERSION), strlen(SSLeay_version(SSLEAY_VERSION))+1);
    return soter_version_text;
}