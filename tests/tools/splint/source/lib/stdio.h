/*
** stdio.h - Unix Specification
*/

/*
** evans 2001-12-30: added from http://www.opengroup.org/onlinepubs/007908799/xsh/stdio.h.html
*/

/*@constant unsignedintegraltype BUFSIZ@*/ 
/*@constant unsignedintegraltype FILENAME_MAX@*/
/*@constant unsignedintegraltype FOPEN_MAX@*/
/*@constant _Bool _IOFBF@*/
/*@constant _Bool _IOLBF@*/
/*@constant _Bool _IONBF@*/
/*@constant unsignedintegraltype L_ctermid@*/
/*@constant unsignedintegraltype L_cuserid@*/
/*@constant unsignedintegraltype L_tmpnam@*/
/*@constant unsignedintegraltype SEEK_CUR@*/
/*@constant unsignedintegraltype SEEK_END@*/
/*@constant unsignedintegraltype SEEK_SET@*/
/*@constant unsignedintegraltype TMP_MAX@*/

/*@constant observer char *P_tmpdir@*/

void clearerr (FILE *stream) /*@modifies *stream@*/ ;

/*@dependent@*/ char *ctermid (/*@returned@*/ /*@null@*/ char *) /*@*/ ;
   /* Result may be static pointer if parameter is NULL, otherwise is fresh. */

  //     *@requires maxSet(s) >= ( L_ctermid - 1) @*/ *ensures maxRead(s) <= ( L_ctermid - 1) /\ maxRead(s) >= 0 */
  /*DRL 9-11-2001 I've modified the definition in ansi.h to remove modifies SystemState and I've added a requires and ensures*/ ;
  
/*check returns*/     
/* cuserid is in the 1988 version of POSIX but removed in 1990 */

char *cuserid (/*@null@*/ /*@out@*/ /*@returned@*/ char *s)
  /*@warn legacy "cuserid is obsolete"@*/ 
  /*@modifies *s@*/
     // *@requires maxSet(s) >= ( L_ctermid - 1) @*/ *@ensures maxRead(s) <= ( L_ctermid - 1) /\ maxRead(s) >= 0  /\ maxRead(result) <= ( L_ctermid - 1) /\ maxRead(result) >= 0 @*/
     ;

/* in standard.h: int fclose (FILE *stream) @modifies *stream, errno, fileSystem;@ */
  
/*@null@*/ /*@dependent@*/ FILE *fdopen (int fd, const char *type)
    /*@modifies errno, fileSystem@*/;

/* feof, ferror fflush, fgetc, fgetpos, fgets - in standard.h */

int fileno (/*@notnull@*/ FILE *)
  /*:errorcode -1:*/ 
  /*@modifies errno@*/ ;

void flockfile (/*@notnull@*/ FILE *f)
   /*@modifies f, fileSystem@*/ ;

/* fopen, fprintf, fputc, fread, frepoen, fscanf, etc. in standard.h */


int fseeko (FILE *stream, off_t offset, int whence)
   /*:errorcode -1:*/
   /*@modifies stream, errno@*/ ;

off_t ftello(FILE *stream)
   /*:errorcode -1:*/ /*@modifies errno*/ ;

int ftrylockfile(FILE *stream)
   /*:errorcode !0:*/
   /*@modifies stream, fileSystem, errno*/ ;

void funlockfile (FILE *stream)
   /*@modifies stream, fileSystem*/ ;

int getc_unlocked(FILE *stream)
   /*@warn multithreaded "getc_unlocked is a thread unsafe version of getc"@*/
   /*@modifies *stream, fileSystem, errno@*/ ;

int getchar_unlocked (void)
   /*@warn multithreaded "getchar_unlocked is a thread unsafe version of getchar"@*/
   /*@globals stdin@*/
   /*@modifies *stdin, fileSystem@*/ ;

/*@unchecked@*/ char *optarg;
/*@unchecked@*/ int optind;
/*@unchecked@*/ int optopt;
/*@unchecked@*/ int opterr;
/*@unchecked@*/ int optreset;

int getopt (int argc, char * const *argv, const char *optstring)
   /*@warn legacy@*/ 
   /*@globals optarg, optind, optopt, opterr, optreset@*/
   /*@modifies optarg, optind, optopt@*/
   /*@requires maxRead(argv) >= (argc - 1) @*/
   ;

int getw (FILE *stream)
   /*@warn legacy@*/ 
   /*:errorcode EOF:*/
   /*@modifies fileSystem, *stream, errno@*/ ;

int pclose(FILE *stream)
   /*:errorcode -1:*/
   /*@modifies fileSystem, *stream, errno@*/ ;

/*@dependent@*/ /*@null@*/ FILE *popen (const char *command, const char *mode)
   /*:errorcode NULL:*/
   /*@modifies fileSystem, errno@*/ ;

int putc_unlocked (int, FILE *stream)
   /*@warn multithreaded "putc_unlocked is a thread unsafe version of putc"@*/
   /*:errorcode EOF:*/
   /*@modifies fileSystem, *stream, errno@*/ ;

int putchar_unlocked(int)
   /*@warn multithreaded "putchar_unlocked is a thread unsafe version of putchar"@*/
   /*:errorcode EOF:*/
   /*@modifies fileSystem, *stdout, errno@*/ ;

int putw(int, FILE *stream)
   /*@warn legacy@*/ 
   /*:errorcode EOF:*/
   /*@modifies fileSystem, *stdout, errno@*/ ;

int remove (char *filename) /*@modifies fileSystem, errno@*/ ;
int rename (char *old, char *new) /*@modifies fileSystem, errno@*/ ;
void rewind (FILE *stream) /*@modifies *stream, fileSystem, errno@*/ ;
  
/* evans 2002-07-09: snprintf moved to standard.h (its in ISO C99 now) */

/*@null@*/ char *tempnam (char *dir, /*@null@*/ char *pfx) 
    /*@modifies internalState, errno@*/
    /*@ensures maxSet(result) >= 0 /\ maxRead(result) >= 0 @*/
    /*@warn toctou "Between the time a pathname is created and the file is opened, it is possible for some other process to create a file with the same name. Use tmpfile instead."*/
    /*drl added errno 09-19-001 */
    ;


/*@null@*/ FILE *tmpfile (void) 
   /*@modifies fileSystem, errno@*/
   /*drl added errno 09-19-001 */
   ;

/*@observer@*/ char *tmpnam (/*@out@*/ /*@null@*/ /*@returned@*/ char *s) 
   /*@modifies *s, internalState @*/
   //      *@requires maxSet(s) >= (L_tmpnam - 1) @*
   /*@warn toctou "Between the time a pathname is created and the file is opened, another process may create a file with the same name. Use tmpfile instead."*/
   ;
     

