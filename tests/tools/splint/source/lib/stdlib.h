/*@constant int EXIT_FAILURE; @*/ 
/*@constant int EXIT_SUCCESS; @*/ 
/*@constant null anytype NULL = 0;@*/
/*@constant int RAND_MAX; @*/
/*@constant size_t MB_CUR_MAX; @*/

//     div_t Structure type returned by div() function. 
//   ldiv_t
//       Structure type returned by ldiv() function.

     
     long a64l(const char *s)     ;

/*@exits@*/ void abort (void) /*@*/ ;
extern int abs (int n) /*@*/ ;
int atexit(void (*func)(void));

extern double atof (char *s) /*@*/ ;
extern int atoi (char *s) /*@*/ ;
extern long int atol (char *s) /*@*/ ;

void * bsearch (const void *key, const void *base, size_t nel, size_t size, 
int (*compar)(const void *, const void *)) /*@*/
     /*@requires maxSet(base) >= (nel - 1) @*/
     ;
     extern /*@null@*/ /*@only@*/ void *calloc (size_t nobj, size_t size) /*@*/
  /*@ensures MaxSet(result) == (nobj - 1); @*/ ;

  /*
    This is defined in ansi.h
    We include it here for reference

    drl 1/4/2002
  */

  /*@-redef@*/
typedef /*@concrete@*/ struct 
{
  int quot;
  int rem;
} div_t ;
/*@=redef@*/



extern div_t div (int num, int denom) /*@*/ ;

  double drand48 (void) /*@modifies internalState@*/ ; 


char *ecvt(double value, int ndigit, /*@out@*/ int *decpt, /*@out@*/ int *sign)
     ;

     char *fcvt(double value, int ndigit, /*@out@*/ int *decpt,  /*@out@*/int *sign);

     char *gcvt(double value, int ndigit, char *buf)
     /*@requires maxSet(buf) >= ndigit @*/
     ;

     extern /*@observer@*/ /*@null@*/ char *getenv (const char *name) /*@*/ ;

     extern  int getsubopt(char **optionp, char * const *tokens, /*@out@*/ char **valuep)
     /*@modifies optionp, valuep @*/ ;

     extern  int grantpt(int fildes)
     /*@modifies fileSystem, errno @*/
     ;
     char *initstate(unsigned int seed, char *state, size_t size)
     /*@modifies internalState, state @*/ /*@requires maxSet(state) >= (size - 1) @*/ /*drl added 09-20-001*/
     ;

     /*drl 1/4/2002: specifying the array sizes is meaningless but we include
       them to be consistent with the unix specification at opengroup.org */
     /*@-fixedformalarray@*/
     long int jrand48 (unsigned short int xsubi[3]) /*@modifies internalState@*/ /*@requires maxSet(xsubi) >= 2 @*/ ; 
     
     char *l64a(long value) /*@ensures maxRead(result) <= 5 /\ maxSet(result) <= 5 @*/ ;
     extern long int labs (long int n) /*@*/ ; 

          /*@-fixedformalarray@*/
extern     void lcong48 (unsigned short int param[7]) /*@modifies internalState@*/ /*@requires maxRead(param) >= 6 @*/ ; 

    /*@=fixedformalarray@*/

  /*
    This is already defined in ansi.h
    We include it here for reference but
    comment it out to avoid a warning
    drl 1/4/2002
  */

/*@-redef@*/
typedef /*@concrete@*/ struct 
{
  long int quot;
  long int rem;
} ldiv_t ;
/*@=redef@*/

extern ldiv_t ldiv (long num, long denom) /*@*/ ;

long int lrand48 (void) /*@modifies internalState@*/ ; 


extern /*@null@*/ /*@out@*/ /*@only@*/ void *malloc (size_t size) /*@modifies errno@*/
     /*drl 09-20-001 added errno*/
     /*@ensures MaxSet(result) == (size - 1); @*/ ;

     extern int mblen (char *s, size_t n)
     /*@modifies errno@*/
     /*@requires maxRead(s) >= (n - 1) @*/
     /*drl 09-20-001 added errno*/ ;

     size_t mbstowcs(/*@null@*/ /*@out@*/ wchar_t *pwcs, const char *s, size_t n)
  /*@requires maxSet(pwcs) >= (n - 1) @*/
     /*drl 09-20-001 added errno*/ ;

extern int mbtowc (/*@null@*/ /*@out@*/ wchar_t *pwc, /*@null@*/ char *s, size_t n) 
     /*@modifies *pwc, errno@*/   /*@requires maxRead(s) >= (n - 1) @*/ /*drl 09-20-001 added errno*/;


     extern char *mktemp(char *template) /*@modifies template @*/
     /*drl added 09-20-001*/ /*warn use  mkstemp */
     ;

     int mkstemp(char *template)
     /*@modifies template, fileSystem @*/ 
     
     /*drl added 09-20-001*/
     ;

     long int mrand48 (void) /*@modifies internalState@*/ ;


          /*drl 1/4/2002: specifying the array size is meaningless but we include
       it to be consistent with the unix specification at opengroup.org */
     /*@-fixedformalarray@*/
 long int nrand48 (unsigned short int xsubi[3]) /*@modifies internalState, xsubi @*/

     /*@requires maxSet(xsubi) >= 2 /\ maxRead(xsubi) >= 2 @*/
     ;
     /*@=fixedformalarray@*/
     
     extern /*@dependent@*/ /*check dependent */ char *ptsname(int fildes) /*drl added 09-20-01*/ ;

	extern int
	putenv (/*@kept@*/ const char *string)
	/*@globals environ@*/
	/*@modifies *environ, errno@*/
     /*drl 09-20-01 added kept */
     ;

     extern void qsort (void *base, size_t nel, size_t size,
			int (*compar)(const void *, const void *) )
     /*@requires maxRead(base) >= (nel - 1) @*/
  /*@modifies *base, errno@*/ ;

     

/*@constant int RAND_MAX; @*/
extern int rand (void) /*@modifies internalState@*/ ;

extern int rand_r(unsigned int *seed) /*@modifies seed@*/   /*drl 09-20-01 added*/
     ;

    
     long random(void)  /*@modifies internalState@*/ ;

 extern /*@null@*/ /*@only@*/ void *
   realloc (/*@null@*/ /*@only@*/ /*@out@*/ /*@returned@*/ void *p, size_t size)      /*@modifies *p, errno @*/ /*@ensures MaxSet(result) == (size - 1) @*/;

extern char *realpath(const char *file_name, /*@out@*/ char *resolved_name)
     //     *@requires maxSet(resolved_name) >=  (PATH_MAX - 1) @*/
     ;

     /*drl 1/4/2002: specifying the array sizes is meaningless but we include
       them to be consistent with the unix specification at opengroup.org */
     /*@-fixedformalarray@*/
unsigned short int *seed48 (unsigned short int seed16v[3]) /*@modifies internalState@*/
     /*@requires maxRead(seed16v) >= 2 @*/
     ; 
     /*@=fixedformalarray@*/

     void setkey(const char *key) /*@requires maxRead(key) >= 63 @*/
     /*@modifies internalState, errno@*/ 
     ;

     /*@only@*/ char *setstate(/*@kept@*/ const char *state)  /*@modifies internalState, errno@*/ ;
     
     extern void srand (unsigned int seed) /*@modifies internalState@*/ ;
     
extern void srand48 (long int seedval) /*@modifies internalState@*/ ;

extern void srandom(unsigned int seed) /*@modifies internalState@*/ /*drl added 09-20-001 */
     ;
extern double strtod (const char *s, /*@null@*/ /*@out@*/ char **endp)
  /*@modifies *endp, errno@*/ ;

extern long strtol (char *s, /*@null@*/ /*@out@*/ char **endp, int base)
  /*@modifies *endp, errno@*/ ;

  extern unsigned long 
  strtoul (char *s, /*@null@*/ /*@out@*/ char **endp, int base)
  /*@modifies *endp, errno@*/ ;

  
  extern int system (/*@null@*/ const char *s) /*@modifies fileSystem, errno@*/
  /*drl 09-20-01 added errno */
     ;

     extern int ttyslot(void) /*@*/
     /*drl added 09-20-001 */ /*legacy*/  ;
  

     extern int unlockpt(int fildes)
     /*@modifies fileSystem, internalState @*/
  /*drl added 09-20-001 */ 
     ;
     
extern void *valloc(size_t size)/*@modifies errno@*/
     /*drl 09-20-001 */
     /*@ensures MaxSet(result) == (size - 1); @*/ 
    /*legacy*/   ;

extern size_t wcstombs (/*@out@*/ char *s, wchar_t *pwcs, size_t n)
     /*@modifies *s, errno@*/ /*@requires maxSet(s) >= (n - 1) @*/ ;

extern int wctomb (/*@out@*/ /*@null@*/ char *s, wchar_t wchar) 
   /*@modifies *s@*/ ;

