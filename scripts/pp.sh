#!/usr/bin/env bash
#
# Copyright (c) 2017 Cossack Labs Limited
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

if [[ $# -ne 2 || ! -f $1 ]] ; then
    echo "Invalid args / Check file "
    exit 
fi

file_name=$1

grep '^\s*#\s*include' $file_name > /tmp/include.c

echo "#define SOTER_ERROR_OUT(message)
#define SOTER_DEBUG_OUT(message)
#define SOTER_CHECK(x) if(!(x)){	\
    SOTER_ERROR_OUT(#x);		\
    return SOTER_FAIL;		\
    }

#define SOTER_CHECK_(x) if(!(x)){	\
    SOTER_ERROR_OUT(#x);		\
    return NULL;		\
    }

#define SOTER_CHECK_PARAM(x) if(!(x)){	\
    SOTER_ERROR_OUT(#x);		\
    return SOTER_INVALID_PARAMETER;\
    }

#define SOTER_CHECK_PARAM_(x) if(!(x)){	\
    SOTER_ERROR_OUT(#x);		\
    return NULL;\
    }

#define SOTER_CHECK_MALLOC(x,y)	\
    y=malloc(sizeof(x));		\
    if(!(x)){			\
    SOTER_ERROR_OUT(#x);		\
    return SOTER_NO_MEMORY;	\
    }

#define SOTER_CHECK_MALLOC_(x)if(!(x)){	\
    SOTER_ERROR_OUT(#x);		\
    return NULL;\
    }

#define SOTER_CHECK_MALLOC_(x)if(!(x)){	\
    SOTER_ERROR_OUT(#x);		\
    return NULL;\
    }

#define SOTER_CHECK_FREE(x,y)if(!(x)){	\
    SOTER_ERROR_OUT(#x);		\
    free(y);			\
    return SOTER_FAIL;		\
    }

#define SOTER_IF_FAIL(x,y)if(!(x)){	\
    SOTER_ERROR_OUT(#x);		\
    {y;}			\
    return SOTER_FAIL;		\
    }

#define SOTER_IF_FAIL_(x,y)if(!(x)){	\
    SOTER_ERROR_OUT(#x);		\
    {y;}			\
    return NULL;		\
    }

#define SOTER_STATUS_CHECK(x,y){		\
  int res=x;					\
  if(res!=y){					\
     SOTER_ERROR_OUT(#x);				\
     return res;					\
  }							\
  }

#define SOTER_STATUS_CHECK_FREE(x,y,z){		\
  int res=x;					\
  if(res!=y){					\
     SOTER_ERROR_OUT(#x);				\
     free(z);						\
     return res;					\
  }							\
  }
#define THEMIS_ERROR_OUT(message)  
#define THEMIS_DEBUG_OUT(message) 

#define THEMIS_CHECK(x) SOTER_CHECK(x)
#define THEMIS_CHECK_(x) SOTER_CHECK_(x)

#define THEMIS_CHECK_PARAM(x) SOTER_CHECK_PARAM(x)
#define THEMIS_CHECK_PARAM_(x) SOTER_CHECK_PARAM_(x)

#define THEMIS_CHECK_MALLOC(x,y) SOTER_CHECK_MALLOC(x,y)
#define THEMIS_CHECK_MALLOC_(x) SOTER_CHECK_MALLOC_(x)
#define THEMIS_CHECK_FREE(x,y) SOTER_CHECK_FREE(x,y)
#define THEMIS_IF_FAIL(x,y) SOTER_IF_FAIL(x,y)
#define THEMIS_IF_FAIL_(x,y) SOTER_IF_FAIL_(x,y)
#define THEMIS_STATUS_CHECK(x,y) SOTER_STATUS_CHECK(x,y)
#define THEMIS_STATUS_CHECK_FREE(x,y,z) SOTER_STATUS_CHECK_FREE(x,y,z)
#define THEMIS_CHECK__(cond, on_fail_call) do{if(!(cond)){on_fail_call;}}while(0)
" > /tmp/code.c
grep -Pv '^\s*#\s*include\b' $file_name >> /tmp/code.c
gcc -I src -E -CC /tmp/code.c | grep -v ^# > /tmp/preprocessed.c
cat /tmp/include.c > $2
cat /tmp/preprocessed.c >> $2