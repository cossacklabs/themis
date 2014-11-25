/*
 * Problem File.
 * Command Line used to run:
 *
 *	/extra/msmoot/lclint-2.5m/bin/lclint -dump newlint.lcd dummyfile.c
 */


#if 1
// This is our normal definition of PRINTF_LIKE - our lclint run normally
// does not use this definition because I think I had a problem in a file
// if I used this definition (I have to retest with 2.5 though).  this
// example does not seem to have a problem
#define PRINTF_LIKE __attribute__ ((format(printf,1,2)))
#else
#define PRINTF_LIKE
#endif

#if 1 // These fail to -dump
int /*@alt void@*/ console_printf(const char *__format, ...)
PRINTF_LIKE;
int /*@alt void@*/ eprintf(const char *__format, ...) PRINTF_LIKE;
int /*@alt void@*/ lprintf(const char *__format, ...) PRINTF_LIKE;
#endif

#if 0 // Even these fail
int /*@alt void@*/ console_printf(const char *__format, ...);
int /*@alt void@*/ eprintf(const char *__format, ...);
int /*@alt void@*/ lprintf(const char *__format, ...);
#endif


/* But all of these work */
int /* @alt void@ */ console_printf(const char *__format, ...);
int /* @alt void@ */ eprintf(const char *__format, ...);
int /* @alt void@ */ lprintf(const char *__format, ...);

int console_printf(const char *__format, ...) PRINTF_LIKE;
int eprintf(const char *__format, ...) PRINTF_LIKE;
int lprintf(const char *__format, ...) PRINTF_LIKE;

int console_printf(const char *__format, ...);
int eprintf(const char *__format, ...);
int lprintf(const char *__format, ...);




