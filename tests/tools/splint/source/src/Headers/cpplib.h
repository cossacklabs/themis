/* Definitions for CPP library.
   Copyright (C) 1995, 1996, 1997 Free Software Foundation, Inc.
   Written by Per Bothner, 1994-95.

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

#include <sys/types.h>
#include <sys/stat.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum cpp_token (*parseUnderflow) (cppReader *);
typedef void (*parseCleanup) (cppBuffer *, cppReader *);

/* Structure returned by create_definition */
typedef struct s_macrodef macroDef;

/* A parse_marker indicates a previous position,
   which we can backtrack to. */

struct parse_marker {
  /*@dependent@*/ cppBuffer *buf;
  /*@dependent@*/ /*@null@*/ struct parse_marker *next;
  int position;
};

/* The arglist structure is built by do_define to tell
   collect_definition where the argument names begin.  That
   is, for a define like "#define f(x,y,z) foo+x-bar*y", the arglist
   would contain pointers to the strings x, y, and z.
   Collect_definition would then build a DEFINITION node,
   with reflist nodes pointing to the places x, y, and z had
   appeared.  So the arglist is just convenience data passed
   between these two routines.  It is not kept around after
   the current #define has been processed and entered into the
   hash table.  */

struct arglist {
  /*@null@*/ struct arglist *next;
  /*@dependent@*/ char *name;
  size_t length;
  int argno;
  int rest_args;
};

extern enum cpp_token cpplib_getToken (cppReader *);
extern enum cpp_token cpplib_getTokenForceExpand (cppReader *);
extern enum cpp_token cpplib_getTokenAux (cppReader *, bool p_forceExpand);
extern int /*@alt void@*/ cppSkipHspace (cppReader *);

/* This frees resources used by PFILE. */
extern /*@unused@*/ void cppCleanup (cppReader *p_pfile);

struct cppBuffer {
  /*@null@*/ /*@only@*/ char *buf;
  /*@null@*/ /*@exposed@*/ char *cur;
  /*@null@*/ /*@exposed@*/ char *rlimit; /* end of valid data */
  /*@null@*/ /*@exposed@*/ char *alimit; /* end of allocated buffer */
  /*@null@*/ /*@exposed@*/ char *prev;

  /*@dependent@*/ cstring fname;

  /* Filename specified with #line command.  */
  /*@exposed@*/ cstring nominal_fname;

  /* Record where in the search path this file was found.
     For #include_next.  */

  /*@dependent@*/ /*@null@*/ struct file_name_list *dir;

  long line_base;
  int lineno; /* Line number at CPP_LINE_BASE. */
  int colno; /* Column number at CPP_LINE_BASE. */
  parseUnderflow underflow;
  parseCleanup cleanup;

  /*@dependent@*/ hashNode hnode;
  /*@dependent@*/ /*@null@*/ struct parse_marker *marks;
  /* Value of if_stack at start of this file.
     Used to prohibit unmatched #endif (etc) in an include file.  */
  /*@null@*/ /*@exposed@*/ struct if_stack *if_stack;

  /* True if this is a header file included using <FILENAME>.  */
  char system_header_p;
  char seen_eof;

  /* True if buffer contains escape sequences.
     Currently there are three kinds:
     "@-" means following identifier should not be macro-expanded.
     "@ " means a token-separator.  This turns into " " in final output
          if not stringizing and needed to separate tokens; otherwise nothing.
     "@@" means a normal '@'.
     (An '@' inside a string stands for itself and is never an escape.) */
  bool has_escapes;
};

struct cpp_pending;  /* Forward declaration - for C++. */
struct file_name_map_list;

/* Maximum nesting of cppBuffers.  We use a static limit, partly for
   efficiency, and partly to limit runaway recursion.  */

/*@constant int CPP_STACK_MAX; @*/
# define CPP_STACK_MAX 200

/* A cppReader encapsulates the "state" of a pre-processor run.
   Applying cppGetToken repeatedly yields a stream of pre-processor
   tokens.  Usually, there is only one cppReader object active. */

struct cppReader {
  parseUnderflow get_token;
  /*@dependent@*/ /*@null@*/ cppBuffer *buffer;
  cppBuffer buffer_stack[CPP_STACK_MAX];

  int errors;			/* Error counter for exit code */
  cppOptions *opts;
  /* void *data;  */

  /* A buffer used for both for cppGetToken's output, and also internally. */
  /*@relnull@*/ char *token_buffer;

  /* Alocated size of token_buffer.  cppReader_reserve allocates space.  */
  size_t token_buffer_size;
  /* End of the written part of token_buffer. */
  /*@exposed@*/ char *limit;

  /* Line where a newline was first seen in a string constant.  */
  int multiline_string_line;

  /* Current depth in #include directives that use <...>.  */
  int system_include_depth;

  /* List of other included files.
     If ->control_macro if nonzero, the file had a #ifndef
     around the entire contents, and ->control_macro gives the macro name.  */
  /*@owned@*/ /*@null@*/ struct file_name_list *all_include_files;

  /* Current maximum length of directory names in the search path
     for include files.  (Altered as we get more of them.)  */
  size_t max_include_len;

  /*@null@*/ struct if_stack *if_stack;

  /* Nonzero means we are inside an IF during a -pcp run.  In this mode
     macro expansion is done, and preconditions are output for all macro
     uses requiring them. */
  char pcp_inside_if;

  /* Nonzero means we have printed (while error reporting) a list of
     containing files that matches the current status. */
  char input_stack_listing_current;

  /* If non-zero, macros are not expanded. */
  bool no_macro_expand;

  /* Print column number in error messages. */
  bool show_column;

  /* If true, character between '<' and '>' are a single (string) token. */
  char parsing_include_directive;

  /* True if escape sequences (as described for has_escapes in
     parse_buffer) should be emitted. */
  char output_escapes;

  /* 0: Have seen non-white-space on this line.
     1: Only seen white space so far on this line.
     2: Only seen white space so far in this file. */
  char only_seen_white;

  int lineno;

  /*@null@*/ /*@observer@*/ struct tm *timebuf;
};

/*@constant int cppReader_fatalErrorLimit; @*/
#define cppReader_fatalErrorLimit 1000

/* True if we have seen a "fatal" error. */
extern bool cpplib_fatalErrors (cppReader *) /*@*/ ; 
#define cpplib_fatalErrors(READER) ((READER)->errors >= cppReader_fatalErrorLimit)

extern int cpplib_bufPeek (cppBuffer *) /*@*/ ;

/* Macros for manipulating the token_buffer. */

/*@notfunction@*/
#define CPP_OUT_BUFFER(PFILE) ((PFILE)->token_buffer)

/* Number of characters currently in PFILE's output buffer. */

extern size_t cpplib_getWritten (/*@sef@*/ cppReader *) /*@*/ ;
# define cpplib_getWritten(PFILE) \
  (size_fromInt ((PFILE)->limit - (PFILE)->token_buffer))

extern /*@exposed@*/ char *cpplib_getPWritten (cppReader *) /*@*/ ;
# define cpplib_getPWritten(PFILE) ((PFILE)->limit)

extern /*@null@*/ macroDef 
cpplib_createDefinition (/*@dependent@*/ cstring p_def, fileloc p_loc,
			 bool p_predefinition, bool p_noExpand) ;
  
/* Make sure PFILE->token_buffer has space for at least N more characters. */

extern void cpplib_reserve (/*@sef@*/ cppReader *, /*@sef@*/ size_t);
#define cpplib_reserve(PFILE, N) \
  (((cpplib_getWritten (PFILE) + (N) > (PFILE)->token_buffer_size)) ? cppReader_growBuffer (PFILE, (N)) : (void) 0)

/* Append string STR (of length N) to PFILE's output buffer.
   Assume there is enough space. */

extern void cppReader_putStrN (/*@sef@*/ cppReader *p_file, 
		        /*@unique@*/ char *p_str, /*@sef@*/ size_t p_n) 
               /*@modifies *p_file; @*/;

#define cppReader_putStrN(PFILE, STR, N) \
  (memcpy ((PFILE)->limit, STR, (N)), (PFILE)->limit += (N))

extern void cppReader_setWritten (/*@sef@*/ /*@special@*/ cppReader *p_file, size_t)
  /*@uses p_file, *p_file, p_file->token_buffer;@*/
  /*@sets p_file->limit;@*/
  /*@modifies *p_file@*/ ;

# define cppReader_setWritten(PFILE,N) \
  ((PFILE)->limit = (PFILE)->token_buffer + (N))

extern /*@dependent@*/ /*@exposed@*/ cppOptions *CPPOPTIONS (/*@special@*/ cppReader *p_pfile)
  /*@uses p_pfile->opts@*/ ;
#define CPPOPTIONS(PFILE) ((PFILE)->opts)

/*@notfunction@*/
#define CPPBUFFER(PFILE) ((PFILE)->buffer)

/* Checks for null */
extern /*@exposed@*/ cppBuffer *
cppReader_getBufferSafe (/*@special@*/ cppReader *p_pfile)
       /*@uses p_pfile->buffer@*/
       /*@modifies nothing@*/ ;

extern /*@exposed@*/ cppBuffer *cppBuffer_prevBuffer (cppBuffer *) /*@*/ ;

/* The bottom of the buffer stack. */
extern /*@exposed@*/ cppBuffer *cppReader_nullBuffer (/*@special@*/ cppReader *p_pfile) /*@uses p_pfile->buffer_stack@*/ /*@*/ ;
# define cppReader_nullBuffer(PFILE) (&(PFILE)->buffer_stack[CPP_STACK_MAX])

/* Pointed to by cppReader::data. */
struct cppOptions {
  /*@dependent@*/ cstring in_fname;
  
  /* Name of output file, for error messages.  */
  /*@dependent@*/ cstring out_fname;

  /*@only@*/ struct file_name_map_list *map_list;

  /* Non-0 means -v, so print the full set of include dirs.  */
  bool verbose;

  /* Nonzero means use extra default include directories for C++.  */

  bool cplusplus;

  /* Nonzero means handle cplusplus style comments */

  bool cplusplus_comments;

  /* Nonzero means this is an assembly file, and allow
     unknown directives, which could be comments.  */

  int lang_asm;

  /* Nonzero means turn NOTREACHED into #pragma NOTREACHED etc */

  bool for_lint;

  /* Nonzero means handle CHILL comment syntax
     and output CHILL string delimiter for __DATE___ etc. */

  bool chill;

  /* Nonzero means copy comments into the output file.  */

  bool put_out_comments;

  /* Nonzero means don't process the ANSI trigraph sequences.  */

  bool no_trigraphs;

  /* Nonzero means print names of header files (-H).  */

  bool print_include_names;

  /* Nonzero means try to make failure to fit ANSI C an error.  */

  bool pedantic_errors;

  /* Nonzero means don't print warning messages.  -w.  */
  /* bool inhibit_warnings; -- removed evans 2001-07-19 */

  /* Nonzero means warn if slash-star appears in a comment.  */
  bool warn_comments;

  /* Nonzero means warn if a macro argument is (or would be)
     stringified with -traditional.  */

  bool warn_stringify;
  bool warnings_are_errors;
  bool no_output;

  /* Nonzero means don't output line number information.  */

  bool no_line_commands;

/* Nonzero means output the text in failing conditionals,
   inside #failed ... #endfailed.  */

  char output_conditionals;

  bool ignore_srcdir;

  /* Zero means dollar signs are punctuation.
     This used to be needed for conformance to the C Standard,
     before the C Standard was corrected.  */
  bool dollars_in_ident;

  /* Nonzero means try to imitate old fashioned non-ANSI preprocessor.  */
  bool traditional;

  /* Nonzero for the 1989 C Standard, including corrigenda and amendments.  */
  bool c89;

  /* Nonzero means give all the error messages the ANSI standard requires.  */
  bool pedantic;

  bool done_initializing;

  /* First dir to search */
  /*@owned@*/ struct file_name_list *include;

  /* First dir to search for <file> */
  /* This is the first element to use for #include <...>.
     If it is 0, use the entire chain for such includes.  */
  /*@dependent@*/ struct file_name_list *first_bracket_include;
  /* This is the first element in the chain that corresponds to
     a directory of system header files.  */
  /*@dependent@*/ struct file_name_list *first_system_include;
  /*@exposed@*/ struct file_name_list *last_include;	/* Last in chain */

  /* Chain of include directories to put at the end of the other chain.  */
  struct file_name_list *after_include;
  /*@exposed@*/ struct file_name_list *last_after_include;	/* Last in chain */

  /* Chain to put at the start of the system include files.  */
  struct file_name_list *before_system;
  /*@exposed@*/ struct file_name_list *last_before_system;	/* Last in chain */

  /* Directory prefix that should replace `/usr' in the standard
     include file directories.  */
  char *include_prefix;

  char inhibit_predefs;
  char no_standard_includes;
  char no_standard_cplusplus_includes;

  /*
  ** DUMP_NAMES means pass #define and the macro name through to output.
  ** DUMP_DEFINITIONS means pass the whole definition (plus #define) through
  */

  /*@-enummemuse@*/
  enum { DUMP_NONE = 0, DUMP_NAMES, DUMP_DEFINITIONS }
     dump_macros;
  /*@=enummemuse@*/

/* Nonzero means pass all #define and #undef directives which we actually
   process through to the output stream.  This feature is used primarily
   to allow cc1 to record the #defines and #undefs for the sake of
   debuggers which understand about preprocessor macros, but it may
   also be useful with -E to figure out how symbols are defined, and
   where they are defined.  */
  int debug_output;
};

extern bool cppReader_isTraditional (/*@special@*/ cppReader *p_pfile)
    /*@uses p_pfile->opts@*/
   /*@modifies nothing@*/ ; 

#define cppReader_isTraditional(PFILE) (CPPOPTIONS(PFILE)->traditional)

extern bool cppReader_isPedantic (cppReader *) /*@*/; 
#define cppReader_isPedantic(PFILE) (CPPOPTIONS (PFILE)->pedantic)

/* The structure of a node in the hash table.  The hash table
   has entries for all tokens defined by #define commands (type T_MACRO),
   plus some special tokens like __LINE__ (these each have their own
   type, and the appropriate code is run when that type of node is seen.
   It does not contain control words like "#define", which are recognized
   by a separate piece of code. */

/* different flavors of hash nodes --- also used in keyword table */
enum node_type {
 T_NONE = 0,
 T_DEFINE = 1,	/* the #define keyword */
 T_INCLUDE,	/* the #include keyword */
 T_INCLUDE_NEXT, /* the #include_next keyword */
 T_IFDEF,	/* the #ifdef keyword */
 T_IFNDEF,	/* the #ifndef keyword */
 T_IF,		/* the #if keyword */
 T_ELSE,	/* #else */
 T_PRAGMA,	/* #pragma */
 T_ELIF,	/* #elif */
 T_UNDEF,	/* #undef */
 T_LINE,	/* #line */
 T_ERROR,	/* #error */
 T_WARNING,	/* #warning */
 T_ENDIF,	/* #endif */
 T_IDENT,	/* #ident, used on system V.  */
 T_SPECLINE,	/* special symbol __LINE__ */
 T_DATE,	/* __DATE__ */
 T_FILE,	/* __FILE__ */
 T_BASE_FILE,	/* __BASE_FILE__ */
 T_INCLUDE_LEVEL, /* __INCLUDE_LEVEL__ */
 T_FUNC,        /* __func__ */
 T_VERSION,	/* __VERSION__ */
 T_SIZE_TYPE,   /* __SIZE_TYPE__ */
 T_PTRDIFF_TYPE,   /* __PTRDIFF_TYPE__ */
 T_WCHAR_TYPE,   /* __WCHAR_TYPE__ */
 T_USER_LABEL_PREFIX_TYPE, /* __USER_LABEL_PREFIX__ */
 T_REGISTER_PREFIX_TYPE,   /* __REGISTER_PREFIX__ */
 T_TIME,	/* __TIME__ */
 T_CONST,	/* Constant value, used by __STDC__ */
 T_MACRO,	/* macro defined by #define */
 T_DISABLED,	/* macro temporarily turned off for rescan */
 T_SPEC_DEFINED, /* special defined macro for use in #if statements */
 T_PCSTRING,	/* precompiled string (hashval is KEYDEF *) */
 T_UNUSED	/* Used for something not defined.  */
} ;

struct s_macrodef
{
  /*@null@*/ struct definition *defn;
  /*@exposed@*/ /*@relnull@*/ char *symnam; /* null if defn is null */
  size_t symlen;
};

/* Structure allocated for every #define.  For a simple replacement
   such as
   	#define foo bar ,
   nargs = -1, the `pattern' list is null, and the expansion is just
   the replacement text.  Nargs = 0 means a functionlike macro with no args,
   e.g.,
       #define getchar() getc (stdin) .
   When there are args, the expansion is the replacement text with the
   args squashed out, and the reflist is a list describing how to
   build the output from the input: e.g., "3 chars, then the 1st arg,
   then 9 chars, then the 3rd arg, then 0 chars, then the 2nd arg".
   The chars here come from the expansion.  Whatever is left of the
   expansion after the last arg-occurrence is copied after that arg.
   Note that the reflist can be arbitrarily long---
   its length depends on the number of times the arguments appear in
   the replacement text, not how many args there are.  Example:
   #define f(x) x+x+x+x+x+x+x would have replacement text "++++++" and
   pattern list
     { (0, 1), (1, 1), (1, 1), ..., (1, 1), NULL }
   where (x, y) means (nchars, argno). */

typedef struct definition DEFINITION;

struct definition {
  int nargs;
  size_t length;			/* length of expansion string */
  bool predefined;		/* True if the macro was builtin or */
                                /* came from the command line */
  /*@dependent@*/ char *expansion;
  long line;			/* Line number of definition */
  /*@exposed@*/ cstring file;	/* File of definition */

  bool noExpand;                /* True if macro should not be expanded in code. */
  bool rest_args;		/* Nonzero if last arg. absorbs the rest */
  
  /*@null@*/ struct reflist {
    /*@null@*/ /*@dependent@*/ struct reflist *next;
    bool stringify;		/* nonzero if this arg was preceded by a
				   # operator. */
    bool raw_before;		/* Nonzero if a ## operator before arg. */
    bool raw_after;		/* Nonzero if a ## operator after arg. */
    bool rest_args;		/* Nonzero if this arg. absorbs the rest */
    int nchars;			/* Number of literal chars to copy before
				   this arg occurrence.  */
    int argno;			/* Number of arg to substitute (origin-0) */
  } *pattern;

  union {
    /* Names of macro args, concatenated in reverse order
       with comma-space between them.
       The only use of this is that we warn on redefinition
       if this differs between the old and new definitions.  */
    /*@null@*/ char *argnames;
  } args;
};

/* Stack of conditionals currently in progress
   (including both successful and failing conditionals).  */

struct if_stack {
  /*@null@*/ struct if_stack *next;  /* for chaining to the next stack frame */
  /*@observer@*/ cstring fname;     /* copied from input when frame is made */
  int lineno;			/* similarly */
  int if_succeeded;		/* true if a leg of this if-group
				    has been passed through rescan */

  /* For #ifndef at start of file, this is the macro name tested.  */
  /*@null@*/ /*@dependent@*/ char *control_macro;
  
				   
  enum node_type type;		/* type of last directive seen in this group */
};
typedef struct if_stack cppIfStackFrame;

extern void cppBuffer_getLineAndColumn (/*@null@*/ cppBuffer *, /*@out@*/ int *,
					/*@out@*/ /*@null@*/ int *);
extern /*@exposed@*/ /*@null@*/ cppBuffer *cppReader_fileBuffer (cppReader *);

extern void cppReader_growBuffer (cppReader *, size_t);
extern int cppReader_parseEscape (cppReader *, char **);

extern /*@exposed@*/ cppBuffer *cppReader_popBuffer (cppReader *p_pfile)
    /*@modifies p_pfile@*/ ;

#ifdef __cplusplus
}
#endif

extern void cppReader_skipRestOfLine (cppReader *p_pfile);

# include <stdlib.h>

/*@constant observer char *GCC_INCLUDE_DIR;@*/
/* This is defined by config.h now. */

/*@constant observer char *GCC_INCLUDE_DIR2@*/
/* This is defined by config.h now. */

struct file_name_list
{
  /*@owned@*/ /*@null@*/ struct file_name_list *next;
  /*@dependent@*/ cstring fname;

  /* If the following is nonzero, it is a macro name.
     Don't include the file again if that macro is defined.  */

  /*@dependent@*/ /*@null@*/ char *control_macro;
  /* If the following is nonzero, it is a C-language system include
     directory.  */

  bool c_system_include_path;

  /* Mapping of file names for this directory.  */
  /*@exposed@*/ /*@relnull@*/ /*@reldef@*/ struct file_name_map *name_map;

  /* Non-zero if name_map is valid.  */
  bool got_name_map;
};

extern void cppReader_addIncludeChain (/*@special@*/ cppReader *p_pfile, 
				       /*@only@*/ struct file_name_list *p_dir)
     /*@uses p_pfile->opts, p_pfile->max_include_len@*/
     /*@modifies p_pfile, p_dir@*/ ;

extern void cppReader_define (cppReader *p_pfile, char *p_str);
extern void cppReader_finish (cppReader *p_pfile);
extern void cpplib_init (/*@out@*/ cppReader *p_pfile) ; 
extern void cppOptions_init (/*@out@*/ cppOptions *p_opts);
extern void cpplib_initializeReader (cppReader *p_pfile) /*@modifies p_pfile@*/ ;

extern int cppReader_startProcess (cppReader *p_pfile, cstring p_fname);

extern bool isIdentifierChar (char) /*@*/ ;

/* Find the largest host integer type and set its size and type.  */

#ifndef HOST_BITS_PER_WIDE_INT

#if HOST_BITS_PER_LONG > HOST_BITS_PER_INT
/*@notfunction@*/
#define HOST_BITS_PER_WIDE_INT HOST_BITS_PER_LONG
/*@notfunction@*/
#define HOST_WIDE_INT long
#else
/*@notfunction@*/
#define HOST_BITS_PER_WIDE_INT HOST_BITS_PER_INT
/*@notfunction@*/
#define HOST_WIDE_INT long
/* was int */
#endif

#endif

#ifndef S_ISREG
/*@-macrounrecog@*/
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
/*@=macrounrecog@*/
#endif

#ifndef S_ISDIR
/*@-macrounrecog@*/
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
/*@=macrounrecog@*/
#endif

#ifndef INCLUDE_LEN_FUDGE
/*@constant int INCLUDE_LEN_FUDGE@*/
#define INCLUDE_LEN_FUDGE 0
#endif

extern size_t cppReader_checkMacroName (cppReader *p_pfile, char *p_symname, 
					cstring p_usage);

extern struct operation cppReader_parseNumber (cppReader * p_pfile, char * p_start, int p_olen)  /*@requires maxRead(p_start) >= (p_olen - 1) @*/;

