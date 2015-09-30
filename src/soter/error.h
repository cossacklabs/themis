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

/**
 * @file soter/error.h
 * @brief Soter return type, return codes and check macros
 *
 * 
 */
#ifndef SOTER_ERROR_H
#define SOTER_ERROR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/** @brief return type */
typedef int soter_status_t;

/**
 * @addtogroup SOTER
 * @{
 * @defgroup SOTER_ERROR_CODES status codes
 * @{
 */

#define SOTER_SUCCESS 0
#define SOTER_FAIL   -1
#define SOTER_INVALID_PARAMETER -2
#define SOTER_NO_MEMORY -3
#define SOTER_BUFFER_TOO_SMALL -4
#define SOTER_DATA_CORRUPT -5
#define SOTER_INVALID_SIGNATURE -6
#define SOTER_NOT_SUPPORTED -7

/** @} */

/**
 * @defgroup SOTER_ERROR_OUT routines for error and debug output
 * @{
 */

#ifdef DEBUG
#define SOTER_ERROR_OUT(message) fprintf(stderr, "%s:%u - error: %s\n",__FILE__,__LINE__,message) 
#define SOTER_DEBUG_OUT(message) fprintf(stdout, "%s:%u - debug: %s\n",__FILE__,__LINE__,message)
#else
#define SOTER_ERROR_OUT(message)  
#define SOTER_DEBUG_OUT(message) 
#endif


/**@}*/

/**
 * @defgroup SOTER_CHECK_ROUTINES routines for parameters and variables checking
 * @{
 */
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

/** @} 
 * @}
 */
#endif /* SOTER_ERROR_H */
