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
** reader.c
*/

# include "splintMacros.nf"
# include "basic.h"

int reader_getInt (char **s)
{
  bool gotOne = FALSE;
  int i = 0;

  while (**s == ' ')
    {
      (*s)++;
    }

  if (**s == '-')
    {
      (*s)++;
      if (**s < '0' || **s > '9')
	{
	  llbug (message ("getInt: bad int: %s", cstring_fromChars (*s))); 
	}
      else
	{
	  i = -1 * (int) (**s - '0');
	  gotOne = TRUE;
	}

      (*s)++;
    }

  while (**s >= '0' && **s <= '9')
    {
      i *= 10;
      i += (int) (**s - '0');
      (*s)++;
      gotOne = TRUE;
    }

  if (!gotOne)
    {
      llbug (message ("No int to read: %s", cstring_fromChars (*s)));
    }

  while (**s == '\n' || **s == ' ' || **s == '\t')
    {
      (*s)++;
    }

  return i;
}

char
reader_loadChar (char **s)
{
  char ret;

  while (**s == ' ')
    {
      (*s)++;
    }
  
  ret = **s;
  (*s)++;
  return ret;
}

/*
** not sure if this works...
*/

double
reader_getDouble (char **s)
{
  char *end = mstring_createEmpty ();
  double ret;

  ret = strtod (*s, &end);

  *s = end;
  return ret;
}

/*
** read to ' ', '\t'. '\n', '#', ',' or '\0'
*/

char *
reader_getWord (char **s)
{
  char *res;
  char *t = *s;
  char c;

  while ((c = **s) != '\0' && (c != ' ') && (c != ',') 
	 && (c != '\n') && (c != '\t') && (c != '#'))
    {
      (*s)++;
    }

  if (*s == t)  
    {
      return NULL;
    }

  **s = '\0';
  res = mstring_copy (t);
  **s = c;
  return res;
}

/*
** read up to x
*/

cstring
reader_readUntil (char **s, char x)
{
  cstring res;
  char *t = *s;
  char c;

  while ((c = **s) != '\0' && (c != x))
    {
      (*s)++;
    }

  llassert (**s != '\0');
  llassert (*s != t);

  **s = '\0';
  res = cstring_fromChars (mstring_copy (t));
  **s = c;
  return res;
}

cstring 
reader_readUntilOne (char **s, char *x)
{
  cstring res;
  char *t = *s;
  char c;

  while ((c = **s) != '\0' && (!mstring_containsChar (x, c)))
    {
      (*s)++;
    }

  llassert (**s != '\0');
  llassert (*s != t);

  **s = '\0';
  res = cstring_fromChars (mstring_copy (t));
  **s = c;
  return res;
}

bool
reader_optCheckChar (char **s, char c)
{
  if (**s == c)
    {
      (*s)++;
      return TRUE;
    }
  else
    {
      return FALSE;
    }
}

void
reader_doCheckChar (char **s, char c, char *file, int line)
{
  /*@unchecked@*/ static int nbadchars = 0;

  if (**s == c)
    {
      (*s)++;
    }
  else
    {
      nbadchars++;

      if (nbadchars > 5)
	{
	  llfatalbug (cstring_makeLiteral 
		      ("checkChar: Too many errors.  Check library is up to date."));
	}
      else
	{
	  llbug (message ("checkChar: %q: Bad char, expecting %h: %s",
			  fileloc_unparseRaw (cstring_fromChars (file), line),
			  c,
			  cstring_fromChars (*s)));
	}
    }
}

void reader_checkUngetc (int c, FILE *f)
{
  int res;

  llassert (c != EOF);
  res = ungetc (c, f);
  llassert (res == c);
}

char *reader_readLine (FILE *f, char *s, int max)
{
  char *res = fgets (s, MAX_DUMP_LINE_LENGTH, f);

  if (res != NULL) 
    {
      if (strlen (res) == size_fromInt (MAX_DUMP_LINE_LENGTH - 1))
	{
	  llfatalerrorLoc (message ("Maximum line length exceeded (%d): %s", max, 
				    cstring_fromChars (s)));
	}

      incLine ();
    }

  return res;
}
