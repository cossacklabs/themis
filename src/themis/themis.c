/*
* Copyright (c) 2015 Cossack Labs Limited
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include <themis/themis.h>
#include <soter/soter.h>
#include <stdio.h>
#include <string.h>

#define THEMIS_VERSION_TEXT_LENGTH sizeof(THEMIS_VERSION_TEXT)

char themis_version_text[1024]=THEMIS_VERSION_TEXT;

const char* themis_version(){
    memcpy(themis_version_text+THEMIS_VERSION_TEXT_LENGTH-1, soter_version(), strlen(soter_version())+2);
    return themis_version_text;
}