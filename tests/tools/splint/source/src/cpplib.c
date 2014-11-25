/*
** Splint - annotation-assisted static program checker
** Copyright (C) 1994-2003 University of Virginia,
**         Massachusetts Institute of Technology
**
** This program is free software; you can redistribute it and/or modify it
** under the terms of the GNU General Public License as published by the
** Free Software Foundation; either version 2 of the License, or (at your
** option) any later version.
** 
** This program is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** General Public License for more details.
** 
** The GNU General Public License is available from http://www.gnu.org/ or
** the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
** MA 02111-1307, USA.
**
** For information on splint: info@splint.org
** To report a bug: splint-bug@splint.org
** For more information: http://www.splint.org
*/
/*
** cpplib.c
*/
/*
   Copyright (C) 1986, 87, 89, 92-6, 1997 Free Software Foundation, Inc.
   Contributed by Per Bothner, 1994-95.
   Based on CCCP program by Paul Rubin, June 1986
   Adapted to ANSI C, Richard Stallman, Jan 1987

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

 In other words, you are welcome to use, share and improve this program.
 You are forbidden to forbid anyone else to use, share and improve
 what you give them.   Help stamp out software-hoarding!  */

/*
 * Herbert 06/12/2000:
 * - OS2 drive specs like WIN32
 * - Includes for IBMs OS/2 compiler
 */

# include <ctype.h>
# include <stdio.h>
# include <signal.h>
# ifdef __STDC__
# include <stdlib.h>
# endif

# include <string.h>
# if !(defined (WIN32) || defined (OS2) && defined (__IBMC__))
# include <unistd.h>
# endif
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# if defined (WIN32) || defined (OS2) && defined (__IBMC__)
# include <io.h>
/* SMF */
# ifndef BCC32
# include <sys/utime.h>		/* for __DATE__ and __TIME__ */
# endif

# include <time.h>
# else
# ifndef VMS
/*
** evans 2002-07-03: exception for WATCOM 10.6 compiler suggest by Adam Clarke 
*/
# if !defined (USG) && !defined (__WATCOMC__)
# include <time.h> /* Reported by Paul Smith */
# include <sys/time.h>
# include <sys/resource.h>
# else
# include <sys/times.h>
# include <time.h>
# include <fcntl.h>
# endif /* USG */
# endif /* not VMS */
# endif /* not WIN32 */

/* This defines "errno" properly for VMS, and gives us EACCES.  */
# include <errno.h>

# include "splintMacros.nf"
# include "basic.h"
# include "lcllib.h"
# include "cpplib.h"
# include "cpperror.h"
# include "cpphash.h"
# include "cppexp.h"
# include "version.h"
# include "osd.h"

/*
** This is really kludgey code...
*/

/*@+boolint@*/
/*@+charint@*/

/* Warnings for using sprintf - suppress them all for now... */
/*@-bufferoverflowhigh@*/
/*@-bounds@*/

#define NO_SHORTNAMES

# ifdef open
# undef open
# undef read
# undef write
# endif /* open */

/*@constant int IMPORT_FOUND@*/
# define IMPORT_FOUND -2

/*@constant int SKIP_INCLUDE@*/
# define SKIP_INCLUDE IMPORT_FOUND

/*@constant unused int IMPORT_NOT_FOUND@*/
# define IMPORT_NOT_FOUND -1

#ifndef STDC_VALUE
/*@constant unused int STDC_VALUE@*/
#define STDC_VALUE 1
#endif

/* By default, colon separates directories in a path.  */
#ifndef PATH_SEPARATOR
/*@constant char PATH_SEPARATOR@*/
#define PATH_SEPARATOR ':'
#endif

static void parse_name (cppReader *, int);

static int cpp_openIncludeFile (char *p_filename)
     /*@modifies fileSystem @*/ ;

static void cpp_setLocation (cppReader *p_pfile)
     /*@modifies g_currentloc@*/ ;

static enum cpp_token cpp_handleComment (cppReader *p_pfile,
					 struct parse_marker *p_smark)
   /*@modifies p_pfile, p_smark@*/;
  
static bool cpp_shouldCheckMacro (cppReader *p_pfile, char *p_p) /*@modifies p_p@*/ ;

static size_t cppReader_checkMacroNameLoc (fileloc p_loc, char *p_symname, cstring p_usage) ;

static bool cpp_skipIncludeFile (cstring p_fname) /*@*/ ;

#ifndef O_RDONLY
#define O_RDONLY 0
#endif

/* Symbols to predefine.  */
  
#ifdef CPP_PREDEFINES
static /*@observer@*/ char *predefs = CPP_PREDEFINES;
#else
static /*@observer@*/ char *predefs = "";
#endif

/* We let tm.h override the types used here, to handle trivial differences
   such as the choice of unsigned int or long unsigned int for size_t.
   When machines start needing nontrivial differences in the size type,
   it would be best to do something here to figure out automatically
   from other information what type to use.  */

/* The string value for __SIZE_TYPE__.  */

#ifndef SIZE_TYPE
/*@constant observer char *SIZE_TYPE@*/
#define SIZE_TYPE "long unsigned int"
#endif

/* The string value for __PTRDIFF_TYPE__.  */

#ifndef PTRDIFF_TYPE
/*@constant observer char *PTRDIFF_TYPE@*/
#define PTRDIFF_TYPE "long int"
#endif

/* The string value for __WCHAR_TYPE__.  */

#ifndef WCHAR_TYPE
/*@constant observer char *WCHAR_TYPE@*/
#define WCHAR_TYPE "int"
#endif
  
/* The string value for __USER_LABEL_PREFIX__ */

#ifndef USER_LABEL_PREFIX
/*@constant observer char *USER_LABEL_PREFIX@*/
#define USER_LABEL_PREFIX ""
#endif

/* The string value for __REGISTER_PREFIX__ */

#ifndef REGISTER_PREFIX
/*@constant observer char *REGISTER_PREFIX@*/
#define REGISTER_PREFIX ""
#endif

/* table to tell if char can be part of a C identifier.  */
static bool is_idchar[256];
/* table to tell if char can be first char of a c identifier.  */
static bool is_idstart[256];
/* table to tell if c is horizontal space.  */
static bool is_hor_space[256];
/* table to tell if c is horizontal or vertical space.  */
static bool is_space[256];
  
static /*@exposed@*/ /*@null@*/ cppBuffer *
cppReader_getBuffer (/*@special@*/ cppReader *p_pfile)
     /*@uses p_pfile->buffer@*/
     /*@modifies nothing@*/ ;

/*@notfunction@*/
# define SKIP_WHITE_SPACE(p) do { /*@access cstring@*/ while (is_hor_space[(int) *(p)]) { (p)++; } } /*@noaccess cstring@*/ while (0)

/*@notfunction@*/
# define SKIP_ALL_WHITE_SPACE(p) do { while (is_space[*(p)]) { (p)++; } } while (0)

static int cpp_peekN (cppReader *p_pfile, int p_n) /*@*/ ;

/*@function static int cppBuffer_get (sef cppBuffer *p_b) modifies *p_b ; @*/
# define cppBuffer_get(BUFFER) \
  ((BUFFER)->cur < (BUFFER)->rlimit ? *(BUFFER)->cur++ : EOF)

/*@function static int cppBuffer_reachedEOF (sef cppBuffer *p_b) modifies nothing; @*/
# define cppBuffer_reachedEOF(b) \
  ((b)->cur < (b)->rlimit ? FALSE : TRUE)

/* Append string STR (of length N) to PFILE's output buffer.  Make space. */
/*@function static void cppReader_puts (sef cppReader *p_file, char *p_str, sef size_t p_n)
                     modifies *p_file; @*/
# define cppReader_puts(PFILE, STR, N) \
  cpplib_reserve(PFILE, N), cppReader_putStrN (PFILE, STR,N)
  
/* Append character CH to PFILE's output buffer.  Assume sufficient space. */

/*@function static void cppReader_putCharQ (cppReader *p_file, char p_ch)
                    modifies *p_file; @*/
# define cppReader_putCharQ(PFILE, CH) (*(PFILE)->limit++ = (CH))
/*
static void cppReader_putCharQ (cppReader *p_file, char p_ch)
{
  fprintf (stderr, "put char: %c\n", p_ch);
  (*(p_file)->limit++ = (p_ch));
}
*/
/* Append character CH to PFILE's output buffer.  Make space if need be. */

/*@function static void cppReader_putChar (sef cppReader *p_file, char p_ch)
                    modifies *p_file; @*/
#define cppReader_putChar(PFILE, CH) (cpplib_reserve (PFILE, (size_t) 1), cppReader_putCharQ (PFILE, CH))

/* Make sure PFILE->limit is followed by '\0'. */
/*@function static void cppReader_nullTerminateQ (cppReader *p_file)
      modifies *p_file; @*/

#define cppReader_nullTerminateQ(PFILE) (*(PFILE)->limit = 0)

/*@function static void cppReader_nullTerminate (sef cppReader *p_file)
                           modifies *p_file; @*/
# define cppReader_nullTerminate(PFILE) \
  (cpplib_reserve (PFILE, (size_t) 1), *(PFILE)->limit = 0)

/*@function static void cppReader_adjustWritten (cppReader *p_file, size_t)
                           modifies *p_file; @*/
#define cppReader_adjustWritten(PFILE,DELTA) ((PFILE)->limit += (DELTA))

/*@function static bool cppReader_isC89 (cppReader *) modifies nothing; @*/
#define cppReader_isC89(PFILE) (CPPOPTIONS(PFILE)->c89)

/*@function static observer char *cppReader_wcharType (cppReader *)
     modifies nothing; @*/

# define cppReader_wcharType(PFILE) \
  (CPPOPTIONS (PFILE)->cplusplus ? "__wchar_t" : WCHAR_TYPE)

static void cppBuffer_forward (cppBuffer *p_buf, int p_n) /*@modifies *p_buf@*/ ;

/*@function static void cppReader_forward (cppReader *p_pfile, int) modifies *p_pfile; @*/
# define cppReader_forward(pfile, N) \
  (cppBuffer_forward (cppReader_getBufferSafe (pfile), (N)))

/*@function static int cppReader_getC (cppReader *p_pfile) modifies *p_pfile; @*/
# define cppReader_getC(pfile)   (cppBuffer_get (cppReader_getBufferSafe (pfile)))

/*@function static int cppReader_reachedEOF (sef cppReader *p_pfile) modifies *p_pfile; @*/
# define cppReader_reachedEOF(pfile)   (cppBuffer_reachedEOF (cppReader_getBufferSafe (pfile)))

/*@function static int cppReader_peekC (cppReader *) modifies nothing;@*/
# define cppReader_peekC(pfile)  (cpplib_bufPeek (cppReader_getBufferSafe (pfile)))

/* Move all backslash-newline pairs out of embarrassing places.
   Exchange all such pairs following BP
   with any potentially-embarrassing characters that follow them.
   Potentially-embarrassing characters are / and *
   (because a backslash-newline inside a comment delimiter
   would cause it not to be recognized).  */

/*@notfunction@*/
# define NEWLINE_FIX \
   do { while (cppReader_peekC (pfile) == '\\' && cpp_peekN (pfile, 1) == '\n') { cppReader_forward (pfile, 2); } } while(FALSE)

     /* Same, but assume we've already read the potential '\\' into C.  */
/*@notfunction@*/
# define NEWLINE_FIX1(C) do { \
     while ((C) == '\\' && cppReader_peekC (pfile) == '\n') { cppReader_forward (pfile, 1); (C) = cppReader_getC (pfile); }\
									   } while(0)

static void parseSetMark (/*@out@*/ struct parse_marker *,
			  cppReader *);
static void parseClearMark (struct parse_marker *);
static void parseGotoMark (struct parse_marker *, cppReader *);
static void parseMoveMark (struct parse_marker *, cppReader *);

/* If we have a huge buffer, may need to cache more recent counts */
static /*@exposed@*/ char *cppLineBase (/*@sef@*/ cppBuffer *);

static /*@exposed@*/ /*@null@*/ cppBuffer *
   cppReader_pushBuffer (cppReader *p_pfile,
			 /*@owned@*/ /*@null@*/ char *, size_t)
     /*@modifies p_pfile@*/ ;

static void cppReader_appendIncludeChain
(cppReader *p_pfile,
 /*@keep@*/ struct file_name_list *p_first,
 /*@dependent@*/ struct file_name_list *p_last);

static void cppReader_macroCleanup (cppBuffer *p_pbuf, cppReader *p_pfile);
static enum cpp_token cppReader_nullUnderflow (/*@unused@*/ cppReader *p_pfile);

static void cppReader_nullCleanup (/*@unused@*/ cppBuffer *p_pbuf,
				   /*@unused@*/ cppReader *p_pfile);

static void cppReader_fileCleanup (cppBuffer *p_pbuf,
				   /*@unused@*/ cppReader *p_pfile);

static int cppReader_handleDirective (cppReader *p_pfile);

static void cppReader_scanBuffer (cppReader *p_pfile);

# if defined (WIN32) || defined (OS2) && defined (__IBMC__)

/* SMF */
# ifndef BCC32
/*
** WIN32 (at least the VC++ include files) does not define mode_t.
*/

/*@-incondefs@*/ /*@-czechtypes@*/
typedef unsigned int mode_t;
/*@=incondefs@*/ /*@=czechtypes@*/
# endif

# endif

static int file_size_and_mode (int p_fd, /*@out@*/ mode_t *p_mode_pointer,
			       /*@out@*/ size_t *p_size_pointer);
static int safe_read (int p_desc, /*@out@*/ char *p_ptr, int p_len);


/*
** cppBuffer_isMacro is true if the buffer contains macro expansion.
** (Note that it is false while we're expanding marco *arguments*.)
*/

static bool cppBuffer_isMacro (/*@null@*/ cppBuffer *) /*@*/ ;

static void path_include (cppReader *p_pfile, char *p_path)
     /*@modifies p_pfile@*/ ;

static void initialize_builtins (cppReader *p_pfile)
     /*@modifies p_pfile@*/ ;

static void initialize_char_syntax (struct cppOptions *p_opts) ;

static int /*@alt void@*/ finclude (cppReader *p_pfile, int p_f,
				    cstring p_fname,
				    bool p_system_header_p,
				    /*@dependent@*/ /*@null@*/ struct file_name_list *p_dirptr);

static void validate_else (cppReader *p_pfile, cstring p_directive);
  
static void conditional_skip (cppReader *p_pfile, int p_skip,
			      enum node_type p_type,
			      /*@dependent@*/ /*@null@*/ char *p_control_macro);

static HOST_WIDE_INT eval_if_expression (cppReader *p_pfile,
					 char *p_buf,
					 int p_length);

static void skip_if_group (cppReader *p_pfile, int p_any);

static bool comp_def_part (bool p_first, char *p_beg1, int p_len1,
			   char *p_beg2, int p_len2, bool p_last);

#ifdef abort
extern void fancy_abort ();
#endif

static bool redundant_include_p (cppReader *p_pfile, /*@null@*/ cstring p_name);
static bool is_system_include (cppReader *p_pfile, cstring p_filename);

static /*@observer@*/ /*@null@*/ struct file_name_map *
read_name_map (cppReader *p_pfile, cstring p_dirname);

static cstring read_filename_string (int p_ch, /*:open:*/ FILE *p_f);

static int open_include_file (cppReader *p_pfile,
			      /*@owned@*/ cstring p_fname,
			      /*@null@*/ struct file_name_list *p_searchptr);

static void push_macro_expansion (cppReader *,
				  /*@owned@*/ char *, size_t,
				  /*@dependent@*/ hashNode);

/* Last arg to output_line_command.  */
enum file_change_code {
  same_file, enter_file, leave_file
};

/* `struct directive' defines one #-directive, including how to handle it.  */

struct directive {
  int length;			/* Length of name */
  /*@null@*/ int (*func)();	/* Function to handle directive */
  /*@observer@*/ cstring name;	/* Name of directive */
  enum node_type type;		/* Code which describes which directive.  */
  bool command_reads_line;      /* One if rest of line is read by func.  */
  bool traditional_comments;	/* Nonzero: keep comments if -traditional.  */
  bool pass_thru;		/* Copy preprocessed directive to output file.*/
};

/* These functions are declared to return int instead of void since they
   are going to be placed in a table and some old compilers have trouble with
   pointers to functions returning void.  */

static int do_define (cppReader *, /*@null@*/ struct directive *, 
		      /*@exposed@*/ char *, char *);
static int do_defineAux (cppReader *, /*@null@*/ struct directive *,
			 /*@exposed@*/ char *, char *, bool);
     
static int do_line (cppReader *, /*@null@*/ struct directive *);
static int do_include (cppReader *, struct directive *, char *, char *);
static int do_undef (cppReader *, struct directive *, char *, char *);
static int do_error (cppReader *, struct directive *, char *, char *);
static int do_pragma (cppReader *, struct directive *, char *, char *);
static int do_ident (cppReader *, struct directive *, char *, char *);
static int do_if (cppReader *, struct directive *, char *, char *);
static int do_xifdef (cppReader *, struct directive *, char *, char *);
static int do_else (cppReader *, struct directive *, char *, char *);
static int do_elif (cppReader *, struct directive *, char *, char *);
static int do_endif (cppReader *, struct directive *, char *, char *);
static int do_warning (cppReader *, struct directive *, char *, char *);

/* If a buffer's dir field is SELF_DIR_DUMMY, it means the file was found
   via the same directory as the file that #included it.  */

/*@constant observer struct file_name_list *SELF_DIR_DUMMY@*/
#define SELF_DIR_DUMMY ((struct file_name_list *) (~0))

/* #include "file" looks in source file dir, then stack.  */
/* #include <file> just looks in the stack.  */
/* -I directories are added to the end, then the defaults are added.  */

/*@access cstring@*/
  
static struct default_include {
  /*@dependent@*/ /*@observer@*/ cstring fname;	/* The name of the directory.  */
  int cplusplus;		/* Only look here if we're compiling C++.  */
  int cxx_aware;		/* Includes in this directory don't need to
				   be wrapped in extern "C" when compiling
				   C++.  */
} include_defaults_array[]
= {
  /* This is the dir for fixincludes.  Put it just before
     the files that we fix.  */
  { GCC_INCLUDE_DIR, 0, 0 },
  { GCC_INCLUDE_DIR2, 0, 0 },
  { cstring_undefined, 0, 0 }
};

/*@noaccess cstring@*/

/* Here is the actual list of #-directives, most-often-used first.
   The initialize_builtins function assumes #define is the very first.  */

/*@access cstring@*/

static struct directive directive_table[] = {
  {  6, do_define, "define", T_DEFINE, FALSE, TRUE, FALSE },
  {  5, do_xifdef, "ifdef", T_IFDEF, TRUE, FALSE, FALSE },
  {  6, do_xifdef, "ifndef", T_IFNDEF, TRUE, FALSE, FALSE },
  {  7, do_include, "include", T_INCLUDE, TRUE, FALSE, FALSE },
  {  5, do_endif, "endif", T_ENDIF, TRUE, FALSE, FALSE },
  {  4, do_else, "else", T_ELSE, TRUE, FALSE, FALSE },
  {  2, do_if, "if", T_IF, TRUE, FALSE, FALSE },
  {  4, do_elif, "elif", T_ELIF, TRUE, FALSE, FALSE },
  {  5, do_undef, "undef", T_UNDEF, FALSE, FALSE, FALSE },
  {  5, do_error, "error", T_ERROR, FALSE, FALSE, FALSE },
  {  7, do_warning, "warning", T_WARNING, FALSE, FALSE, FALSE },
  {  6, do_pragma, "pragma", T_PRAGMA, FALSE, FALSE, TRUE},
  {  4, do_line, "line", T_LINE, TRUE, FALSE, FALSE },
  {  5, do_ident, "ident", T_IDENT, TRUE, FALSE, TRUE },
  /* {  8, do_unassert, "unassert", T_UNASSERT, TRUE, FALSE, FALSE }, */
  {  -1, NULL, "", T_UNUSED, FALSE, FALSE, FALSE },
};
/*@noaccess cstring@*/

static cstring searchPath_unparse (struct file_name_list *search_start) 
{
  cstring res = cstring_newEmpty ();
  struct file_name_list *searchptr = NULL;

  for (searchptr = search_start; searchptr != NULL;
       searchptr = searchptr->next)
    {
      if (!cstring_isEmpty (searchptr->fname)) {
	res = cstring_concatFree1 (res, searchptr->fname);
	if (searchptr->next != NULL) {
	  res = cstring_appendChar (res, ';');
	}
      }
    }

  return res;
}

/*@+charint@*/
static void
initialize_char_syntax (struct cppOptions *opts)
{
  char i;

  /*
   * Set up is_idchar and is_idstart tables.  These should be
   * faster than saying (is_alpha (c) || c == '_'), etc.
   * Set up these things before calling any routines tthat
   * refer to them.
   */

  for (i = 'a'; i <= 'z'; i++) {
    is_idchar[i - 'a' + 'A'] = TRUE;
    is_idchar[(int) i] = TRUE;
    is_idstart[i - 'a' + 'A'] = TRUE;
    is_idstart[(int) i] = TRUE;
  }

  for (i = '0'; i <= '9'; i++)
    {
      is_idchar[(int) i] = TRUE;
    }

  is_idchar['_'] = TRUE;
  is_idstart['_'] = TRUE;
  is_idchar['$'] = opts->dollars_in_ident;
  is_idstart['$'] = opts->dollars_in_ident;

  /* horizontal space table */
  is_hor_space[' '] = TRUE;
  is_hor_space['\t'] = TRUE;
  is_hor_space['\v'] = TRUE;
  is_hor_space['\f'] = TRUE;
  is_hor_space['\r'] = TRUE;

  is_space[' '] = TRUE;
  is_space['\t'] = TRUE;
  is_space['\v'] = TRUE;
  is_space['\f'] = TRUE;
  is_space['\n'] = TRUE;
  is_space['\r'] = TRUE;
}

bool isIdentifierChar (char c)
{
  return is_idchar[(int) c];
}

/* Place into P_PFILE a quoted string representing the string SRC.
   Caller must reserve enough space in pfile->token_buffer.  */

static void
quote_string (cppReader *pfile, char *src)
{
  char c;
  
  cppReader_putCharQ (pfile, '\"');
  for (;;)
    {
      switch ((c = *src++))
	{
	default:
	  if (isprint (c))
	    cppReader_putCharQ (pfile, c);
	  else
	    {
	      sprintf (cpplib_getPWritten (pfile), "\\%03o",
		       (unsigned int) c);
	      cppReader_adjustWritten (pfile, (size_t) 4);
	    }
	  /*@switchbreak@*/ break;

	case '\"':
	case '\\':
	  cppReader_putCharQ (pfile, '\\');
	  cppReader_putCharQ (pfile, c);
	  /*@switchbreak@*/ break;

	case '\0':
	  cppReader_putCharQ (pfile, '\"');
	  cppReader_nullTerminateQ (pfile);
	  return;
	}
    }
}

/* Re-allocates PFILE->token_buffer so it will hold at least N more chars.  */

void
cppReader_growBuffer (cppReader *pfile, size_t n)
{
  size_t old_written = cpplib_getWritten (pfile);
  pfile->token_buffer_size = n + 2 * pfile->token_buffer_size;
  pfile->token_buffer = (char *)
    drealloc (pfile->token_buffer, pfile->token_buffer_size);
  cppReader_setWritten (pfile, old_written);
}

/*
 * process a given definition string, for initialization
 * If STR is just an identifier, define it with value 1.
 * If STR has anything after the identifier, then it should
 * be identifier=definition.
 */

void
cppReader_define (cppReader *pfile, char *str)
{
  char *buf = NULL;
  char *p = str;

  DPRINTF (("Cpp reader define: %s", str));

  if (!is_idstart[(int) *p])
    {
      DPRINTF (("ERROR 1"));
      cppReader_error (pfile,
		       message ("Malformed option `-D%s'",
				cstring_fromChars (str)));
      
      return;
    }

  p++;

  DPRINTF (("Here 2"));

  while (is_idchar[(int) *p])
    {
      p++;
    }

  if (*p == '(') {
    p++;
    while (*p != ')' && *p != '\0') {
      p++;
    }

    if (*p == ')') {
      p++;
    } else {
      cppReader_error 
	(pfile,
	 message ("Malformed option: -D%s (no closing parenthesis)", 
		  cstring_fromChars (str)));
    }
  }

  DPRINTF (("Here 2"));

  if (*p == '\0')
    {
      buf = (char *) dmalloc (size_fromInt (p - str + 4));
      strcpy ((char *) buf, str);
      strcat ((char *) buf, " 1");
    }
  else if (*p != '=')
    {
      DPRINTF (("ERROR 2"));
      cppReader_error (pfile,
		       message ("Malformed option: -D%s (expected '=', found '%c')",
				cstring_fromChars (str),
				*p));
      return;
    }
  else
    {
      char *q;
      /* Copy the entire option so we can modify it.  */
      DPRINTF (("Copying..."));
      buf = (char *) dmalloc (2 * strlen (str) + 1);
      strncpy (buf, str, size_fromInt (p - str));

      /* Change the = to a space.  */
      buf[p - str] = ' ';
      /* Scan for any backslash-newline and remove it.  */
      p++;
      q = &buf[p - str];

      while (*p != '\0')
	{
	  if (*p == '\\' && p[1] == '\n')
	    p += 2;
	  else
	    *q++ = *p++;
	}

      DPRINTF (("Here we are..."));
      *q = '\0';
    }

  llassert (buf != NULL);
  DPRINTF (("Do define: %s / %ld", buf, size_toLong (strlen (buf))));
  (void) do_define (pfile, NULL, buf, buf + strlen (buf));
  sfree (buf);
}
  
/* Append a chain of `struct file_name_list's
   to the end of the main include chain.
   FIRST is gthe beginning of the chain to append, and LAST is the end.  */

void
cppReader_appendIncludeChain (cppReader *pfile,
		      struct file_name_list *first,
		      struct file_name_list *last)
{
  struct cppOptions *opts = CPPOPTIONS (pfile);
  struct file_name_list *dir;

  if (first == NULL || last == NULL)
    {
      return;
    }

  if (opts->include == 0)
    {
      opts->include = first;
    }
  else
    {
      llassert (opts->last_include->next == NULL);
      opts->last_include->next = first;
    }

  if (opts->first_bracket_include == 0)
    {
      opts->first_bracket_include = first;

      for (dir = first; ; dir = dir->next) {
	size_t len = cstring_length (dir->fname) + INCLUDE_LEN_FUDGE;

	if (len > pfile->max_include_len)
	  {
	    pfile->max_include_len = len;
	  }

	if (dir == last)
	  {
	    break;
	  }
      }
    }

  llassert (last->next == NULL);
  /* last->next = NULL; */
  opts->last_include = last;
}
  
# if 0
static /*@unused@*/ void 
cppReader_showIncludeChain (cppReader *pfile)
{
  struct file_name_list *dirs = CPPOPTIONS (pfile)->include;

  if (dirs != NULL)
    {
      while (dirs != NULL)
	{
	  fprintf (stderr, "*%s*:", cstring_toCharsSafe (dirs->fname));
	  dirs = dirs->next;
	}

      fprintf (stderr, "\n");
    }
  else
    {
      fprintf (stderr, "No includes\n");
    }
}
# endif
  
cstring 
cppReader_getIncludePath ()
{
  cppReader *pfile = &g_cppState;
  struct file_name_list *dirs = CPPOPTIONS (pfile)->include;
  cstring res = cstring_undefined;

  if (dirs != NULL)
    {
      while (dirs != NULL)
	{
	  res = message ("%q%c%s", res, PATH_SEPARATOR, dirs->fname);
	  dirs = dirs->next;
	}
    }
  else
    {
      res = cstring_makeLiteral ("<no include path>");
    }

  return res;
}

void
cppReader_addIncludeChain (cppReader *pfile, /*@only@*/ struct file_name_list *dir)
{
  struct cppOptions *opts = CPPOPTIONS (pfile);

  if (dir == NULL)
    {
      return;
    }

  if (opts->include == 0)
    {
      opts->include = dir;
    }
  else
    {
      llassert (opts->last_include->next == NULL);
      opts->last_include->next = dir;
    }

  if (opts->first_bracket_include == 0)
    {
      size_t len = cstring_length (dir->fname) + INCLUDE_LEN_FUDGE;
      opts->first_bracket_include = dir;
      
      if (len > pfile->max_include_len)
	{
	  pfile->max_include_len = len;
	}
    }

  dir->next = NULL;
  opts->last_include = dir;
  /* cppReader_showIncludeChain (pfile); */
}

/* Given a colon-separated list of file names PATH,
   add all the names to the search path for include files.  */

static void
path_include (cppReader *pfile, char *path)
{
  char *p;
  
#ifdef __CYGWIN32__
  char *win32temp;

  /* if we have a posix path list, convert to win32 path list */
  win32temp = (char *) dmalloc /*@i4@*/
    (cygwin32_posix_to_win32_path_list_buf_size (path));
  cygwin32_posix_to_win32_path_list (path, win32temp);
  path = win32temp;
#endif

  p = path;

  if (*p != '\0')
    while (1) {
      char *q = p;
      char *name;
      struct file_name_list *dirtmp;

      /* Find the end of this name.  */
      while (*q != '\0' && *q != PATH_SEPARATOR)
	{
	  q++;
	}

      if (p == q)
	{
	  /* An empty name in the path stands for the current directory.  */
	  name = (char *) dmalloc ((size_t) 2);
	  name[0] = '.';
	  name[1] = '\0';
	}
      else
	{
	  /* Otherwise use the directory that is named.  */
	  name = (char *) dmalloc (size_fromInt (q - p + 1));
	  memcpy (name, p, size_fromInt (q - p));
	  name[q - p] = '\0';
	}

      dirtmp = (struct file_name_list *) dmalloc (sizeof (*dirtmp));
      dirtmp->next = 0;		/* New one goes on the end */
      dirtmp->control_macro = 0;
      dirtmp->c_system_include_path = 0;
      dirtmp->fname = cstring_fromChars (name);
      dirtmp->got_name_map = 0;
      cppReader_addIncludeChain (pfile, dirtmp);

      /* Advance past this name.  */
      p = q;
      if (*p == '\0')
	break;
      /* Skip the colon.  */
      p++;
    }
}
  
void
cppOptions_init (cppOptions *opts)
{
  memset ((char *) opts, 0, sizeof *opts);
  assertSet (opts);

  opts->in_fname = NULL;
  opts->out_fname = NULL;

  /* Initialize is_idchar to allow $.  */
  opts->dollars_in_ident = TRUE;

  opts->no_line_commands = 0;
  opts->no_trigraphs = TRUE;
  opts->put_out_comments = 1;
  opts->print_include_names = 0;
  opts->dump_macros = DUMP_DEFINITIONS; /* DUMP_NONE; */
  opts->no_output = 0;
  opts->cplusplus = 0;

  opts->cplusplus_comments = 1;
  opts->verbose = 0;
  opts->lang_asm = 0;
  opts->for_lint = 0;
  opts->chill = 0;
  opts->pedantic_errors = 0;
  opts->warn_comments = 0;
  opts->warnings_are_errors = 0;

  /* Added 2003-07-10: */
  opts->traditional = FALSE;
  opts->c89 = TRUE;
  initialize_char_syntax (opts);
}

enum cpp_token
cppReader_nullUnderflow (/*@unused@*/ cppReader *pfile)
{
  return CPP_EOF;
}

void
cppReader_nullCleanup (/*@unused@*/ cppBuffer *pbuf,
		       /*@unused@*/ cppReader *pfile)
{
  ;
}

void
cppReader_macroCleanup (cppBuffer *pbuf, /*@unused@*/ cppReader *pfile)
{
  hashNode macro = pbuf->hnode;

  if (macro->type == T_DISABLED)
    {
      macro->type = T_MACRO;
    }

  if (macro->type != T_MACRO || pbuf->buf != macro->value.defn->expansion)
    {
      sfree (pbuf->buf);
      pbuf->buf = NULL;
    }
}

void
cppReader_fileCleanup (cppBuffer *pbuf, /*@unused@*/ cppReader *pfile)
{
  if (pbuf->buf != NULL)
    {
      sfree (pbuf->buf);
      pbuf->buf = NULL;
    }
}

/* Assuming we have read '/'.
   If this is the start of a comment (followed by '*' or '/'),
   skip to the end of the comment, and return ' '.
   Return EOF if we reached the end of file before the end of the comment.
   If not the start of a comment, return '/'.  */

static int
skip_comment (cppReader *pfile, /*@null@*/ long *linep)
{
  int c = 0;

  llassert (pfile->buffer != NULL);
  llassert (pfile->buffer->cur != NULL);

  while (cppReader_peekC (pfile) == '\\' && cpp_peekN (pfile, 1) == '\n')
    {
      if (linep != NULL)
	{
	  (*linep)++;
	}

      cppReader_forward (pfile, 2);
    }

  if (cppReader_peekC (pfile) == '*')
    {
      cppReader_forward (pfile, 1);

      for (;;)
	{
	  int prev_c = c;
	  c = cppReader_getC (pfile);

	  if (c == EOF)
	    {
	      return EOF;
	    }

	  while (c == (int) '\\' && cppReader_peekC (pfile) == (int) '\n')
	    {
	      if (linep != NULL )
		{
		  (*linep)++;
		}

	      cppReader_forward (pfile, 1), c = cppReader_getC (pfile);
	    }

	  if (prev_c == (int) '*' && c == (int) '/')
	    {
	      return (int) ' ';
	    }

	  if (c == (int) '\n' && (linep != NULL))
	    {
	      (*linep)++;
	    }
	}
    }
  else if (cppReader_peekC (pfile) == '/' 
	   && CPPOPTIONS (pfile)->cplusplus_comments)
    {
      
      (void) cppoptgenerror 
	(FLG_SLASHSLASHCOMMENT,
	 message ("C++ style // comment" 
		 ),
	 pfile);
      
      cppReader_forward (pfile, 1);

      for (;;)
	{
	  c = cppReader_getC (pfile);

	  if (c == EOF)
	    {
	      /* Allow hash comment to be terminated by EOF.  */
	      return (int) ' '; 
	    }

	  while (c == (int) '\\' && cppReader_peekC (pfile) == '\n')
	    {
	      cppReader_forward (pfile, 1);
	      c = cppReader_getC (pfile);

	      if (linep != NULL)
		{
		  (*linep)++;
		}
	    }

	  if (c == (int) '\n')
	    {
	      /* Don't consider final '\n' to be part of comment.  */
	      cppReader_forward (pfile, -1);
	      return (int) ' ';
	    }
	}
    }
  else
    {
      return (int) '/';
    }
}

/* Skip whitespace \-newline and comments.  Does not macro-expand.  */
int /*@alt void@*/
cppSkipHspace (cppReader *pfile)
{
  int nspaces = 0;

  while (TRUE)
    {
      int c;

      llassert (pfile->buffer != NULL);

      c = cppReader_peekC (pfile);

      if (c == EOF)
	{
	  return 0; /* FIXME */
	}

      if (is_hor_space[c])
	{
	  if ((c == '\f' || c == '\v') && cppReader_isPedantic (pfile))
	    cppReader_pedwarn (pfile,
			 message ("%s in preprocessing directive",
				  c == '\f'
				  ? cstring_makeLiteralTemp ("formfeed")
				  : cstring_makeLiteralTemp ("vertical tab")));

	  nspaces++;
	  cppReader_forward (pfile, 1);
	}
      else if (c == '/')
	{
	  cppReader_forward (pfile, 1);
	  c = skip_comment (pfile, NULL);

	  if (c == '/')
	    {
	      cppReader_forward (pfile, -1);
	    }

	  if (c == EOF || c == '/')
	    {
	      return nspaces;
	    }
	}
      else if (c == '\\' && cpp_peekN (pfile, 1) == '\n')
	{
	  cppReader_forward (pfile, 2);
	}
      else if (c == '@' && CPPBUFFER (pfile)->has_escapes
	       && is_hor_space [cpp_peekN (pfile, 1)])
	{
	  cppReader_forward (pfile, 2);
	}
      else
	{
	  return nspaces;
	}
    }
}

/* Read the rest of the current line.
   The line is appended to PFILE's output buffer.  */

static void
copy_rest_of_line (cppReader *pfile)
{
  struct cppOptions *opts = CPPOPTIONS (pfile);

  for (;;)
    {
      int c;
      int nextc;

      llassert (pfile->buffer != NULL);

      c = cppReader_getC (pfile);

      switch (c)
	{
	case EOF:
	  goto end_directive;
	case '\\':
	  /*
	  ** Patch from Brian St. Pierre for handling MS-DOS files.
	  */

	  DPRINTF (("Reading directive: %d", (int) c));

	  if (cppReader_peekC (pfile) == '\n'
	      || cppReader_peekC (pfile) == '\r')
	    {
	      DPRINTF (("Reading directive..."));
	      if (cppReader_peekC (pfile) == '\r')
		{
		  DPRINTF (("Reading directive..."));
		  cppReader_forward (pfile, 1);
		}
             
	      DPRINTF (("Reading directive..."));
	      cppReader_forward (pfile, 1);
	      continue;
	    }

	  DPRINTF (("Falling..."));
	/*@fallthrough@*/ case '\'': case '\"':
	  goto scan_directive_token;

	case '/':
	  nextc = cppReader_peekC (pfile);

	  /*
	  ** was (opts->cplusplus_comments && nextc == '*')
	  ** yoikes!
	  */

	  if (nextc == '*'
	      || (opts->cplusplus_comments && nextc == '/'))
	    {
	      goto scan_directive_token;
	    }
	  /*@switchbreak@*/ break;
	case '\f':
	case '\v':
	  if (cppReader_isPedantic (pfile))
	    cppReader_pedwarn (pfile,
			 message ("%s in preprocessing directive",
				  c == '\f'
				  ? cstring_makeLiteralTemp ("formfeed")
				  : cstring_makeLiteralTemp ("vertical tab")));
	  /*@switchbreak@*/ break;

	case '\n':
	  cppReader_forward (pfile, -1);
	  goto end_directive;
	scan_directive_token:
	  cppReader_forward (pfile, -1);
	  (void) cpplib_getToken (pfile);
	  continue;
	}
      cppReader_putChar (pfile, c);
    }
end_directive: ;
  cppReader_nullTerminate (pfile);
}

void
cppReader_skipRestOfLine (cppReader *pfile)
{
  size_t old = cpplib_getWritten (pfile);
  copy_rest_of_line (pfile);
  cppReader_setWritten (pfile, old);
}

/* Handle a possible # directive.
   '#' has already been read.  */

int
cppReader_handleDirective (cppReader *pfile)
{
  int c;
  struct directive *kt = NULL;
  int ident_length;
  size_t after_ident = 0;
  char *ident = NULL;
  char *line_end = NULL;
  size_t old_written = cpplib_getWritten (pfile);
  int nspaces = cppSkipHspace (pfile);

  c = cppReader_peekC (pfile);

  if (c >= '0' && c <= '9')
    {
      /* Handle # followed by a line number.  */
      if (cppReader_isPedantic (pfile))
	{
	  cppReader_pedwarnLit
	    (pfile,
	     cstring_makeLiteralTemp ("`#' followed by integer"));
	}

      (void) do_line (pfile, NULL);
      goto done_a_directive;
    }


  /* Now find the directive name.  */

  cppReader_putChar (pfile, '#');

  parse_name (pfile, cppReader_getC (pfile));

  llassert (pfile->token_buffer != NULL);
  ident = pfile->token_buffer + old_written + 1;

  ident_length = cpplib_getPWritten (pfile) - ident;

  if (ident_length == 0 && cppReader_peekC (pfile) == '\n')
    {
      /* A line of just `#' becomes blank.  */
      return 1; 
    }

  for (kt = directive_table; ; kt++) 
    {
      if (kt->length <= 0)
	{
	  return 0; /* goto not_a_directive; */
	}

      if (kt->length == ident_length
	  && (cstring_equalPrefix (kt->name, cstring_fromChars (ident))))
	{
	  break;
	}
    }

  if (kt->command_reads_line)
    {
      after_ident = 0;
    }
  else
    {
      /* Nonzero means do not delete comments within the directive.
	 #define needs this when -traditional.  */
      bool comments = 1; /*cppReader_isTraditional (pfile) && kt->traditional_comments;  */
      int save_put_out_comments = CPPOPTIONS (pfile)->put_out_comments;
      CPPOPTIONS (pfile)->put_out_comments = comments;
      after_ident = cpplib_getWritten (pfile);
      copy_rest_of_line (pfile);
      CPPOPTIONS (pfile)->put_out_comments = save_put_out_comments;
    }


  /* For #pragma and #define, we may want to pass through the directive.
     Other directives may create output, but we don't want the directive
     itself out, so we pop it now.  For example #include may write a #line
     command (see comment in do_include), and conditionals may emit
     #failed ... #endfailed stuff.  But note that popping the buffer
     means the parameters to kt->func may point after pfile->limit
     so these parameters are invalid as soon as something gets appended
     to the token_buffer.  */

  line_end = cpplib_getPWritten (pfile);


  if (!kt->pass_thru && kt->type != T_DEFINE)
    {
      cppReader_setWritten (pfile, old_written);
    }

  llassert (pfile->token_buffer != NULL);

  /* was kt->pass_thru || */

  if (kt->type == T_DEFINE
      && cpp_shouldCheckMacro (pfile, pfile->token_buffer + old_written))
    {
      char *p = pfile->token_buffer + old_written;

      /*
      ** Still need to record value for preprocessing, so 
      ** #ifdef's, etc. using the value behave correctly.
      */
      
      (void) do_defineAux (pfile, kt, 
			   pfile->token_buffer + after_ident,
			   line_end,
			   TRUE);
      
      if (*p == '#')
	{
	  *p = ' ';
	}

      SKIP_WHITE_SPACE (p);

      llassert (*p == 'd');
      *p++ = LLMRCODE[0];

      llassert (*p == 'e');
      *p++ = LLMRCODE[1];

      llassert (*p == 'f');
      *p++ = LLMRCODE[2];

      llassert (*p == 'i');
      *p++ = LLMRCODE[3];

      llassert (*p == 'n');
      *p++ = LLMRCODE[4];

      llassert (*p == 'e');

      /*
      ** This is way-bogus.  We use the last char to record the number of
      ** spaces.  Its too hard to get them back into the input stream.
      */

      if (nspaces > 9) nspaces = 9;

      *p++ = '0' + nspaces;

      return 0; /* not_a_directive */
    }
  else if (kt->pass_thru)
    {
      /* Just leave the entire #define in the output stack.  */
      return 0; /* not_a_directive */

    }
  else if (kt->type == T_DEFINE
	   && CPPOPTIONS (pfile)->dump_macros == DUMP_NAMES)
    {
      char *p = pfile->token_buffer + old_written + 7;  /* Skip "#define". */
      SKIP_WHITE_SPACE (p);

      while (is_idchar[(int) *p])
	{
	  p++;
	}

      pfile->limit = p;
      cppReader_putChar (pfile, '\n');
    }
  else if (kt->type == T_DEFINE)
    {
      cppReader_setWritten (pfile, old_written);
    }
  else
    {
      ;
    }

done_a_directive:
  if (kt == NULL) {
    return 1;
  } else {
    llassert (kt->func != NULL);
    (void) (kt->func) (pfile, kt, pfile->token_buffer + after_ident, line_end);
    return 1;
  }
}

/* Pass a directive through to the output file.
   BUF points to the contents of the directive, as a contiguous string.
   LIMIT points to the first character past the end of the directive.
   KEYWORD is the keyword-table entry for the directive.  */

static void
pass_thru_directive (char *buf, char *limit,
		     cppReader *pfile,
		     struct directive *keyword)
{
  int keyword_length = keyword->length;

  cpplib_reserve (pfile,
		     size_fromInt (2 + keyword_length + (limit - buf)));
  cppReader_putCharQ (pfile, '#');
  /*@-observertrans@*/
  cppReader_putStrN (pfile, cstring_toCharsSafe (keyword->name),
		     size_fromInt (keyword_length));
  /*:=observertrans@*/

  if (limit != buf && buf[0] != ' ')
    {
      /* Was a bug, since reserve only used 1 + ... */
      cppReader_putCharQ (pfile, ' ');
    }

  cppReader_putStrN (pfile, buf, size_fromInt (limit - buf));
}

/* Read a replacement list for a macro with parameters.
   Build the DEFINITION structure.
   Reads characters of text starting at BUF until END.
   ARGLIST specifies the formal parameters to look for
   in the text of the definition; NARGS is the number of args
   in that list, or -1 for a macro name that wants no argument list.
   MACRONAME is the macro name itself (so we can avoid recursive expansion)
   and NAMELEN is its length in characters.

   Note that comments, backslash-newlines, and leading white space
   have already been deleted from the argument.  */

static DEFINITION *
collect_expansion (cppReader *pfile, char *buf, char *limit,
		   int nargs, /*@null@*/ struct arglist *arglist)
{
  DEFINITION *defn;
  char *p, *lastp, *exp_p;
  struct reflist *endpat = NULL;
  /* Pointer to first nonspace after last ## seen.  */
  char *concat = 0;
  /* Pointer to first nonspace after last single-# seen.  */
  char *stringify = 0;
  size_t maxsize;
  char expected_delimiter = '\0';


  /* Scan thru the replacement list, ignoring comments and quoted
     strings, picking up on the macro calls.  It does a linear search
     thru the arg list on every potential symbol.  Profiling might say
     that something smarter should happen.  */

  if (limit < buf)
    abort ();

  /* Find the beginning of the trailing whitespace.  */
  p = buf;

  while (p < limit && is_space[(int) limit[-1]])
    {
      limit--;
    }

  /* Allocate space for the text in the macro definition.
     Leading and trailing whitespace chars need 2 bytes each.
     Each other input char may or may not need 1 byte,
     so this is an upper bound.  The extra 5 are for invented
     leading and trailing newline-marker and final null.  */
  maxsize = (sizeof (*defn) + (limit - p) + 5);

  /* Occurrences of '@' get doubled, so allocate extra space for them.  */
  while (p < limit)
    {
      if (*p++ == '@')
	{
	  maxsize++;
	}
    }

  defn = (DEFINITION *) dmalloc (maxsize);
  defn->noExpand = FALSE;
  defn->file = NULL;
  defn->pattern = NULL;
  defn->nargs = nargs;
  defn->predefined = NULL;

  exp_p = defn->expansion = (char *) defn + sizeof (*defn);
  *defn->expansion = '\0'; /* convince splint it is initialized */

  defn->line = 0;
  defn->rest_args = NULL;
  defn->args.argnames = NULL;

  lastp = exp_p;

  p = buf;

  /* Add one initial space escape-marker to prevent accidental
     token-pasting (often removed by cpplib_macroExpand).  */
  *exp_p++ = '@';
  *exp_p++ = ' ';

  if (limit - p >= 2 && p[0] == '#' && p[1] == '#') {
    cppReader_errorLit (pfile,
			cstring_makeLiteralTemp ("`##' at start of macro definition"));
    p += 2;
  }

  /* Process the main body of the definition.  */
  while (p < limit) {
    int skipped_arg = 0;
    register char c = *p++;

    *exp_p++ = c;

    if (!cppReader_isTraditional (pfile)) {
      switch (c) {
      case '\'':
      case '\"':
	if (expected_delimiter != '\0')
	  {
	    if (c == expected_delimiter)
	      expected_delimiter = '\0';
	  }
	else
	  {
	    expected_delimiter = c;
	  }
	/*@switchbreak@*/ break;

      case '\\':
	if (p < limit && (expected_delimiter != '\0'))
	  {
	    /* In a string, backslash goes through
	       and makes next char ordinary.  */
	    *exp_p++ = *p++;
	  }
	/*@switchbreak@*/ break;

      case '@':
	/* An '@' in a string or character constant stands for itself,
	   and does not need to be escaped.  */
	if (expected_delimiter == '\0')
	  {
	    *exp_p++ = c;
	  }

	/*@switchbreak@*/ break;

      case '#':
	/* # is ordinary inside a string.  */
	if (expected_delimiter != '\0')
	  {
	    /*@switchbreak@*/ break;
	  }

	if (p < limit && *p == '#') {
	  /* ##: concatenate preceding and following tokens.  */
	  /* Take out the first #, discard preceding whitespace.  */
	  exp_p--;

	  /*@-usedef@*/
	  while (exp_p > lastp && is_hor_space[(int) exp_p[-1]])
	    {
	      --exp_p;
	    }
	  /*@=usedef@*/

	  /* Skip the second #.  */
	  p++;
	  /* Discard following whitespace.  */
	  SKIP_WHITE_SPACE (p);
	  concat = p;
	  if (p == limit)
	    {
	      cppReader_errorLit (pfile,
			    cstring_makeLiteralTemp ("`##' at end of macro definition"));
	    }
	} else if (nargs >= 0) {
	  /* Single #: stringify following argument ref.
	     Don't leave the # in the expansion.  */
	  exp_p--;
	  SKIP_WHITE_SPACE (p);
	  if (p == limit || ! is_idstart[(int) *p]
	      || (*p == 'L' && p + 1 < limit && (p[1] == '\'' || p[1] == '\"')))
	    cppReader_errorLit (pfile,
			  cstring_makeLiteralTemp ("`#' operator is not followed by a macro argument name"));
	  else
	    stringify = p;
	} else {
	  ; /* BADBRANCH; */
	}

	/*@switchbreak@*/ break;
      }
    } else {
      /* In -traditional mode, recognize arguments inside strings and
	 and character constants, and ignore special properties of #.
	 Arguments inside strings are considered "stringified", but no
	 extra quote marks are supplied.  */
      switch (c) {
      case '\'':
      case '\"':
	if (expected_delimiter != '\0') {
	  if (c == expected_delimiter)
	    expected_delimiter = '\0';
	} else
	  expected_delimiter = c;
	/*@switchbreak@*/ break;

      case '\\':
	/* Backslash quotes delimiters and itself, but not macro args.  */
	if (expected_delimiter != '\0' && p < limit
	    && (*p == expected_delimiter || *p == '\\')) {
	  *exp_p++ = *p++;
	  continue;
	}
	/*@switchbreak@*/ break;

      case '/':
	if (expected_delimiter != '\0') /* No comments inside strings.  */
	  /*@switchbreak@*/ break;
	if (*p == '*') {
	  /* If we find a comment that wasn't removed by cppReader_handleDirective,
	     this must be -traditional.  So replace the comment with
	     nothing at all.  */
	  exp_p--;
	  p += 1;
	  while (p < limit && !(p[-2] == '*' && p[-1] == '/'))
	    {
	      p++;
	    }
	}
	/*@switchbreak@*/ break;
      }
    }

    /* Handle the start of a symbol.  */
    if (is_idchar[(int) c] && nargs > 0) {
      char *id_beg = p - 1;
      size_t id_len;

      --exp_p;
      while (p != limit && is_idchar[(int) *p])
	{
	  p++;
	}

      id_len = size_fromInt (p - id_beg);

      if (is_idstart[(int) c]
	  && ! (id_len == 1 && c == 'L' && (*p == '\'' || *p == '\"'))) {
	register struct arglist *arg;

	for (arg = arglist; arg != NULL; arg = arg->next) {
	  struct reflist *tpat;

	  if (arg->name[0] == c
	      && arg->length == id_len
	      && strncmp (arg->name, id_beg, id_len) == 0) {
	    char *p1;

	    if (expected_delimiter && CPPOPTIONS (pfile)->warn_stringify) {
	      if (cppReader_isTraditional (pfile)) {
		cppReader_warning (pfile,
				   message ("macro argument `%x' is stringified.",
					    cstring_prefix (cstring_fromChars (arg->name), id_len)));
	      } else {
		cppReader_warning (pfile,
				   message ("macro arg `%x' would be stringified with -traditional.",
					    cstring_prefix (cstring_fromChars (arg->name), id_len)));
	      }
	    }
	    /* If ANSI, don't actually substitute inside a string.  */
	    if (!cppReader_isTraditional (pfile) && expected_delimiter)
	      /*@innerbreak@*/ break;
	    /* make a pat node for this arg and append it to the end of
	       the pat list */
	    tpat = (struct reflist *) dmalloc (sizeof (*tpat));
	    tpat->next = NULL;
	    tpat->raw_before = (concat == id_beg);
	    tpat->raw_after = 0;
	    tpat->rest_args = arg->rest_args;
	    tpat->stringify = (cppReader_isTraditional (pfile)
			       ? expected_delimiter != '\0'
			       : stringify == id_beg);

	    if (endpat == NULL)
	      {
		defn->pattern = tpat;
	      }
	    else
	      {
		endpat->next = tpat;
		/*@-branchstate@*/
	      } /*@=branchstate@*/ /* evs 2000 was =branchstate */

	    endpat = tpat;

	    tpat->argno = arg->argno;
	    tpat->nchars = exp_p - lastp;

	    p1 = p;

	    SKIP_WHITE_SPACE (p1);

	    if (p1 + 2 <= limit && p1[0] == '#' && p1[1] == '#')
	      {
		tpat->raw_after = 1;
	      }

	    lastp = exp_p;	/* place to start copying from next time */
	    skipped_arg = 1;

	    /*@innerbreak@*/ break;
	  }
	}
      }

      /* If this was not a macro arg, copy it into the expansion.  */
      if (skipped_arg == 0) {
	register char *lim1 = p;
	p = id_beg;

	while (p != lim1)
	  {
	    *exp_p++ = *p++;
	  }

	if (stringify == id_beg)
	  cppReader_errorLit (pfile,
			      cstring_makeLiteralTemp ("`#' operator should be followed by a macro argument name"));
      }
    }
  }

  if (!cppReader_isTraditional (pfile) && expected_delimiter == '\0')
    {
      /* If ANSI, put in a "@ " marker to prevent token pasting.
	 But not if "inside a string" (which in ANSI mode
	 happens only for -D option).  */
      *exp_p++ = '@';
      *exp_p++ = ' ';
    }

  *exp_p = '\0';

  defn->length = size_fromInt (exp_p - defn->expansion);

  /* Crash now if we overrun the allocated size.  */
  if (defn->length + 1 > maxsize)
    {
      llfatalbug (cstring_makeLiteral ("Maximum definition size exceeded."));
    }

  return defn; /* Spurious warning here */
}

/*
** evans 2001-12-31
** Gasp...cut-and-pasted from above to deal with pfile (should replace throughout with this...)
*/

static DEFINITION *
collect_expansionLoc (fileloc loc, char *buf, char *limit,
		      int nargs, /*@null@*/ struct arglist *arglist)
{
  DEFINITION *defn;
  char *p, *lastp, *exp_p;
  struct reflist *endpat = NULL;
  /* Pointer to first nonspace after last ## seen.  */
  char *concat = 0;
  /* Pointer to first nonspace after last single-# seen.  */
  char *stringify = 0;
  size_t maxsize;
  char expected_delimiter = '\0';


  /* Scan thru the replacement list, ignoring comments and quoted
     strings, picking up on the macro calls.  It does a linear search
     thru the arg list on every potential symbol.  Profiling might say
     that something smarter should happen.  */

  if (limit < buf)
    {
      llfatalbug (message ("%q: Limit is less than initial buffer pointer",
			   fileloc_unparse (loc)));
    }

  /* Find the beginning of the trailing whitespace.  */
  p = buf;

  while (p < limit && is_space[(int) limit[-1]])
    {
      limit--;
    }

  /* Allocate space for the text in the macro definition.
     Leading and trailing whitespace chars need 2 bytes each.
     Each other input char may or may not need 1 byte,
     so this is an upper bound.  The extra 5 are for invented
     leading and trailing newline-marker and final null.  */
  maxsize = (sizeof (*defn) + (limit - p) + 5);

  /* Occurrences of '@' get doubled, so allocate extra space for them.  */
  while (p < limit)
    {
      if (*p++ == '@')
	{
	  maxsize++;
	}
    }

  defn = (DEFINITION *) dmalloc (maxsize);
  defn->noExpand = FALSE;
  defn->file = NULL;
  defn->pattern = NULL;
  defn->nargs = nargs;
  defn->predefined = NULL;
  exp_p = defn->expansion = (char *) defn + sizeof (*defn);

  defn->line = 0;
  defn->rest_args = NULL;
  defn->args.argnames = NULL;

  lastp = exp_p;

  p = buf;

  /* Add one initial space escape-marker to prevent accidental
     token-pasting (often removed by cpplib_macroExpand).  */
  *exp_p++ = '@';
  *exp_p++ = ' ';

  if (limit - p >= 2 && p[0] == '#' && p[1] == '#') {
    voptgenerror (FLG_PREPROC,
		  cstring_makeLiteral ("Paste marker ## at start of macro definition"),
		  loc);
    p += 2;
  }

  /* Process the main body of the definition.  */
  while (p < limit) {
    int skipped_arg = 0;
    register char c = *p++;

    *exp_p++ = c;

    if (TRUE) { /* !cppReader_isTraditional (pfile)) { */
      switch (c) {
      case '\'':
      case '\"':
	if (expected_delimiter != '\0')
	  {
	    if (c == expected_delimiter)
	      expected_delimiter = '\0';
	  }
	else
	  {
	    expected_delimiter = c;
	  }
	/*@switchbreak@*/ break;

      case '\\':
	if (p < limit && (expected_delimiter != '\0'))
	  {
	    /* In a string, backslash goes through
	       and makes next char ordinary.  */
	    *exp_p++ = *p++;
	  }
	/*@switchbreak@*/ break;

      case '@':
	/* An '@' in a string or character constant stands for itself,
	   and does not need to be escaped.  */
	if (expected_delimiter == '\0')
	  {
	    *exp_p++ = c;
	  }

	/*@switchbreak@*/ break;

      case '#':
	/* # is ordinary inside a string.  */
	if (expected_delimiter != '\0')
	  {
	    /*@switchbreak@*/ break;
	  }

	if (p < limit && *p == '#') {
	  /* ##: concatenate preceding and following tokens.  */
	  /* Take out the first #, discard preceding whitespace.  */
	  exp_p--;

	  /*@-usedef@*/
	  while (exp_p > lastp && is_hor_space[(int) exp_p[-1]])
	    {
	      --exp_p;
	    }
	  /*@=usedef@*/

	  /* Skip the second #.  */
	  p++;
	  /* Discard following whitespace.  */
	  SKIP_WHITE_SPACE (p);
	  concat = p;
	  if (p == limit)
	    {
		voptgenerror (FLG_PREPROC,
			      cstring_makeLiteral ("`##' at end of macro definition"),
			      loc);
	    }
	} else if (nargs >= 0) {
	  /* Single #: stringify following argument ref.
	     Don't leave the # in the expansion.  */
	  exp_p--;
	  SKIP_WHITE_SPACE (p);
	  if (p == limit || ! is_idstart[(int) *p]
	      || (*p == 'L' && p + 1 < limit && (p[1] == '\'' || p[1] == '\"')))
	    {
		voptgenerror 
		  (FLG_PREPROC,
		   cstring_makeLiteral ("`#' operator is not followed by a macro argument name"),
		   loc);
	    }
	  else
	    stringify = p;
	} else {
	  ; /* BADBRANCH; */
	}

	/*@switchbreak@*/ break;
      }
    } else {
      /* In -traditional mode, recognize arguments inside strings and
	 and character constants, and ignore special properties of #.
	 Arguments inside strings are considered "stringified", but no
	 extra quote marks are supplied.  */
      switch (c) {
      case '\'':
      case '\"':
	if (expected_delimiter != '\0') {
	  if (c == expected_delimiter)
	    expected_delimiter = '\0';
	} else
	  expected_delimiter = c;
	/*@switchbreak@*/ break;

      case '\\':
	/* Backslash quotes delimiters and itself, but not macro args.  */
	if (expected_delimiter != '\0' && p < limit
	    && (*p == expected_delimiter || *p == '\\')) {
	  *exp_p++ = *p++;
	  continue;
	}
	/*@switchbreak@*/ break;
  
      case '/':
	if (expected_delimiter != '\0') /* No comments inside strings.  */
	  /*@switchbreak@*/ break;
	if (*p == '*') {
	  /* If we find a comment that wasn't removed by cppReader_handleDirective,
	     this must be -traditional.  So replace the comment with
	     nothing at all.  */
	  exp_p--;
	  p += 1;
	  while (p < limit && !(p[-2] == '*' && p[-1] == '/'))
	    {
	      p++;
	    }
	}
	/*@switchbreak@*/ break;
      }
    }

    /* Handle the start of a symbol.  */
    if (is_idchar[(int) c] && nargs > 0) {
      char *id_beg = p - 1;
      size_t id_len;

      --exp_p;
      while (p != limit && is_idchar[(int) *p])
	{
	  p++;
	}

      id_len = size_fromInt (p - id_beg);

      if (is_idstart[(int) c]
	  && ! (id_len == 1 && c == 'L' && (*p == '\'' || *p == '\"'))) {
	register struct arglist *arg;

	for (arg = arglist; arg != NULL; arg = arg->next) {
	  struct reflist *tpat;

	  if (arg->name[0] == c
	      && arg->length == id_len
	      && strncmp (arg->name, id_beg, id_len) == 0) {
	    char *p1;

	    if (expected_delimiter) { /* && CPPOPTIONS (pfile)->warn_stringify) { */
	      if (FALSE) { /* cppReader_isTraditional (pfile)) { */
		voptgenerror (FLG_PREPROC,
			      message ("macro argument `%x' is stringified.",
				       cstring_prefix (cstring_fromChars (arg->name), id_len)),
			      loc);

	      } else {
		voptgenerror (FLG_PREPROC,
			      message ("Macro arg `%x' would be stringified with -traditional.",
				       cstring_prefix (cstring_fromChars (arg->name), id_len)),
			      loc);
		
	      }
	    }
	    /* If ANSI, don't actually substitute inside a string.  */
	    if (TRUE /* !cppReader_isTraditional (pfile) */ && expected_delimiter)
	      /*@innerbreak@*/ break;
	    /* make a pat node for this arg and append it to the end of
	       the pat list */
	    tpat = (struct reflist *) dmalloc (sizeof (*tpat));
	    tpat->next = NULL;
	    tpat->raw_before = (concat == id_beg);
	    tpat->raw_after = 0;
	    tpat->rest_args = arg->rest_args;
	    tpat->stringify = (FALSE /* cppReader_isTraditional (pfile) */
			       ? expected_delimiter != '\0'
			       : stringify == id_beg);

	    if (endpat == NULL)
	      {
		defn->pattern = tpat;
	      }
	    else
	      {
		endpat->next = tpat;
		/*@-branchstate@*/
	      } /*@=branchstate@*/ /* evs 2000 was =branchstate */

	    endpat = tpat;

	    tpat->argno = arg->argno;
	    tpat->nchars = exp_p - lastp;

	    p1 = p;

	    SKIP_WHITE_SPACE (p1);

	    if (p1 + 2 <= limit && p1[0] == '#' && p1[1] == '#')
	      {
		tpat->raw_after = 1;
	      }

	    lastp = exp_p;	/* place to start copying from next time */
	    skipped_arg = 1;

	    /*@innerbreak@*/ break;
	  }
	}
      }

      /* If this was not a macro arg, copy it into the expansion.  */
      if (skipped_arg == 0) {
	register char *lim1 = p;
	p = id_beg;

	while (p != lim1)
	  {
	    *exp_p++ = *p++;
	  }

	if (stringify == id_beg)
	  {
	    voptgenerror
	      (FLG_PREPROC,
	       cstring_makeLiteral ("# operator should be followed by a macro argument name"),
	       loc);
	  }
      }
    }
  }

  if (/*!cppReader_isTraditional (pfile) && */ expected_delimiter == '\0')
    {
      /* If ANSI, put in a "@ " marker to prevent token pasting.
	 But not if "inside a string" (which in ANSI mode
	 happens only for -D option).  */
      *exp_p++ = '@';
      *exp_p++ = ' ';
    }

  *exp_p = '\0';

  defn->length = size_fromInt (exp_p - defn->expansion);

  /* Crash now if we overrun the allocated size.  */
  if (defn->length + 1 > maxsize)
    {
      llfatalbug (cstring_makeLiteral ("Maximum definition size exceeded."));
    }
  
  /*@-compdef@*/ /* defn->expansion defined? */
  return defn; 
  /*@=compdef@*/
}

/*
 * special extension string that can be added to the last macro argument to
 * allow it to absorb the "rest" of the arguments when expanded.  Ex:
 * 		#define wow(a, b...)		process (b, a, b)
 *		{ wow (1, 2, 3); }	->	{ process (2, 3, 1, 2, 3); }
 *		{ wow (one, two); }	->	{ process (two, one, two); }
 * if this "rest_arg" is used with the concat token '##' and if it is not
 * supplied then the token attached to with ## will not be outputted.  Ex:
 * 		#define wow (a, b...)		process (b ## , a, ## b)
 *		{ wow (1, 2); }		->	{ process (2, 1, 2); }
 *		{ wow (one); }		->	{ process (one); {
 */

/*@-readonlytrans@*/
static char rest_extension[] = "...";
/*:=readonlytrans@*/

/*@notfunction@*/
#define REST_EXTENSION_LENGTH	(sizeof (rest_extension) - 1)

/* Create a DEFINITION node from a #define directive.  Arguments are
   as for do_define.  */

  
static /*@null@*/ macroDef
create_definition (/*@exposed@*/ char *buf, char *limit,
		   cppReader *pfile, bool predefinition,
		   bool noExpand)
{
  char *bp;			/* temp ptr into input buffer */
  char *symname;		/* remember where symbol name starts */
  size_t sym_length;		/* and how long it is */
  int rest_args = 0;   /* really int! */
  int line;
  int col;
  cstring file = (CPPBUFFER (pfile) != NULL)
    ? CPPBUFFER (pfile)->nominal_fname : cstring_makeLiteralTemp ("");
  DEFINITION *defn;
  int arglengths = 0;		/* Accumulate lengths of arg names
				   plus number of args.  */
  macroDef mdef;
  char save = *limit;
  *limit = '\0';
  DPRINTF (("Create definition: %s", buf));
  *limit = save;

  cppBuffer_getLineAndColumn (CPPBUFFER (pfile), &line, &col);

  bp = buf;

  while (is_hor_space[(int) *bp])
    {
      bp++;
    }

  symname = bp;			/* remember where it starts */

  sym_length = cppReader_checkMacroName (pfile, bp, cstring_makeLiteralTemp ("macro"));

  bp += sym_length;

  /* Lossage will occur if identifiers or control keywords are broken
     across lines using backslash.  This is not the right place to take
     care of that.  */

  if (*bp == '(') {
    struct arglist *arg_ptrs = NULL;
    int argno = 0;

    bp++;			/* skip '(' */
    SKIP_WHITE_SPACE (bp);

    /* Loop over macro argument names.  */
    while (*bp != ')')
      {
	struct arglist *temp = (struct arglist *) dmalloc (sizeof (*temp));
	temp->name = bp;
	temp->next = arg_ptrs;
	temp->argno = argno++;
	temp->rest_args = 0;

	arg_ptrs = temp;

	if (rest_args != 0)
	  {
	    cppReader_pedwarn (pfile,
			 message ("another parameter follows `%s'",
				  cstring_fromChars (rest_extension)));
	  }

	if (!is_idstart[(int) *bp])
	  {
	    cppReader_pedwarnLit (pfile,
			    cstring_makeLiteralTemp ("Invalid character in macro parameter name"));
	  }

	/* Find the end of the arg name.  */
	while (is_idchar[(int) *bp])
	  {
	    bp++;
	    /* do we have a "special" rest-args extension here? */
	    if (limit - bp > size_toInt (REST_EXTENSION_LENGTH)
		&& strncmp (rest_extension, bp, REST_EXTENSION_LENGTH) == 0)
	      {
		rest_args = 1;
		temp->rest_args = 1;
		/*@innerbreak@*/ break;
	      }
	  }

	temp->length = size_fromInt (bp - temp->name);

	if (rest_args != 0)
	  {
	    bp += REST_EXTENSION_LENGTH;
	  }

	arglengths += temp->length + 2;
	SKIP_WHITE_SPACE (bp);

	if (temp->length == 0 || (*bp != ',' && *bp != ')')) {
	  cppReader_errorLit (pfile,
			cstring_makeLiteralTemp ("Parameter list for #define is not parseable"));
	  goto nope;
	}

	if (*bp == ',') {
	  bp++;
	  SKIP_WHITE_SPACE (bp);
	}
	if (bp >= limit) {
	  cppReader_errorLit (pfile,
			cstring_makeLiteralTemp ("unterminated parameter list in `#define'"));
	  goto nope;
	}
	{
	  struct arglist *otemp;

	  for (otemp = temp->next; otemp != NULL; otemp = otemp->next)
	    {
	      if (temp->length == otemp->length &&
		  strncmp (temp->name, otemp->name, temp->length) == 0) {
		cstring name = cstring_copyLength (temp->name, temp->length);
		cppReader_error (pfile,
			   message ("duplicate argument name `%x' in `#define'", name));
		goto nope;
	      }
	    }
	}
      }
  
    ++bp;			/* skip paren */
    SKIP_WHITE_SPACE (bp);
    /* now everything from bp before limit is the definition.  */
    defn = collect_expansion (pfile, bp, limit, argno, arg_ptrs);
    defn->rest_args = rest_args;
    
    /* Now set defn->args.argnames to the result of concatenating
       the argument names in reverse order
       with comma-space between them.  */
    defn->args.argnames = (char *) dmalloc (size_fromInt (arglengths + 1));
  
    {
      struct arglist *temp;
      int i = 0;
      for (temp = arg_ptrs; temp != NULL; temp = temp->next) 
	{
	  memcpy (&defn->args.argnames[i], temp->name, temp->length);
	  i += temp->length;
	  if (temp->next != 0)
	    {
	      defn->args.argnames[i++] = ',';
	      defn->args.argnames[i++] = ' ';
	    }
	}
      
      defn->args.argnames[i] = '\0';
    }

    sfree (arg_ptrs);
  } else {
    /* Simple expansion or empty definition.  */

    if (bp < limit)
      {
	if (is_hor_space[(int) *bp]) {
	  bp++;
	  SKIP_WHITE_SPACE (bp);
	} else {
	  switch (*bp) {
	  case '!':  case '\"':  case '#':  case '%':  case '&':  case '\'':
	  case ')':  case '*':  case '+':  case ',':  case '-':  case '.':
	  case '/':  case ':':  case ';':  case '<':  case '=':  case '>':
	  case '?':  case '[':  case '\\': case ']':  case '^':  case '{':
	  case '|':  case '}':  case '~':
	    cppReader_warning (pfile,
			 message ("Missing white space after #define %x",
				  cstring_prefix (cstring_fromChars (symname),
						  sym_length)));
	    break;

	  default:
	    cppReader_pedwarn (pfile,
			 message ("Missing white space after #define %x",
				  cstring_prefix (cstring_fromChars (symname),
						  sym_length)));
	    break;
	  }
	}
      }
    /* now everything from bp before limit is the definition.  */
    defn = collect_expansion (pfile, bp, limit, -1, NULL);
    defn->args.argnames = mstring_createEmpty ();
  }

  defn->noExpand = noExpand;
  DPRINTF (("No expand: %d", noExpand));

  defn->line = line;

  /* not: llassert (cstring_isUndefined (defn->file)); */
  defn->file = file;

  /* OP is null if this is a predefinition */
  defn->predefined = predefinition;
  mdef.defn = defn;
  mdef.symnam = symname;
  mdef.symlen = sym_length;

  return mdef;

nope:
  mdef.defn = NULL;
  mdef.symnam = NULL;
  return mdef;
}

/*@null@*/ macroDef
cpplib_createDefinition (cstring def,
			 fileloc loc,
			 bool predefinition,
			 bool noExpand)
{
  char *buf = cstring_toCharsSafe (def);
  char *limit = buf + cstring_length (def);
  char *bp;			/* temp ptr into input buffer */
  char *symname;		/* remember where symbol name starts */
  size_t sym_length;		/* and how long it is */
  int rest_args = 0;   /* really int! */
  int line = fileloc_lineno (loc);
  cstring file = fileloc_filename (loc);
  DEFINITION *defn;
  int arglengths = 0;		/* Accumulate lengths of arg names
				   plus number of args.  */
  macroDef mdef;

  bp = buf;

  DPRINTF (("Creating definition: %s", buf));

  while (is_hor_space[(int) *bp])
    {
      bp++;
    }

  symname = bp;	/* remember where it starts */

  sym_length = cppReader_checkMacroNameLoc (loc, symname, cstring_makeLiteralTemp ("macro"));

  DPRINTF (("length: %d", sym_length));

  bp += sym_length;

  DPRINTF (("Here: %s", bp));

  /* Lossage will occur if identifiers or control keywords are broken
     across lines using backslash.  This is not the right place to take
     care of that.  */

  if (*bp == '(') {
    struct arglist *arg_ptrs = NULL;
    int argno = 0;
  
    bp++;			/* skip '(' */
    SKIP_WHITE_SPACE (bp);

    /* Loop over macro argument names.  */
    while (*bp != ')')
      {
	struct arglist *temp = (struct arglist *) dmalloc (sizeof (*temp));
	temp->name = bp;
	temp->next = arg_ptrs;
	temp->argno = argno++;
	temp->rest_args = 0;

	arg_ptrs = temp;

	if (rest_args != 0)
	  {
	    voptgenerror (FLG_PREPROC,
			  message ("Another parameter follows %s",
				   cstring_fromChars (rest_extension)),
			  loc);
	  }

	if (!is_idstart[(int) *bp])
	  {
	    voptgenerror (FLG_PREPROC,
			  message ("Invalid character in macro parameter name: %c", *bp),
			  loc);
	  }

	/* Find the end of the arg name.  */
	while (is_idchar[(int) *bp])
	  {
	    bp++;
	    /* do we have a "special" rest-args extension here? */
	    if (limit - bp > size_toInt (REST_EXTENSION_LENGTH)
		&& strncmp (rest_extension, bp, REST_EXTENSION_LENGTH) == 0)
	      {
		rest_args = 1;
		temp->rest_args = 1;
		/*@innerbreak@*/ break;
	      }
	  }

	temp->length = size_fromInt (bp - temp->name);
	
	if (rest_args != 0)
	  {
	    bp += REST_EXTENSION_LENGTH;
	  }

	arglengths += temp->length + 2;
	SKIP_WHITE_SPACE (bp);
	
	if (temp->length == 0 || (*bp != ',' && *bp != ')')) {
	  voptgenerror (FLG_PREPROC,
			cstring_makeLiteral ("Parameter list for #define is not parseable"),
			loc);
	  goto nope;
	}

	if (*bp == ',') {
	  bp++;
	  SKIP_WHITE_SPACE (bp);
	}
	if (bp >= limit) {
	  voptgenerror (FLG_PREPROC,
			cstring_makeLiteral ("Unterminated parameter list in #define'"),
			loc);
	  goto nope;
	}
	{
	  struct arglist *otemp;

	  for (otemp = temp->next; otemp != NULL; otemp = otemp->next)
	    {
	      if (temp->length == otemp->length &&
		  strncmp (temp->name, otemp->name, temp->length) == 0) {
		cstring name = cstring_copyLength (temp->name, temp->length);

		voptgenerror (FLG_PREPROC,
			      message ("Duplicate argument name in #define: %s", name),
			      loc);
		goto nope;
	      }
	    }
	}
      }
    
    ++bp;			/* skip paren */
    SKIP_WHITE_SPACE (bp);
    /* now everything from bp before limit is the definition.  */
    defn = collect_expansionLoc (loc, bp, limit, argno, arg_ptrs);
    defn->rest_args = rest_args;
    
    /* Now set defn->args.argnames to the result of concatenating
       the argument names in reverse order
       with comma-space between them.  */
    defn->args.argnames = (char *) dmalloc (size_fromInt (arglengths + 1));

    {
      struct arglist *temp;
      int i = 0;
      for (temp = arg_ptrs; temp != NULL; temp = temp->next) {
	memcpy (&defn->args.argnames[i], temp->name, temp->length);
	i += temp->length;
	if (temp->next != 0) {
	  defn->args.argnames[i++] = ',';
	  defn->args.argnames[i++] = ' ';
	}
      }

      defn->args.argnames[i] = '\0';
    }

    sfree (arg_ptrs);
  } else {
    /* Simple expansion or empty definition.  */

    if (bp < limit)
      {
	if (is_hor_space[(int) *bp]) {
	  bp++;
	  SKIP_WHITE_SPACE (bp);
	} else {
	  switch (*bp) {
	  case '!':  case '\"':  case '#':  case '%':  case '&':  case '\'':
	  case ')':  case '*':  case '+':  case ',':  case '-':  case '.':
	  case '/':  case ':':  case ';':  case '<':  case '=':  case '>':
	  case '?':  case '[':  case '\\': case ']':  case '^':  case '{':
	  case '|':  case '}':  case '~':
	    voptgenerror (FLG_PREPROC,
			  message ("Missing white space after #define %x",
				   cstring_prefix (cstring_fromChars (symname),
						   sym_length)),
			  loc);
	    break;

	  default:
	    voptgenerror (FLG_PREPROC,
			  message ("Missing white space after #define %x",
				   cstring_prefix (cstring_fromChars (symname),
						   sym_length)),
			  loc);
	    break;
	  }
	}
      }

    /* now everything from bp before limit is the definition.  */
    llassert (limit > bp);
    defn = collect_expansionLoc (loc, bp, limit, -1, NULL);
    defn->args.argnames = mstring_createEmpty ();
  }

  defn->noExpand = noExpand;
  DPRINTF (("No expand: %d", noExpand));

  defn->line = line;

  /* not: llassert (cstring_isUndefined (defn->file)); */
  defn->file = file;

  /* OP is null if this is a predefinition */
  defn->predefined = predefinition;

  mdef.defn = defn;
  mdef.symnam = symname;
  mdef.symlen = sym_length;

  return mdef;

nope:
  mdef.defn = NULL;
  mdef.symnam = NULL;
  return mdef;
}

/* Check a purported macro name SYMNAME, and yield its length.
   USAGE is the kind of name this is intended for.  */

size_t cppReader_checkMacroName (cppReader *pfile, char *symname, cstring usage)
{
  char *p;
  size_t sym_length;
  
  for (p = symname; is_idchar[(int) *p]; p++)
    {
      ;
    }
  
  sym_length = size_fromInt (p - symname);
  
  if (sym_length == 0
      || (sym_length == 1 && *symname == 'L' && (*p == '\'' || *p == '\"')))
    {
      cppReader_error (pfile, message ("invalid %s name", usage));
    }
  else if (!is_idstart[(int) *symname])
    {
      char *msg = (char *) dmalloc (sym_length + 1);
      memcpy (msg, symname, sym_length);
      msg[sym_length] = '\0';
      cppReader_error (pfile, message ("invalid %s name `%s'", usage,
				       cstring_fromChars (msg)));
      sfree (msg);
    }
  else
    {
      if ((strncmp (symname, "defined", 7) == 0) && sym_length == 7)
	{
	  cppReader_error (pfile, message ("invalid %s name `defined'", usage));
	}
    }

  return sym_length;
}
  
/*
** evans 2001-12-31
** Gasp...cut-and-pasted from above to deal with pfile (should replace throughout with this...)
*/

size_t cppReader_checkMacroNameLoc (fileloc loc, char *symname, cstring usage)
{
  char *p;
  size_t sym_length;
  
  for (p = symname; is_idchar[(int) *p]; p++)
    {
      ;
    }
  
  sym_length = size_fromInt (p - symname);
  
  if (sym_length == 0
      || (sym_length == 1 && *symname == 'L' && (*p == '\'' || *p == '\"')))
    {
      voptgenerror (FLG_PREPROC, message ("Invalid %s name: %s", usage, 
					  cstring_fromChars (symname)), loc);
    }
  else if (!is_idstart[(int) *symname])
    {
      char *msg = (char *) dmalloc (sym_length + 1);
      memcpy (msg, symname, sym_length);
      msg[sym_length] = '\0';
      voptgenerror (FLG_PREPROC, message ("Invalid %s name: %s", usage, 
					  cstring_fromChars (msg)),
		    loc);
      sfree (msg);
    }
  else
    {
      if ((strncmp (symname, "defined", 7) == 0) && sym_length == 7)
	{
	  voptgenerror (FLG_PREPROC, message ("Invalid %s name: defined", usage), loc);
	}
    }

  return sym_length;
}
  
/* Return zero if two DEFINITIONs are isomorphic.  */

static bool
compare_defs (DEFINITION *d1, DEFINITION *d2)
{
  register struct reflist *a1, *a2;
  register char *p1 = d1->expansion;
  register char *p2 = d2->expansion;
  bool first = TRUE;

  if (d1->nargs != d2->nargs)
    {
      return TRUE;
    }

  llassert (d1->args.argnames != NULL);
  llassert (d2->args.argnames != NULL);

  if (strcmp ((char *)d1->args.argnames, (char *)d2->args.argnames) != 0)
    {
      return TRUE;
    }

  for (a1 = d1->pattern, a2 = d2->pattern;
       (a1 != NULL) && (a2 != NULL);
       a1 = a1->next, a2 = a2->next) {
    if (!((a1->nchars == a2->nchars
	   && (strncmp (p1, p2, size_fromInt (a1->nchars)) == 0))
	  || ! comp_def_part (first, p1, a1->nchars, p2, a2->nchars, 0))
	|| a1->argno != a2->argno
	|| a1->stringify != a2->stringify
	|| a1->raw_before != a2->raw_before
	|| a1->raw_after != a2->raw_after)
      return TRUE;
    first = 0;
    p1 += a1->nchars;
    p2 += a2->nchars;
  }
  if (a1 != a2)
    return TRUE;

  if (comp_def_part (first, p1, size_toInt (d1->length - (p1 - d1->expansion)),
		     p2, size_toInt (d2->length - (p2 - d2->expansion)), 1))
    return TRUE;

  return FALSE;
}

/*
** Return TRUE if two parts of two macro definitions are effectively different.
**    One of the parts starts at BEG1 and has LEN1 chars;
**    the other has LEN2 chars at BEG2.
**    Any sequence of whitespace matches any other sequence of whitespace.
**    FIRST means these parts are the first of a macro definition;
**    so ignore leading whitespace entirely.
**    LAST means these parts are the last of a macro definition;
**    so ignore trailing whitespace entirely.  
*/

static bool
comp_def_part (bool first, char *beg1, int len1, char *beg2, int len2, bool last)
{
  char *end1 = beg1 + len1;
  char *end2 = beg2 + len2;

  if (first) {
    while (beg1 != end1 && is_space[(int) *beg1]) { beg1++; }
    while (beg2 != end2 && is_space[(int) *beg2]) { beg2++; }
  }
  if (last) {
    while (beg1 != end1 && is_space[(int) end1[-1]]) { end1--; }
    while (beg2 != end2 && is_space[(int) end2[-1]]) { end2--; }
  }
  while (beg1 != end1 && beg2 != end2) {
    if (is_space[(int) *beg1] && is_space[(int) *beg2]) {
      while (beg1 != end1 && is_space[(int) *beg1]) { beg1++; }
      while (beg2 != end2 && is_space[(int) *beg2]) { beg2++; }
    } else if (*beg1 == *beg2) {
      beg1++; beg2++;
    } else break;
  }
  return (beg1 != end1) || (beg2 != end2);
}

/* 
** Process a #define command.
**    BUF points to the contents of the #define command, as a contiguous string.
**    LIMIT points to the first character past the end of the definition.
**    KEYWORD is the keyword-table entry for #define,
**    or NULL for a "predefined" macro.  
*/

static int
do_defineAux (cppReader *pfile, struct directive *keyword,
	      /*@exposed@*/ char *buf, char *limit, bool noExpand)
{
  int hashcode;
  macroDef mdef;
  hashNode hp;

  mdef = create_definition (buf, limit, pfile, keyword == NULL, noExpand);

  if (mdef.defn == 0)
    goto nope;

  hashcode = cpphash_hashCode (mdef.symnam, mdef.symlen, CPP_HASHSIZE);

  if ((hp = cpphash_lookup (mdef.symnam, size_toInt (mdef.symlen), hashcode)) != NULL)
    {
      bool ok = FALSE;

      /* Redefining a precompiled key is ok.  */
      if (hp->type == T_PCSTRING)
	ok = TRUE;
      /* Redefining a macro is ok if the definitions are the same.  */
      else if (hp->type == T_MACRO)
	ok = !compare_defs (mdef.defn, hp->value.defn);
      /* Redefining a constant is ok with -D.  */
      else if (hp->type == T_CONST)
	ok = !CPPOPTIONS (pfile)->done_initializing;
      else {
	ok = FALSE; /* Redefining anything else is bad. */
      }

      /* Print the warning if it's not ok.  */
      if (!ok)
	{
	  /*
	  ** If we are passing through #define and #undef directives, do
	  ** that for this re-definition now.
	  */

	  if (CPPOPTIONS (pfile)->debug_output && (keyword != NULL))
	    {
	      /* llassert (keyword != NULL); */
	      pass_thru_directive (buf, limit, pfile, keyword);
	    }

	  cpp_setLocation (pfile);

	  if (hp->type == T_MACRO)
	    {
	      if (hp->value.defn->noExpand)
		{
		  ; /* error will be reported checking macros */
		}
	      else
		{
		  genppllerrorhint
		    (FLG_MACROREDEF,
		     message ("Macro %q already defined",
			      cstring_copyLength (mdef.symnam, mdef.symlen)),
		     message ("%q: Previous definition of %q",
			      fileloc_unparseRaw (hp->value.defn->file,
						 (int) hp->value.defn->line),
			      cstring_copyLength (mdef.symnam, mdef.symlen)));
		}
	    }
	  else
	    {
	      genppllerror (FLG_MACROREDEF,
			    message ("Macro %q already defined",
				     cstring_copyLength (mdef.symnam,
							 mdef.symlen)));
	    }
	}

      /* Replace the old definition.  */
      hp->type = T_MACRO;
      hp->value.defn = mdef.defn;
    }
  else
    {
      /*
      ** If we are passing through #define and #undef directives, do
      ** that for this new definition now.
      */

      hashNode hn;

      if (CPPOPTIONS (pfile)->debug_output && (keyword != NULL))
	{
	  pass_thru_directive (buf, limit, pfile, keyword);
	}

      DPRINTF (("Define macro: %s / %d", 
		mdef.symnam, mdef.defn->noExpand));
      
      hn = cpphash_installMacro (mdef.symnam, mdef.symlen, mdef.defn, hashcode);
      /*@-branchstate@*/
    } /*@=branchstate@*/

  return 0;

nope:
  return 1;
}

static int
do_define (cppReader *pfile, struct directive *keyword,
	   /*@exposed@*/ char *buf, char *limit)
{
  DPRINTF (("Regular do define"));
  return do_defineAux (pfile, keyword, buf, limit, FALSE);
}

/*
** This structure represents one parsed argument in a macro call.
** `raw' points to the argument text as written (`raw_length' is its length).
** `expanded' points to the argument's macro-expansion
** (its length is `expand_length').
**  `stringified_length' is the length the argument would have
** if stringified.
**  `use_count' is the number of times this macro arg is substituted
** into the macro.  If the actual use count exceeds 10,
** the value stored is 10.  
*/

/* raw and expanded are relative to ARG_BASE */
/*@notfunction@*/
#define ARG_BASE ((pfile)->token_buffer)

struct argdata {
  /* Strings relative to pfile->token_buffer */
  long raw;
  size_t expanded;
  size_t stringified;
  int raw_length;
  int expand_length;
  int stringified_length;
  bool newlines;
  int use_count;
};

/* 
** Allocate a new cppBuffer for PFILE, and push it on the input buffer stack.
**   If BUFFER != NULL, then use the LENGTH characters in BUFFER
**   as the new input buffer.
**   Return the new buffer, or NULL on failure.  
*/

/*@null@*/ /*@exposed@*/ cppBuffer *
cppReader_pushBuffer (cppReader *pfile, char *buffer, size_t length)
{
  cppBuffer *buf = cppReader_getBufferSafe (pfile);

  if (buf == pfile->buffer_stack)
    {
      cppReader_fatalError
	(pfile,
	 message ("%s: macro or `#include' recursion too deep",
		  (buf->fname != NULL)
		  ? buf->fname
		  : cstring_makeLiteral ("<no name>")));
      sfreeEventually (buffer);
      return NULL;
    }

  llassert (buf != NULL);

  buf--;
  memset ((char *) buf, 0, sizeof (*buf));
  DPRINTF (("Pushing buffer: %s", cstring_copyLength (buffer, length)));
  CPPBUFFER (pfile) = buf;

  buf->if_stack = pfile->if_stack;
  buf->cleanup = cppReader_nullCleanup;
  buf->underflow = cppReader_nullUnderflow;
  buf->buf = buffer;
  buf->cur = buf->buf;

  if (buffer != NULL)
    {
      buf->alimit = buf->rlimit = buffer + length;
    }
  else
    {
      buf->alimit = buf->rlimit = NULL;
    }

  return buf;
}

cppBuffer *
cppReader_popBuffer (cppReader *pfile)
{
  cppBuffer *buf = CPPBUFFER (pfile);

  llassert (buf != NULL);

  (void) (*buf->cleanup) (buf, pfile);
  return ++CPPBUFFER (pfile);
}

/*
** Scan until CPPBUFFER (PFILE) is exhausted into PFILE->token_buffer.
** Pop the buffer when done.  
*/

void
cppReader_scanBuffer (cppReader *pfile)
{
  cppBuffer *buffer = CPPBUFFER (pfile);
  for (;;)
    {
      enum cpp_token token;
      
      token = cpplib_getToken (pfile);

      if (token == CPP_EOF) /* Should not happen ...  */
	{
	  break;
	}

      if (token == CPP_POP && CPPBUFFER (pfile) == buffer)
	{
	  (void) cppReader_popBuffer (pfile);
	  break;
	}
    }
}

/*
 * Rescan a string (which may have escape marks) into pfile's buffer.
 * Place the result in pfile->token_buffer.
 *
 * The input is copied before it is scanned, so it is safe to pass
 * it something from the token_buffer that will get overwritten
 * (because it follows cpplib_getWritten).  This is used by do_include.
 */

static void
cpp_expand_to_buffer (cppReader *pfile, char *buf, size_t length)
{
  register cppBuffer *ip;
  char *limit = buf + length;
  char *buf1, *p1, *p2;

  DPRINTF (("Expand to buffer: %s", cstring_copyLength (buf, length)));

  /* evans - 2001-08-26
  ** length is unsigned - this doesn't make sense
  if (length < 0)
    abort ();
  **
  */

  /* Set up the input on the input stack.  */

  buf1 = (char *) dmalloc (length + 1);

  p1 = buf;
  p2 = buf1;

  while (p1 != limit)
    {
      *p2++ = *p1++;
    }

  buf1[length] = '\0';

  ip = cppReader_pushBuffer (pfile, buf1, length);

  if (ip == NULL)
    return;

  ip->has_escapes = TRUE;

  /* Scan the input, create the output.  */
  cppReader_scanBuffer (pfile);

  cppReader_nullTerminate (pfile);
}

static void
adjust_position (char *buf, char *limit, int *linep, int *colp)
{
  while (buf < limit)
    {
      char ch = *buf++;
      if (ch == '\n')
	(*linep)++, (*colp) = 1;
      else
	(*colp)++;
    }
}

/* Move line_base forward, updating lineno and colno.  */

static void
update_position (cppBuffer *pbuf)
{
  char *old_pos;
  char *new_pos = pbuf->cur;
  register struct parse_marker *mark;

  llassert (pbuf->buf != NULL);
  old_pos = pbuf->buf + pbuf->line_base;

  for (mark = pbuf->marks;  mark != NULL; mark = mark->next)
    {
      if (pbuf->buf + mark->position < new_pos)
	new_pos = pbuf->buf + mark->position;
    }
  pbuf->line_base += new_pos - old_pos;

  llassert (old_pos != NULL);
  llassert (new_pos != NULL);

  adjust_position (old_pos, new_pos, &pbuf->lineno, &pbuf->colno);
}

void
cppBuffer_getLineAndColumn (/*@null@*/ cppBuffer *pbuf, /*@out@*/ int *linep,
			    /*@null@*/ /*@out@*/ int *colp)
{
  int dummy;

  if (colp == NULL)
    {
      colp = &dummy;
      /*@-branchstate@*/
    } /*@=branchstate@*/

  if (pbuf != NULL)
    {
      *linep = pbuf->lineno;
      *colp = pbuf->colno;

      llassert (pbuf->buf != NULL);
      llassert (pbuf->cur != NULL);

      adjust_position (pbuf->buf + pbuf->line_base, pbuf->cur, linep, colp);
    }
  else
    {
      *linep = 0;
      *colp = 0;
    }
}

/* Return the cppBuffer that corresponds to a file (not a macro).  */

/*@exposed@*/ /*@null@*/ cppBuffer *cppReader_fileBuffer (cppReader *pfile)
{
  cppBuffer *ip = cppReader_getBuffer (pfile);

  for ( ;
	ip != NULL && ip != cppReader_nullBuffer (pfile); 
	ip = cppBuffer_prevBuffer (ip))
    {
      if (ip->fname != NULL)
	{
	  return ip;
	}
    }
  
  return NULL;
}

static long
count_newlines (char *buf, char *limit)
{
  register long count = 0;

  while (buf < limit)
    {
      char ch = *buf++;
      if (ch == '\n')
	count++;
    }
  return count;
}

/*
 * write out a #line command, for instance, after an #include file.
 * If CONDITIONAL is nonzero, we can omit the #line if it would
 * appear to be a no-op, and we can output a few newlines instead
 * if we want to increase the line number by a small amount.
 * FILE_CHANGE says whether we are entering a file, leaving, or neither.
 */

static void
output_line_command (cppReader *pfile, bool conditional,
		     enum file_change_code file_change)
{
  int line, col;
  cppBuffer *ip = CPPBUFFER (pfile);
  cppBuffer *buf;

  llassert (ip != NULL);

  if (ip->fname == NULL)
    return;

  update_position (ip);

  if (CPPOPTIONS (pfile)->no_line_commands
      || CPPOPTIONS (pfile)->no_output)
    return;

  buf = CPPBUFFER (pfile);

  llassert (buf != NULL);

  line = buf->lineno;
  col = buf->colno;

  llassert (ip->cur != NULL);

  adjust_position (cppLineBase (ip), ip->cur, &line, &col);

  if (CPPOPTIONS (pfile)->no_line_commands)
    return;

  if (conditional) {
    if (line == pfile->lineno)
      return;

    /* If the inherited line number is a little too small,
       output some newlines instead of a #line command.  */

    if (line > pfile->lineno && line < pfile->lineno + 8)
      {
	cpplib_reserve (pfile, 20);
	while (line > pfile->lineno)
	  {
	    cppReader_putCharQ (pfile, '\n');
	    pfile->lineno++;
	  }

	return;
      }
  }

  cpplib_reserve (pfile, 4 * cstring_length (ip->nominal_fname) + 50);

  {
#ifdef OUTPUT_LINE_COMMANDS
    static char sharp_line[] = "#line ";
#else
    static char sharp_line[] = "# ";
#endif
    cppReader_putStrN (pfile, sharp_line, sizeof(sharp_line)-1);
  }

  sprintf (cpplib_getPWritten (pfile), "%d ", line);
  cppReader_adjustWritten (pfile, strlen (cpplib_getPWritten (pfile)));

  quote_string (pfile, cstring_toCharsSafe (ip->nominal_fname));

  if (file_change != same_file) {
    cppReader_putCharQ (pfile, ' ');
    cppReader_putCharQ (pfile, file_change == enter_file ? '1' : '2');
  }
  /* Tell cc1 if following text comes from a system header file.  */
  if (ip->system_header_p != '\0') {
    cppReader_putCharQ (pfile, ' ');
    cppReader_putCharQ (pfile, '3');
  }
#ifndef NO_IMPLICIT_EXTERN_C
  /* Tell cc1plus if following text should be treated as C.  */
  if (ip->system_header_p == (char) 2 && CPPOPTIONS (pfile)->cplusplus) {
    cppReader_putCharQ (pfile, ' ');
    cppReader_putCharQ (pfile, '4');
  }
#endif
  cppReader_putCharQ (pfile, '\n');
  pfile->lineno = line;
}


/*
 * Parse a macro argument and append the info on PFILE's token_buffer.
 * REST_ARGS means to absorb the rest of the args.
 * Return nonzero to indicate a syntax error.
 */

static enum cpp_token
macarg (cppReader *pfile, int rest_args)
{
  int paren = 0;
  enum cpp_token token;
  char save_put_out_comments = CPPOPTIONS (pfile)->put_out_comments;
  bool oldexpand = pfile->no_macro_expand;
  CPPOPTIONS (pfile)->put_out_comments = 1;

  /* Try to parse as much of the argument as exists at this
     input stack level.  */

  pfile->no_macro_expand = TRUE;

  for (;;)
    {
      token = cpplib_getToken (pfile);

      switch (token)
	{
	case CPP_EOF:
	  goto done;
	case CPP_POP:
	  /* If we've hit end of file, it's an error (reported by caller).
	     Ditto if it's the end of cpp_expand_to_buffer text.
	     If we've hit end of macro, just continue.  */
	  if (!cppBuffer_isMacro (CPPBUFFER (pfile)))
	    goto done;
	  /*@switchbreak@*/ break;
	case CPP_LPAREN:
	  paren++;
	  /*@switchbreak@*/ break;
	case CPP_RPAREN:
	  if (--paren < 0)
	    goto found;
	  /*@switchbreak@*/ break;
	case CPP_COMMA:
	  /* if we've returned to lowest level and
	     we aren't absorbing all args */
	  if (paren == 0 && rest_args == 0)
	    goto found;
	  /*@switchbreak@*/ break;
	found:
	  /* Remove ',' or ')' from argument buffer.  */
	  cppReader_adjustWritten (pfile, -1);
	  goto done;
	default:
	  ;
	}
    }

done:
  CPPOPTIONS (pfile)->put_out_comments = save_put_out_comments;
  pfile->no_macro_expand = oldexpand;

  return token;
}


/* Turn newlines to spaces in the string of length LENGTH at START,
   except inside of string constants.
   The string is copied into itself with its beginning staying fixed.  */

static int
change_newlines (char *start, int length)
{
  register char *ibp;
  register char *obp;
  register char *limit;
  char c;

  ibp = start;
  limit = start + length;
  obp = start;

  while (ibp < limit) {
    *obp++ = c = *ibp++;
    switch (c) {

    case '\'':
    case '\"':
      /* Notice and skip strings, so that we don't delete newlines in them.  */
      {
	char quotec = c;
	while (ibp < limit) {
	  *obp++ = c = *ibp++;
	  if (c == quotec)
	    /*@innerbreak@*/ break;
	  if (c == '\n' && quotec == '\'')
	    /*@innerbreak@*/ break;
	}
      }
    /*@switchbreak@*/ break;
    }
  }

  return obp - start;
}

static /*@observer@*/ struct tm *
timestamp (/*@returned@*/ cppReader *pfile)
{
  if (pfile->timebuf == NULL)
    {
      time_t t = time ((time_t *) 0);
      pfile->timebuf = localtime (&t);
    }

  llassert (pfile->timebuf != NULL);

  return pfile->timebuf;
}

static ob_mstring monthnames[] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
} ;

/*
 * expand things like __FILE__.  Place the expansion into the output
 * buffer *without* rescanning.
 */

static void
special_symbol (hashNode hp, cppReader *pfile)
{
  cstring buf = cstring_undefined;
  size_t len;
  int true_indepth;
  cppBuffer *ip;
  struct tm *timebuf;

  int paren = 0;		/* For special `defined' keyword */

  for (ip = cppReader_getBuffer (pfile); ip != NULL; ip = cppBuffer_prevBuffer (ip))
    {
      if (ip == cppReader_nullBuffer (pfile))
	{
	  cppReader_errorLit (pfile,
			cstring_makeLiteralTemp ("cccp error: not in any file?!"));
	  return;			/* the show must go on */
	}

      if (ip != NULL && ip->fname != NULL)
	{
	  break;
	}
    }

  switch (hp->type)
    {
    case T_FILE:
    case T_BASE_FILE:
      {
	char *string;
	if (hp->type == T_BASE_FILE)
	  {
	    while (cppBuffer_prevBuffer (ip) != cppReader_nullBuffer (pfile))
	      {
		ip = cppBuffer_prevBuffer (ip);
	      }
	  }

	llassert (ip != NULL);
	string = cstring_toCharsSafe (ip->nominal_fname);

	if (string == NULL)
	  {
	    string = "";
	  }

	cpplib_reserve (pfile, 3 + 4 * strlen (string));
	quote_string (pfile, string);
	return;
      }
    case T_FUNC: /* added in ISO C99 */
      {
	/* We don't know the actual name of the function, but it doesn't matter */
	char *string = "** function-name **";
	cpplib_reserve (pfile, 3 + 4 * strlen (string));
	quote_string (pfile, string);
	return;
      }

    case T_INCLUDE_LEVEL:
      true_indepth = 0;
      ip = cppReader_getBuffer (pfile);

      for (;  ip != cppReader_nullBuffer (pfile) && ip != NULL;
	   ip = cppBuffer_prevBuffer (ip))
	{
	  if (ip != NULL && ip->fname != NULL)
	    {
	      true_indepth++;
	    }
	}

      buf = message ("%d", true_indepth - 1);
      break;

    case T_VERSION:
      buf = cstring_makeLiteral ("\"--- cpp version---\"");
      break;

#ifndef NO_BUILTIN_SIZE_TYPE
    case T_SIZE_TYPE:
      buf = cstring_makeLiteral (SIZE_TYPE);
      break;
#endif

#ifndef NO_BUILTIN_PTRDIFF_TYPE
    case T_PTRDIFF_TYPE:
      buf = cstring_makeLiteral (PTRDIFF_TYPE);
      break;
#endif

    case T_WCHAR_TYPE:
      buf = cstring_makeLiteral (cppReader_wcharType (pfile));
      break;

    case T_USER_LABEL_PREFIX_TYPE:
      buf = cstring_makeLiteral (USER_LABEL_PREFIX);
      break;

    case T_REGISTER_PREFIX_TYPE:
      buf = cstring_makeLiteral (REGISTER_PREFIX);
      break;

    case T_CONST:
      buf = message ("%d", hp->value.ival);
      break;

    case T_SPECLINE:
      {
	if (ip != NULL)
	  {
	    int line = ip->lineno;
	    int col = ip->colno;

	    llassert (ip->cur != NULL);
	    adjust_position (cppLineBase (ip), ip->cur, &line, &col);

	    buf = message ("%d", (int) line);
	  }
	else
	  {
	    BADBRANCH;
	  }
      }
    break;

    case T_DATE:
    case T_TIME:
      {
	char *sbuf = (char *) dmalloc (20);
	timebuf = timestamp (pfile);
	if (hp->type == T_DATE)
	  {
	    sprintf (sbuf, "\"%s %2d %4d\"", monthnames[timebuf->tm_mon],
		     timebuf->tm_mday, timebuf->tm_year + 1900);
	  }
	else
	  {
	    sprintf (sbuf, "\"%02d:%02d:%02d\"", timebuf->tm_hour, timebuf->tm_min,
		     timebuf->tm_sec);
	  }

	buf = cstring_fromCharsNew (sbuf);
	sfree (sbuf);
	break;
      }

    case T_SPEC_DEFINED:
      buf = cstring_makeLiteral (" 0 ");     /* Assume symbol is not defined */
      ip = cppReader_getBuffer (pfile);
      llassert (ip != NULL);
      llassert (ip->cur != NULL);
      SKIP_WHITE_SPACE (ip->cur);

      if (*ip->cur == '(')
	{
	  paren++;
	  ip->cur++;			/* Skip over the paren */
	  SKIP_WHITE_SPACE (ip->cur);
	}

      if (!is_idstart[(int) *ip->cur])
	goto oops;
      if (ip->cur[0] == 'L' && (ip->cur[1] == '\'' || ip->cur[1] == '\"'))
	goto oops;

      if ((hp = cpphash_lookup (ip->cur, -1, -1)) != 0)
	{
	  cstring_free (buf);
	  buf = cstring_makeLiteral (" 1 ");
	}

      while (is_idchar[(int) *ip->cur])
	{
	  ++ip->cur;
	}

      SKIP_WHITE_SPACE (ip->cur);

      if (paren != 0)
	{
	  if (*ip->cur != ')')
	    goto oops;
	  ++ip->cur;
	}
      break;

    oops:

      cppReader_errorLit (pfile,
		    cstring_makeLiteralTemp ("`defined' without an identifier"));
      break;

    default:
      cpp_setLocation (pfile);
      llfatalerror (message ("Pre-processing error: invalid special hash type"));
    }

  len = cstring_length (buf);

  cpplib_reserve (pfile, len + 1);
  cppReader_putStrN (pfile, cstring_toCharsSafe (buf), len);
  cppReader_nullTerminateQ (pfile);

  cstring_free (buf);
  return;
}

/* Write out a #define command for the special named MACRO_NAME
   to PFILE's token_buffer.  */

static void
dump_special_to_buffer (cppReader *pfile, char *macro_name)
{
  static char define_directive[] = "#define ";
  size_t macro_name_length = strlen (macro_name);
  output_line_command (pfile, 0, same_file);
  cpplib_reserve (pfile, sizeof(define_directive) + macro_name_length);
  cppReader_putStrN (pfile, define_directive, sizeof(define_directive)-1);
  cppReader_putStrN (pfile, macro_name, macro_name_length);
  cppReader_putCharQ (pfile, ' ');
  cpp_expand_to_buffer (pfile, macro_name, macro_name_length);
  cppReader_putChar (pfile, '\n');
}

/* Initialize the built-in macros.  */

static void
cpplib_installBuiltin (/*@observer@*/ char *name, ctype ctyp,
		       int len, enum node_type type,
		       int ivalue, /*@null@*/ /*@only@*/ char *value,
		       int hash)
{
  cstring sname = cstring_fromCharsNew (name);

  llassert (usymtab_inGlobalScope ());

  /*
  ** Be careful here: this is done before the ctype table has
  ** been initialized.
  */

  if (!usymtab_exists (sname))
    {
      uentry ue = uentry_makeConstant (sname, ctyp, fileloc_createBuiltin ());

      if (ctype_equal (ctyp, ctype_string))
	{
	  qualList ql = qualList_new ();
	  ql = qualList_add (ql, qual_createObserver ());
	  uentry_reflectQualifiers (ue, ql);
	  qualList_free (ql);
	}
      
      usymtab_addGlobalEntry (ue);
    }
  else
    {
      ;
    }

  (void) cpphash_install (name, len, type, ivalue, value, hash);
  cstring_free (sname);
}

static void
cpplib_installBuiltinType (/*@observer@*/ char *name, ctype ctyp,
			   int len, enum node_type type,
			   int ivalue,
			   /*@only@*/ /*@null@*/ char *value, int hash)
{
  cstring sname = cstring_fromChars (name);
  /* evs 2000 07 10 - removed a memory leak, detected by splint */

  llassert (usymtab_inGlobalScope ());

  if (!usymtab_existsTypeEither (sname))
    {
      uentry ue = uentry_makeDatatype (sname, ctyp,
				       NO, qual_createConcrete (),
				       fileloc_createBuiltin ());
      llassert (!usymtab_existsEither (sname));
      usymtab_addGlobalEntry (ue);
    }

  (void) cpphash_install (name, len, type, ivalue, value, hash);
}

static void
initialize_builtins (cppReader *pfile)
{
  cpplib_installBuiltin ("__LINE__", ctype_int, -1, T_SPECLINE, 0, NULL, -1);
  cpplib_installBuiltin ("__DATE__", ctype_string, -1, T_DATE, 0, NULL, -1);
  cpplib_installBuiltin ("__FILE__", ctype_string, -1, T_FILE, 0, NULL, -1);
  cpplib_installBuiltin ("__BASE_FILE__", ctype_string, -1, T_BASE_FILE, 0, NULL, -1);
  cpplib_installBuiltin ("__func__", ctype_string, -1, T_FUNC, 0, NULL, -1);
  cpplib_installBuiltin ("__INCLUDE_LEVEL__", ctype_int, -1, T_INCLUDE_LEVEL, 0, NULL, -1);
  cpplib_installBuiltin ("__VERSION__", ctype_string, -1, T_VERSION, 0, NULL, -1);
#ifndef NO_BUILTIN_SIZE_TYPE
  cpplib_installBuiltinType ("__SIZE_TYPE__", ctype_anyintegral, -1, T_SIZE_TYPE, 0, NULL, -1);
#endif
#ifndef NO_BUILTIN_PTRDIFF_TYPE
  cpplib_installBuiltinType ("__PTRDIFF_TYPE__", ctype_anyintegral, -1, T_PTRDIFF_TYPE, 0, NULL, -1);
#endif
  cpplib_installBuiltinType ("__WCHAR_TYPE__", ctype_anyintegral, -1, T_WCHAR_TYPE, 0, NULL, -1);
  cpplib_installBuiltin ("__USER_LABEL_PREFIX__", ctype_string, -1, T_USER_LABEL_PREFIX_TYPE, 0, NULL, -1);
  cpplib_installBuiltin ("__REGISTER_PREFIX__", ctype_string, -1, T_REGISTER_PREFIX_TYPE, 0, NULL, -1);
  cpplib_installBuiltin ("__TIME__", ctype_string, -1, T_TIME, 0, NULL, -1);

  if (!cppReader_isTraditional (pfile))
    {
      cpplib_installBuiltin ("__STDC__", ctype_int, -1, T_CONST, STDC_VALUE, NULL, -1);
    }

# ifdef WIN32
    cpplib_installBuiltin ("_WIN32", ctype_int, -1, T_CONST, STDC_VALUE, NULL, -1);
# endif

  /*
  ** This is supplied using a -D by the compiler driver
  ** so that it is present only when truly compiling with GNU C.
  */

  /*  cpplib_install ("__GNUC__", -1, T_CONST, 2, 0, -1);  */

  cpplib_installBuiltin ("S_SPLINT_S", ctype_int, -1, T_CONST, 2, NULL, -1);
  cpplib_installBuiltin ("__LCLINT__", ctype_int, -1, T_CONST, 2, NULL, -1);

  /*drl 1/9/2001/ try to define the right symbol for the architecture
    We use autoconf to determine the target cpu 
   */
# ifndef S_SPLINT_S
  cpplib_installBuiltin ("__" TARGET_CPU, ctype_int, -1, T_CONST, 2, NULL, -1);
# endif

  /*drl 1/2/2002  set some flags based on uname
    I'd like to be able to do this with autoconf macro instead...
   */

  /*Thanks to Nelson Beebe for suggesting possible values for these */
  
  if (! strcmp (UNAME, "Linux"))
    {
#ifdef __ppc
      cpplib_installBuiltin ("__BIG_ENDIAN__", ctype_int, -1, T_CONST, 2, NULL, -1);
#endif
    }
  
  else if(! strcmp (UNAME, "Darwin"))
    {
      cpplib_installBuiltin ("__ppc__", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("__BIG_ENDIAN__", ctype_int, -1, T_CONST, 2, NULL, -1);
    }
  else if(! strcmp (UNAME, "HP-UX"))
    {
      cpplib_installBuiltin ("PWB", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("_HIUX_SOURCE", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("_HPUX_SOURCE", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("_PA_RISC1_1", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("__PWB", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("__PWB__", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("__STDC_EXT__", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("__hp9000s700", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("__hp9000s800", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("__hp9000s800__", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("__hp9k8", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("__hp9k8__", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("__hppa", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("__hppa__", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("__hpux", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("__hpux__", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("__unix", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("__unix__", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("hp9000s800", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("hp9k8", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("hppa", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("hpux", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("unix", ctype_int, -1, T_CONST, 2, NULL, -1);
    }
  else if(! strcmp (UNAME, "IRIX64"))
    {
      cpplib_installBuiltin ("LANGUAGE_C", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("MIPSEB", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("_ABIN32", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("_COMPILER_VERSION", ctype_int, -1, T_CONST, 730, NULL, -1);
      cpplib_installBuiltin ("_LANGUAGE_C", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("_LONGLONG", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("_MIPSEB", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("_MIPS_FPSET", ctype_int, -1, T_CONST, 32, NULL, -1);
      cpplib_installBuiltin ("_MIPS_ISA", ctype_int, -1, T_CONST, 3, NULL, -1);
      /*_MIPS_SIM=_ABIN32*/
      cpplib_installBuiltin ("_MIPS_SIM", ctype_int, -1, T_CONST, 2, NULL , -1);
      cpplib_installBuiltin ("_MIPS_SZINT", ctype_int, -1, T_CONST, 32, NULL, -1);
      cpplib_installBuiltin ("_MIPS_SZLONG", ctype_int, -1, T_CONST, 32, NULL, -1);
      cpplib_installBuiltin ("_MIPS_SZPTR", ctype_int, -1, T_CONST, 32, NULL, -1);
      cpplib_installBuiltin ("_MODERN_C", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("_PIC", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("_SGI_SOURCE", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("_SIZE_INT", ctype_int, -1, T_CONST, 32, NULL, -1);
      cpplib_installBuiltin ("_SIZE_LONG", ctype_int, -1, T_CONST, 32, NULL, -1);
      cpplib_installBuiltin ("_SIZE_PTR", ctype_int, -1, T_CONST, 32, NULL, -1);
      cpplib_installBuiltin ("_SVR4_SOURCE", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("_SYSTYPE_SVR4", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("__DSO__", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("__EXTENSIONS__", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("__INLINE_INTRINSICS", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("__MATH_HAS_NO_SIDE_EFFECTS", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("__host_mips", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("__mips", ctype_int, -1, T_CONST, 3, NULL, -1);
      cpplib_installBuiltin ("__sgi", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("__unix", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("host_mips", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("mips", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("sgi", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("unix", ctype_int, -1, T_CONST, 2, NULL, -1);
    }
  else if(! strcmp (UNAME, "OSF1"))
    {
      cpplib_installBuiltin ("__alpha", ctype_int, -1, T_CONST, 2, NULL, -1);
    }
  else if (!strcmp (UNAME, "Rhapsody"))
    {
      cpplib_installBuiltin ("__ppc__", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("__BIG_ENDIAN__", ctype_int, -1, T_CONST, 2, NULL, -1);
    }

  else if (!strcmp (UNAME, "SunOS"))
    {
      cpplib_installBuiltin ("__OPEN_MAX", ctype_int, -1, T_CONST, 20, NULL, -1);
      cpplib_installBuiltin ("__STDC__", ctype_int, -1, T_CONST, 2, NULL, -1);
      cpplib_installBuiltin ("__sparc", ctype_int, -1, T_CONST, 2, NULL, -1);
      /*       This define  "-Dfileno(f)=0" should be inserted but we're going to stick to deinfe constants for now...*/
    }
  else
    {
      /*
	types which we have not explictedly handled.
	AIX, FreeBSD, IRIX, Mach
       */

    }
  
  if (CPPOPTIONS (pfile)->debug_output)
    {
      dump_special_to_buffer (pfile, "__BASE_FILE__");
      dump_special_to_buffer (pfile, "__VERSION__");
#ifndef NO_BUILTIN_SIZE_TYPE
      dump_special_to_buffer (pfile, "__SIZE_TYPE__");
#endif
#ifndef NO_BUILTIN_PTRDIFF_TYPE
      dump_special_to_buffer (pfile, "__PTRDIFF_TYPE__");
#endif
      dump_special_to_buffer (pfile, "__WCHAR_TYPE__");
      dump_special_to_buffer (pfile, "__DATE__");
      dump_special_to_buffer (pfile, "__TIME__");
      if (!cppReader_isTraditional (pfile))
	dump_special_to_buffer (pfile, "__STDC__");
    }
}


/* Return 1 iff a token ending in C1 followed directly by a token C2
   could cause mis-tokenization.  */

static bool
unsafe_chars (char c1, char c2)
{
  switch (c1)
    {
    case '+': case '-':
      if (c2 == c1 || c2 == '=')
	return 1;
      goto letter;
    case '.':
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
    case 'e': case 'E': case 'p': case 'P':
      if (c2 == '-' || c2 == '+')
	return 1; /* could extend a pre-processing number */
      goto letter;
    case 'L':
      if (c2 == '\'' || c2 == '\"')
	return 1;   /* Could turn into L"xxx" or L'xxx'.  */
      goto letter;
    letter:
    case '_':
    case 'a': case 'b': case 'c': case 'd':           case 'f':
    case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
    case 'm': case 'n': case 'o':           case 'q': case 'r':
    case 's': case 't': case 'u': case 'v': case 'w': case 'x':
    case 'y': case 'z':
    case 'A': case 'B': case 'C': case 'D':           case 'F':
    case 'G': case 'H': case 'I': case 'J': case 'K':
    case 'M': case 'N': case 'O':           case 'Q': case 'R':
    case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
    case 'Y': case 'Z':
      /* We're in the middle of either a name or a pre-processing number.  */
      return (is_idchar[(int) c2] || c2 == '.');
    case '<': case '>': case '!': case '%': case '#': case ':':
    case '^': case '&': case '|': case '*': case '/': case '=':
      return (c2 == c1 || c2 == '=');
    }
  return 0;
}

/* Expand a macro call.
   HP points to the symbol that is the macro being called.
   Put the result of expansion onto the input stack
   so that subsequent input by our caller will use it.

   If macro wants arguments, caller has already verified that
   an argument list follows; arguments come from the input stack.  */

static void
cpplib_macroExpand (cppReader *pfile, /*@dependent@*/ hashNode hp)
{
  int nargs;
  DEFINITION *defn = hp->value.defn;
  char *xbuf;
  char *oxbuf = NULL;
  int start_line;
  int start_column;
  int end_line;
  int end_column;
  size_t xbuf_len;
  size_t old_written = cpplib_getWritten (pfile);
  int rest_args;
  int rest_zero = 0;
  int i;
  struct argdata *args = NULL;

  pfile->output_escapes++;
  cppBuffer_getLineAndColumn (cppReader_fileBuffer (pfile), &start_line, &start_column);
  DPRINTF (("Expand macro: %d:%d", start_line, start_column));

  nargs = defn->nargs;

  if (nargs >= 0)
    {
      enum cpp_token token = CPP_EOF;

      args = (struct argdata *) dmalloc ((nargs + 1) * sizeof (*args));

      for (i = 0; i < nargs; i++)
	{
	  args[i].expanded = 0;
	  args[i].raw = 0;
	  args[i].raw_length = 0;
	  args[i].expand_length = args[i].stringified_length = -1;
	  args[i].use_count = 0;
	}

      /*
      ** Parse all the macro args that are supplied.  I counts them.
      ** The first NARGS args are stored in ARGS.
      ** The rest are discarded.  If rest_args is set then we assume
      ** macarg absorbed the rest of the args.
      */

      i = 0;
      rest_args = 0;

      cppReader_forward (pfile, 1); /* Discard the open-parenthesis before the first arg.  */
      do
	{
	  if (rest_args != 0)
	    {
	      continue;
	    }

	  if (i < nargs || (nargs == 0 && i == 0))
	    {
	      /* if we are working on last arg which absorbs rest of args... */
	      if (i == nargs - 1 && defn->rest_args)
		{
		  rest_args = 1;
		}

	      args[i].raw = size_toLong (cpplib_getWritten (pfile));
	      token = macarg (pfile, rest_args);
	      args[i].raw_length = size_toInt (cpplib_getWritten (pfile) - args[i].raw);
	      args[i].newlines = FALSE; /* FIXME */
	    }
	  else
	    {
	      token = macarg (pfile, 0);
	    }

	  if (token == CPP_EOF || token == CPP_POP)
	    {
	      cppReader_errorWithLine (pfile, start_line, start_column,
				   cstring_fromCharsNew ("unterminated macro call"));
	      sfree (args);
	      return;
	    }
	  i++;
	} while (token == CPP_COMMA);

      /* If we got one arg but it was just whitespace, call that 0 args.  */
      if (i == 1)
	{
	  char *bp;
	  char *lim;

	  assertSet (args);

	  bp = ARG_BASE + args[0].raw;
	  lim = bp + args[0].raw_length;

	  /* cpp.texi says for foo ( ) we provide one argument.
	     However, if foo wants just 0 arguments, treat this as 0.  */

	  if (nargs == 0)
	    {
	      while (bp != lim && is_space[(int) *bp])
		{
		  bp++;
		}
	    }

	  if (bp == lim)
	    i = 0;
	}

      /* Don't output an error message if we have already output one for
	 a parse error above.  */
      rest_zero = 0;

      if (nargs == 0 && i > 0)
	{
	  cppReader_error (pfile,
		     message ("arguments given to macro `%s'", hp->name));
	}
      else if (i < nargs)
	{
	  /* traditional C allows foo() if foo wants one argument.  */
	  if (nargs == 1 && i == 0 && cppReader_isTraditional (pfile))
	    {
	      ;
	    }
	  /* the rest args token is allowed to absorb 0 tokens */
	  else if (i == nargs - 1 && defn->rest_args)
	    rest_zero = 1;
	  else if (i == 0)
	    cppReader_error (pfile,
		       message ("macro `%s' used without args", hp->name));
	  else if (i == 1)
	    cppReader_error (pfile,
		       message ("macro `%s' used with just one arg", hp->name));
	  else
	    {
	      cppReader_error (pfile,
			 message ("macro `%s' used with only %d args",
				  hp->name, i));
	    }
	}
      else if (i > nargs)
	{
	  cppReader_error (pfile,
		     message ("macro `%s' used with too many (%d) args", hp->name, i));
	}
      else
	{
	  ;
	}
    }

  /*
  ** If the agrument list was multiple lines, need to insert new lines to keep line
  ** numbers accurate.
  */

  cppBuffer_getLineAndColumn (cppReader_fileBuffer (pfile), &end_line, &end_column);
  DPRINTF (("Expand macro: %d:%d", end_line, end_column));
  
  /* If macro wants zero args, we parsed the arglist for checking only.
     Read directly from the macro definition.  */

  if (nargs <= 0)
    {
      xbuf = defn->expansion;
      xbuf_len = defn->length;
    }
  else
    {
      char *exp = defn->expansion;
      int offset;	/* offset in expansion,
				   copied a piece at a time */
      size_t totlen;	/* total amount of exp buffer filled so far */

      register struct reflist *ap, *last_ap;
      
      assertSet (args); /* args is defined since the nargs > 0 path was taken */

      /* Macro really takes args.  Compute the expansion of this call.  */

      /* Compute length in characters of the macro's expansion.
	 Also count number of times each arg is used.  */
      xbuf_len = defn->length;

      llassert (args != NULL);

      for (ap = defn->pattern; ap != NULL; ap = ap->next)
	{
	  if (ap->stringify)
	    {
	      struct argdata *arg = &args[ap->argno];

	      /* Stringify it it hasn't already been */
	      assertSet (arg);

	      if (arg->stringified_length < 0)
		{
		  int arglen = arg->raw_length;
		  bool escaped = FALSE;
		  char in_string = '\0';
		  char c;

		  /* Initially need_space is -1.  Otherwise, 1 means the
		     previous character was a space, but we suppressed it;
		     0 means the previous character was a non-space.  */
		  int need_space = -1;

		  i = 0;
		  arg->stringified = cpplib_getWritten (pfile);
		  if (!cppReader_isTraditional (pfile))
		    cppReader_putChar (pfile, '\"'); /* insert beginning quote */
		  for (; i < arglen; i++)
		    {
		      c = (ARG_BASE + arg->raw)[i];

		      if (in_string == '\0')
			{
			  /* Internal sequences of whitespace are replaced by
			     one space except within an string or char token.*/
			  if (is_space[(int) c])
			    {
			      if (cpplib_getWritten (pfile) > arg->stringified
				  && (cpplib_getPWritten (pfile))[-1] == '@')
				{
				  /* "@ " escape markers are removed */
				  cppReader_adjustWritten (pfile, -1);
				  /*@innercontinue@*/ continue;
				}
			      if (need_space == 0)
				need_space = 1;
			      /*@innercontinue@*/ continue;
			    }
			  else if (need_space > 0)
			    cppReader_putChar (pfile, ' ');
			  else
			    {
			      ;
			    }

			  need_space = 0;
			}

		      if (escaped)
			escaped = 0;
		      else
			{
			  if (c == '\\')
			    escaped = 1;

			  if (in_string != '\0')
			    {
			      if (c == in_string)
				in_string = '\0';
			    }
			  else if (c == '\"' || c == '\'')
			    {
			      in_string = c;
			    }
			  else
			    {
			      ;
			    }
			}

		      /* Escape these chars */
		      if (c == '\"' || (in_string != '\0' && c == '\\'))
			cppReader_putChar (pfile, '\\');
		      if (isprint (c))
			cppReader_putChar (pfile, c);
		      else
			{
			  cpplib_reserve (pfile, 4);
			  sprintf (cpplib_getPWritten (pfile), "\\%03o",
				   (unsigned int) c);
			  cppReader_adjustWritten (pfile, 4);
			}
		    }
		  if (!cppReader_isTraditional (pfile))
		    cppReader_putChar (pfile, '\"'); /* insert ending quote */
		  arg->stringified_length
		    = size_toInt (cpplib_getWritten (pfile) - arg->stringified);
		}

	      xbuf_len += args[ap->argno].stringified_length;
	    }
	  else if (ap->raw_before || ap->raw_after || cppReader_isTraditional (pfile))
	    {
	      /* Add 4 for two newline-space markers to prevent token concatenation. */
	      assertSet (args); /* Splint shouldn't need this */
	      xbuf_len += args[ap->argno].raw_length + 4;
	    }
	  else
	    {
	      /* We have an ordinary (expanded) occurrence of the arg.
		 So compute its expansion, if we have not already.  */

	      assertSet (args); /* shouldn't need this */

	      if (args[ap->argno].expand_length < 0)
		{
		  args[ap->argno].expanded = cpplib_getWritten (pfile);
		  cpp_expand_to_buffer (pfile,
					ARG_BASE + args[ap->argno].raw,
					size_fromInt (args[ap->argno].raw_length));

		  args[ap->argno].expand_length
		    = size_toInt (cpplib_getWritten (pfile) - args[ap->argno].expanded);
		}

	      /* Add 4 for two newline-space markers to prevent
		 token concatenation.  */
	      xbuf_len += args[ap->argno].expand_length + 4;
 	   }
	  if (args[ap->argno].use_count < 10)
	    args[ap->argno].use_count++;
	}

      xbuf = (char *) dmalloc (xbuf_len + 1);
      oxbuf = xbuf;

      /*
      ** Generate in XBUF the complete expansion
      ** with arguments substituted in.
      ** TOTLEN is the total size generated so far.
      ** OFFSET is the index in the definition
      ** of where we are copying from.
      */

      offset = 0;
      totlen = 0;

      for (last_ap = NULL, ap = defn->pattern; ap != NULL;
	   last_ap = ap, ap = ap->next)
	{
	  register struct argdata *arg = &args[ap->argno];
	  size_t count_before = totlen;

	  /* Add chars to XBUF.  */
	  for (i = 0; i < ap->nchars; i++, offset++)
	    {
	      xbuf[totlen++] = exp[offset];
	    }

	  /* If followed by an empty rest arg with concatenation,
	     delete the last run of nonwhite chars.  */
	  if (rest_zero && totlen > count_before
	      && ((ap->rest_args && ap->raw_before)
		  || (last_ap != NULL && last_ap->rest_args
		      && last_ap->raw_after)))
	    {
	      /* Delete final whitespace.  */
	      while (totlen > count_before && is_space[(int) xbuf[totlen - 1]])
		{
		  totlen--;
		}

	      /* Delete the nonwhites before them.  */
	      while (totlen > count_before && ! is_space[(int) xbuf[totlen - 1]])
		{
		  totlen--;
		}
	    }

	  if (ap->stringify != 0)
	    {
	      assertSet(arg);
	      memcpy (xbuf + totlen,
		      ARG_BASE + arg->stringified,
		      size_fromInt (arg->stringified_length));
	      totlen += arg->stringified_length;
	    }
	  else if (ap->raw_before || ap->raw_after || cppReader_isTraditional (pfile))
	    {
	      char *p1;
	      char *l1;

	      assertSet (arg);

	      p1 = ARG_BASE + arg->raw;
	      l1 = p1 + arg->raw_length;

	      if (ap->raw_before)
		{
		  while (p1 != l1 && is_space[(int) *p1])
		    {
		      p1++;
		    }

		  while (p1 != l1 && is_idchar[(int) *p1])
		    {
		      xbuf[totlen++] = *p1++;
		    }

		  /* Delete any no-reexpansion marker that follows
		     an identifier at the beginning of the argument
		     if the argument is concatenated with what precedes it.  */
		  if (p1[0] == '@' && p1[1] == '-')
		    p1 += 2;
		}
	      if (ap->raw_after)
		{
		  /* Arg is concatenated after: delete trailing whitespace,
		     whitespace markers, and no-reexpansion markers.  */
		  while (p1 != l1)
		    {
		      if (is_space[(int) l1[-1]]) l1--;
		      else if (l1[-1] == '-')
			{
			  char *p2 = l1 - 1;
			  /* If a `-' is preceded by an odd number of newlines then it
			     and the last newline are a no-reexpansion marker.  */
			  while (p2 != p1 && p2[-1] == '\n')
			    {
			      p2--;
			    }

			  if (((l1 - 1 - p2) & 1) != 0)
			    {
			      l1 -= 2;
			    }
			  else
			    {
			      /*@innerbreak@*/ break;
			    }
			}
		      else
			{
			  /*@innerbreak@*/ break;
			}
		    }
		}

	      memcpy (xbuf + totlen, p1, size_fromInt (l1 - p1));
	      totlen += l1 - p1;
	    }
	  else
	    {
	      char *expanded;

	      assertSet (arg);
	      expanded = ARG_BASE + arg->expanded;

	      if (!ap->raw_before && totlen > 0
		  && (arg->expand_length != 0)
		  && !cppReader_isTraditional(pfile)
		  && unsafe_chars (xbuf[totlen-1], expanded[0]))
		{
		  xbuf[totlen++] = '@';
		  xbuf[totlen++] = ' ';
		}

	      memcpy (xbuf + totlen, expanded,
		      size_fromInt (arg->expand_length));
	      totlen += arg->expand_length;

	      if (!ap->raw_after && totlen > 0
		  && offset < size_toInt (defn->length)
		  && !cppReader_isTraditional(pfile)
		  && unsafe_chars (xbuf[totlen-1], exp[offset]))
		{
		  xbuf[totlen++] = '@';
		  xbuf[totlen++] = ' ';
		}

	      /* If a macro argument with newlines is used multiple times,
		 then only expand the newlines once.  This avoids creating
		 output lines which don't correspond to any input line,
		 which confuses gdb and gcov.  */
	      if (arg->use_count > 1 && arg->newlines > 0)
		{
		  /* Don't bother doing change_newlines for subsequent
		     uses of arg.  */
		  arg->use_count = 1;
		  arg->expand_length
		    = change_newlines (expanded, arg->expand_length);
		}
	    }

	  if (totlen > xbuf_len)
	    abort ();
	}

      /* if there is anything left of the definition
	 after handling the arg list, copy that in too.  */

      for (i = offset; i < size_toInt (defn->length); i++)
	{
	  /* if we've reached the end of the macro */
	  if (exp[i] == ')')
	    rest_zero = 0;
	  if (! (rest_zero && last_ap != NULL && last_ap->rest_args
		 && last_ap->raw_after))
	    xbuf[totlen++] = exp[i];
	}

      xbuf[totlen] = '\0';
      xbuf_len = totlen;
    }

  pfile->output_escapes--;

  /* Now put the expansion on the input stack
     so our caller will commence reading from it.  */
  DPRINTF (("Pushing expansion: %s", cstring_copyLength (xbuf, xbuf_len)));
  
  if (end_line != start_line)
    {
      /* xbuf must have enough newlines */
      int newlines = end_line - start_line;
      int foundnewlines = 0;
      char *xbufptr = xbuf;

      while ((xbufptr = strchr (xbufptr, '\n')) != NULL && foundnewlines <= newlines)
	{
	  foundnewlines++;
	  xbufptr++;

	  if (*xbufptr == '\0') 
	    {
	      break;
	    }
	}
	  
      if (foundnewlines < newlines)
	{
	  cstring newbuf = cstring_copyLength (xbuf, xbuf_len);
	  
	  while (foundnewlines < newlines)
	    {
	      newbuf = cstring_appendChar (newbuf, '\n');
	      foundnewlines++;
	    }

	  sfree (oxbuf);
	  xbuf = cstring_toCharsSafe (newbuf);
	  xbuf_len = cstring_length (newbuf);
	  /*@-branchstate@*/ 
	} /*@=branchstate@*/ 
    }
  
  DPRINTF (("Pushing expansion: %s", cstring_copyLength (xbuf, xbuf_len)));

  push_macro_expansion (pfile, xbuf, xbuf_len, hp);
  DPRINTF (("After pushing expansion: %s", cstring_copyLength (xbuf, xbuf_len)));
  cppReader_getBufferSafe (pfile)->has_escapes = 1;

  /* Pop the space we've used in the token_buffer for argument expansion.  */
  cppReader_setWritten (pfile, old_written);
  DPRINTF (("Done set written"));
  
  /* Recursive macro use sometimes works traditionally.
     #define foo(x,y) bar (x (y,0), y)
     foo (foo, baz)  */

  if (!cppReader_isTraditional (pfile))
    hp->type = T_DISABLED;

  sfree (args);
}

static void
push_macro_expansion (cppReader *pfile, char *xbuf, size_t xbuf_len,
		      /*@dependent@*/ hashNode hp)
{
  cppBuffer *mbuf = cppReader_pushBuffer (pfile, xbuf, xbuf_len);

  if (mbuf == NULL)
    {
      return;
    }

  mbuf->cleanup = cppReader_macroCleanup;

  llassert (mbuf->hnode == NULL);
  mbuf->hnode = hp;

  /* The first chars of the expansion should be a "@ " added by
     collect_expansion.  This is to prevent accidental token-pasting
     between the text preceding the macro invocation, and the macro
     expansion text.

     We would like to avoid adding unneeded spaces (for the sake of
     tools that use cpp, such as imake).  In some common cases we can
     tell that it is safe to omit the space.

     The character before the macro invocation cannot have been an
     idchar (or else it would have been pasted with the idchars of
     the macro name).  Therefore, if the first non-space character
     of the expansion is an idchar, we do not need the extra space
     to prevent token pasting.

     Also, we don't need the extra space if the first char is '(',
     or some other (less common) characters.  */

  if (xbuf[0] == '@' && xbuf[1] == ' '
      && (is_idchar[(int) xbuf[2]] || xbuf[2] == '(' || xbuf[2] == '\''
	  || xbuf[2] == '\"'))
  {
    llassert (mbuf->cur != NULL);
    DPRINTF (("Eating: %c", xbuf[2]));
    mbuf->cur += 2;
  }
  
}


/* Like cpplib_getToken, except that it does not read past end-of-line.
   Also, horizontal space is skipped, and macros are popped.  */

static enum cpp_token
get_directive_token (cppReader *pfile)
{
  for (;;)
    {
      size_t old_written = cpplib_getWritten (pfile);
      enum cpp_token token;
      cppSkipHspace (pfile);
      if (cppReader_peekC (pfile) == '\n')
	{
	  return CPP_VSPACE;
	}

      token = cpplib_getToken (pfile);

      switch (token)
	{
	case CPP_POP:
	  if (!cppBuffer_isMacro (cppReader_getBuffer (pfile)))
	    return token;
	  /*@fallthrough@*/
	case CPP_HSPACE:
	case CPP_COMMENT:
	  cppReader_setWritten (pfile, old_written);
	  /*@switchbreak@*/ break;
	default:
	  return token;
	}
    }
}


/* Handle #include and #import.
   This function expects to see "fname" or <fname> on the input.

   The input is normally in part of the output_buffer following
   cpplib_getWritten, and will get overwritten by output_line_command.
   I.e. in input file specification has been popped by cppReader_handleDirective.
   This is safe.  */

static int
do_include (cppReader *pfile, struct directive *keyword,
	    /*@unused@*/ char *unused1, /*@unused@*/ char *unused2)
{
  bool skip_dirs = (keyword->type == T_INCLUDE_NEXT);
  cstring fname;
  char *fbeg, *fend;		/* Beginning and end of fname */
  enum cpp_token token;

  /* Chain of dirs to search */
  struct file_name_list *search_start = CPPOPTIONS (pfile)->include;
  struct file_name_list dsp[1];	/* First in chain, if #include "..." */
  struct file_name_list *searchptr = NULL;
  size_t old_written = cpplib_getWritten (pfile);
  size_t flen;

  int f;			/* file number */
  int angle_brackets = 0;	/* 0 for "...", 1 for <...> */
  f= -1;			/* JF we iz paranoid! */

  pfile->parsing_include_directive++;
  token = get_directive_token (pfile);
  pfile->parsing_include_directive--;

  if (token == CPP_STRING)
    {
      /* FIXME - check no trailing garbage */
      fbeg = pfile->token_buffer + old_written + 1;
      fend = cpplib_getPWritten (pfile) - 1;
      if (fbeg[-1] == '<')
	{
	  angle_brackets = 1;
	  /* If -I-, start with the first -I dir after the -I-.  */
	  if (CPPOPTIONS (pfile)->first_bracket_include != NULL)
	    search_start = CPPOPTIONS (pfile)->first_bracket_include;
	}
      /* If -I- was specified, don't search current dir, only spec'd ones.  */
      else if (!CPPOPTIONS (pfile)->ignore_srcdir)
	{
	  cppBuffer *fp = CPPBUFFER (pfile);
	  /* We have "filename".  Figure out directory this source
	     file is coming from and put it on the front of the list.  */

	  for ( ; fp != cppReader_nullBuffer (pfile); fp = cppBuffer_prevBuffer (fp))
	    {
	      int n;
	      char *ep,*nam;

	      llassert (fp != NULL);

	      nam = NULL;

	      if (cstring_isDefined (fp->nominal_fname))
		{
		  nam = cstring_toCharsSafe (fp->nominal_fname);

		  /* Found a named file.  Figure out dir of the file,
		     and put it in front of the search list.  */
		  dsp[0].next = search_start;
		  search_start = dsp;

#ifndef VMS
		  ep = strrchr (nam, CONNECTCHAR);
#else				/* VMS */
		  ep = strrchr (nam, ']');
		  if (ep == NULL) ep = strrchr (nam, '>');
		  if (ep == NULL) ep = strrchr (nam, ':');
		  if (ep != NULL) ep++;
#endif				/* VMS */
		  if (ep != NULL)
		    {
		      char save;

		      n = ep - nam;
		      save = nam[n];
		      nam[n] = '\0';

		      /*@-onlytrans@*/ /* This looks like a memory leak... */ 
		      dsp[0].fname = cstring_fromCharsNew (nam); /* evs 2000-07-20: was fromChars */
		      /*@=onlytrans@*/
		      nam[n] = save;

		      if (n + INCLUDE_LEN_FUDGE > size_toInt (pfile->max_include_len))
			pfile->max_include_len = size_fromInt (n + INCLUDE_LEN_FUDGE);
		    }
		  else
		    {
		      dsp[0].fname = cstring_undefined; /* Current directory */
		    }

		  dsp[0].got_name_map = 0;
		  break;
		}
	    }
	}
      else
	{
	  ;
	}
    }
#ifdef VMS
  else if (token == CPP_NAME)
    {
      /*
       * Support '#include xyz' like VAX-C to allow for easy use of all the
       * decwindow include files. It defaults to '#include <xyz.h>' (so the
       * code from case '<' is repeated here) and generates a warning.
       */
      cppReader_warning (pfile,
		   "VAX-C-style include specification found, use '#include <filename.h>' !");
      angle_brackets = 1;
      /* If -I-, start with the first -I dir after the -I-.  */
      if (CPPOPTIONS (pfile)->first_bracket_include)
	search_start = CPPOPTIONS (pfile)->first_bracket_include;
      fbeg = pfile->token_buffer + old_written;
      fend = cpplib_getPWritten (pfile);
    }
#endif
  else
    {
      cppReader_error (pfile,
		 message ("Preprocessor command #%s expects \"FILENAME\" or <FILENAME>",
			  keyword->name));

      cppReader_setWritten (pfile, old_written);
      cppReader_skipRestOfLine (pfile);
      return 0;
    }

  *fend = 0;

  token = get_directive_token (pfile);
  if (token != CPP_VSPACE)
    {
      cppReader_errorLit (pfile,
		    cstring_makeLiteralTemp ("Junk at end of #include"));

      while (token != CPP_VSPACE && token != CPP_EOF && token != CPP_POP)
	{
	  token = get_directive_token (pfile);
	}
    }

  /*
  ** For #include_next, skip in the search path
  ** past the dir in which the containing file was found.
  */

  if (skip_dirs)
    {
      cppBuffer *fp = CPPBUFFER (pfile);

      for (; fp != cppReader_nullBuffer (pfile); fp = cppBuffer_prevBuffer (fp))
	{
	  llassert (fp != NULL);

	  if (fp->fname != NULL)
	    {
	      /* fp->dir is null if the containing file was specified with
		 an absolute file name.  In that case, don't skip anything.  */
	      if (fp->dir == SELF_DIR_DUMMY)
		{
		  search_start = CPPOPTIONS (pfile)->include;
		}
	      else if (fp->dir != NULL)
		{
		  search_start = fp->dir->next;
		}
	      else
		{
		  ;
		}

	      break;
	    }
	}
    }
  
  cppReader_setWritten (pfile, old_written);

  flen = size_fromInt (fend - fbeg);

  DPRINTF (("fbeg: %s", fbeg));

  if (flen == 0)
    {
      cppReader_error (pfile,
		 message ("Empty file name in #%s", keyword->name));
      return 0;
    }

  /*
  ** Allocate this permanently, because it gets stored in the definitions
  ** of macros.
  */

  fname = cstring_undefined;

  /* + 2 above for slash and terminating null.  */
  /* + 2 added for '.h' on VMS (to support '#include filename') */

  /* If specified file name is absolute, just open it.  */

  if (osd_isConnectChar (*fbeg)
# if defined (WIN32) || defined (OS2)
      || (*(fbeg + 1) == ':')
# endif
      )
    {
      fname = cstring_copyLength (fbeg, flen);
      
      if (redundant_include_p (pfile, fname))
	{
	  cstring_free (fname);
	  return 0;
	}
      
      f = open_include_file (pfile, fname, NULL);
      
      if (f == IMPORT_FOUND)
	{
	  return 0;		/* Already included this file */
	}
    } 
  else 
    {
      /* Search directory path, trying to open the file.
	 Copy each filename tried into FNAME.  */

      for (searchptr = search_start; searchptr != NULL;
	   searchptr = searchptr->next)
	{
	  if (!cstring_isEmpty (searchptr->fname))
	    {
	      /* The empty string in a search path is ignored.
		 This makes it possible to turn off entirely
		 a standard piece of the list.  */
	      if (cstring_isEmpty (searchptr->fname))
		continue;
	      
	      fname = cstring_copy (searchptr->fname);
	      fname = cstring_appendChar (fname, CONNECTCHAR);
	      DPRINTF (("Here: %s", fname));
	    }
	  else
	    {
	      ;
	    }
	  
	  fname = cstring_concatLength (fname, fbeg, flen);

	  DPRINTF (("fname: %s", fname));
	  
	  /* Win32 directory fix from Kay Buschner. */
#if defined (WIN32) || defined (OS2)
	  /* Fix all unixdir slashes to win dir slashes */
	  if (searchptr->fname && (searchptr->fname[0] != 0)) 
	    {
	      cstring_replaceAll (fname, '/', '\\');
	    }
#endif /* WIN32 */

#ifdef VMS
	  /* Change this 1/2 Unix 1/2 VMS file specification into a
	     full VMS file specification */
	  if (searchptr->fname && (searchptr->fname[0] != 0)) {
	    /* Fix up the filename */
	    hack_vms_include_specification (fname);
	  } else {
	    /* This is a normal VMS filespec, so use it unchanged.  */
	    strncpy (fname, fbeg, flen);
	    fname[flen] = 0;
	    /* if it's '#include filename', add the missing .h */
	    if (strchr (fname,'.') == NULL) {
	      strcat (fname, ".h");
	    }
	  }
#endif /* VMS */
	  /* ??? There are currently 3 separate mechanisms for avoiding processing
	     of redundant include files: #import, #pragma once, and
	     redundant_include_p.  It would be nice if they were unified.  */
	  
	  if (redundant_include_p (pfile, fname))
	    {
	      cstring_free (fname);
	      return 0;
	    }

	  DPRINTF (("Trying: %s", fname));

	  f = open_include_file (pfile, fname, searchptr);
	  
	  if (f == IMPORT_FOUND)
	    {
	      return 0; /* Already included this file */
	    }
#ifdef EACCES
	  else if (f == IMPORT_NOT_FOUND && errno == EACCES)
	    {
	      cppReader_warning (pfile,
				 message ("Header file %s exists, but is not readable", fname));
	    }
#endif
	  
	  if (f >= 0)
	    {
	      break;
	    }
	}
    }
  
  if (f < 0)
    {
      /* A file that was not found.  */
      fname = cstring_copyLength (fbeg, flen);

      if (search_start != NULL)
	{
	  cppReader_error (pfile,
			   message ("Cannot find include file %s on search path: %x", 
				    fname,
				    searchPath_unparse (search_start)));
	}
      else
	{
	  cppReader_error (pfile,
			   message ("No include path in which to find %s", fname));
	}
    }
  else {
    /*
    ** Check to see if this include file is a once-only include file.
    ** If so, give up.
    */

    struct file_name_list *ptr;

    for (ptr = pfile->all_include_files; ptr != NULL; ptr = ptr->next)
      {
	if (cstring_equal (ptr->fname, fname))
	  {
	    /* This file was included before.  */
	    break;
	  }
      }

    if (ptr == NULL)
      {
	/* This is the first time for this file.  */
	/* Add it to list of files included.  */

	ptr = (struct file_name_list *) dmalloc (sizeof (*ptr));
	ptr->control_macro = NULL;
	ptr->c_system_include_path = NULL;
	ptr->next = pfile->all_include_files;
	ptr->fname = fname;
	ptr->got_name_map = NULL;

	DPRINTF (("Including file: %s", fname));
	pfile->all_include_files = ptr;
	assertSet (pfile->all_include_files);
      }

    if (angle_brackets != 0)
      {
	pfile->system_include_depth++;
      }

    /* Actually process the file */
    if (cppReader_pushBuffer (pfile, NULL, 0) == NULL)
      {
	cstring_free (fname);
	return 0;
      }

    if (finclude (pfile, f, fname, is_system_include (pfile, fname),
		  searchptr != dsp ? searchptr : SELF_DIR_DUMMY))
      {
	output_line_command (pfile, 0, enter_file);
	pfile->only_seen_white = 2;
      }

    if (angle_brackets)
      {
	pfile->system_include_depth--;
      }
    /*@-branchstate@*/
  } /*@=branchstate@*/ 

  return 0;
}

/* Return nonzero if there is no need to include file NAME
   because it has already been included and it contains a conditional
   to make a repeated include do nothing.  */

static bool
redundant_include_p (cppReader *pfile, cstring name)
{
  struct file_name_list *l = pfile->all_include_files;

  for (; l != NULL; l = l->next)
    {
      if (cstring_equal (name, l->fname)
	  && (l->control_macro != NULL)
	  && (cpphash_lookup (l->control_macro, -1, -1) != NULL))
	{
	  return TRUE;
	}
    }

  return FALSE;
}

/* Return nonzero if the given FILENAME is an absolute pathname which
   designates a file within one of the known "system" include file
   directories.  We assume here that if the given FILENAME looks like
   it is the name of a file which resides either directly in a "system"
   include file directory, or within any subdirectory thereof, then the
   given file must be a "system" include file.  This function tells us
   if we should suppress pedantic errors/warnings for the given FILENAME.

   The value is 2 if the file is a C-language system header file
   for which C++ should (on most systems) assume `extern "C"'.  */

static bool
is_system_include (cppReader *pfile, cstring filename)
{
  struct file_name_list *searchptr;

  for (searchptr = CPPOPTIONS (pfile)->first_system_include;
       searchptr != NULL;
       searchptr = searchptr->next)
    {
      if (!cstring_isEmpty (searchptr->fname)) 
	{
	  cstring sys_dir = searchptr->fname;
	  size_t length = cstring_length (sys_dir);
	  
	  if (cstring_equalLen (sys_dir, filename, length)
	      && osd_isConnectChar (cstring_getChar (filename, length)))
	    {
	      if (searchptr->c_system_include_path)
		return 2;
	      else
		return 1;
	    }
	}
    }
  
  return 0;
}

/* Convert a character string literal into a nul-terminated string.
   The input string is [IN ... LIMIT).
   The result is placed in RESULT.  RESULT can be the same as IN.
   The value returned in the end of the string written to RESULT,
   or NULL on error.  */

static /*@null@*/ char *
convert_string (cppReader *pfile, /*@returned@*/ char *result,
		char *in, char *limit, int handle_escapes)
{
  char c;
  c = *in++;

  if (c != '\"')
    {
      return NULL;
    }

  while (in < limit)
    {
      c = *in++;

      switch (c)
	{
	case '\0':
	  return NULL;
	case '\"':
	  limit = in;
	  /*@switchbreak@*/ break;
	case '\\':
	  if (handle_escapes)
	    {
	      char *bpc = (char *) in;
	      int i = (char) cppReader_parseEscape (pfile, &bpc);
	      in = (char *) bpc;
	      if (i >= 0)
		*result++ = (char) c;
	      /*@switchbreak@*/ break;
	    }

	  /*@fallthrough@*/
	default:
	  *result++ = c;
	}
    }

  *result = 0;
  return result;
}

/*
 * interpret #line command.  Remembers previously seen fnames
 * in its very own hash table.
 */

/*@constant int FNAME_HASHSIZE@*/
#define FNAME_HASHSIZE 37

static int
do_line (cppReader *pfile, /*@unused@*/ struct directive *keyword)
{
  cppBuffer *ip = cppReader_getBuffer (pfile);
  int new_lineno;
  size_t old_written = cpplib_getWritten (pfile);
  enum file_change_code file_change = same_file;
  enum cpp_token token;

  llassert (ip != NULL);
  token = get_directive_token (pfile);

  if (token != CPP_NUMBER
      || !isdigit(pfile->token_buffer[old_written]))
    {
      cppReader_errorLit (pfile,
		    cstring_makeLiteralTemp ("invalid format `#line' command"));

      goto bad_line_directive;
    }

  /* The Newline at the end of this line remains to be processed.
     To put the next line at the specified line number,
     we must store a line number now that is one less.  */
  new_lineno = atoi (pfile->token_buffer + old_written) - 1;
  cppReader_setWritten (pfile, old_written);

  /* NEW_LINENO is one less than the actual line number here.  */
  if (cppReader_isPedantic (pfile) && new_lineno < 0)
    cppReader_pedwarnLit (pfile,
		    cstring_makeLiteralTemp ("line number out of range in `#line' command"));

  token = get_directive_token (pfile);

  if (token == CPP_STRING) {
    char *fname = pfile->token_buffer + old_written;
    char *end_name;
    static hashNode fname_table[FNAME_HASHSIZE];
    hashNode hp; 
    hashNode *hash_bucket;
    char *p;
    size_t num_start;
    size_t fname_length;

    /* Turn the file name, which is a character string literal,
       into a null-terminated string.  Do this in place.  */
    end_name = convert_string (pfile, fname, fname, cpplib_getPWritten (pfile), 1);
    if (end_name == NULL)
      {
	cppReader_errorLit (pfile,
		      cstring_makeLiteralTemp ("invalid format `#line' command"));
	goto bad_line_directive;
      }

    fname_length = size_fromInt (end_name - fname);
    num_start = cpplib_getWritten (pfile);

    token = get_directive_token (pfile);
    if (token != CPP_VSPACE && token != CPP_EOF && token != CPP_POP) {
      p = pfile->token_buffer + num_start;
      if (cppReader_isPedantic (pfile))
	cppReader_pedwarnLit (pfile,
			cstring_makeLiteralTemp ("garbage at end of `#line' command"));

      if (token != CPP_NUMBER || *p < '0' || *p > '4' || p[1] != '\0')
	{
	  cppReader_errorLit (pfile,
			cstring_makeLiteralTemp ("invalid format `#line' command"));
	  goto bad_line_directive;
	}
      if (*p == '1')
	file_change = enter_file;
      else if (*p == 2)
	file_change = leave_file;
      else if (*p == 3)
	ip->system_header_p = 1;
      else /* if (*p == 4) */
	ip->system_header_p = 2;

      cppReader_setWritten (pfile, num_start);
      token = get_directive_token (pfile);
      p = pfile->token_buffer + num_start;
      if (token == CPP_NUMBER && p[1] == '\0' && (*p == '3' || *p== '4')) {
	ip->system_header_p = *p == 3 ? 1 : 2;
	token = get_directive_token (pfile);
      }
      if (token != CPP_VSPACE) {
	cppReader_errorLit (pfile,
		      cstring_makeLiteralTemp ("invalid format `#line' command"));

	goto bad_line_directive;
      }
    }

    hash_bucket =
      &fname_table[cpphash_hashCode (fname, fname_length, FNAME_HASHSIZE)];

    for (hp = *hash_bucket; hp != NULL; hp = hp->next)
      {
	if (hp->length == fname_length)
	  {
	    llassert (hp->value.cpval != NULL);
	    
	    if (strncmp (hp->value.cpval, fname, fname_length) == 0) 
	      {
		ip->nominal_fname = cstring_fromChars (hp->value.cpval);
		break;
	      }
	  }
      }
    
    if (hp == 0) {
      /* Didn't find it; cons up a new one.  */
      hp = (hashNode) dmalloc (sizeof (*hp));

      hp->prev = NULL;
      hp->bucket_hdr = NULL;
      hp->type = T_NONE;
      hp->name = cstring_undefined;
      hp->next = *hash_bucket;

      *hash_bucket = hp;

      hp->length = fname_length;
      hp->value.cpval = dmalloc (sizeof (*hp->value.cpval) * (fname_length + 1));
      memcpy (hp->value.cpval, fname, fname_length);
      hp->value.cpval[fname_length] = '\0';
      ip->nominal_fname = cstring_fromChars (hp->value.cpval);
    }
  }
  else if (token != CPP_VSPACE && token != CPP_EOF)
    {
      cppReader_errorLit (pfile,
		    cstring_makeLiteralTemp ("invalid format `#line' command"));
      goto bad_line_directive;
    }
  else
    {
      ;
    }

  ip->lineno = new_lineno;
bad_line_directive:
  cppReader_skipRestOfLine (pfile);
  cppReader_setWritten (pfile, old_written);
  output_line_command (pfile, 0, file_change);
  return 0;
}

/*
 * remove the definition of a symbol from the symbol table.
 * according to un*x /lib/cpp, it is not an error to undef
 * something that has no definitions, so it isn't one here either.
 */

static int
do_undef (cppReader *pfile, struct directive *keyword, char *buf, char *limit)
{

  size_t sym_length;
  hashNode hp;
  char *orig_buf = buf;

  SKIP_WHITE_SPACE (buf);

  sym_length = cppReader_checkMacroName (pfile, buf, cstring_makeLiteralTemp ("macro"));

  while ((hp = cpphash_lookup (buf, size_toInt (sym_length), -1)) != NULL)
    {
      /* If we are generating additional info for debugging (with -g) we
	 need to pass through all effective #undef commands.  */
      if (CPPOPTIONS (pfile)->debug_output && (keyword != NULL))
	{
	  pass_thru_directive (orig_buf, limit, pfile, keyword);
	}

      if (hp->type != T_MACRO)
	{
	  cppReader_warning (pfile,
		       message ("Undefining preprocessor builtin: %s",
				hp->name));
	}

      cppReader_deleteMacro (hp);
    }

  if (cppReader_isPedantic (pfile)) {
    buf += sym_length;
    SKIP_WHITE_SPACE (buf);
    if (buf != limit)
      {
	cppReader_pedwarnLit (pfile,
			cstring_makeLiteralTemp ("garbage after `#undef' directive"));
      }
  }

  return 0;
}


/*
 * Report an error detected by the program we are processing.
 * Use the text of the line in the error message.
 * (We use error because it prints the filename & line#.)
 */

static int
do_error (cppReader *pfile, /*@unused@*/ struct directive *keyword,
	  char *buf, char *limit)
{
  size_t length = size_fromInt (limit - buf);
  cstring copy = cstring_copyLength (buf, length);
  cstring adv = cstring_advanceWhiteSpace (copy);

  cppReader_error (pfile, message ("#error %s", adv));
  cstring_free (copy);
  return 0;
}

/*
 * Report a warning detected by the program we are processing.
 * Use the text of the line in the warning message, then continue.
 * (We use error because it prints the filename & line#.)
 */

static int
do_warning (cppReader *pfile, /*@unused@*/ struct directive *keyword,
	    char *buf, char *limit)
{
  size_t length = size_fromInt (limit - buf);
  cstring copy = cstring_copyLength (buf, length);
  cstring adv = cstring_advanceWhiteSpace (copy);
  cppReader_warning (pfile, message ("#warning %s", adv));
  cstring_free (copy);
  return 0;
}


/* #ident has already been copied to the output file, so just ignore it.  */

static int
do_ident (cppReader *pfile, /*@unused@*/ struct directive *keyword,
	  /*@unused@*/ char *buf, /*@unused@*/ char *limit)
{
  /* Allow #ident in system headers, since that's not user's fault.  */
  if (cppReader_isPedantic (pfile) 
      && !cppReader_getBufferSafe (pfile)->system_header_p)
    cppReader_pedwarnLit (pfile,
		    cstring_makeLiteralTemp ("ANSI C does not allow `#ident'"));

  /* Leave rest of line to be read by later calls to cpplib_getToken.  */

  return 0;
}

/* #pragma and its argument line have already been copied to the output file.
   Just check for some recognized pragmas that need validation here.  */

static int
do_pragma (cppReader *pfile, /*@unused@*/ struct directive *keyword,
	   /*@unused@*/ char *buf, /*@unused@*/ char *limit)
{
  while (*buf == ' ' || *buf == '\t')
    {
      buf++;
    }

  if (!strncmp (buf, "implementation", 14)) {
    /* Be quiet about `#pragma implementation' for a file only if it hasn't
       been included yet.  */
    struct file_name_list *ptr;
    char *p = buf + 14, *fname, *inc_fname;
    size_t fname_len;
    SKIP_WHITE_SPACE (p);
    if (*p == '\n' || *p != '\"')
      return 0;

    fname = p + 1;
    p = (char *) strchr (fname, '\"');
    fname_len = p != NULL ? size_fromInt (p - fname) : mstring_length (fname);

    for (ptr = pfile->all_include_files; ptr != NULL; ptr = ptr->next)
      {
	inc_fname = (char *) strrchr (cstring_toCharsSafe (ptr->fname), CONNECTCHAR);
	inc_fname = (inc_fname != NULL)
	  ? inc_fname + 1 : cstring_toCharsSafe (ptr->fname);

	if ((inc_fname != NULL)
	    && (strncmp (inc_fname, fname, fname_len) == 0))
	  {
	    cpp_setLocation (pfile);

	    ppllerror (message ("`#pragma implementation' for `%s' appears "
				"after file is included",
				cstring_fromChars (fname)));
	  }
      }
  }

  return 0;
}

/*
 * handle #if command by
 *   1) inserting special `defined' keyword into the hash table
 *	that gets turned into 0 or 1 by special_symbol (thus,
 *	if the luser has a symbol called `defined' already, it won't
 *      work inside the #if command)
 *   2) rescan the input into a temporary output buffer
 *   3) pass the output buffer to the yacc parser and collect a value
 *   4) clean up the mess left from steps 1 and 2.
 *   5) call conditional_skip to skip til the next #endif (etc.),
 *      or not, depending on the value from step 3.
 */

static int
do_if (cppReader *pfile, /*@unused@*/ struct directive *keyword,
       char *buf, char *limit)
{
  HOST_WIDE_INT value;
  DPRINTF (("Do if: %s", buf));
  value = eval_if_expression (pfile, buf, limit - buf);
  conditional_skip (pfile, value == 0, T_IF, NULL);
  return 0;
}

/*
 * handle a #elif directive by not changing  if_stack  either.
 * see the comment above do_else.
 */

static int do_elif (cppReader *pfile, /*@unused@*/ struct directive *keyword,
		    char *buf, char *limit)
{
  if (pfile->if_stack == cppReader_getBufferSafe (pfile)->if_stack)
    {
      cppReader_errorLit (pfile,
		    cstring_makeLiteralTemp ("Preprocessor command #elif is not within a conditional"));
      return 0;
    }
  else
    {
      llassert (pfile->if_stack != NULL);

      if (pfile->if_stack->type != T_IF && pfile->if_stack->type != T_ELIF)
	{
	  cppReader_errorLit (pfile,
			cstring_makeLiteralTemp ("`#elif' after `#else'"));

	  if (pfile->if_stack->fname != NULL
	      && cppReader_getBufferSafe (pfile)->fname != NULL
	      && !cstring_equal (pfile->if_stack->fname,
				 cppReader_getBufferSafe (pfile)->nominal_fname))
	    fprintf (stderr, ", file %s", cstring_toCharsSafe (pfile->if_stack->fname));
	  fprintf (stderr, ")\n");
	}
      pfile->if_stack->type = T_ELIF;
    }

  if (pfile->if_stack->if_succeeded)
    {
      skip_if_group (pfile, 0);
    }
  else
    {
      HOST_WIDE_INT value = eval_if_expression (pfile, buf, limit - buf);
      if (value == 0)
	skip_if_group (pfile, 0);
      else
	{
	  ++pfile->if_stack->if_succeeded;	/* continue processing input */
	  output_line_command (pfile, 1, same_file);
	}
    }

  return 0;
}

/*
 * evaluate a #if expression in BUF, of length LENGTH,
 * then parse the result as a C expression and return the value as an int.
 */

static HOST_WIDE_INT
eval_if_expression (cppReader *pfile,
		    /*@unused@*/ char *buf,
		    /*@unused@*/ int length)
{
  hashNode save_defined;
  HOST_WIDE_INT value;
  size_t old_written = cpplib_getWritten (pfile);

  DPRINTF (("Saving defined..."));
  save_defined = cpphash_install ("defined", -1, T_SPEC_DEFINED, 0, 0, -1);
  pfile->pcp_inside_if = 1;

  value = cppReader_parseExpression (pfile);
  pfile->pcp_inside_if = 0;

  /* Clean up special symbol */
  DPRINTF (("Removing defined..."));
  cppReader_deleteMacro (save_defined);
  cppReader_setWritten (pfile, old_written); /* Pop */

  return value;
}

/*
 * routine to handle ifdef/ifndef.  Try to look up the symbol,
 * then do or don't skip to the #endif/#else/#elif depending
 * on what directive is actually being processed.
 */

static int
do_xifdef (cppReader *pfile, struct directive *keyword,
	   /*@unused@*/ char *unused1, /*@unused@*/ char *unused2)
{
  int skip;
  cppBuffer *ip = cppReader_getBufferSafe (pfile);
  char *ident;
  size_t ident_length;
  enum cpp_token token;
  int start_of_file = 0;
  char *control_macro = 0;
  size_t old_written = cpplib_getWritten (pfile);

  DPRINTF (("do xifdef: %d",
	    keyword->type == T_IFNDEF));

  /* Detect a #ifndef at start of file (not counting comments).  */
  if (cstring_isDefined (ip->fname) && keyword->type == T_IFNDEF)
    {
      start_of_file = pfile->only_seen_white == 2;
    }

  pfile->no_macro_expand++;
  token = get_directive_token (pfile);
  pfile->no_macro_expand--;

  ident = pfile->token_buffer + old_written;
  DPRINTF (("Ident: %s", ident));

  ident_length = cpplib_getWritten (pfile) - old_written;
  cppReader_setWritten (pfile, old_written); /* Pop */

  if (token == CPP_VSPACE || token == CPP_POP || token == CPP_EOF)
    {
      skip = (keyword->type == T_IFDEF);
      if (! cppReader_isTraditional (pfile))
	{
	  cppReader_pedwarn (pfile,
			     message ("`#%s' with no argument", keyword->name));
	}
    }
  else if (token == CPP_NAME)
    {
      hashNode hp = cpphash_lookup (ident, size_toInt (ident_length), -1);

      skip = (keyword->type == T_IFDEF) ? (hp == NULL) : (hp != NULL);
      
      DPRINTF (("hp null: %d / %d / %d", hp == NULL, keyword->type == T_IFNDEF, skip));
      
      if (start_of_file && !skip)
	{
	  DPRINTF (("Not skipping!"));
	  control_macro = (char *) dmalloc (ident_length + 1);
	  memcpy (control_macro, ident, ident_length + 1);
	}
    }
  else
    {
      skip = (keyword->type == T_IFDEF);
      if (! cppReader_isTraditional (pfile))
	{
	  cppReader_error (pfile,
		     message ("`#%s' with invalid argument", keyword->name));
	}
    }

  if (!cppReader_isTraditional (pfile))
    {
      int c;
      cppSkipHspace (pfile);
      c = cppReader_peekC (pfile);
      if (c != EOF && c != '\n')
	{
	  cppReader_pedwarn (pfile,
			     message ("garbage at end of `#%s' argument", keyword->name));
	}
    }

  cppReader_skipRestOfLine (pfile);

  DPRINTF (("Conditional skip: %d", skip));
  conditional_skip (pfile, skip, T_IF, control_macro);
  return 0;
}

/* Push TYPE on stack; then, if SKIP is nonzero, skip ahead.
   If this is a #ifndef starting at the beginning of a file,
   CONTROL_MACRO is the macro name tested by the #ifndef.
   Otherwise, CONTROL_MACRO is 0.  */

static void
conditional_skip (cppReader *pfile, int skip,
		  enum node_type type,
		  /*@dependent@*/ char *control_macro)
{
  cppIfStackFrame *temp = (cppIfStackFrame *) dmalloc (sizeof (*temp));

  temp->fname = cppReader_getBufferSafe (pfile)->nominal_fname;
  temp->next = pfile->if_stack;
  temp->control_macro = control_macro;
  temp->lineno = 0;
  temp->if_succeeded = 0;

  pfile->if_stack = temp;
  pfile->if_stack->type = type;

  if (skip != 0)
    {
      skip_if_group (pfile, 0);
      return;
    }
  else
    {
      ++pfile->if_stack->if_succeeded;
      output_line_command (pfile, 1, same_file);
    }
}

/*
 * skip to #endif, #else, or #elif.  adjust line numbers, etc.
 * leaves input ptr at the sharp sign found.
 * If ANY is nonzero, return at next directive of any sort.
 */

static void
skip_if_group (cppReader *pfile, int any)
{
  int c;
  struct directive *kt;
  cppIfStackFrame *save_if_stack = pfile->if_stack; /* don't pop past here */
  register int ident_length;
  char *ident;
  struct parse_marker line_start_mark;

  parseSetMark (&line_start_mark, pfile);

  if (CPPOPTIONS (pfile)->output_conditionals) {
    static char failed[] = "#failed\n";
    cppReader_puts (pfile, failed, sizeof(failed)-1);
    pfile->lineno++;
    output_line_command (pfile, 1, same_file);
  }

beg_of_line:
  if (CPPOPTIONS (pfile)->output_conditionals)
    {
      cppBuffer *pbuf = cppReader_getBufferSafe (pfile);
      char *start_line;

      llassert (pbuf->buf != NULL);

      start_line = pbuf->buf + line_start_mark.position;
      cppReader_puts (pfile, start_line, size_fromInt (pbuf->cur - start_line));
    }

  parseMoveMark (&line_start_mark, pfile);

  if (!cppReader_isTraditional (pfile))
    {
      cppSkipHspace (pfile);
    }

  c  = cppReader_getC (pfile);
  if (c == '#')
    {
      size_t old_written = cpplib_getWritten (pfile);
      cppSkipHspace (pfile);

      parse_name (pfile, cppReader_getC (pfile));
      ident_length = size_toInt (cpplib_getWritten (pfile) - old_written);
      ident = pfile->token_buffer + old_written;
      pfile->limit = ident;

      for (kt = directive_table; kt->length >= 0; kt++)
	{
	  cppIfStackFrame *temp;
	  if (ident_length == kt->length
	      && cstring_equalPrefix (kt->name, cstring_fromChars (ident)))
	    {
	      /* If we are asked to return on next directive, do so now.  */
	      if (any)
		{
		  goto done;
		}

	      switch (kt->type)
		{
		case T_IF:
		case T_IFDEF:
		case T_IFNDEF:
		  temp = (cppIfStackFrame *) dmalloc (sizeof (*temp));
		  temp->next = pfile->if_stack;
		  temp->fname = cppReader_getBufferSafe (pfile)->nominal_fname;
		  temp->type = kt->type;
		  temp->lineno = 0;
		  temp->if_succeeded = 0;
		  temp->control_macro = NULL;

		  pfile->if_stack = temp;
		  /*@switchbreak@*/ break;
		case T_ELSE:
		case T_ENDIF:
		  if (cppReader_isPedantic (pfile) && pfile->if_stack != save_if_stack)
		    validate_else (pfile,
				   cstring_makeLiteralTemp (kt->type == T_ELSE ? "#else" : "#endif"));
		  /*@fallthrough@*/
		case T_ELIF:
		  if (pfile->if_stack == cppReader_getBufferSafe (pfile)->if_stack)
		    {
		      cppReader_error (pfile,
				 message ("Preprocessor command #%s is not within a conditional", kt->name));
		      /*@switchbreak@*/ break;
		    }
		  else if (pfile->if_stack == save_if_stack)
		    {
		      goto done;		/* found what we came for */
		    }
		  else
		    {
		      ;
		    }

		  if (kt->type != T_ENDIF)
		    {
		      llassert (pfile->if_stack != NULL);

		      if (pfile->if_stack->type == T_ELSE)
			{
			  cppReader_errorLit (pfile,
					cstring_makeLiteralTemp ("`#else' or `#elif' after `#else'"));
			}

		      pfile->if_stack->type = kt->type;
		      /*@switchbreak@*/ break;
		    }

		  temp = pfile->if_stack;
		  llassert (temp != NULL);
		  pfile->if_stack = temp->next;
		  sfree (temp);
		  /*@switchbreak@*/ break;
		default: ;
		  /*@-branchstate@*/ 
#if defined (OS2) && defined (__IBMC__)
      /* Dummy code to eleminate optimization problems with icc */
      c = 0;
# endif

		}
	      /*@=branchstate@*/
	      break;
	    }
	  
	  /* Don't let erroneous code go by.  */
	  
	  if (kt->length < 0 && !CPPOPTIONS (pfile)->lang_asm
	      && cppReader_isPedantic (pfile))
	    {
	      cppReader_pedwarnLit (pfile,
				    cstring_makeLiteralTemp ("Invalid preprocessor directive name"));
	    }
	}

      c = cppReader_getC (pfile);
    }
  /* We're in the middle of a line.  Skip the rest of it.  */
  for (;;) {
    size_t old;

    switch (c)
      {
      case EOF:
	goto done;
      case '/':			/* possible comment */
	c = skip_comment (pfile, NULL);
	if (c == EOF)
	  goto done;
	/*@switchbreak@*/ break;
      case '\"':
      case '\'':
	cppReader_forward (pfile, -1);
	old = cpplib_getWritten (pfile);
	(void) cpplib_getToken (pfile);
	cppReader_setWritten (pfile, old);
	/*@switchbreak@*/ break;
      case '\\':
	/* Char after backslash loses its special meaning.  */
	if (cppReader_peekC (pfile) == '\n')
	  {
	    cppReader_forward (pfile, 1);
	  }

	/*@switchbreak@*/ break;
      case '\n':
	goto beg_of_line;
      }
    c = cppReader_getC (pfile);
  }
done:
  if (CPPOPTIONS (pfile)->output_conditionals) {
    static char end_failed[] = "#endfailed\n";
    cppReader_puts (pfile, end_failed, sizeof(end_failed)-1);
    pfile->lineno++;
  }
  pfile->only_seen_white = 1;

  parseGotoMark (&line_start_mark, pfile);
  parseClearMark (&line_start_mark);
}

/*
 * handle a #else directive.  Do this by just continuing processing
 * without changing  if_stack ;  this is so that the error message
 * for missing #endif's etc. will point to the original #if.  It
 * is possible that something different would be better.
 */

static int
do_else (cppReader *pfile, /*@unused@*/ struct directive *keyword,
	/*@unused@*/ char *buf, /*@unused@*/ char *limit)
{
  if (cppReader_isPedantic (pfile))
    {
      validate_else (pfile, cstring_makeLiteralTemp ("#else"));
    }

  cppReader_skipRestOfLine (pfile);

  if (pfile->if_stack == cppReader_getBufferSafe (pfile)->if_stack) {
    cppReader_errorLit (pfile,
		  cstring_makeLiteralTemp ("Preprocessor command #else is not within a conditional"));
    return 0;
  } else {
    /* #ifndef can't have its special treatment for containing the whole file
       if it has a #else clause.  */

    llassert (pfile->if_stack != NULL);

    pfile->if_stack->control_macro = 0;

    if (pfile->if_stack->type != T_IF && pfile->if_stack->type != T_ELIF)
      {
	cpp_setLocation (pfile);
	genppllerrorhint (FLG_PREPROC,
			  message ("Pre-processor directive #else after #else"),
			  message ("%q: Location of match",
				   fileloc_unparseRaw (pfile->if_stack->fname,
						       pfile->if_stack->lineno)));
      }

    pfile->if_stack->type = T_ELSE;
  }

  if (pfile->if_stack->if_succeeded)
    skip_if_group (pfile, 0);
  else {
    ++pfile->if_stack->if_succeeded;	/* continue processing input */
    output_line_command (pfile, 1, same_file);
  }

  return 0;
}

/*
 * unstack after #endif command
 */

static int
do_endif (cppReader *pfile, /*@unused@*/ struct directive *keyword,
	  /*@unused@*/ char *buf, /*@unused@*/ char *limit)
{
  if (cppReader_isPedantic (pfile))
    {
      validate_else (pfile, cstring_makeLiteralTemp ("#endif"));
    }

  cppReader_skipRestOfLine (pfile);

  if (pfile->if_stack == cppReader_getBufferSafe (pfile)->if_stack)
    {
      cppReader_errorLit (pfile, cstring_makeLiteralTemp ("Unbalanced #endif"));
    }
  else
    {
      cppIfStackFrame *temp = pfile->if_stack;

      llassert (temp != NULL);

      pfile->if_stack = temp->next;
      if (temp->control_macro != 0)
	{
	  /* This #endif matched a #ifndef at the start of the file.
	     See if it is at the end of the file.  */
	  struct parse_marker start_mark;
	  int c;

	  parseSetMark (&start_mark, pfile);

	  for (;;)
	    {
	      cppSkipHspace (pfile);
	      c = cppReader_getC (pfile);

	      if (c != '\n')
		break;
	    }

	  parseGotoMark (&start_mark, pfile);
	  parseClearMark (&start_mark);

	  if (c == EOF)
	    {
	      /* If we get here, this #endif ends a #ifndef
		 that contains all of the file (aside from whitespace).
		 Arrange not to include the file again
		 if the macro that was tested is defined.

		 Do not do this for the top-level file in a -include or any
		 file in a -imacros.  */
	      struct file_name_list *ifile = pfile->all_include_files;

	      for ( ; ifile != NULL; ifile = ifile->next)
		{
		  if (cstring_equal (ifile->fname, cppReader_getBufferSafe (pfile)->fname))
		    {
		      ifile->control_macro = temp->control_macro;
		      break;
		    }
		}
	    }
	}

      sfree (temp);
      output_line_command (pfile, 1, same_file);
    }
  return 0;
}

/* When an #else or #endif is found while skipping failed conditional,
   if -pedantic was specified, this is called to warn about text after
   the command name.  P points to the first char after the command name.  */

static void
validate_else (cppReader *pfile, cstring directive)
{
  int c;
  cppSkipHspace (pfile);
  c = cppReader_peekC (pfile);
  if (c != EOF && c != '\n')
    {
      cppReader_pedwarn (pfile,
		   message ("text following `%s' violates ANSI standard", directive));
    }
}

/*
** Get the next token, and add it to the text in pfile->token_buffer.
** Return the kind of token we got.
*/

enum cpp_token
cpplib_getToken (cppReader *pfile)
{
  return cpplib_getTokenAux (pfile, FALSE);
}

enum cpp_token
cpplib_getTokenForceExpand (cppReader *pfile)
{
  return cpplib_getTokenAux (pfile, TRUE);
}

enum cpp_token
cpplib_getTokenAux (cppReader *pfile, bool forceExpand)
{
  int c, c2, c3;
  size_t old_written = 0;
  int start_line, start_column;
  enum cpp_token token;
  struct cppOptions *opts = CPPOPTIONS (pfile);
  cppReader_getBufferSafe (pfile)->prev = cppReader_getBufferSafe (pfile)->cur;

get_next:
  c = cppReader_getC (pfile);
  DPRINTF (("Get next token: %c", c));

  if (c == EOF)
    {
    handle_eof:
      if (cppReader_getBufferSafe (pfile)->seen_eof)
	{
	  cppBuffer *buf = cppReader_popBuffer (pfile);

	  if (buf != cppReader_nullBuffer (pfile))
	    {
	      goto get_next;
	    }
	  else
	    {
	      return CPP_EOF;
	    }
	}
      else
	{
	  cppBuffer *next_buf = cppBuffer_prevBuffer (cppReader_getBufferSafe (pfile));
	  cppReader_getBufferSafe (pfile)->seen_eof = 1;

	  if (cstring_isDefined (cppReader_getBufferSafe (pfile)->nominal_fname)
	      && next_buf != cppReader_nullBuffer (pfile))
	    {
	      /* We're about to return from an #include file.
		 Emit #line information now (as part of the CPP_POP) result.
		 But the #line refers to the file we will pop to.  */
	      cppBuffer *cur_buffer = CPPBUFFER (pfile);
	      CPPBUFFER (pfile) = next_buf;
	      pfile->input_stack_listing_current = 0;
	      output_line_command (pfile, 0, leave_file);
	      CPPBUFFER (pfile) = cur_buffer;
	    }
	  return CPP_POP;
	}
    }
  else
    {
      long newlines;
      struct parse_marker start_mark;

      switch (c)
	{
	case '/':
	  if (cppReader_peekC (pfile) == '=')
	    {
	      goto op2;
	    }

	  if (opts->put_out_comments)
	    {
	      parseSetMark (&start_mark, pfile);
	    }

	  newlines = 0;
	  cppBuffer_getLineAndColumn (cppReader_fileBuffer (pfile),
				   &start_line, &start_column);
	  c = skip_comment (pfile, &newlines);
	  DPRINTF (("c = %c", c));
	  if (opts->put_out_comments && (c == '/' || c == EOF))
	    {
	      assertSet (&start_mark);
	      parseClearMark (&start_mark);
	    }

	  if (c == '/')
	    goto randomchar;
	  if (c == EOF)
	    {
	      cppReader_errorWithLine (pfile, start_line, start_column,
				       cstring_makeLiteral ("Unterminated comment"));
	      goto handle_eof;
	    }
	  c = '/';  /* Initial letter of comment.  */
	return_comment:
	  /* Comments are equivalent to spaces.
	     For -traditional, a comment is equivalent to nothing.  */

	  if (opts->put_out_comments)
	    {
	      enum cpp_token res;

	      assertSet (&start_mark);
	      res = cpp_handleComment (pfile, &start_mark);
	      pfile->lineno += newlines;
	      return res;
	    }
	  else if (cppReader_isTraditional (pfile))
	    {
	      return CPP_COMMENT;
	    }
	  else
	    {
	      cpplib_reserve(pfile, 1);
	      cppReader_putCharQ (pfile, ' ');
	      return CPP_HSPACE;
	    }

	case '#':
	  if (!pfile->only_seen_white)
	    {
	      goto randomchar;
	    }

	  if (cppReader_handleDirective (pfile))
	    {
	      return CPP_DIRECTIVE;
	    }

	  pfile->only_seen_white = 0;
	  return CPP_OTHER;

	case '\"':
	case '\'':
	  /* A single quoted string is treated like a double -- some
	     programs (e.g., troff) are perverse this way */
	  cppBuffer_getLineAndColumn (cppReader_fileBuffer (pfile),
				      &start_line, &start_column);
	  old_written = cpplib_getWritten (pfile);
	string:
	  DPRINTF (("Reading string: %c", c));
	  cppReader_putChar (pfile, c);
	  while (TRUE)
	    {
	      /* evans-2003-06-07
	      ** Because of ISO8859-1 characters in string literals, we need a special test here.
	      */

	      if (cppReader_reachedEOF (pfile)) 
		{
		  
		  DPRINTF (("Matches EOF!"));
		  if (cppBuffer_isMacro (CPPBUFFER (pfile)))
		    {
		      /* try harder: this string crosses a macro expansion
			 boundary.  This can happen naturally if -traditional.
			 Otherwise, only -D can make a macro with an unmatched
			 quote.  */
		      cppBuffer *next_buf
			= cppBuffer_prevBuffer (cppReader_getBufferSafe (pfile));
		      (*cppReader_getBufferSafe (pfile)->cleanup)
			(cppReader_getBufferSafe (pfile), pfile);
		      CPPBUFFER (pfile) = next_buf;
		      continue;
		    }
		  
		  if (!cppReader_isTraditional (pfile))
		    {
		      cpp_setLocation (pfile);

		      setLine (long_toInt (start_line));
		      setColumn (long_toInt (start_column));
		      
		      if (pfile->multiline_string_line != long_toInt (start_line)
			  && pfile->multiline_string_line != 0)
			{
			  genppllerrorhint
			    (FLG_PREPROC,
			     message ("Unterminated string or character constant"),
			     message ("%q: Possible real start of unterminated constant",
				      fileloc_unparseRaw 
				      (fileloc_filename (g_currentloc),
				       pfile->multiline_string_line)));
			  pfile->multiline_string_line = 0;
			}
		      else
			{
			  genppllerror
			    (FLG_PREPROC,
			     message ("Unterminated string or character constant"));
			}
		    }
		  /*@loopbreak@*/ break;		  
		} 
	      else
		{
		  int cc = cppReader_getC (pfile); 
		  DPRINTF (("cc: %c [%d] [%d]", cc, cc, EOF));
		  DPRINTF (("putting char: %c", cc));
		  cppReader_putChar (pfile, cc);
		  switch (cc)
		    {
		    case '\n':
		      /* Traditionally, end of line ends a string constant with
			 no error.  So exit the loop and record the new line.  */
		      if (cppReader_isTraditional (pfile))
			goto while2end;
		      if (c == '\'')
			{
			  goto while2end;
			}
		      if (cppReader_isPedantic (pfile)
			  && pfile->multiline_string_line == 0)
			{
			  cppReader_pedwarnWithLine
			    (pfile, long_toInt (start_line),
			     long_toInt (start_column),
			     cstring_makeLiteral ("String constant runs past end of line"));
			}
		      if (pfile->multiline_string_line == 0)
			{
			  pfile->multiline_string_line = start_line;
			}
		      
		      /*@switchbreak@*/ break;
		      
		    case '\\':
		      cc = cppReader_getC (pfile);
		      if (cc == '\n')
			{
			  /* Backslash newline is replaced by nothing at all.  */
			  pfile->lineno++; /* 2003-11-03: AMiller suggested adding this, but
					      its not clear why it is needed. */
			  cppReader_adjustWritten (pfile, -1);
			  pfile->lineno++;
			}
		      else
			{
			  /* ANSI stupidly requires that in \\ the second \
			     is *not* prevented from combining with a newline.  */
			  NEWLINE_FIX1(cc);
			  if (cc != EOF)
			    cppReader_putChar (pfile, cc);
			}
		      /*@switchbreak@*/ break;
		      
		    case '\"':
		    case '\'':
		      if (cc == c)
			goto while2end;
		      /*@switchbreak@*/ break;
		    }
		}
	    }
	while2end:
	  pfile->lineno += count_newlines (pfile->token_buffer + old_written,
					   cpplib_getPWritten (pfile));
	  pfile->only_seen_white = 0;
	  return c == '\'' ? CPP_CHAR : CPP_STRING;

	case '$':
	  if (!opts->dollars_in_ident)
	    goto randomchar;
	  goto letter;

	case ':':
	  if (opts->cplusplus && cppReader_peekC (pfile) == ':')
	    goto op2;
	  goto randomchar;

	case '&':
	case '+':
	case '|':
	  NEWLINE_FIX;
	  c2 = cppReader_peekC (pfile);
	  if (c2 == c || c2 == '=')
	    goto op2;
	  goto randomchar;

	case '*':
	case '!':
	case '%':
	case '=':
	case '^':
	  NEWLINE_FIX;
	  if (cppReader_peekC (pfile) == '=')
	    goto op2;
	  goto randomchar;

	case '-':
	  NEWLINE_FIX;
	  c2 = cppReader_peekC (pfile);
	  if (c2 == '-' && opts->chill)
	    {
	      /* Chill style comment */
	      if (opts->put_out_comments)
		{
		  parseSetMark (&start_mark, pfile);
		}

	      cppReader_forward (pfile, 1);  /* Skip second '-'.  */

	      for (;;)
		{
		  c = cppReader_getC (pfile);
		  if (c == EOF)
		    /*@loopbreak@*/ break;
		  if (c == '\n')
		    {
		      /* Don't consider final '\n' to be part of comment.  */
		      cppReader_forward (pfile, -1);
		      /*@loopbreak@*/ break;
		    }
		}
	      c = '-';
	      goto return_comment;
	    }
	  if (c2 == '-' || c2 == '=' || c2 == '>')
	    goto op2;
	  goto randomchar;

	case '<':
	  if (pfile->parsing_include_directive)
	    {
	      for (;;)
		{
		  cppReader_putChar (pfile, c);
		  if (c == '>')
		    /*@loopbreak@*/ break;
		  c = cppReader_getC (pfile);
		  NEWLINE_FIX1 (c);
		  if (c == '\n' || c == EOF)
		    {
		      cppReader_errorLit (pfile,
				    cstring_makeLiteralTemp ("Missing '>' in \"#include <FILENAME>\""));
		      /*@loopbreak@*/ break;
		    }
		}
	      return CPP_STRING;
	    }
	  /*@fallthrough@*/
	case '>':
	  NEWLINE_FIX;
	  c2 = cppReader_peekC (pfile);
	  if (c2 == '=')
	    goto op2;
	  if (c2 != c)
	    goto randomchar;
	  cppReader_forward (pfile, 1);
	  cpplib_reserve (pfile, 4);
	  cppReader_putChar (pfile, c);
	  cppReader_putChar (pfile, c2);
	  NEWLINE_FIX;
	  c3 = cppReader_peekC (pfile);
	  if (c3 == '=')
	    cppReader_putCharQ (pfile, cppReader_getC (pfile));
	  cppReader_nullTerminateQ (pfile);
	  pfile->only_seen_white = 0;
	  return CPP_OTHER;

	case '@':
	  DPRINTF (("Macro @!"));
	  if (cppReader_getBufferSafe (pfile)->has_escapes)
	    {
	      c = cppReader_getC (pfile);
	      DPRINTF (("got c: %c", c));
	      if (c == '-')
		{
		  if (pfile->output_escapes)
		    cppReader_puts (pfile, "@-", 2);
		  parse_name (pfile, cppReader_getC (pfile));
		  return CPP_NAME;
		}
	      else if (is_space [c])
		{
		  cpplib_reserve (pfile, 2);
		  if (pfile->output_escapes)
		    cppReader_putCharQ (pfile, '@');
		  cppReader_putCharQ (pfile, c);
		  return CPP_HSPACE;
		}
	      else
		{
		  ;
		}
	    }
	  if (pfile->output_escapes)
	    {
	      cppReader_puts (pfile, "@@", 2);
	      return CPP_OTHER;
	    }
	  goto randomchar;
	case '.':
	  NEWLINE_FIX;
	  c2 = cppReader_peekC (pfile);
	  if (isdigit(c2))
	    {
	      cpplib_reserve(pfile, 2);
	      cppReader_putCharQ (pfile, '.');
	      c = cppReader_getC (pfile);
	      goto number;
	    }

	  /* FIXME - misses the case "..\\\n." */
	  if (c2 == '.' && cpp_peekN (pfile, 1) == '.')
	    {
	      cpplib_reserve(pfile, 4);
	      cppReader_putCharQ (pfile, '.');
	      cppReader_putCharQ (pfile, '.');
	      cppReader_putCharQ (pfile, '.');
	      cppReader_forward (pfile, 2);
	      cppReader_nullTerminateQ (pfile);
	      pfile->only_seen_white = 0;
	      return CPP_3DOTS;
	    }
	  goto randomchar;
	op2:
	  token = CPP_OTHER;
	  pfile->only_seen_white = 0;
        op2any: /* jumped to for \ continuations */
	  cpplib_reserve(pfile, 3);
	  cppReader_putCharQ (pfile, c);

	  /* evans 2003-08-24: This is a hack to fix line output for \
	     continuations.  Someday I really should get a decent pre-processor! 
	  */

	  if (c == '\\') {
	    (void) cppReader_getC (pfile); /* skip the newline to avoid extra lines */
	  } else {
	    cppReader_putCharQ (pfile, cppReader_getC (pfile)); 
	  }

	  cppReader_nullTerminateQ (pfile);
	  return token;

	case 'L':
	  NEWLINE_FIX;
	  c2 = cppReader_peekC (pfile);
	  if ((c2 == '\'' || c2 == '\"') && !cppReader_isTraditional (pfile))
	    {
	      cppReader_putChar (pfile, c);
	      c = cppReader_getC (pfile);
	      goto string;
	    }
	  goto letter;

	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
	number:
	  c2  = '.';
	  for (;;)
	    {
	      cpplib_reserve (pfile, 2);
	      cppReader_putCharQ (pfile, c);
	      NEWLINE_FIX;
	      c = cppReader_peekC (pfile);
	      if (c == EOF)
		/*@loopbreak@*/ break;
	      if (!is_idchar[c] && c != '.'
		  && ((c2 != 'e' && c2 != 'E'
		       && ((c2 != 'p' && c2 != 'P') || cppReader_isC89 (pfile)))
		      || (c != '+' && c != '-')))
		/*@loopbreak@*/ break;
	      cppReader_forward (pfile, 1);
	      c2= c;
	    }

	  cppReader_nullTerminateQ (pfile);
	  pfile->only_seen_white = 0;
	  return CPP_NUMBER;

	case 'b': case 'c': case 'd': case 'h': case 'o':
	case 'B': case 'C': case 'D': case 'H': case 'O':
	  if (opts->chill && cppReader_peekC (pfile) == '\'')
	    {
	      pfile->only_seen_white = 0;
	      cpplib_reserve (pfile, 2);
	      cppReader_putCharQ (pfile, c);
	      cppReader_putCharQ (pfile, '\'');
	      cppReader_forward (pfile, 1);
	      for (;;)
		{
		  c = cppReader_getC (pfile);
		  if (c == EOF)
		    goto chill_number_eof;
		  if (!is_idchar[c])
		    {
		      if (c == '\\' && cppReader_peekC (pfile) == '\n')
			{
			  cppReader_forward (pfile, 2);
			  continue;
			}
		      /*@loopbreak@*/ break;
		    }
		  cppReader_putChar (pfile, c);
		}
	      if (c == '\'')
		{
		  cpplib_reserve (pfile, 2);
		  cppReader_putCharQ (pfile, c);
		  cppReader_nullTerminateQ (pfile);
		  return CPP_STRING;
		}
	      else
		{
		  cppReader_forward (pfile, -1);
		chill_number_eof:
		  cppReader_nullTerminate (pfile);
		  return CPP_NUMBER;
		}
	    }
	  else
	    goto letter;
	case '_':
	case 'a': case 'e': case 'f': case 'g': case 'i': case 'j':
	case 'k': case 'l': case 'm': case 'n': case 'p': case 'q':
	case 'r': case 's': case 't': case 'u': case 'v': case 'w':
	case 'x': case 'y': case 'z':
	case 'A': case 'E': case 'F': case 'G': case 'I': case 'J':
	case 'K': case 'M': case 'N': case 'P': case 'Q': case 'R':
	case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
	case 'Y': case 'Z':
        letter:
          {
	    hashNode hp;
	    char *ident;
	    size_t before_name_written = cpplib_getWritten (pfile);
	    size_t ident_len;
	    parse_name (pfile, c);
	    pfile->only_seen_white = 0;

	    if (pfile->no_macro_expand)
	      {
		DPRINTF (("Not expanding: %s", pfile->token_buffer));
		return CPP_NAME;
	      }

	    ident = pfile->token_buffer + before_name_written;
	    DPRINTF (("Ident: %s", ident));

	    ident_len = size_fromInt ((cpplib_getPWritten (pfile)) - ident);

	    hp = cpphash_lookupExpand (ident, size_toInt (ident_len), -1, forceExpand);

	    if (hp == NULL)
	      {
		DPRINTF (("No expand: %s %d", ident, ident_len));
		return CPP_NAME;
	      }

	    if (hp->type == T_DISABLED)
	      {
		DPRINTF (("Disabled!"));

		if (pfile->output_escapes)
		  { /* Return "@-IDENT", followed by '\0'.  */
		    int i;
		    cpplib_reserve (pfile, 3);
		    ident = pfile->token_buffer + before_name_written;
		    cppReader_adjustWritten (pfile, 2);

		    for (i = size_toInt (ident_len); i >= 0; i--)
		      {
			ident[i+2] = ident[i];
		      }

		    ident[0] = '@';
		    ident[1] = '-';
		  }
		return CPP_NAME;
	      }

	    /* 
	    ** If macro wants an arglist, verify that a '(' follows.
	    ** first skip all whitespace, copying it to the output
	    ** after the macro name.  Then, if there is no '(',
	    ** decide this is not a macro call and leave things that way.  
	    */
	    
	    if (hp->type == T_MACRO && hp->value.defn->nargs >= 0)
	      {
		struct parse_marker macro_mark;
		int is_macro_call;

		DPRINTF (("Arglist macro!"));

		/*
		** evans 2002-07-03: Moved this here (from below).
		**   This bug caused necessary whitespace to be lost
		**   when parsing parameterized macros without parameters.
		*/

		parseSetMark (&macro_mark, pfile); 

		while (cppBuffer_isMacro (CPPBUFFER (pfile)))
		  {
		    cppBuffer *next_buf;
		    cppSkipHspace (pfile);
		    if (cppReader_peekC (pfile) != EOF)
		      {
			DPRINTF (("Peeking!"));
			/*@loopbreak@*/ break;
		      }

		  next_buf = cppBuffer_prevBuffer (cppReader_getBufferSafe (pfile));
		  (*cppReader_getBufferSafe (pfile)->cleanup) (cppReader_getBufferSafe (pfile), pfile);
		  CPPBUFFER (pfile) = next_buf;
		  }

		/* parseSetMark (&macro_mark, pfile); */

		for (;;)
		  {
		    cppSkipHspace (pfile);
		    c = cppReader_peekC (pfile);
		    DPRINTF (("c: %c", c));
		    is_macro_call = c == '(';
		    if (c != '\n')
		      /*@loopbreak@*/ break;
		    cppReader_forward (pfile, 1);
		  }

		if (!is_macro_call)
		  {
		    parseGotoMark (&macro_mark, pfile);
		  }

		parseClearMark (&macro_mark);

		if (!is_macro_call)
		  {
		    DPRINTF (("not macro call!"));
		    return CPP_NAME;
		  }
	      }

	    /* This is now known to be a macro call.  */

	    /* it might not actually be a macro.  */
	    if (hp->type != T_MACRO)
	      {
		size_t xbuf_len;
		char *xbuf;

		cppReader_setWritten (pfile, before_name_written);
		special_symbol (hp, pfile);
		xbuf_len = cpplib_getWritten (pfile) - before_name_written;
		xbuf = (char *) dmalloc (xbuf_len + 1);
		cppReader_setWritten (pfile, before_name_written);
		memcpy (xbuf, cpplib_getPWritten (pfile), xbuf_len + 1);
		push_macro_expansion (pfile, xbuf, xbuf_len, hp);
	      }
	    else
	      {
		/*
		** Expand the macro, reading arguments as needed,
		** and push the expansion on the input stack. 
		*/

		cpplib_macroExpand (pfile, hp);
		cppReader_setWritten (pfile, before_name_written);
	      }

	    /* An extra "@ " is added to the end of a macro expansion
	       to prevent accidental token pasting.  We prefer to avoid
	       unneeded extra spaces (for the sake of cpp-using tools like
	       imake).  Here we remove the space if it is safe to do so.  */

	    llassert (pfile->buffer->rlimit != NULL);

	    if (pfile->buffer->rlimit - pfile->buffer->cur >= 3
		&& pfile->buffer->rlimit[-2] == '@'
		&& pfile->buffer->rlimit[-1] == ' ')
	      {
		int c1 = pfile->buffer->rlimit[-3];
		int cl2 = cpplib_bufPeek (cppBuffer_prevBuffer (CPPBUFFER (pfile)));

		if (cl2 == EOF || !unsafe_chars ((char) c1, (char) cl2))
		  pfile->buffer->rlimit -= 2;
	      }
	  }
	  goto get_next;


	case ' ':  case '\t':  case '\v':  case '\r':
	  for (;;)
	    {
	      cppReader_putChar (pfile, c);
	      c = cppReader_peekC (pfile);
	      if (c == EOF || !is_hor_space[c])
		/*@loopbreak@*/ break;
	      cppReader_forward (pfile, 1);
	    }
	  return CPP_HSPACE;

        case '\\':
	  c2 = cppReader_peekC (pfile);
	  /* allow other stuff here if a flag is set? */
	  DPRINTF (("Got continuation!"));
	  if (c2 != '\n')
	    goto randomchar;
	  token = CPP_HSPACE;
	  goto op2any;

	case '\n':
	  cppReader_putChar (pfile, c);
	  if (pfile->only_seen_white == 0)
	    pfile->only_seen_white = 1;
	  pfile->lineno++;
	  output_line_command (pfile, 1, same_file);
	  return CPP_VSPACE;

	case '(': token = CPP_LPAREN;    goto char1;
	case ')': token = CPP_RPAREN;    goto char1;
	case '{': token = CPP_LBRACE;    goto char1;
	case '}': token = CPP_RBRACE;    goto char1;
	case ',': token = CPP_COMMA;     goto char1;
	case ';': token = CPP_SEMICOLON; goto char1;

	randomchar:
	default:
	  token = CPP_OTHER;
	char1:
	  pfile->only_seen_white = 0;
	  cppReader_putChar (pfile, c);
	  return token;
	}
    }

  BADBRANCH;
  /*@notreached@*/
}

/* Parse an identifier starting with C.  */

void
parse_name (cppReader *pfile, int c)
{
  for (;;)
    {
      if (!is_idchar[c])
	{
	  if (c == '\\' && cppReader_peekC (pfile) == '\n')
	    {
	      cppReader_forward (pfile, 2);
	      continue;
	    }

	  cppReader_forward (pfile, -1);
	  break;
	}

      if (c == '$' && cppReader_isPedantic (pfile))
	{
	  cppReader_pedwarnLit (pfile,
			  cstring_makeLiteralTemp ("`$' in identifier"));
	}

      cpplib_reserve(pfile, 2); /* One more for final NUL.  */
      cppReader_putCharQ (pfile, c);
      c = cppReader_getC (pfile);

      if (c == EOF)
	break;
    }

  cppReader_nullTerminateQ (pfile);
}

/* The file_name_map structure holds a mapping of file names for a
   particular directory.  This mapping is read from the file named
   FILE_NAME_MAP_FILE in that directory.  Such a file can be used to
   map filenames on a file system with severe filename restrictions,
   such as DOS.  The format of the file name map file is just a series
   of lines with two tokens on each line.  The first token is the name
   to map, and the second token is the actual name to use.  */

struct file_name_map
{
  struct file_name_map *map_next;
  cstring map_from;
  cstring map_to;
};

/*@constant observer char *FILE_NAME_MAP_FILE*/
#define FILE_NAME_MAP_FILE "header.gcc"

/* Read a space delimited string of unlimited length from a stdio
   file.  */

static cstring read_filename_string (int ch, /*:open:*/ FILE *f)
{
  char *alloc, *set;
  size_t len;

  len = 20;
  set = alloc = dmalloc (len + 1);

  if (!is_space[ch])
    {
      *set++ = ch;
      while ((ch = getc (f)) != EOF && ! is_space[ch])
	{
	  if (set - alloc == size_toInt (len))
	    {
	      len *= 2;
	      alloc = drealloc (alloc, len + 1);
	      set = alloc + len / 2;
	      /*@-branchstate@*/ }

	  *set++ = ch;
	} /*@=branchstate@*/
    }
  *set = '\0';
  check (ungetc (ch, f) != EOF);

  return cstring_fromChars (alloc);
}

/* This structure holds a linked list of file name maps, one per directory.  */

struct file_name_map_list
{
  /*@only@*/ struct file_name_map_list *map_list_next;
  /*@only@*/ cstring map_list_name;
  /*@null@*/ struct file_name_map *map_list_map;
};

/* Read the file name map file for DIRNAME.  */

static struct file_name_map *
read_name_map (cppReader *pfile, cstring dirname)
{
  struct file_name_map_list *map_list_ptr;
  cstring name;
  FILE *f;

  for (map_list_ptr = CPPOPTIONS (pfile)->map_list;
       map_list_ptr != NULL;
       map_list_ptr = map_list_ptr->map_list_next)
    {
      if (cstring_equal (map_list_ptr->map_list_name, dirname))
	{
	  return map_list_ptr->map_list_map;
	}
    }

  map_list_ptr = (struct file_name_map_list *) dmalloc (sizeof (*map_list_ptr));
  map_list_ptr->map_list_name = cstring_copy (dirname);
  map_list_ptr->map_list_map = NULL;

  name = cstring_copy (dirname);

  if (cstring_length (dirname) > 0)
    {
      name = cstring_appendChar (name, CONNECTCHAR);
    }

  name = cstring_concatFree1 (name, cstring_makeLiteralTemp (FILE_NAME_MAP_FILE));

  f = fileTable_openReadFile (context_fileTable (), name);
  cstring_free (name);

  if (f == NULL)
    {
      map_list_ptr->map_list_map = NULL;
    }
  else
    {
      int ch;

      while ((ch = getc (f)) != EOF)
	{
	  cstring from, to;
	  struct file_name_map *ptr;

	  if (is_space[ch])
	    {
	      continue;
	    }

	  from = read_filename_string (ch, f);
	  while ((ch = getc (f)) != EOF && is_hor_space[ch])
	    {
	      ;
	    }

	  to = read_filename_string (ch, f);

	  ptr = (struct file_name_map *) dmalloc (sizeof (*ptr));
	  ptr->map_from = from;

	  /* Make the real filename absolute.  */
	  if (cstring_length (to) > 1 
	      && osd_isConnectChar (cstring_firstChar (to)))
	    {
	      ptr->map_to = to;
	    }
	  else
	    {
	      ptr->map_to = cstring_copy (dirname);
	      ptr->map_to = cstring_appendChar (ptr->map_to, CONNECTCHAR);
	      ptr->map_to = cstring_concatFree (ptr->map_to, to);
	    }

	  ptr->map_next = map_list_ptr->map_list_map;
	  map_list_ptr->map_list_map = ptr;

	  while ((ch = getc (f)) != '\n')
	    {
	      if (ch == EOF)
		{
		  /*@innerbreak@*/ break;
		}
	    }
	}

      assertSet (map_list_ptr->map_list_map);
      check (fileTable_closeFile (context_fileTable (),f) == 0);
    }

  map_list_ptr->map_list_next = pfile->opts->map_list;
  pfile->opts->map_list = map_list_ptr;

  return map_list_ptr->map_list_map;
}

/* Try to open include file FILENAME.  SEARCHPTR is the directory
   being tried from the include file search path.  This function maps
   filenames on file systems based on information read by
   read_name_map.  */

static int
open_include_file (cppReader *pfile,
		   cstring fname,
		   struct file_name_list *searchptr)
{
  char *filename = cstring_toCharsSafe (fname);
  struct file_name_map *map;
  char *from;
  char *p, *dir;

  cstring_markOwned (fname);

  cpp_setLocation (pfile);

  if (context_getFlag (FLG_NEVERINCLUDE))
    {
      if (isHeaderFile (fname))
	{
	  return SKIP_INCLUDE;
	}
    }

  if ((searchptr != NULL) && ! searchptr->got_name_map)
    {
      searchptr->name_map = read_name_map (pfile,
					   !cstring_isEmpty (searchptr->fname)
					   ? searchptr->fname :
					   cstring_makeLiteralTemp ("."));
      searchptr->got_name_map = 1;
    }

  /* First check the mapping for the directory we are using.  */

  if ((searchptr != NULL)
      && (searchptr->name_map != NULL))
    {
      from = filename;

      if (!cstring_isEmpty (searchptr->fname))
	{
	  from += cstring_length (searchptr->fname) + 1;
	}

      for (map = searchptr->name_map;
	   map != NULL;
	   map = map->map_next)
	{
	  if (cstring_equal (map->map_from, cstring_fromChars (from)))
	    {
	      /*
	      ** Found a match.  Check if the file should be skipped
	      */
	      
	      if (cpp_skipIncludeFile (map->map_to))
		{
		  return SKIP_INCLUDE;
		}
	      else
		{
		  return cpp_openIncludeFile (cstring_toCharsSafe (map->map_to));
		}
	    }
	}
    }

  /*
  ** Try to find a mapping file for the particular directory we are
  ** looking in.  Thus #include <sys/types.h> will look up sys/types.h
  ** in /usr/include/header.gcc and look up types.h in
  ** /usr/include/sys/header.gcc.
  */

  p = strrchr (filename, CONNECTCHAR);

  if (p == NULL)
    {
      p = filename;
    }

  if ((searchptr != NULL)
      && (cstring_isDefined (searchptr->fname))
      && (size_toInt (cstring_length (searchptr->fname)) == p - filename)
      && !strncmp (cstring_toCharsSafe (searchptr->fname),
		   filename,
		   size_fromInt (p - filename)))
    {
      /* filename is in SEARCHPTR, which we've already checked.  */

      if (cpp_skipIncludeFile (cstring_fromChars (filename)))
	{
	  return SKIP_INCLUDE;
	}
      else
	{
	  return cpp_openIncludeFile (filename);
	}
    }

  if (p == filename)
    {
      dir = mstring_copy (".");
      from = filename;
    }
  else
    {
      dir = (char *) dmalloc (size_fromInt (p - filename + 1));
      memcpy (dir, filename, size_fromInt (p - filename));
      dir[p - filename] = '\0';
      from = p + 1;
    }

  for (map = read_name_map (pfile, cstring_fromChars (dir));
       map != NULL;
       map = map->map_next)
    {
      if (cstring_equal (map->map_from, cstring_fromChars (from)))
	{
	  sfree (dir);

	  if (cpp_skipIncludeFile (map->map_to))
	    {
	      return SKIP_INCLUDE;
	    }
	  else
	    {
	      return cpp_openIncludeFile (cstring_toCharsSafe (map->map_to));
	    }
	}
    }

  sfree (dir);

  if (cpp_skipIncludeFile (cstring_fromChars (filename)))
    {
      return SKIP_INCLUDE;
    }
  else
    {
      return cpp_openIncludeFile (filename);
    }
}

/* Process the contents of include file FNAME, already open on descriptor F,
   with output to OP.
   SYSTEM_HEADER_P is 1 if this file resides in any one of the known
   "system" include directories (as decided by the `is_system_include'
   function above).
   DIRPTR is the link in the dir path through which this file was found,
   or 0 if the file name was absolute or via the current directory.
   Return 1 on success, 0 on failure.

   The caller is responsible for the cppReader_pushBuffer.  */

static int
finclude (cppReader *pfile, int f,
	  cstring fname,
	  bool system_header_p,
	  /*@dependent@*/ struct file_name_list *dirptr)
{
  mode_t st_mode; /* was __mode_t */
  size_t st_size;
  long i;
  int length = 0;
  cppBuffer *fp;			/* For input stack frame */

  if (file_size_and_mode (f, &st_mode, &st_size) < 0)
    {
      cppReader_perrorWithName (pfile, fname);
      check (close (f) == 0);
      (void) cppReader_popBuffer (pfile);
      /*@-mustfree@*/
      return 0;
      /*@=mustfree@*/
    }

  fp = cppReader_getBufferSafe (pfile);

  /*@-temptrans@*/ /* fname shouldn't really be temp */
  fp->nominal_fname = fp->fname = fname;
  /*@=temptrans@*/

  fp->dir = dirptr;
  fp->system_header_p = system_header_p;
  fp->lineno = 1;
  fp->colno = 1;
  fp->cleanup = cppReader_fileCleanup;

  if (S_ISREG (st_mode))
    {
      sfree (fp->buf);
      fp->buf = (char *) dmalloc (st_size + 2);
      fp->alimit = fp->buf + st_size + 2;
      fp->cur = fp->buf;

      /* Read the file contents, knowing that st_size is an upper bound
	 on the number of bytes we can read.  */
      length = safe_read (f, fp->buf, size_toInt (st_size));
      fp->rlimit = fp->buf + length;
      if (length < 0) goto nope;
    }
  else if (S_ISDIR (st_mode))
    {
      cppReader_error (pfile,
		       message ("Directory specified where file is expected: %s", fname));
      check (close (f) == 0);
      return 0;
    }
  else
    {
      /*
      ** Cannot count its file size before reading.
      ** First read the entire file into heap and
      ** copy them into buffer on stack.
      */

      size_t bsize = 2000;

      st_size = 0;

      sfree (fp->buf);
      fp->buf = (char *) dmalloc (bsize + 2);

      for (;;) {
	i = safe_read (f, fp->buf + st_size, size_toInt (bsize - st_size));

	if (i < 0)
	  goto nope;      /* error! */
	st_size += i;

	if (st_size != bsize)
	  {
	    break;	/* End of file */
	  }

	bsize *= 2;
	fp->buf = (char *) drealloc (fp->buf, bsize + 2);
      }

      fp->cur = fp->buf;
      length = size_toInt (st_size);
    }

  if ((length > 0 && fp->buf[length - 1] != '\n')
      /* Backslash-newline at end is not good enough.  */
      || (length > 1 && fp->buf[length - 2] == '\\')) {
    fp->buf[length++] = '\n';
  }

  fp->buf[length] = '\0';
  fp->rlimit = fp->buf + length;

  /* Close descriptor now, so nesting does not use lots of descriptors.  */
  check (close (f) == 0);

  /* Must do this before calling trigraph_pcp, so that the correct file name
     will be printed in warning messages.  */

  pfile->input_stack_listing_current = 0;
  return 1;

 nope:

  cppReader_perrorWithName (pfile, fname);
  check (close (f) == 0);
  sfree (fp->buf);
  return 1;
}

void
cpplib_init (cppReader *pfile)
{
  memset ((char *) pfile, 0, sizeof (*pfile));

  pfile->get_token = cpplib_getToken;
  pfile->token_buffer_size = 200;
  pfile->token_buffer = (char *) dmalloc (pfile->token_buffer_size);
  pfile->all_include_files = NULL;

  assertSet (pfile);

  cppReader_setWritten (pfile, 0);

  pfile->system_include_depth = 0;
  pfile->max_include_len = 0;
  pfile->timebuf = NULL;
  pfile->only_seen_white = 1;

  pfile->buffer = cppReader_nullBuffer (pfile);
}

void
cppReader_finish (/*@unused@*/ cppReader *pfile)
{
  ;
}

/* Free resources used by PFILE.
   This is the cppReader 'finalizer' or 'destructor' (in C++ terminology).  */

void
cppCleanup (/*@special@*/ cppReader *pfile) 
     /*@uses pfile@*/
     /*@releases pfile@*/
{
  DPRINTF (("cppCleanup!"));

  while (CPPBUFFER (pfile) != cppReader_nullBuffer (pfile))
    {
      (void) cppReader_popBuffer (pfile);
    }

  if (pfile->token_buffer != NULL)
    {
      sfree (pfile->token_buffer);
      pfile->token_buffer = NULL;
    }

  while (pfile->if_stack != NULL)
    {
      cppIfStackFrame *temp = pfile->if_stack;
      pfile->if_stack = temp->next;
      sfree (temp);
    }

  while (pfile->all_include_files != NULL)
    {
      struct file_name_list *temp = pfile->all_include_files;
      pfile->all_include_files = temp->next;
      /*@-dependenttrans@*/
      cstring_free (temp->fname);
      /*@=dependenttrans@*/
      sfree (temp);
    }

  /* evans 2002-07-12 */
  while (pfile->opts->map_list != NULL)
    {
      struct file_name_map_list *temp = pfile->opts->map_list;
      pfile->opts->map_list = pfile->opts->map_list->map_list_next;
      cstring_free (temp->map_list_name);
      sfree (temp);
    }

  while (pfile->opts->include != NULL)
    {
      struct file_name_list *temp = pfile->opts->include;
      pfile->opts->include = pfile->opts->include->next;
      /* cstring_free (temp->fname); */
      sfree (temp);
    }

  sfree (pfile->opts);
  pfile->opts = NULL;
  cppReader_hashCleanup ();
}

/*
** Get the file-mode and data size of the file open on FD
** and store them in *MODE_POINTER and *SIZE_POINTER.
*/

static int
file_size_and_mode (int fd, mode_t *mode_pointer, size_t *size_pointer)
{
  struct stat sbuf;

  if (fstat (fd, &sbuf) < 0) {
    *mode_pointer = 0;
    *size_pointer = 0;
    /*@-compdestroy@*/ /* possibly spurious warnings here (or memory leak) */
    return (-1);
    /*@=compdestroy@*/
  }

  if (mode_pointer != NULL)
    {
      /*@-type@*/ /* confusion between __mode_t and mode_t types */
      *mode_pointer = sbuf.st_mode;
      /*@=type@*/
    }

  if (size_pointer != NULL)
    {
      *size_pointer = (size_t) sbuf.st_size;
    }

  /*@-compdestroy@*/ /* possibly spurious warnings here (or memory leak) */
  return 0;
  /*@=compdestroy@*/
}

/* Read LEN bytes at PTR from descriptor DESC, for file FILENAME,
   retrying if necessary.  Return a negative value if an error occurs,
   otherwise return the actual number of bytes read,
   which must be LEN unless end-of-file was reached.  */

static int safe_read (int desc, char *ptr, int len)
{
  int left = len;

  while (left > 0)
    {
# if defined (WIN32) || defined (OS2) && defined (__IBMC__)
	  /*@-compdef@*/ /* ptr is an out parameter */
      int nchars = _read (desc, ptr, (unsigned) left);
	  /*@=compdef@*/
# else
      ssize_t nchars = read (desc, ptr, size_fromInt (left));
# endif

      if (nchars < 0)
	{
#ifdef EINTR
	  if (errno == EINTR)
	    continue;
#endif
	  return (int) nchars;
	}

      if (nchars == 0) {
	break;
      }

      ptr += nchars;
      left -= nchars;
    }

  return len - left;
}

/* Initialize PMARK to remember the current position of PFILE.  */

void
parseSetMark (struct parse_marker *pmark, cppReader *pfile)
{
  cppBuffer *pbuf = cppReader_getBufferSafe (pfile);

  pmark->next = pbuf->marks;
  /*@-temptrans@*/
  pbuf->marks = pmark;
  /*@=temptrans@*/

  pmark->buf = pbuf;
  pmark->position = pbuf->cur - pbuf->buf;
  DPRINTF (("set mark: %d / %s", pmark->position, pbuf->cur));
}

/* Cleanup PMARK - we no longer need it.  */

void parseClearMark (struct parse_marker *pmark)
{
  struct parse_marker **pp = &pmark->buf->marks;

  for (; ; pp = &(*pp)->next)
    {
      llassert (*pp != NULL);
      if (*pp == pmark) break;
    }

  *pp = pmark->next;
}

/* Backup the current position of PFILE to that saved in PMARK.  */

void
parseGotoMark (struct parse_marker *pmark, cppReader *pfile)
{
  cppBuffer *pbuf = cppReader_getBufferSafe (pfile);

  if (pbuf != pmark->buf)
    {
      cpp_setLocation (pfile);
      llfatalbug (cstring_makeLiteral ("Internal error parseGotoMark"));
    }

  llassert (pbuf->buf != NULL);
  pbuf->cur = pbuf->buf + pmark->position;
  DPRINTF (("goto mark: %d / %s", pmark->position, pbuf->cur));
}

/* Reset PMARK to point to the current position of PFILE.  (Same
   as parseClearMark (PMARK), parseSetMark (PMARK, PFILE) but faster.  */

void
parseMoveMark (struct parse_marker *pmark, cppReader *pfile)
{
  cppBuffer *pbuf = cppReader_getBufferSafe (pfile);

  if (pbuf != pmark->buf)
    {
      cpp_setLocation (pfile);
      llfatalerror (cstring_makeLiteral ("Internal error parseMoveMark"));
    }

  pmark->position = pbuf->cur - pbuf->buf;
  DPRINTF (("move mark: %s", pmark->position));
}

void cpplib_initializeReader (cppReader *pfile) /* Must be done after library is loaded. */
{
  struct cppOptions *opts = CPPOPTIONS (pfile);
  cstring xp;

  /* The code looks at the defaults through this pointer, rather than through
     the constant structure above.  This pointer gets changed if an environment
     variable specifies other defaults.  */

  struct default_include *include_defaults = include_defaults_array;

  /* Add dirs from INCLUDEPATH_VAR after dirs from -I.  */
  /* There seems to be confusion about what CPATH should do,
     so for the moment it is not documented.  */
  /* Some people say that CPATH should replace the standard include dirs,
     but that seems pointless: it comes before them, so it overrides them
     anyway.  */

  xp = osd_getEnvironmentVariable (INCLUDEPATH_VAR);

  if (cstring_isDefined (xp) && !opts->no_standard_includes)
    {
      path_include (pfile, cstring_toCharsSafe (xp));
    }

  /* Now that dollars_in_ident is known, initialize is_idchar.  */
  initialize_char_syntax (opts);

  /* CppReader_Install __LINE__, etc.  Must follow initialize_char_syntax
     and option processing.  */

  initialize_builtins (pfile);

  /* Do standard #defines and assertions
     that identify system and machine type.  */

  if (!opts->inhibit_predefs) {
    char *p = (char *) dmalloc (strlen (predefs) + 1);
    strcpy (p, predefs);

    while (*p)
      {
	char *q;

	while (*p == ' ' || *p == '\t')
	  {
	    p++;
	  }

	/* Handle -D options.  */
	if (p[0] == '-' && p[1] == 'D')
	  {
	    q = &p[2];

	    while (*p && *p != ' ' && *p != '\t')
	      {
		p++;
	      }

	    if (*p != 0)
	      {
		*p++= 0;
	      }

	    if (opts->debug_output)
	      {
		output_line_command (pfile, 0, same_file);
	      }

	    cppReader_define (pfile, q);

	    while (*p == ' ' || *p == '\t')
	      {
		p++;
	      }
	  }
	else
	  {
	    abort ();
	  }
      }

    sfree (p);
  }

  opts->done_initializing = 1;

  { /* Read the appropriate environment variable and if it exists
       replace include_defaults with the listed path.  */
    char *epath = 0;
#ifdef __CYGWIN32__
  char *win32epath;
  int win32_buf_size = 0; /* memory we need to allocate */
#endif

  if (opts->cplusplus)
    {
      epath = getenv ("CPLUS_INCLUDE_PATH");
    }
  else
    {
      epath = getenv ("C_INCLUDE_PATH");
    }

  /*
  ** If the environment var for this language is set,
  ** add to the default list of include directories.
  */

    if (epath != NULL) {
      char *nstore = (char *) dmalloc (strlen (epath) + 2);
      int num_dirs;
      char *startp, *endp;

#ifdef __CYGWIN32__
      /* if we have a posix path list, convert to win32 path list */
      if (cygwin32_posix_path_list_p (epath))
        {
          win32_buf_size = cygwin32_posix_to_win32_path_list_buf_size (epath);
          win32epath = (char *) dmalloc /*@i4@*/ (win32_buf_size);
          cygwin32_posix_to_win32_path_list (epath, win32epath);
          epath = win32epath;
        }
#endif
      for (num_dirs = 1, startp = epath; *startp; startp++)
	{
	  if (*startp == PATH_SEPARATOR)
	    num_dirs++;
	}

      /*@-sizeoftype@*/
      include_defaults
	= (struct default_include *) dmalloc ((num_dirs
					       * sizeof (struct default_include))
					      + sizeof (include_defaults_array));
      /*@=sizeoftype@*/

      startp = endp = epath;
      num_dirs = 0;
      while (1) {
        /* Handle cases like c:/usr/lib:d:/gcc/lib */
        if ((*endp == PATH_SEPARATOR) || *endp == 0)
	  {
	    strncpy (nstore, startp, size_fromInt (endp - startp));
	    if (endp == startp)
	      {
		strcpy (nstore, ".");
	      }
	    else
	      {
		nstore[endp-startp] = '\0';
	      }

	    include_defaults[num_dirs].fname = cstring_fromCharsNew (nstore);
	    include_defaults[num_dirs].cplusplus = opts->cplusplus;
	    include_defaults[num_dirs].cxx_aware = 1;
	    num_dirs++;

	    if (*endp == '\0')
	      {
		break;
	      }
	    endp = startp = endp + 1;
	  }
	else
	  {
	    endp++;
	  }
      }
      /* Put the usual defaults back in at the end.  */
      memcpy ((char *) &include_defaults[num_dirs],
	      (char *) include_defaults_array,
	      sizeof (include_defaults_array));

      sfree (nstore);
      /*@-branchstate@*/ } /*@=branchstate@*/
  }

  cppReader_appendIncludeChain (pfile, opts->before_system,
				opts->last_before_system);

  opts->first_system_include = opts->before_system;

  /* Unless -fnostdinc,
     tack on the standard include file dirs to the specified list */

  if (!opts->no_standard_includes) {
    struct default_include *p = include_defaults;
    char *specd_prefix = opts->include_prefix;
    char *default_prefix = mstring_copy (GCC_INCLUDE_DIR);
    size_t default_len = 0;
    
    /* Remove the `include' from /usr/local/lib/gcc.../include.  */
    if (default_prefix != NULL) {
      if (!strcmp (default_prefix + strlen (default_prefix) - 8, "/include")) {
	default_len = strlen (default_prefix) - 7;
	default_prefix[default_len] = 0;
      }
    }
    
    /* Search "translated" versions of GNU directories.
       These have /usr/local/lib/gcc... replaced by specd_prefix.  */
    if (specd_prefix != 0 && default_len != 0)
      for (p = include_defaults; p->fname != NULL; p++) {
	/* Some standard dirs are only for C++.  */
	if (!p->cplusplus
	    || (opts->cplusplus && !opts->no_standard_cplusplus_includes)) {
	  /* Does this dir start with the prefix?  */
	  llassert (default_prefix != NULL);

	  if (!strncmp (cstring_toCharsSafe (p->fname), default_prefix, default_len))
	    {
	      /* Yes; change prefix and add to search list.  */
	      struct file_name_list *nlist
		= (struct file_name_list *) dmalloc (sizeof (*nlist));
	      size_t this_len = strlen (specd_prefix) + cstring_length (p->fname) - default_len;
	      char *str = (char *) dmalloc (this_len + 1);
	      strcpy (str, specd_prefix);
	      strcat (str, cstring_toCharsSafe (p->fname) + default_len);

	      nlist->next = NULL;
	      nlist->fname = cstring_fromChars (str);
	      nlist->control_macro = 0;
	      nlist->c_system_include_path = !p->cxx_aware;
	      nlist->got_name_map = 0;

	      if (opts->first_system_include == 0)
		{
		  opts->first_system_include = nlist;
		}
         
	      cppReader_addIncludeChain (pfile, nlist);
	    }
	}
      }
    
    /* Search ordinary names for GNU include directories.  */

    for (p = include_defaults; p->fname != NULL; p++)
      {
	/* Some standard dirs are only for C++.  */
	if (!p->cplusplus
	    || (opts->cplusplus && !opts->no_standard_cplusplus_includes))
	  {
	    struct file_name_list *nlist
	      = (struct file_name_list *) dmalloc (sizeof (*nlist));
	    nlist->control_macro = 0;
	    nlist->c_system_include_path = !p->cxx_aware;
	    nlist->fname = p->fname;
	    nlist->got_name_map = 0;
	    nlist->next = NULL;

	    /* Spurious warning reported for opts->first_system_include */
	    /*@-usereleased@*/ if (opts->first_system_include == NULL) 
	      {
		opts->first_system_include = nlist;
	      }
	    /*@=usereleased@*/

	    cppReader_addIncludeChain (pfile, nlist);
	  }
      }
    sfree (default_prefix);
  }

  /* Tack the after_include chain at the end of the include chain.  */
  cppReader_appendIncludeChain (pfile, opts->after_include,
				opts->last_after_include);

  /* Spurious warnings for opts->first_system_include */
  /*@-usereleased@*/
  if (opts->first_system_include == NULL)
    {
      opts->first_system_include = opts->after_include;
    }
  /*@=usereleased@*/
  
  /* With -v, print the list of dirs to search.  */
  if (opts->verbose) {
    struct file_name_list *p;
    fprintf (stderr, "#include \"...\" search starts here:\n");

    for (p = opts->include; p != NULL; p = p->next) {
      if (p == opts->first_bracket_include)
	fprintf (stderr, "#include <...> search starts here:\n");

      fprintf (stderr, " %s\n", cstring_toCharsSafe (p->fname));
    }
    fprintf (stderr, "End of search list.\n");
  }
}

int cppReader_startProcess (cppReader *pfile, cstring fname)
{
  cppBuffer *fp;
  int f;
  struct cppOptions *opts = CPPOPTIONS (pfile);

  fp = cppReader_pushBuffer (pfile, NULL, 0);

  if (fp == NULL)
    {
      return 0;
    }

  if (opts->in_fname == NULL)
    {
      opts->in_fname = cstring_makeLiteralTemp ("");
    }

  fp->fname = opts->in_fname;
  fp->nominal_fname = fp->fname;
  fp->lineno = 0;

  /* Copy the entire contents of the main input file into
     the stacked input buffer previously allocated for it.  */

  if (cstring_isEmpty (fname))
    {
      fname = cstring_makeLiteralTemp ("");
      f = 0;
    }
  else if ((f = open (cstring_toCharsSafe (fname), O_RDONLY, 0666)) < 0)
    {
      cppReader_error (pfile,
		       message ("Error opening %s for reading: %s",
				fname, lldecodeerror (errno)));

      return 0;
    }
  else
    {
      ;
    }

  if (finclude (pfile, f, fname, 0, NULL))
    {
      output_line_command (pfile, 0, same_file);
    }

  return 1;
}

static /*@exposed@*/ /*@null@*/ cppBuffer *cppReader_getBuffer (cppReader *pfile)
{
  return pfile->buffer;
}

/*@exposed@*/ cppBuffer *cppReader_getBufferSafe (cppReader *pfile)
{
  llassert (pfile->buffer != NULL);
  return pfile->buffer;
}

/*@exposed@*/ char *cppLineBase (cppBuffer *buf)
{
  llassert (buf->buf != NULL);
  return (buf->buf + buf->line_base);
}

int cpplib_bufPeek (cppBuffer *buf)
{
  if (buf->cur == NULL || buf->rlimit == NULL) {
    return EOF;
  }

  if (buf->cur < buf->rlimit) {
    return *(buf->cur);
  }

  return EOF;
}

bool cppBuffer_isMacro (cppBuffer *buf)
{
  if (buf != NULL)
    {
      return (buf->cleanup == cppReader_macroCleanup);
    }

  return FALSE;
}

/*
** Returns true if the macro should be checked, false
** if it should be expanded normally.
*/

static bool notparseable = FALSE;  /* preceeded by @notparseable@ */
static bool notfunction = FALSE;   /* preceeded by @notfunction@ */
static bool expectiter = FALSE;    /* preceeded by @iter@ */
static bool expectenditer = FALSE; /* second after @iter@ */
static bool expectfunction = FALSE;    /* preceeded by @function@ */
static bool expectconstant = FALSE;   /* preceeded by @constant@ */
static bool expectmacro = FALSE;   /* preceeded by notfunction or notparseable */

static void cpp_setLocation (cppReader *pfile)
{
  fileId fid;
  int line;

  if (pfile->buffer != NULL)
    {
      if (cstring_isDefined (cppReader_getBufferSafe (pfile)->nominal_fname))
	{
	  cstring fname = cppReader_getBufferSafe (pfile)->nominal_fname;
	  
	  DPRINTF (("Looking up: %s", fname));
	  
	  if (fileTable_exists (context_fileTable (), fname))
	    {
	      fid = fileTable_lookup (context_fileTable (), fname);
	    }
	  else
	    {
	      DPRINTF (("Trying %s", cppReader_getBuffer (pfile)->fname));

	      fid = fileTable_lookup (context_fileTable (),
				      cppReader_getBufferSafe (pfile)->fname);
	    }
	}
      else
	{
	  fid = fileTable_lookup (context_fileTable (),
				  cppReader_getBufferSafe (pfile)->fname);
	}
      
      line = cppReader_getBufferSafe (pfile)->lineno;
      fileloc_free (g_currentloc);

      if (fileId_isValid (fid))
	{
	  g_currentloc = fileloc_create (fid, line, 1);
	}
      else
	{
	  g_currentloc = fileloc_createBuiltin ();
	}
    }
  else
    {
      fileloc_free (g_currentloc);
      g_currentloc = fileloc_createBuiltin ();
    }
}

static bool cpp_shouldCheckMacro (cppReader *pfile, char *p) /*@modifies p*/
{
  bool checkmacro = FALSE;
  bool hasParams = FALSE;
  bool noexpand = FALSE;
  cstring sname;
  char c;

  cpp_setLocation (pfile);

  DPRINTF (("Should check macro? %s", p));

  if (expectiter || expectconstant || expectenditer)
    {
      if (expectiter)
	{
	  expectiter = FALSE;
	  expectenditer = TRUE;
	}
      else
	{
	  expectiter = FALSE;
	  expectconstant = FALSE;
	  expectenditer = FALSE;
	}

      if (notfunction || notparseable)
	{
	  notfunction = FALSE;
	  notparseable = FALSE;
	  return FALSE;
	}
      else
	{
	  return TRUE;
	}
    }

  llassert (*p == '#');
  p++;

  while (*p == ' ' || *p == '\t')
    {
      p++;
    }

  llassert (*p == 'd'); /* define starts */

  p += 6;

  while (*p == ' ' || *p == '\t')
    {
      p++;
    }

  sname = cstring_fromChars (p);
  DPRINTF (("Check macro: %s", sname));

  while (((c = *p) != ' ')
	 && c != '\0' && c != '('
	 && c != '\t' && c != '\\' && c != '\n'
	 && !iscntrl (c))
    {
      p++;
    }

  hasParams = (c == '(');
  *p = '\0';

  if (notparseable)
    {
      notparseable = FALSE;
    }
  else if (notfunction || fileloc_isStandardLib (g_currentloc))
    {
      DPRINTF (("Clear notfunction"));
      notfunction = FALSE;
    }
  else
    {
      if (noexpand)
	{
	  checkmacro = TRUE;

	  if (!expectenditer)
	    {
	      noexpand = FALSE;
	    }
	}
      else
	{
	  if (usymtab_existsReal (sname))
	    {
	      uentry ue = usymtab_lookup (sname);

	      DPRINTF (("Lookup macro: %s", uentry_unparse (ue)));

	      if (fileloc_isPreproc (uentry_whereLast (ue)))
		{
		  goto macroDne;
		}
	      else
		{
		  if (uentry_isSpecified (ue))
		    {
		      checkmacro = context_getFlag (FLG_SPECMACROS);
		    }
		  else
		    {
		      if (hasParams)
			{
			  checkmacro = context_getFlag (FLG_LIBMACROS)
			    || context_getFlag (FLG_FCNMACROS);
			}
		    }
		}
	    }
	  else
	    {
	    macroDne:
	      DPRINTF (("Macro doesn't exist: %s", bool_unparse (checkmacro)));

	      if (fileloc_isSystemFile (g_currentloc)
		  && context_getFlag (FLG_SYSTEMDIREXPAND))
		{
		  ; /* don't check this macro */
		  DPRINTF (("Don't check 1"));
		}
	      else
		{
		  uentry le;
		  
		  if (hasParams)
		    {
		      DPRINTF (("Has params..."));

		      if (context_getFlag (FLG_FCNMACROS))
			{
			  if (usymtab_exists (sname))
			    {
                              /*
			      ** only get here is macro is redefined
			      ** error reported elsewhere
			      */

			      DPRINTF (("It exists!"));
			    }
			  else
			    {
			      /*
			      ** We make it a forward function, since it might be declared elsewhere.
			      ** After all headers have been processed, we should check the forward
			      ** functions.
			      */

			      fileloc loc = fileloc_makePreproc (g_currentloc);

			      /* the line is off-by-one, since the newline was already read */
			      decLine ();

			      if (expectfunction)
				{
				  expectfunction = FALSE;
				}

			      le = uentry_makeForwardFunction (sname,
							       typeId_invalid, loc);

			      fileloc_free (loc);

			      incLine ();

			      /* Do not define here! */

			      (void) usymtab_addEntry (le);
			    }

			  checkmacro = TRUE;
			  DPRINTF (("Check: TRUE"));
			}
		      else 
			{
			  DPRINTF (("Flag FCN_MACROS not set!"));
			}
		    }
		  else
		    {
		      DPRINTF (("No params"));

		      if (context_getFlag (FLG_CONSTMACROS))
			{
			  bool nocontent = FALSE;

			  if (c == '\0')
			    {
			      nocontent = TRUE;
			    }
			  else
			    {
			      if (isspace (c))
				{
				  char *rest = p + 1;

				  /*
				  ** Check if there is nothing after the define.
				  */

				  while ((*rest) != '\0' && isspace (*rest))
				    {
				      rest++;
				    }

				  if (*rest == '\0')
				    {
				      nocontent = TRUE; /* empty macro, don't check */
				    }
				}
			    }

			  if (usymtab_exists (sname))
			    {
			      ;
			    }
			  else
			    {
			      fileloc loc = fileloc_makePreproc (g_currentloc);
			      DPRINTF (("Make constant: %s", sname));
			      le = uentry_makeMacroConstant (sname, ctype_unknown, loc);
			      (void) usymtab_addEntry (le);
			    }

			  checkmacro = !nocontent;
			}
		    }
		}

	      if (checkmacro && usymtab_existsType (sname))
		{
		  DPRINTF (("Making false..."));
		  decLine ();
		  ppllerror (message ("Specified type implemented as macro: %s", sname));
		  checkmacro = FALSE;
		  incLine ();
		}
	    }
	}
    }

  if (!checkmacro)
    {
      if (usymtab_exists (sname))
	{
	  uentry ue = usymtab_lookupExpose (sname);
	  fileloc tloc = fileloc_makePreproc (g_currentloc);

	  uentry_setDefined (ue, tloc);
	  fileloc_free (tloc);
	  uentry_setUsed (ue, fileloc_undefined);
	}
      else
	{
	  fileloc tloc = fileloc_makePreproc (g_currentloc);
	  uentry ue = uentry_makeExpandedMacro (sname, tloc);
	  DPRINTF (("Make expanded macro: %s", sname));
	  DPRINTF (("Not in symbol table: %s", sname));
	  
	  (void) usymtab_addGlobalEntry (ue);
	  fileloc_free (tloc);
	}
    }

  *p = c;
  DPRINTF (("Returning: %s", bool_unparse (checkmacro)));
  return checkmacro;
}

static enum cpp_token
cpp_handleComment (cppReader *pfile, struct parse_marker *smark)
{
  cppBuffer *pbuf = cppReader_getBufferSafe (pfile);
  char *start;
  int len;
  fileloc loc;
  bool eliminateComment = FALSE;

  llassert (pbuf->buf != NULL);

  start = pbuf->buf + smark->position;

  llassert (pbuf->cur != NULL);
  len = pbuf->cur - start;

  if (start[0] == '*'
      && start[1] == context_getCommentMarkerChar ())
    {
      int i;
      char c = ' ';
      char *scomment = start + 2;
      char savec = start[len];
      
      cpp_setLocation (pfile);
      loc = fileloc_copy (g_currentloc);

      start[0] = BEFORE_COMMENT_MARKER[0];
      start[1] = BEFORE_COMMENT_MARKER[1];

      llassert (start[len - 2] == '*');
      start[len - 2] = AFTER_COMMENT_MARKER[0];

      llassert (start[len - 1] == '/');
      start[len - 1] = AFTER_COMMENT_MARKER[1];

      cpplib_reserve(pfile, size_fromInt (1 + len));
      cppReader_putCharQ (pfile, c);

      cpp_setLocation (pfile);

      start[len] = '\0';

      if (mstring_containsString (scomment, "/*"))
	{
	  (void) cppoptgenerror 
	    (FLG_NESTCOMMENT,
	     message ("Comment starts inside syntactic comment: %s", 
		      cstring_fromChars (scomment)),
	     pfile);
	}

      start[len] = savec;

      if (mstring_equalPrefix (scomment, "ignore"))
	{
	  if (!context_getFlag (FLG_NOCOMMENTS))
	    {
	      context_enterSuppressRegion (loc);
	    }
	}
      else if (mstring_equalPrefix (scomment, "end"))
	{
	  if (!context_getFlag (FLG_NOCOMMENTS))
	    {
	      context_exitSuppressRegion (loc);
	    }
	}
      else if (mstring_equalPrefix (scomment, "notparseable"))
	{
	  notparseable = TRUE;
	  expectmacro = TRUE;
	  eliminateComment = TRUE;
	}
      else if (mstring_equalPrefix (scomment, "notfunction"))
	{
	  notfunction = TRUE;
	  expectmacro = TRUE;
	  eliminateComment = TRUE;
	}
      else if (mstring_equalPrefix (scomment, "iter"))
	{
	  expectiter = TRUE;
	}
      else if (mstring_equalPrefix (scomment, "function"))
	{
	  expectfunction = TRUE;
	}
      else if (mstring_equalPrefix (scomment, "constant"))
	{
	  expectconstant = TRUE;
	}
      else
	{
	  char sChar = *scomment;

	  if (sChar == '='
	      || sChar == '-'
	      || sChar == '+')
	    {
	      char *rest = scomment + 1;

	      if (mstring_equalPrefix (rest, "commentchar"))
		{
		  eliminateComment = TRUE;

		  if (sChar == '=')
		    {
		      ppllerror (cstring_makeLiteral
				 ("Cannot restore commentchar"));
		    }
		  else
		    {
		      char *next = scomment + 12; /* strlen commentchar = 12 */

		      if (*next != ' ' && *next != '\t' && *next != '\n')
			{
			  ppllerror
			    (message
			     ("Syntactic commentchar comment is not followed by a "
			      "whitespace character: %c",
			      *next));
			}
		      else
			{
			  char cchar = *(next + 1);

			  if (cchar == '\0')
			    {
			      ppllerror
				(cstring_makeLiteral
				 ("Cannot set commentchar to NUL"));
			    }
			  else
			    {
			      context_setCommentMarkerChar (cchar);
			      /* setComment = TRUE; */
			    }
			}
		    }
		}
	      else if (mstring_equalPrefix (scomment, "nestcomment"))
		{
		  /* fix from Mike Miller <MikeM@xata.com> */
		  context_fileSetFlag (FLG_NESTCOMMENT,
				       ynm_fromCodeChar (sChar),
				       loc);
		}
	      else if (mstring_equalPrefix (rest, "namechecks"))
		{
		  context_fileSetFlag (FLG_NAMECHECKS,
				       ynm_fromCodeChar (sChar),
				       loc);
		}
	      else if (mstring_equalPrefix (rest, "macroredef"))
		{
		  context_fileSetFlag (FLG_MACROREDEF,
				       ynm_fromCodeChar (sChar),
				       loc);
		}
	      else if (mstring_equalPrefix (rest, "usevarargs"))
		{
		  context_fileSetFlag (FLG_USEVARARGS,
				       ynm_fromCodeChar (sChar),
				       loc);
		}
	      else if (mstring_equalPrefix (rest, "nextlinemacros"))
		{
		  context_fileSetFlag (FLG_MACRONEXTLINE,
				       ynm_fromCodeChar (sChar),
				       loc);
		}
	      else if (mstring_equalPrefix (rest, "allmacros")
		       || mstring_equalPrefix (rest, "fcnmacros")
		       || mstring_equalPrefix (rest, "constmacros"))
		{
		  flagcode fl;

		  if (mstring_equalPrefix (rest, "allmacros"))
		    {
		      fl = FLG_ALLMACROS;
		    }
		  else if (mstring_equalPrefix (rest, "fcnmacros"))
		    {
		      fl = FLG_FCNMACROS;
		    }
		  else
		    {
		      llassert (mstring_equalPrefix (rest, "constmacros"));
		      fl = FLG_CONSTMACROS;
		    }

		  context_fileSetFlag (fl, ynm_fromCodeChar (sChar), loc);
		  notfunction = FALSE;
		}
	      else
		{
		  ;
		}
	    }
	  else
	    {
	      ;
	    }
	}

      if (eliminateComment)
	{
	  goto removeComment;
	}

      /* Replaces comment char's in start with spaces */

      for (i = 2; i < len - 2; i++)
	{
	  if (start[i] == BEFORE_COMMENT_MARKER[0]
	      || start[i] == BEFORE_COMMENT_MARKER[1]
	      || start[i] == context_getCommentMarkerChar ())
	    {
	      start[i] = ' ';
	    }
	}

      cppReader_putStrN (pfile, start, size_fromInt (len));
      parseClearMark (smark);
      return CPP_COMMENT;
    }
  else
    {
    removeComment:
      {
	int i;

	/*
	** Output the comment as all spaces so line/column
	** in output file is still correct.
	*/

	char c = ' ';
	cstring lintcomment = cstring_undefined;

	if (context_getFlag (FLG_LINTCOMMENTS))
	  {
	    if (mstring_equalPrefix (start, "*NOTREACHED*/"))
	      {
		lintcomment = cstring_makeLiteralTemp ("l_notreach");
	      }
	    else if (mstring_equalPrefix (start, "*PRINTFLIKE*/"))
	      {
		lintcomment = cstring_makeLiteralTemp ("l_printfli");
	      }
	    else if (mstring_equalPrefix (start, "*FALLTHROUGH*/"))
	      {
		lintcomment = cstring_makeLiteralTemp ("l_fallthrou");
	      }
	    else if (mstring_equalPrefix (start, "*ARGSUSED*/"))
	      {
		lintcomment = cstring_makeLiteralTemp ("l_argsus");
	      }
	    else if (mstring_equalPrefix (start, "*FALLTHRU*/"))
	      {
		lintcomment = cstring_makeLiteralTemp ("l_fallth");
	      }
	    else
	      {
		lintcomment = cstring_undefined;
	      }
	  }
	else
	  {
	    lintcomment = cstring_undefined;
	  }

	if (cstring_isDefined (lintcomment))
	  {
	    c = BEFORE_COMMENT_MARKER[0];
	    start[0] = BEFORE_COMMENT_MARKER[1];

	    llassert (size_toLong (cstring_length (lintcomment)) == len - 3);

	    for (i = 1; i < len - 2; i++)
	      {
		start[i] = cstring_getChar (lintcomment, size_fromInt (i));
	      }
	    
	    start[len - 2] = AFTER_COMMENT_MARKER[0];
	    start[len - 1] = AFTER_COMMENT_MARKER[1];
	  }
	else
	  {
	    /* Replaces  char's in start with spaces */
	    for (i = 0; i < len; i++)
	      {
		if (start[i] == '/'
		    && i < len - 1
		    && start[i + 1] == '*') {
		  (void) cppoptgenerror 
		    (FLG_NESTCOMMENT,
		     message ("Comment starts inside comment"),
		     pfile);
		}
		
		if (start[i] != '\n')
		  {
		    start[i] = ' ';
		  }
	      }
	  }

	cpplib_reserve (pfile, size_fromInt (1 + len));
	cppReader_putCharQ (pfile, c);
	cppReader_putStrN (pfile, start, size_fromInt (len));
	parseClearMark (smark);
	return CPP_COMMENT;
      }
    }
}

static int cpp_openIncludeFile (char *filename)
{
  int res = open (filename, O_RDONLY, 0666);

  /* evans 2001-08-23: was (res) - open returns -1 on error! reported by Robin Watts */
  if (res >= 0)
    {
      if (!fileTable_exists (context_fileTable (),
			     cstring_fromChars (filename)))
	{
	  if (fileloc_isXHFile (g_currentloc))
	    {
	      /*
	      ** Files includes by XH files are also XH files
	      */

	      (void) fileTable_addXHFile (context_fileTable (),
					  cstring_fromChars (filename));
	    }
	  else
	    {
	      (void) fileTable_addHeaderFile (context_fileTable (),
					      cstring_fromChars (filename));
	    }
	}
      else
	{
	  DPRINTF (("File already exists: %s", filename));
	}
    }

  return res;
}

static bool cpp_skipIncludeFile (cstring fname)
{
  if (context_isSystemDir (fname))
    {
      DPRINTF (("System dir: %s", fname));

      if (lcllib_isSkipHeader (fname))
	{
	  DPRINTF (("Skip include TRUE: %s", fname));
	  return TRUE;
	}
      
      if (context_getFlag (FLG_SKIPSYSHEADERS))
	{
	  /*
	  ** 2003-04-18: Patch from Randal Parsons
	  */

	  /*
	  ** Don't skip include file unless the file actually exists.  
	  ** It may be in a different directory.
	  */

	  int f = open (cstring_toCharsSafe (fname), O_RDONLY, 0666);

	  if (f >= 0)
	    {
	      check (close (f) == 0);
	      DPRINTF (("Skip include TRUE: %s", fname));
	      return TRUE;
	    }
	  else
	    {
	      /* Keep looking... */
	    }
	}
    }

  if (context_getFlag (FLG_SINGLEINCLUDE))
    {
      fname = removePreDirs (fname);

# if defined (WIN32) || defined (OS2)
      cstring_replaceAll (fname, '\\', '/');
# endif

      if (fileTable_exists (context_fileTable (), fname))
	{
	  DPRINTF (("Skip include TRUE: %s", fname));
	  return TRUE;
	}
    }

  DPRINTF (("Skip include FALSE: %s", fname));
  return FALSE;
}

static int cpp_peekN (cppReader *pfile, int n)
{
  cppBuffer *buf = cppReader_getBufferSafe (pfile);

  llassert (buf->cur != NULL);

  return (buf->rlimit - buf->cur >= (n)
	  ? buf->cur[n]
	  : EOF);
}

cppBuffer *cppBuffer_prevBuffer (cppBuffer *buf)
{
  return buf + 1;
}

void cppBuffer_forward (cppBuffer *buf, int n)
{
  llassert (buf->cur != NULL);
  buf->cur += n;
}

/*@=bufferoverflowhigh@*/
/*@=bounds@*/
