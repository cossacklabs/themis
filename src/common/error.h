/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef HERMES_ERROR_H
#define HERMES_ERROR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define HERMES_SUCCESS 0
#define HERMES_FAIL   -1
#define HERMES_INVALID_PARAMETER -2
#define HERMES_NO_MEMORY -3
#define HERMES_BUFFER_TOO_SMALL -4
#define HERMES_DATA_CORRUPT -5
#define HERMES_INVALID_SIGNATURE -6
#define HERMES_NOT_SUPPORTED -7
#define HERMES_SSESSION_KA_NOT_FINISHED -8
#define HERMES_SSESSION_TRANSPORT_ERROR -9

#define HERMES_ERROR_OUT(message) fprintf(stderr, "%s:%u - error: %s\n",__FILE__,__LINE__,message) 
#define HERMES_DEBUG_OUT(message) fprintf(stdout, "%s:%u - debug: %s\n",__FILE__,__LINE__,message) 

#define HERMES_CHECK(x) if(!(x)){	\
	HERMES_ERROR_OUT(#x);		\
	return HERMES_FAIL;		\
    }

#define HERMES_CHECK_(x) if(!(x)){	\
	HERMES_ERROR_OUT(#x);		\
	return NULL;		\
    }

#define HERMES_CHECK_PARAM(x) if(!(x)){	\
	HERMES_ERROR_OUT(#x);		\
	return HERMES_INVALID_PARAMETER;\
    }

#define HERMES_CHECK_PARAM_(x) if(!(x)){	\
	HERMES_ERROR_OUT(#x);		\
	return NULL;\
    }

#define HERMES_CHECK_MALLOC(x,y)	\
	y=malloc(sizeof(x));		\
	if(!(x)){			\
	HERMES_ERROR_OUT(#x);		\
	return HERMES_NO_MEMORY;	\
    }

#define HERMES_CHECK_MALLOC_(x)if(!(x)){	\
	HERMES_ERROR_OUT(#x);		\
	return NULL;\
    }

#define HERMES_CHECK_MALLOC_(x)if(!(x)){	\
	HERMES_ERROR_OUT(#x);		\
	return NULL;\
    }

#define HERMES_CHECK_FREE(x,y)if(!(x)){	\
	HERMES_ERROR_OUT(#x);		\
	free(y);			\
	return HERMES_FAIL;		\
    }

#define HERMES_IF_FAIL(x,y)if(!(x)){	\
	HERMES_ERROR_OUT(#x);		\
	{y;}			\
	return HERMES_FAIL;		\
    }

#define HERMES_IF_FAIL_(x,y)if(!(x)){	\
	HERMES_ERROR_OUT(#x);		\
	{y;}			\
	return NULL;		\
    }

#ifdef DEBUG
    static void hermes_out_buffer(const uint8_t* buffer, const size_t buffer_length){
    int i;
    for(i=0;i<buffer_length;++i){    
	fprintf(stderr, "%x ",buffer[i]);
    }
    fprintf(stderr, "\n");
}
#endif

#endif /* HERMES_ERROR_H */
