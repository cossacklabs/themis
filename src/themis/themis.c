#include "themis.h"
#include <soter/soter.h>
#include <stdio.h>
#include <string.h>

#define THEMIS_VERSION_TEXT "0.2.0 with soter "
#define THEMIS_VERSION_TEXT_LENGTH sizeof(THEMIS_VERSION_TEXT)

char themis_version_text[1024]=THEMIS_VERSION_TEXT;

const char* themis_version(){
    memcpy(themis_version_text+THEMIS_VERSION_TEXT_LENGTH-1, soter_version(), strlen(soter_version())+2);
    return themis_version_text;
}