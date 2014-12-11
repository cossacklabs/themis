/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef HERMES_ERROR_H
#define HERMES_ERROR_H

#define HERMES_SUCCESS 0
#define HERMES_FAIL   -1
#define HERMES_INVALID_PARAMETER -2
#define HERMES_NO_MEMORY -3
#define HERMES_BUFFER_TOO_SMALL -4
#define HERMES_DATA_CORRUPT -5
#define HERMES_INVALID_SIGNATURE -6

#define HERMES_ERROR_OUT(message) fprintf(stderr, "%s:%u - error: %s",__FILE__,__LINE__,message) 
#define HERMES_DEBUG_OUT(message) fprintf(stdout, "%s:%u - debug: %s",__FILE__,__LINE__,message) 

#endif /* HERMES_ERROR_H */
