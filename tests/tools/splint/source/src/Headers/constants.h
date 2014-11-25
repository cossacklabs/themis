/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
**  constants.h
*/

/*
 * This is constants.h from Mike Smith's Win32 port of splint.
 * Modified by Herbert 04/19/97:
 * added conditional 'OS2' to conditional 'MSDOS'
 */

# ifndef CONSTANTS_H
# define CONSTANTS_H

# if defined(MSDOS) 
/*@constant observer char *RCFILE; @*/
# define RCFILE         "splint.rc"
# else
/*@constant observer char *RCFILE; @*/
# define RCFILE         ".splintrc"

/*@constant observer char *ALTRCFILE; @*/
# define ALTRCFILE      ".lclintrc"
# endif

/*@constant observer cstring LARCH_PATH; @*/
# define LARCH_PATH		cstring_makeLiteralTemp ("LARCH_PATH")

/*@constant observer char *LCLIMPORTDIR; @*/
# define LCLIMPORTDIR           "LCLIMPORTDIR"

/*@constant observer char *LLSTDLIBS_NAME; @*/
# define LLSTDLIBS_NAME          "standard"

/*@constant observer char *LLSTRICTLIBS_NAME; @*/
# define LLSTRICTLIBS_NAME      "standardstrict"

/*@constant observer char *LLUNIXLIBS_NAME; @*/
# define LLUNIXLIBS_NAME        "unix"

/*@constant observer char *LLUNIXSTRICTLIBS_NAME; @*/
# define LLUNIXSTRICTLIBS_NAME  "unixstrict"

/*@constant observer char *LLPOSIXLIBS_NAME; @*/
# define LLPOSIXLIBS_NAME       "posix"

/*@constant observer char *LLPOSIXSTRICTLIBS_NAME; @*/
# define LLPOSIXSTRICTLIBS_NAME "posixstrict"

/*@constant observer cstring REFSNAME; @*/
# define REFSNAME               cstring_makeLiteralTemp ("refs")

/*
** Used to be .lldmp.  Truncated to .lcd to fix within
** MS-DOS filename limitations.
*/

/*@constant observer char *DUMP_SUFFIX; @*/
# define DUMP_SUFFIX ".lcd"

/*
** All valid libraries start with this
*/

/*@constant observer char *LIBRARY_MARKER@*/
# define LIBRARY_MARKER ";;; Splint Library "

/*@constant int MAX_NAME_LENGTH=256; @*/
# define MAX_NAME_LENGTH 1024

/*@constant int MAX_LINE_LENGTH=1024; @*/
# define MAX_LINE_LENGTH 1024

/*@constant int MAX_DUMP_LINE_LENGTH=16384; @*/
# define MAX_DUMP_LINE_LENGTH 16384

/*@constant int MINLINELEN=20; @*/
# define MINLINELEN 20


/*
** WARNING: Can't use macros in token for cgrammar.l -->
**   must keep these consistent!
*/

/*@constant observer char *LLMRCODE; @*/
# define LLMRCODE "@QLMR"  
/* MUST be 5 chars = defin[e]. The number of spaces between the
   # and the d is the sixth tag. 
*/

/*@constant observer char *PPMRCODE; @*/
# define PPMRCODE "@@MR@@"  

# ifdef OS2
/*@constant observer char *DEFAULT_SYSTEMDIR; @*/
# define DEFAULT_SYSTEMDIR "/os2/"
# else                            
/*@constant observer char *DEFAULT_SYSTEMDIR; @*/
# define DEFAULT_SYSTEMDIR "/usr/"
# endif

/*@constant char DEFAULT_COMMENTCHAR; @*/
# define DEFAULT_COMMENTCHAR '@'

/*@constant int DEFAULT_LINELEN=80; @*/
# define DEFAULT_LINELEN 80

/*@constant int DEFAULT_BUGSLIMIT=3; @*/
# define DEFAULT_BUGSLIMIT 3

/*@constant int DEFAULT_INDENTSPACES=3; @*/
# define DEFAULT_INDENTSPACES 3

/*
** Setting this to 0 means emacs compile mode will
** jump to the locations in message hints.
*/

/*@constant int DEFAULT_LOCINDENTSPACES=3; @*/
# define DEFAULT_LOCINDENTSPACES 3

/*
** These constants are based on implementation limits in ANSI standard,
** Section 3.1. 
*/

/*@constant int ANSI89_EXTERNALNAMELEN=6; @*/
# define ANSI89_EXTERNALNAMELEN 6

/*@constant int ANSI89_INTERNALNAMELEN=31; @*/
# define ANSI89_INTERNALNAMELEN 31

/*@constant int ANSI89_CONTROLNESTDEPTH=15; @*/
# define ANSI89_CONTROLNESTDEPTH 15

/*@constant int ANSI89_STRINGLITERALLEN=509; @*/
# define ANSI89_STRINGLITERALLEN 509

/*@constant int ANSI89_INCLUDENEST=8; @*/
# define ANSI89_INCLUDENEST 8

/*@constant int ANSI89_NUMSTRUCTFIELDS=127; @*/
# define ANSI89_NUMSTRUCTFIELDS 127

/*@constant int ANSI89_NUMENUMMEMBERS=127; @*/
# define ANSI89_NUMENUMMEMBERS 127

/*
** From ISO C99 5.2.4.1
*/

/*@constant int ISO99_EXTERNALNAMELEN=31; @*/
# define ISO99_EXTERNALNAMELEN 31

/*@constant int ISO99_INTERNALNAMELEN=63; @*/
# define ISO99_INTERNALNAMELEN 63

/*@constant int ISO99_CONTROLNESTDEPTH=63; @*/
# define ISO99_CONTROLNESTDEPTH 63

/*@constant int ISO99_STRINGLITERALLEN=4095; @*/
# define ISO99_STRINGLITERALLEN 4095

/*@constant int ISO99_INCLUDENEST=63; @*/
# define ISO99_INCLUDENEST 63

/*@constant int ISO99_NUMSTRUCTFIELDS=1023 @*/
# define ISO99_NUMSTRUCTFIELDS 1023

/*@constant int ISO99_NUMENUMMEMBERS=1023 @*/
# define ISO99_NUMENUMMEMBERS 1023

/*@constant int DEFAULT_LIMIT=-1; @*/
# define DEFAULT_LIMIT -1    /* unlimited messages */

/*@constant char PFX_UPPERCASE; @*/
# define PFX_UPPERCASE '^'

/*@constant char PFX_LOWERCASE; @*/
# define PFX_LOWERCASE '&'

/*@constant char PFX_ANY; @*/
# define PFX_ANY '?'

/*@constant char PFX_DIGIT; @*/
# define PFX_DIGIT '#'

/*@constant char PFX_NOTUPPER; @*/
# define PFX_NOTUPPER '%'

/*@constant char PFX_NOTLOWER; @*/
# define PFX_NOTLOWER '~'

/*@constant char PFX_ANYLETTER; @*/
# define PFX_ANYLETTER '$'

/*@constant char PFX_ANYLETTERDIGIT; @*/
# define PFX_ANYLETTERDIGIT '/'
         
/*
** _Bool is defined by ISO C99 (replaced old lltx_Bool)
*/

/*@constant observer char *DEFAULT_BOOLTYPE;@*/
# define DEFAULT_BOOLTYPE "_Bool"

/*@constant observer char *PRAGMA_EXPAND; @*/
# define PRAGMA_EXPAND "expand"

/*@constant int PRAGMA_LEN_EXPAND=6; @*/
# define PRAGMA_LEN_EXPAND 6

/*@constant int MAX_PRAGMA_LEN; @*/
# define MAX_PRAGMA_LEN PRAGMA_LEN_EXPAND

/*
** Flex doesn't pre-process input, so remember to copy these manually
** to cscanner.l.
*/

/*@constant observer char *BEFORE_COMMENT_MARKER@*/
# define BEFORE_COMMENT_MARKER "%{"

/*@constant observer char *AFTER_COMMENT_MARKER@*/
# define AFTER_COMMENT_MARKER "%}"

# else
# error "Multiple include"
# endif
