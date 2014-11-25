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
** cppexp.c
*/
/* Parse C expressions for CCCP.
   Copyright (C) 1987, 1992, 1994, 1995, 1997 Free Software Foundation.

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
Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.

 In other words, you are welcome to use, share and improve this program.
 You are forbidden to forbid anyone else to use, share and improve
 what you give them.   Help stamp out software-hoarding!

Written by Per Bothner 1994.  */

/* Parse a C expression from text in a string  */

/*@+charint@*/
/*@+ignorequals@*/
/*@+ignoresigns@*/
/*@+matchanyintegral@*/

# include <string.h> 
# include "splintMacros.nf"
# include "basic.h"
# include "cpplib.h"
# include "cpphash.h"
# include "cppexp.h"
# include "cpperror.h"

/* Yield nonzero if adding two numbers with A's and B's signs can yield a
   number with SUM's sign, where A, B, and SUM are all C integers.  */

/*@function static bool possibleSumSign (sef int, int, int) 
      modifies nothing ; @*/

#define possibleSumSign(a, b, sum) ((((a) ^ (b)) | ~ ((a) ^ (sum))) < 0)

/* these are guesses! */ 

/*@constant int BITS_PER_UNIT = 8@*/
# define BITS_PER_UNIT 8

/*@constant size_t BITS_PER_CHAR@*/
# define BITS_PER_CHAR 8

/*@constant size_t BITS_PER_WORD@*/
# define BITS_PER_WORD 32

/*@constant size_t HOST_BITS_PER_INT@*/
# define HOST_BITS_PER_INT 32

/*@constant size_t HOST_BITS_PER_LONG = 32@*/
# define HOST_BITS_PER_LONG 32

/*@constant char TARGET_BELL@*/
# define TARGET_BELL (char) 6 

/*@constant char TARGET_BS@*/
# define TARGET_BS   (char) 7

/*@constant char TARGET_FF@*/
# define TARGET_FF   (char) 8

/*@constant char TARGET_NEWLINE@*/
# define TARGET_NEWLINE '\n' 

/*@constant char TARGET_CR@*/
# define TARGET_CR '\n'

/*@constant char TARGET_TAB@*/
# define TARGET_TAB '\t'

/*@constant char TARGET_VT@*/
# define TARGET_VT '\v'

#ifdef MULTIBYTE_CHARS
#include <stdlib.h>
#include <locale.h>
#endif

#include <stdio.h>

#ifndef INT_TYPE_SIZE
/*@constant size_t INT_TYPE_SIZE@*/
#define INT_TYPE_SIZE BITS_PER_WORD
#endif

#ifndef LONG_TYPE_SIZE
/*@constant size_t LONG_TYPE_SIZE@*/
#define LONG_TYPE_SIZE BITS_PER_WORD
#endif

#ifndef WCHAR_TYPE_SIZE
/*@constant size_t WCHAR_TYPE_SIZE@*/
#define WCHAR_TYPE_SIZE INT_TYPE_SIZE
#endif

# ifndef CHAR_TYPE_SIZE
/*@constant size_t CHAR_TYPE_SIZE@*/
# define CHAR_TYPE_SIZE BITS_PER_CHAR
# endif

#ifndef MAX_CHAR_TYPE_SIZE
/*@constant size_t MAX_CHAR_TYPE_SIZE@*/
#define MAX_CHAR_TYPE_SIZE CHAR_TYPE_SIZE
#endif

#ifndef MAX_LONG_TYPE_SIZE
/*@constant size_t MAX_LONG_TYPE_SIZE@*/
#define MAX_LONG_TYPE_SIZE LONG_TYPE_SIZE
#endif

#ifndef MAX_WCHAR_TYPE_SIZE
/*@constant size_t MAX_WCHAR_TYPE_SIZE@*/
#define MAX_WCHAR_TYPE_SIZE WCHAR_TYPE_SIZE
#endif

static struct operation cppexp_lex (cppReader *);
static void integer_overflow (cppReader *);
static long left_shift (cppReader *, long, bool p_unsignedp, unsigned long);
static long right_shift (long, bool p_unsignedp, unsigned long);

/*@constant short CPPREADER_ERRORTOK@*/
#define CPPREADER_ERRORTOK 299

/*@constant int OROR@*/
#define OROR 300

/*@constant int ANDAND@*/
#define ANDAND 301

/*@constant int CPP_EQUALTOK@*/
#define CPP_EQUALTOK 302

/*@constant int NOTEQUAL@*/
#define NOTEQUAL 303

/*@constant int LEQ@*/
#define LEQ 304

/*@constant int GEQ@*/
#define GEQ 305

/*@constant int LSH@*/
#define LSH 306

/*@constant int RSH@*/
#define RSH 307

/*@constant int NAME@*/
#define NAME 308

/*@constant short CPPEXP_INT@*/
#define CPPEXP_INT 309

/*@constant short CPPEXP_CHAR@*/
#define CPPEXP_CHAR 310

/*@constant int LEFT_OPERAND_REQUIRED@*/
#define LEFT_OPERAND_REQUIRED 1

/*@constant int RIGHT_OPERAND_REQUIRED@*/
#define RIGHT_OPERAND_REQUIRED 2

/*@constant int HAVE_VALUE@*/
#define HAVE_VALUE 4

#ifndef HOST_BITS_PER_WIDE_INT

#if HOST_BITS_PER_LONG > HOST_BITS_PER_INT
/*@constant int HOST_BITS_PER_WIDE_INT@*/
#define HOST_BITS_PER_WIDE_INT HOST_BITS_PER_LONG
/*@notfunction@*/
#define HOST_WIDE_INT long
#else
/*@constant int HOST_BITS_PER_WIDE_INT@*/
#define HOST_BITS_PER_WIDE_INT HOST_BITS_PER_INT
/*@notfunction@*/
#define HOST_WIDE_INT long
#endif

#endif

struct operation {
  short op;

  /* Priority of op (relative to it right operand).  */
  /*@reldef@*/ char rprio;

  /*@reldef@*/ char flags;

  /* true if value should be treated as unsigned */
  /*@reldef@*/ bool unsignedp;

  /* The value logically "right" of op.  */
  /*@reldef@*/ HOST_WIDE_INT value;
} ;

/* Take care of parsing a number (anything that starts with a digit).
   LEN is the number of characters in it.  */

/* maybe needs to actually deal with floating point numbers */

struct operation
cppReader_parseNumber (cppReader *pfile, char *start, int olen) /*@requires maxRead(start) >= (olen - 1) @*/
{
  struct operation op;
  char *p = start;
  char c;
  int i;
  long n = 0;
  unsigned long nd, ULONG_MAX_over_base;
  int base = 10;
  int len = olen;
  bool overflow = FALSE;
  int digit, largest_digit = 0;
  bool spec_long = FALSE;

  op.unsignedp = FALSE;

  for (i = 0; i < len; i++)
    {
       if (p[i] == '.') {
	/* It's a float since it contains a point.  */
	cppReader_errorLit
	  (pfile,
	   cstring_makeLiteralTemp
	   ("Floating point numbers not allowed in #if expressions"));
	op.op = CPPREADER_ERRORTOK;
	return op;
      }
    }
      
  if (len >= 3 && (mstring_equalPrefix (p, "0x") 
		   || mstring_equalPrefix (p, "0X")))
    {
      p += 2;
      base = 16;
      len -= 2;
    }
  else if (*p == '0')
    {
      base = 8;
    }
  else
    {
      ;
    }

  /* Some buggy compilers (e.g. MPW C) seem to need both casts.  */
  ULONG_MAX_over_base = ((unsigned long) -1) / ((unsigned long) base);

  for (; len > 0; len--) {
    c = *p++;

    if (c >= '0' && c <= '9')
      {
	digit = (int) (c - '0');
      }
    else if (base == 16 && c >= 'a' && c <= 'f')
      {
	digit = (int) (c - 'a') + 10;
      }
    else if (base == 16 && c >= 'A' && c <= 'F')
      {
	digit = (int) (c - 'A') + 10;
      }
    else 
      {
	/* `l' means long, and `u' means unsigned.  */
	while (TRUE)
	  {
	    if (c == 'l' || c == 'L')
	      {
		if (spec_long)
		  cppReader_errorLit (pfile,
				      cstring_makeLiteralTemp ("two `l's in integer constant"));
		spec_long = TRUE;
	      }
	    else if (c == 'u' || c == 'U')
	      {
		if (op.unsignedp)
		  cppReader_errorLit (pfile, 
				      cstring_makeLiteralTemp ("two `u's in integer constant"));
		op.unsignedp = TRUE;
	      }
	    else
	      {
		/*@innerbreak@*/ break;
	      }
	    
	    if (--len == 0)
	      {
		/*@innerbreak@*/ break;
	      }

	     c = *p++;
	  }
	/* Don't look for any more digits after the suffixes.  */
	break;
      }
    
    if (largest_digit < digit)
      {
	largest_digit = digit;
      }
    
    nd = (long unsigned) (n * base + digit);
    overflow |= (ULONG_MAX_over_base < (unsigned long) n) 
      | (nd < (unsigned long) n);
    n = (long) nd;
  }

  if (len != 0)
    {
      cppReader_errorLit 
	(pfile, 
	 cstring_makeLiteralTemp ("Invalid number in #if expression"));
      op.op = CPPREADER_ERRORTOK;
      return op;
    }
  
  if (base <= largest_digit)
    {
      cppReader_pedwarnLit 
	(pfile, 
	 cstring_makeLiteralTemp 
	 ("Integer constant contains digits beyond the radix"));
    }
  
  if (overflow)
    {
      cppReader_pedwarnLit
	(pfile, 
	 cstring_makeLiteralTemp ("Integer constant out of range"));
    }

  /* If too big to be signed, consider it unsigned.  */
  if ((long) n < 0 && ! op.unsignedp)
    {
      if (base == 10)
	{
	  cppReader_warningLit
	    (pfile,
	     cstring_makeLiteralTemp ("Integer constant is so large that it is unsigned"));
	}
	 
      op.unsignedp = TRUE;
    }
  
  op.value = n;
  op.op = CPPEXP_INT;
  DPRINTF (("Parse number: %d", op.value));
  return op;
}

struct token {
  /*@null@*/ /*@observer@*/ char *operator;
  int token;
};

static struct token tokentab2[] = {
  { "&&", ANDAND },
  { "||", OROR },
  { "<<", LSH },
  { ">>", RSH },
  { "==", CPP_EQUALTOK },
  { "!=", NOTEQUAL },
  { "<=", LEQ },
  { ">=", GEQ },
  { "++", CPPREADER_ERRORTOK },
  { "--", CPPREADER_ERRORTOK },
  { NULL, CPPREADER_ERRORTOK }
} ;

/* Read one token.  */

struct operation cppexp_lex (cppReader *pfile)
{
  int ic;
  char c;
  register struct token *toktab;
  enum cpp_token token;
  struct operation op;
  char *tok_start, *tok_end;
  int old_written;

 retry:
  
  old_written = size_toInt (cpplib_getWritten (pfile));
  cppSkipHspace (pfile);
  ic = cpplib_bufPeek (cppReader_getBufferSafe (pfile));

  c = (char) ic;

  if  (c == '#') 
    {
      /* was: llassert (c != '#'); - Solaris uses this, attempt to continue anyway... */
      cppReader_pedwarn (pfile, 
			 message ("non-standard pre-processor directive: %c", c));
    }

  DPRINTF (("Read: %c", c));

  if (c == '\n')
    {
      op.op = 0;
      return op;
    }

  token = cpplib_getTokenForceExpand (pfile);

  tok_start = pfile->token_buffer + old_written;
  tok_end = cpplib_getPWritten (pfile);

  DPRINTF (("Token: %s < %s", tok_start, tok_end));

  pfile->limit = tok_start;

  switch (token)
    {
    case CPP_EOF: /* Should not happen ...  */
    case CPP_VSPACE:
      op.op = 0;
      return op;
    case CPP_POP:
      if (cstring_isDefined (cppReader_getBufferSafe (pfile)->fname))
	{
	  op.op = 0;
	  return op;
	}
      (void) cppReader_popBuffer (pfile);
      goto retry;
    case CPP_HSPACE:   case CPP_COMMENT: 
      goto retry;
    case CPP_NUMBER:
      return cppReader_parseNumber (pfile, tok_start, tok_end - tok_start);
    case CPP_STRING:
      cppReader_errorLit (pfile, 
			  cstring_makeLiteralTemp ("string constants not allowed in #if expressions"));
      op.op = CPPREADER_ERRORTOK;
      return op;
    case CPP_CHAR:
      /* This code for reading a character constant
	 handles multicharacter constants and wide characters.
	 It is mostly copied from c-lex.c.  */
      {
        int result = 0;
	int num_chars = 0;
	size_t width = MAX_CHAR_TYPE_SIZE;
	int wide_flag = 0;
	int max_chars;
	char *ptr = tok_start;
#ifdef MULTIBYTE_CHARS
	char token_buffer[MAX_LONG_TYPE_SIZE/MAX_CHAR_TYPE_SIZE + MB_CUR_MAX];
#else
	char token_buffer[MAX_LONG_TYPE_SIZE/MAX_CHAR_TYPE_SIZE + 1];
#endif
	
	if (*ptr == 'L')
	  {
	    ptr++;
	    wide_flag = 1;
	    width = MAX_WCHAR_TYPE_SIZE;
#ifdef MULTIBYTE_CHARS
	    max_chars = MB_CUR_MAX;
#else
	    max_chars = 1;
#endif
	  }
	else
	  {
	    max_chars = size_toInt (MAX_LONG_TYPE_SIZE / width);
	  }
	
	++ptr;
	 while (ptr < tok_end && ((c = *ptr++) != '\''))
	  {
	    if (c == '\\')
	      {
		c = cppReader_parseEscape (pfile, &ptr);
		if (width < HOST_BITS_PER_INT && c >= (1 << width))
		  {
		    cppReader_pedwarnLit 
		      (pfile,
		       cstring_makeLiteralTemp ("Escape sequence out of range for character"));
		  }
	      }
		
	    num_chars++;
	    
	    /* Merge character into result; ignore excess chars.  */
	    if (num_chars < max_chars + 1)
	      {
	        if (width < HOST_BITS_PER_INT)
		  {
		    result = (int) ((unsigned) result << width) | (c & ((1 << width) - 1));
		  }
		else
		  {
		    result = c;
		  }

		token_buffer[num_chars - 1] = c;
	      }
	  }

	 token_buffer[num_chars] = 0;

	if (c != '\'')
	  cppReader_errorLit (pfile,
			      cstring_makeLiteralTemp ("malformatted character constant"));
	else if (num_chars == 0)
	  cppReader_errorLit (pfile, 
			      cstring_makeLiteralTemp ("empty character constant"));
	else if (num_chars > max_chars)
	  {
	    num_chars = max_chars;
	    cppReader_errorLit (pfile, 
				cstring_makeLiteralTemp ("character constant too long"));
	  }
	else if (num_chars != 1 && ! cppReader_isTraditional (pfile))
	  {
	    cppReader_warningLit (pfile, 
				  cstring_makeLiteralTemp ("multi-character character constant"));
	  }
	else
	  {
	    ;
	  }

	/* If char type is signed, sign-extend the constant.  */
	if (wide_flag == 0)
	  {
	    int num_bits = num_chars * width;

	    if ((cpphash_lookup ("__CHAR_UNSIGNED__",
			     sizeof ("__CHAR_UNSIGNED__") - 1, -1) != NULL)
		|| (((unsigned) result >> (int_toNonNegative (num_bits - 1))) & 1) == 0)
	      {
		op.value
		  = result & ((unsigned long) ~0 
			      >> int_toNonNegative ((HOST_BITS_PER_LONG - num_bits)));
	      }
	    else
	      {
		op.value
		  = result | ~((unsigned long) ~0 
			       >> int_toNonNegative ((HOST_BITS_PER_LONG - num_bits)));
	      }
	  }
	else
	  {
#ifdef MULTIBYTE_CHARS
	    /* Set the initial shift state and convert the next sequence.  */
	      result = 0;
	      /* In all locales L'\0' is zero and mbtowc will return zero,
		 so don't use it.  */
	      if (num_chars > 1
		  || (num_chars == 1 && token_buffer[0] != '\0'))
	        {
		  wchar_t wc;
		  (void) mbtowc (NULL, NULL, 0);
		  if (mbtowc (& wc, token_buffer, num_chars) == num_chars)
		    result = wc;
		  else
		    cppReader_pedwarn (pfile,"Ignoring invalid multibyte character");
	        }
#endif
	      op.value = result;
	    }
        }

      /* This is always a signed type.  */
      op.unsignedp = FALSE;
      op.op = CPPEXP_CHAR;
    
      return op;

    case CPP_NAME:
      DPRINTF (("Name!"));
      return cppReader_parseNumber (pfile, "0", 0);

    case CPP_OTHER:
      /* See if it is a special token of length 2.  */
      if (tok_start + 2 == tok_end)
        {
	  for (toktab = tokentab2; toktab->operator != NULL; toktab++)
	    {
	     if (tok_start[0] == toktab->operator[0]  &&
		 tok_start[1] == toktab->operator[1])
		{
		  /*@loopbreak@*/ break;
		}
	    }

	  if (toktab->token == CPPREADER_ERRORTOK)
	    {
	      cppReader_error (pfile, 
			       message ("`%s' not allowed in operand of `#if'",
					cstring_fromChars (tok_start)));
	    }

	  op.op = toktab->token; 
	  return op;
	}
      /*@fallthrough@*/ 
    default:
      op.op = *tok_start;
      return op;
  }

  BADEXIT;
  /*@notreached@*/ 
}


/* Parse a C escape sequence.  STRING_PTR points to a variable
   containing a pointer to the string to parse.  That pointer
   is updated past the characters we use.  The value of the
   escape sequence is returned.

   A negative value means the sequence \ newline was seen,
   which is supposed to be equivalent to nothing at all.

   If \ is followed by a null character, we return a negative
   value and leave the string pointer pointing at the null character.

   If \ is followed by 000, we return 0 and leave the string pointer
   after the zeros.  A value of 0 does not mean end of string.  */

int
cppReader_parseEscape (cppReader *pfile, char **string_ptr)
{
   char c = *(*string_ptr)++;

  switch (c)
    {
    case 'a':
      return TARGET_BELL;
    case 'b':
      return TARGET_BS;
    case 'e':
    case 'E':
      if (cppReader_isPedantic (pfile))
	{
	  cppReader_pedwarn (pfile, 
		       message ("non-standard escape sequence, `\\%c'", c));
	}
      return (char) 033;
    case 'f':
      return TARGET_FF;
    case 'n':
      return TARGET_NEWLINE;
    case 'r':
      return TARGET_CR;
    case 't':
      return TARGET_TAB;
    case 'v':
      return TARGET_VT;
    case '\n':
      return -2;
    case 0:
      (*string_ptr)--;
      return 0;
      
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
      {
	int i = (int) c - '0';
	int count = 0;

	while (++count < 3)
	  {
	    c = *(*string_ptr)++;
	    if (c >= '0' && c <= '7')
	      {
		i = ((unsigned) i << 3) + c - '0';
	      }

	    else
	      {
	 	(*string_ptr)--;
		/*@loopbreak@*/ break;
	      }
	  }
	if ((i & ~((1 << MAX_CHAR_TYPE_SIZE) - 1)) != 0)
	  {
	    i &= (1 << MAX_CHAR_TYPE_SIZE) - 1;
	    cppReader_pedwarnLit (pfile,
			    cstring_makeLiteralTemp ("octal character constant does not fit in a byte"));
	  }
	return i;
      }
    case 'x':
      {
	register unsigned i = 0, overflow = 0, digits_found = 0, digit;
	for (;;)
	  {
	    c = *(*string_ptr)++;

	    if (c >= '0' && c <= '9')
	      {
		digit = (unsigned int) (c - '0');
	      }
	    else if (c >= 'a' && c <= 'f')
	      {
		digit = (unsigned int) (c - 'a') + 10;
	      }
	    else if (c >= 'A' && c <= 'F')
	      {
		digit = (unsigned int) (c - 'A') + 10;
	      }
	    else
	      {
		(*string_ptr)--;
		/*@loopbreak@*/ break;
	      }
	    overflow |= i ^ (i << 4 >> 4);
	    i = (i << 4) + digit;
	    digits_found = 1;
	  }
	
	if (digits_found == 0)
	  {
	    cppReader_errorLit (pfile,
				cstring_makeLiteralTemp ("\\x used with no following hex digits"));
	  }

	if ((overflow | (i & ~((1 << int_toNonNegative (BITS_PER_UNIT)) - 1))) != 0)
	  {
	    i &= (1 << BITS_PER_UNIT) - 1;
	    cppReader_pedwarnLit (pfile,
			    cstring_makeLiteralTemp ("hex character constant does not fit in a byte"));
	  }

	return i;
      }
    default:
      return c;
    }
}

static void
integer_overflow (cppReader *pfile)
{
  if (cppReader_isPedantic (pfile))
    cppReader_pedwarnLit (pfile, 
		    cstring_makeLiteralTemp ("integer overflow in preprocessor expression"));
}

static long
left_shift (cppReader *pfile, long a, bool unsignedp, unsigned long b)
{
  if (b >= HOST_BITS_PER_LONG)
    {
      if (!unsignedp && a != 0)
	{
	  integer_overflow (pfile);
	}

      return 0;
    }
  else if (unsignedp)
    {
      return (unsigned long) a << b;
    }
  else
    {
      /*@-shiftimplementation@*/
      long l = a << b; /* int_toNonNegative removed (allow shifts of negative values) */ 

# ifdef WIN32
# pragma warning( disable : 4018 )
# endif

      if (l >> b != a) /* int_toNonNegative removed (allow shifts of negative values) */ 
	{
	  integer_overflow (pfile);
	}

      return l;
    }
}

static long
right_shift (long a, bool unsignedp, unsigned long b)
{
  if (b >= HOST_BITS_PER_LONG)
    return (unsignedp ? 0 : int_toNonNegative (a) >> (HOST_BITS_PER_LONG - 1));
  else if (unsignedp)
    return (unsigned long) a >> b;
  else
    return a >> b; /* int_toNonNegative removed (allow shifts of negative values) */ 
  /*@=shiftimplementation@*/
}

/* These priorities are all even, so we can handle associatively.  */

/*@constant int PAREN_INNER_PRIO@*/
#define PAREN_INNER_PRIO 0

/*@constant int COMMA_PRIO@*/
#define COMMA_PRIO 4

/*@constant int COND_PRIO@*/
#define COND_PRIO (COMMA_PRIO+2)

/*@constant int OROR_PRIO@*/
#define OROR_PRIO (COND_PRIO+2)

/*@constant int ANDAND_PRIO@*/
#define ANDAND_PRIO (OROR_PRIO+2)

/*@constant int OR_PRIO@*/
#define OR_PRIO (ANDAND_PRIO+2)

/*@constant int XOR_PRIO@*/
#define XOR_PRIO (OR_PRIO+2)

/*@constant int AND_PRIO@*/
#define AND_PRIO (XOR_PRIO+2)

/*@constant int CPP_EQUAL_PRIO@*/
#define CPP_EQUAL_PRIO (AND_PRIO+2)

/*@constant int LESS_PRIO@*/
#define LESS_PRIO (CPP_EQUAL_PRIO+2)

/*@constant int SHIFT_PRIO@*/
#define SHIFT_PRIO (LESS_PRIO+2)

/*@constant int PLUS_PRIO@*/
#define PLUS_PRIO (SHIFT_PRIO+2)

/*@constant int MUL_PRIO@*/
#define MUL_PRIO (PLUS_PRIO+2)

/*@constant int UNARY_PRIO@*/
#define UNARY_PRIO (MUL_PRIO+2)

/*@constant int PAREN_OUTER_PRIO@*/
#define PAREN_OUTER_PRIO (UNARY_PRIO+2)

/*@notfunction@*/
#define COMPARE(OP) \
  top->unsignedp = FALSE;\
  top->value = ((unsigned1 || unsigned2) \
                 ? (unsigned long) v1 OP (unsigned long) v2 \
                 : ((long) v1 OP (long) v2)) ? 1 : 0

/* Parse and evaluate a C expression, reading from PFILE.
   Returns the value of the expression.  */

/*@constant int INIT_STACK_SIZE@*/
# define INIT_STACK_SIZE 20

HOST_WIDE_INT
cppReader_parseExpression (cppReader *pfile)
{
  /* The implementation is an operator precedence parser,
     i.e. a bottom-up parser, using a stack for not-yet-reduced tokens.

     The stack base is 'stack', and the current stack pointer is 'top'.
     There is a stack element for each operator (only),
     and the most recently pushed operator is 'top->op'.
     An operand (value) is stored in the 'value' field of the stack
     element of the operator that precedes it.
     In that case the 'flags' field has the HAVE_VALUE flag set.  */

  struct operation init_stack[INIT_STACK_SIZE];
  struct operation *stack = init_stack;
  struct operation *limit = stack + INIT_STACK_SIZE;
  register struct operation *top = stack;
  int lprio, rprio = 0;
  int skip_evaluation = 0;

  top->rprio = 0;
  top->flags = 0;

  for (;;)
    {
      struct operation op;
      int flags = 0;

      /* Read a token */
      op = cppexp_lex (pfile);

      /* See if the token is an operand, in which case go to set_value.
	 If the token is an operator, figure out its left and right
	 priorities, and then goto maybe_reduce.  */

      switch (op.op)
	{
	case NAME:
	  top->value = 0, top->unsignedp = FALSE;
	  goto set_value;
	case CPPEXP_INT:
	case CPPEXP_CHAR:
	  top->value = op.value;
	  top->unsignedp = op.unsignedp;
	  goto set_value;
	case 0:
	  lprio = 0;  goto maybe_reduce;
	case '+':  case '-':
	  /* Is this correct if unary ? FIXME */
	  flags = RIGHT_OPERAND_REQUIRED;
	  lprio = PLUS_PRIO;  rprio = lprio + 1;  goto maybe_reduce;
	case '!':  case '~':
	  flags = RIGHT_OPERAND_REQUIRED;
	  rprio = UNARY_PRIO;  lprio = rprio + 1;  goto maybe_reduce;
	case '*':  case '/':  case '%':
	  lprio = MUL_PRIO;  goto binop;
	case '<':  case '>':  case LEQ:  case GEQ:
	  lprio = LESS_PRIO;  goto binop;
	case CPP_EQUALTOK:  case NOTEQUAL:
	  lprio = CPP_EQUAL_PRIO;  goto binop;
	case LSH:  case RSH:
	  lprio = SHIFT_PRIO;  goto binop;
	case '&':  lprio = AND_PRIO;  goto binop;
	case '^':  lprio = XOR_PRIO;  goto binop;
	case '|':  lprio = OR_PRIO;  goto binop;
	case ANDAND:  lprio = ANDAND_PRIO;  goto binop;
	case OROR:  lprio = OROR_PRIO;  goto binop;
	case ',':
	  lprio = COMMA_PRIO;  goto binop;
	case '(':
	  lprio = PAREN_OUTER_PRIO;  rprio = PAREN_INNER_PRIO;
	  goto maybe_reduce;
	case ')':
	  lprio = PAREN_INNER_PRIO;  rprio = PAREN_OUTER_PRIO;
	  goto maybe_reduce;
        case ':':
	  lprio = COND_PRIO;  rprio = COND_PRIO;
	  goto maybe_reduce;
        case '?':
	  lprio = COND_PRIO + 1;  rprio = COND_PRIO;
	  goto maybe_reduce;
	binop:
	  flags = LEFT_OPERAND_REQUIRED | RIGHT_OPERAND_REQUIRED;
	  rprio = lprio + 1;
	  goto maybe_reduce;
	default:
	  cppReader_error 
	    (pfile, 
	     message ("Invalid character in #if: %c", 
		      (char) op.op));
	  goto syntax_error;
	}

    set_value:
      /* Push a value onto the stack.  */
      if ((top->flags & HAVE_VALUE) != 0)
	{
	  cppReader_errorLit (pfile, 
			      cstring_makeLiteralTemp ("syntax error in #if"));
	  goto syntax_error;
	}
      top->flags |= HAVE_VALUE;
      continue;

    maybe_reduce:
      /* Push an operator, and check if we can reduce now.  */
      while (top->rprio > lprio)
	{
	  /*@-usedef@*/
	  HOST_WIDE_INT v1 = top[-1].value;
	  HOST_WIDE_INT v2 = top[0].value;
	  bool unsigned1 = top[-1].unsignedp;
	  bool unsigned2 = top[0].unsignedp;

	  top--;

	  if (((top[1].flags & LEFT_OPERAND_REQUIRED) != 0)
	      && ((top[0].flags & HAVE_VALUE) == 0))
	    {
	      cppReader_errorLit (pfile, 
				  cstring_makeLiteralTemp ("syntax error - missing left operand"));
	      goto syntax_error;
	    }
	  if (((top[1].flags & RIGHT_OPERAND_REQUIRED) != 0)
	      && ((top[1].flags & HAVE_VALUE) == 0))
	    {
	      cppReader_errorLit (pfile, 
				  cstring_makeLiteralTemp ("syntax error - missing right operand"));
	      goto syntax_error;
	    }
	  /* top[0].value = (top[1].op)(v1, v2);*/
	  switch (top[1].op)
	    {
	    case '+':
	      if ((top->flags & HAVE_VALUE) == 0)
		{ /* Unary '+' */
		  top->value = v2;
		  top->unsignedp = unsigned2;
		  top->flags |= HAVE_VALUE;
		}
	      else
		{
		  top->value = v1 + v2;
		  top->unsignedp = unsigned1 || unsigned2;
		  if (!top->unsignedp && (skip_evaluation == 0)
		      && ! possibleSumSign (v1, v2, top->value))
		    integer_overflow (pfile);
		}
	      /*@switchbreak@*/ break;
	    case '-':
	      if ((top->flags & HAVE_VALUE) == 0)
		{ /* Unary '-' */
		  top->value = - v2;
		  if ((skip_evaluation == 0) 
		      && (top->value & v2) < 0 && !unsigned2)
		    integer_overflow (pfile);
		  top->unsignedp = unsigned2;
		  top->flags |= HAVE_VALUE;
		}
	      else
		{ /* Binary '-' */
		  top->value = v1 - v2;
		  top->unsignedp = unsigned1 || unsigned2;
		  if (!top->unsignedp && (skip_evaluation == 0)
		      && !possibleSumSign (top->value, v2, v1))
		    {
		      integer_overflow (pfile);
		    }
		}
	      /*@switchbreak@*/ break;
	    case '*':
	      top->unsignedp = unsigned1 || unsigned2;

	      if (top->unsignedp)
		{
		  top->value = (unsigned long) v1 * v2;
		}
	      else if (skip_evaluation == 0)
		{
		  top->value = v1 * v2;
		  if ((v1 != 0)
		      && (top->value / v1 != v2
			  || (top->value & v1 & v2) < 0))
		    {
		      integer_overflow (pfile);
		    }
		}
	      else
		{
		  ;
		}

	      /*@switchbreak@*/ break;
	    case '/':
	      if (skip_evaluation != 0)
		/*@switchbreak@*/ break;
	      if (v2 == 0)
		{
		  cppReader_errorLit (pfile, 
				      cstring_makeLiteralTemp ("Division by zero in #if"));
		  v2 = 1;
		}
	      top->unsignedp = unsigned1 || unsigned2;
	      if (top->unsignedp)
		top->value = (unsigned long) v1 / v2;
	      else
		{
		  top->value = v1 / v2;
		  if ((top->value & v1 & v2) < 0)
		    integer_overflow (pfile);
		}
	      /*@switchbreak@*/ break;
	    case '%':
	      if (skip_evaluation != 0)
		/*@switchbreak@*/ break;
	      if (v2 == 0)
		{
		  cppReader_errorLit (pfile, 
				      cstring_makeLiteralTemp ("Division by zero in #if"));
		  v2 = 1;
		}
	      top->unsignedp = unsigned1 || unsigned2;
	      if (top->unsignedp)
		top->value = (unsigned long) v1 % v2;
	      else
		top->value = v1 % v2;
	      /*@switchbreak@*/ break;
	    case '!':
	      if ((top->flags & HAVE_VALUE) != 0)
		{
		  cppReader_errorLit (pfile, 
				      cstring_makeLiteralTemp ("Syntax error"));
		  goto syntax_error;
		}

	      top->value = (v2 == 0) ? 1 : 0;
	      top->unsignedp = FALSE;
	      top->flags |= HAVE_VALUE;
	      /*@switchbreak@*/ break;
	    case '~':
	      if ((top->flags & HAVE_VALUE) != 0)
		{
		  cppReader_errorLit (pfile, 
				      cstring_makeLiteralTemp ("syntax error"));
		  goto syntax_error;
		}
	      top->value = ~ v2;
	      top->unsignedp = unsigned2;
	      top->flags |= HAVE_VALUE;
	      /*@switchbreak@*/ break;
	    case '<':  COMPARE(<);  /*@switchbreak@*/ break;
	    case '>':  COMPARE(>);  /*@switchbreak@*/ break;
	    case LEQ:  COMPARE(<=); /*@switchbreak@*/ break;
	    case GEQ:  COMPARE(>=); /*@switchbreak@*/ break;
	    case CPP_EQUALTOK:
	      top->value = (v1 == v2) ? 1 : 0;
	      top->unsignedp = FALSE;
	      /*@switchbreak@*/ break;
	    case NOTEQUAL:
	      top->value = (v1 != v2) ? 1 : 0;
	      top->unsignedp = FALSE;
	      /*@switchbreak@*/ break;
	    case LSH:
	      if (skip_evaluation != 0)
		{
		  /*@switchbreak@*/ break;
		}

	      top->unsignedp = unsigned1;
	      if (v2 < 0 && ! unsigned2)
		top->value = right_shift (v1, unsigned1, -v2);
	      else
		top->value = left_shift (pfile, v1, unsigned1, v2);
	      /*@switchbreak@*/ break;
	    case RSH:
	      if (skip_evaluation != 0)
		{
		  /*@switchbreak@*/ break;
		}
	      top->unsignedp = unsigned1;
	      if (v2 < 0 && ! unsigned2)
		top->value = left_shift (pfile, v1, unsigned1, -v2);
	      else
		top->value = right_shift (v1, unsigned1, v2);
	      /*@switchbreak@*/ break;

/*@notfunction@*/
#define LOGICAL(OP) \
	      top->value = v1 OP v2;\
	      top->unsignedp = unsigned1 || unsigned2;

	    case '&':  LOGICAL(&); /*@switchbreak@*/ break;
	    case '^':  LOGICAL(^); /*@switchbreak@*/ break;
	    case '|':  LOGICAL(|); /*@switchbreak@*/ break;
	    case ANDAND:
	      top->value = ((v1 != 0) && (v2 != 0)) ? 1 : 0;
	      top->unsignedp = FALSE;

	      if (v1 == 0)
		{
		  skip_evaluation--;
		}
	      /*@switchbreak@*/ break;
	    case OROR:
	      top->value = ((v1 != 0) || (v2 != 0)) ? 1 : 0;
	      top->unsignedp = FALSE;
	      if (v1 != 0)
		{
		  skip_evaluation--;
		}
	      /*@switchbreak@*/ break;
	    case ',':
	      if (cppReader_isPedantic (pfile))
		cppReader_pedwarnLit (pfile, 
				cstring_makeLiteralTemp ("comma operator in operand of `#if'"));
	      top->value = v2;
	      top->unsignedp = unsigned2;
	      /*@switchbreak@*/ break;
	    case '(':  case '?':
	      cppReader_errorLit (pfile, 
				  cstring_makeLiteralTemp ("syntax error in #if"));
	      goto syntax_error;
	    case ':':
	      if (top[0].op != '?')
		{
		  cppReader_errorLit (pfile,
				      cstring_makeLiteralTemp ("syntax error ':' without preceding '?'"));
		  goto syntax_error;
		}
	      else if (((top[1].flags & HAVE_VALUE) == 0)
		       || ((top[-1].flags & HAVE_VALUE) == 0)
		       || ((top[0].flags & HAVE_VALUE) == 0))
		{
		  cppReader_errorLit (pfile, 
				      cstring_makeLiteralTemp ("bad syntax for ?: operator"));
		  goto syntax_error;
		}
	      else
		{
		  top--;
		  if (top->value != 0)
		    {
		      skip_evaluation--;
		    }

		  top->value = (top->value != 0) ? v1 : v2;
		  top->unsignedp = unsigned1 || unsigned2;
		}
	      /*@switchbreak@*/ break;
	    case ')':
	      if (((top[1].flags & HAVE_VALUE) != 0)
		  || ((top[0].flags & HAVE_VALUE) == 0)
		  || top[0].op != '('
		  || ((top[-1].flags & HAVE_VALUE) != 0))
		{
		  cppReader_errorLit (pfile, 
				      cstring_makeLiteralTemp ("mismatched parentheses in #if"));
		  goto syntax_error;
		}
	      else
		{
		  top--;
		  top->value = v1;
		  top->unsignedp = unsigned1;
		  top->flags |= HAVE_VALUE;
		}
	      /*@switchbreak@*/ break;
	    default:
	      /*@-formatconst@*/
	      fprintf (stderr,
		       top[1].op >= ' ' && top[1].op <= '~'
		       ? "unimplemented operator '%c'\n"
		       : "unimplemented operator '\\%03o'\n",
		       top[1].op);
	      /*@=formatconst@*/
	    }
	}
      if (op.op == 0)
	{
	  long val;

	  if (top != stack)
	    {
	      cppReader_errorLit (pfile, 
				  cstring_makeLiteralTemp ("internal error in #if expression"));
	    }

	  val = top->value;

	  if (stack != init_stack)
	    {
	      sfree (stack);
	      /*@-branchstate@*/
	    } /*@=branchstate@*/

	  return val;
	}
      top++;
      
      /* Check for and handle stack overflow.  */
      if (top == limit)
	{
	  struct operation *new_stack;
	  size_t old_size = size_fromInt ((char *) limit - (char *) stack);
	  size_t new_size = (size_t) (2 * old_size);

	  if (stack != init_stack)
	    {
	      new_stack = (struct operation *) drealloc ((char *) stack,
							 new_size);
	    }
	  else
	    {
	      new_stack = (struct operation *) dmalloc (new_size);

	      /* Bug: the parameters were in the wrong order! */
	      memcpy ((char *) new_stack, (char *) stack, old_size);
	      /*@-branchstate@*/
	    } /*@=branchstate@*/

	  stack = new_stack;
	  top = (struct operation *) ((char *) new_stack + old_size);
	  limit = (struct operation *) ((char *) new_stack + new_size);
	  /*@-branchstate@*/ 
	} /*@=branchstate@*/ 
      
      top->flags = flags;
      top->rprio = rprio;
      top->op = op.op;
      if ((op.op == OROR && (top[-1].value != 0))
	  || (op.op == ANDAND && (top[-1].value == 0))
	  || (op.op == '?' && (top[-1].value == 0)))
	{
	  skip_evaluation++;
	}
      else if (op.op == ':')
	{
	  if (top[-2].value != 0) /* Was condition true? */
	    {
	      skip_evaluation++;
	    }
	  else
	    {
	      skip_evaluation--;
	    }
	}
      else
	{
	  ;
	}
    }
 syntax_error:
  /*@-usereleased@*/
  if (stack != init_stack)
    {
      sfree (stack);
      /*@-branchstate@*/
    } /*@=branchstate@*/
  /*@=usereleased@*/

  cppReader_skipRestOfLine (pfile);
  return 0;
}
