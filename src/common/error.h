/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef _ERROR_HPP_
#define _ERROR_HPP_

#define HERMES_SUCCESS 0
#define HERMES_FAIL   -1
#define HERMES_INVALID_PARAMETER -2
#define HERMES_NO_MEMORY -3

#define HERMES_ERROR_OUT(message) fprintf(stderr, "%s:%u - error: %s",__FILE__,__LINE__,message) 
#define HERMES_DEBUG_OUT(message) fprintf(stdout, "%s:%u - debug: %s",__FILE__,__LINE__,message) 

#endif /* _ERROR_HPP_ */
