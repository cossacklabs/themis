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
** message.c
*/

# include "splintMacros.nf"
# include "basic.h"
 
/* patch for linux? solaris? */

static char strbuf[64];
static int modcode;

typedef enum
{
  XINVALID, 
  XCHAR, XSTRING, XSTRINGFREE, XTSTRINGFREE, XINT, XFLOAT, XBOOL, XUENTRY,
  XPERCENT, XCTYPE, XPLURAL, XREPREFIX, XFILELOC, XPOINTER
} ccode;

/*@function void GETPRINTF (char *p_s, anytype p_v) modifies strbuf@*/

/*@notfunction@*/
# ifndef WIN32
  /* ISO requires this, but not all implementations (e.g., Microsoft's) provide it */
# define GETPRINTF(s,v) (snprintf (strbuf, 64, s, v), mstring_copy (strbuf))
# else
  /* MS provides _snprintf instead */
# define GETPRINTF(s,v) (_snprintf (strbuf, 64, s, v), mstring_copy (strbuf))
# endif

/*
** returns control code indicated by *c, and
** advances *c to next character.
*/

static ccode identify_control (char **s)
{
  char c;

  modcode = 0;

  c = **s;
  if (c == '\0')
    {
      return (XINVALID);
    }

  if (c >= '0' && c <= '9')
    {
      modcode = reader_getInt (s);
    }

  c = **s;

  (*s)++;

 /*
 ** handle single-char codes
 */

  switch (c)
    {
    case '%':
      return (XPERCENT);
    case 'h':
    case 'c':
      return (XCHAR);
    case 's':
      return (XSTRING);
    case 'q':
      return (XSTRINGFREE);
    case 'x':
      return (XSTRINGFREE);
    case 'd':
      return (XINT);
    case 'u':
      return (XINT); /* unsigned */
    case 'w':
      return (XINT); /* unsigned long */
    case 'f':
      return (XFLOAT);
    case 'b':
      return (XBOOL);
    case 't':
      return (XCTYPE);
    case 'p':
      return (XPOINTER);
    case 'l':
      return (XFILELOC);
    case '&':
      return (XPLURAL);
    case 'r':
      return (XREPREFIX);
    default:
      llcontbug (message ("Message: invalid code: %h (%s)", c, 
			  cstring_fromChars (*s)));
      return (XINVALID);
    }
}

/*
** message
**
** returns a cstring containing the message, as formated by s.
**
** the format codes are similar to printf:
**
**         %s    cstring (don't free after print)
**         %q    cstring (free after print)
**         %d    int
**         %f    float
**         %b    bool     (uses bool_unparse)
**         %u    uentry
**         %l    fileloc
**         %t    ctype
*/


# if USEVARARGS
cstring
message (fmt, va_alist)
     char *fmt;
     va_dcl
# else
/*@messagelike@*/ /*@only@*/ cstring
message (/*@temp@*/ char *fmt, ...)
# endif
{
  char c;
  int lastint = 0;
  char *ret = mstring_createEmpty ();
  char *ofmt = fmt;
  va_list pvar;

# if USEVARARGS
  va_start (pvar);
# else
  va_start (pvar, fmt);
# endif  

  while ((c = *fmt++) != '\0')
    {
      if (c == '%')
	{
	  /*@-loopswitchbreak@*/

	  switch (identify_control (&fmt))
	    {
	    case XPERCENT:
	      {
		ret = mstring_concatFree1 (ret, "%");
		break;
	      }
	    case XCHAR:
	      {
		/*
                ** some systems don't handle char va_arg correctly, so it must be
		** passed as an int here
		*/

		char lc = (char) va_arg (pvar, int);

		ret = mstring_append (ret, lc);
		break;
	      }
	    case XSTRING:
	      {
		cstring s = va_arg (pvar, cstring);
		
		if (modcode != 0)
		  {
		    ret = mstring_concatFree (ret, cstring_toCharsSafe 
					      (cstring_fill (s, size_fromInt (modcode))));
		  }
		else
		  {
		    if (cstring_isDefined (s))
		      {
			ret = mstring_concatFree1 (ret, cstring_toCharsSafe (s));
		      }
		  }
	      }
	      break;
	    case XSTRINGFREE:
	    case XTSTRINGFREE:
	      {
		cstring s = va_arg (pvar, cstring);
		
		if (modcode != 0)
		  {
		    ret = mstring_concatFree (ret, cstring_toCharsSafe 
					      (cstring_fill (s, size_fromInt (modcode))));
		  }
		else
		  {
		    if (cstring_isDefined (s))
		      {
			ret = mstring_concatFree 
			  (ret, cstring_toCharsSafe (s));
		      }
		  }
	      }
	      break;
	    case XREPREFIX:
	      lastint = va_arg (pvar, int);

	      if (lastint != 0)
		{
		  ret = mstring_concatFree1 (ret, "re");
		}
	      break;
	    case XPLURAL:
	      if (lastint != 1)
		{
		  ret = mstring_concatFree1 (ret, "s");
		}
	      break;
	    case XINT:
	      lastint = va_arg (pvar, int);
	      ret = mstring_concatFree (ret, GETPRINTF ("%d", lastint));
	      break;
	    case XFLOAT:
	      ret = mstring_concatFree (ret, GETPRINTF ("%.2lf", va_arg (pvar, double)));
	      break;
	    case XBOOL:
	      ret = mstring_concatFree1 
		(ret, cstring_toCharsSafe 
		 (bool_unparse (bool_fromInt ((va_arg (pvar, int)))))); 
	      /* va_arg should not use bool type */
	      break;
	    case XUENTRY:
	      ret = mstring_concatFree (ret, cstring_toCharsSafe 
				   (uentry_unparse (va_arg (pvar, uentry))));
	      break;
	    case XCTYPE:
	      /* cannot free ctype_unparse */
	      ret = mstring_concatFree1 (ret, cstring_toCharsSafe 
				   (ctype_unparse (va_arg (pvar, ctype)))); 
	      break;
	    case XPOINTER:
	      ret = mstring_concatFree (ret, GETPRINTF ("%p", va_arg (pvar, void *)));
	      break;

	    case XFILELOC:
	      ret = mstring_concatFree (ret, cstring_toCharsSafe 
				   (fileloc_unparse (va_arg (pvar, fileloc))));
	      break;
	    case XINVALID:
	    default:
	      llcontbug (cstring_makeLiteral ("message: bad control flag"));
	      fprintf (stdout, "\tFormat string: %s", ofmt);
	    }
	  /*@=loopswitchbreak@*/
	}
      else
	{
	  ret = mstring_append (ret, c);
	}
    }

  va_end (pvar);

  /*
  ** cstring_fromChars returns the same storage (exposed)
  */

  /*@-mustfree@*/ return (cstring_fromChars (ret)); /*@=mustfree@*/
}
