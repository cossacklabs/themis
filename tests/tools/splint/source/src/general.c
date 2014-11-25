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
** general.c
*/

# include "splintMacros.nf"
# include "basic.h"

# undef malloc
# undef realloc
# undef calloc

# ifdef USEDMALLOC
# include "dmalloc.h"
# endif

# include "osd.h"

/*
** redefine undef'd memory ops
*/

# ifndef USEDMALLOC

/*@-mustdefine@*/

void sfree (void *x)
{
  if (x != NULL)
    {
      /* fprintf (stderr, "Freeing: %p\n", x); */

      /*
      if ((unsigned long) x > 0xbf000000) {
	fprintf (stderr, "Looks bad!\n");
      }
      */
      
      free (x);
      
      /* fprintf (stderr, "Done.\n"); */
    }
}
# endif

void sfreeEventually (void *x)
{
  if (x != NULL)
    {
      ; /* should keep in a table */
    }
/*@-mustfree@*/
} /*@=mustfree@*/

/*
** all memory should be allocated from dimalloc
*/

static long unsigned size_toLongUnsigned (size_t x)
{
  long unsigned res = (long unsigned) x;

  llassert ((size_t) res == x);
  return res;
}

/*@out@*/ void *dimalloc (size_t size, const char *name, int line)
     /*@ensures maxSet(result) == (size - 1); @*/ 
{
  /*
  static void *lastaddr = 0;
  static int numallocs = 0;
  static int numbad = 0;
  */

  /* was malloc, use calloc to initialize to zero */
  void *ret = (void *) calloc (1, size);

  /*
  numallocs++;

  if (ret < lastaddr)
    {
      numbad++;
      fprintf (stderr, "Bad alloc: %d / %d\n", numbad, numallocs);
    }

  lastaddr = ret;
  */

  if (ret == NULL)
    {
      if (size == 0)
	{
	  llcontbug (message ("Zero allocation at %q.",
			      fileloc_unparseRaw (cstring_fromChars (name), line)));
	  
	  /* 
	  ** evans 2002-03-01
	  ** Return some allocated storage...hope we get lucky.
	  */

	  return dimalloc (16, name, line);
	}
      else
	{
	  /* drl
	     fix this so message doesn't run out of
	     memory*/
	  
	  llbuglit("Out of memory");
	  
	  llfatalerrorLoc
	    (message ("Out of memory.  Allocating %w bytes at %s:%d.", 
		      size_toLongUnsigned (size),
		      cstring_fromChars (name), line));
	  
	}
    }
      
  /*@-null@*/ /* null okay for size = 0 */
  /* fprintf (stderr, "%s:%d: Allocating: [%p / %d]\n", name, line, ret, size);  */
  return ret; 
  /*@=null@*/
}

void *dicalloc (size_t num, size_t size, const char *name, int line)
{
  void *ret = (void *) calloc (num, size);

  if (ret == NULL)
    {
      llfatalerrorLoc 
	(message ("Out of memory.  Allocating %w bytes at %s:%d.", 
		  size_toLongUnsigned (size),
		  cstring_fromChars (name), line));
    }
  
  return ret;
}

void *direalloc (/*@out@*/ /*@null@*/ void *x, size_t size, 
		 char *name, int line)
{
  void *ret;

  if (x == NULL)
    {				       
      ret = (void *) dmalloc (size);
    }
  else
    {
      ret = (void *) realloc (x, size);
    }

  if (ret == NULL)
    {
      llfatalerrorLoc
	(message ("Out of memory.  Allocating %w bytes at %s:%d.", 
		  size_toLongUnsigned (size),
		  cstring_fromChars (name), line));
    }
  
  return ret;
}

/*@=mustdefine@*/

bool firstWord (char *s, char *w)
{
  llassert (s != NULL);
  llassert (w != NULL);
  
  for (; *w != '\0'; w++, s++)
    {
      if (*w != *s || *s == '\0')
	return FALSE;
    }
  return TRUE;
}

void mstring_markFree (char *s)
{
  sfreeEventually (s);
}

char *mstring_spaces (int n)
{
  int i;
  char *ret;
  char *ptr;

  llassert (n >= 0);

  ret = (char *) dmalloc (size_fromInt (n + 1));
  ptr = ret;

  for (i = 0; i < n; i++)
    {
      *ptr++ = ' ';
    }

  *ptr = '\0';

  return ret;
}

bool mstring_containsChar (const char *s, char c)
{
  if (mstring_isDefined (s))
    {
      return (strchr (s, c) != NULL);
    }
  else
    {
      return FALSE;
    }
}

bool mstring_containsString (const char *s, const char *c)
{
  if (mstring_isDefined (s))
    {
      return (strstr (s, c) != NULL);
    }
  else
    {
      return FALSE;
    }
}
 
char *mstring_concat (const char *s1, const char *s2)
{
  char *s = (char *) dmalloc (strlen (s1) + strlen (s2) + 1);
  strcpy (s, s1);
  strcat (s, s2);
  return s;
}

extern /*@only@*/ char *
mstring_concatFree (/*@only@*/ char *s1, /*@only@*/ char *s2)
{
  /* like mstring_concat but deallocates old strings */
  char *s = (char *) dmalloc (strlen (s1) + strlen (s2) + 1);
  strcpy (s, s1);
  strcat (s, s2);

  sfree (s1);
  sfree (s2);
  return s;
}

extern /*@only@*/ char *
mstring_concatFree1 (/*@only@*/ char *s1, const char *s2)
{
  char *s = (char *) dmalloc (strlen (s1) + strlen (s2) + 1);
  strcpy (s, s1);
  strcat (s, s2);
  sfree (s1);

  return s;
}

extern /*@only@*/ char *
mstring_append (/*@only@*/ char *s1, char c)
{
  size_t l = strlen (s1);
  char *s;

  s = (char *) dmalloc (sizeof (*s) * (l + 2));

  strcpy (s, s1);
  *(s + l) = c;
  *(s + l + 1) = '\0';
  sfree (s1); 
  return s;
}

extern 
char *mstring_copy (char *s1) /*@ensures maxRead(result) == maxRead(s1) /\  maxSet(result) == maxSet(s1) @*/
{
  if (s1 == NULL)
    {
      return NULL;
    }
  else
    {
      char *s = (char *) dmalloc ((strlen (s1) + 1) * sizeof (*s));
      strcpy (s, s1);
      return s;
    }
}

extern
char *mstring_safePrint (char *s)
{
  if (s == NULL)
    {
      return ("<undefined>");
    }
  else
    {
      return s;
    }
}

extern
char *mstring_create (size_t n)
{
  char *s;

  s = dmalloc (sizeof (*s) * (n + 1));
  *s = '\0';
  return s;
}

void
fputline (FILE *out, char *s)
{
  if (strlen (s) > 0) 
    {
      check (fputs (s, out) != EOF);
    }

  check (fputc ('\n', out) == (int) '\n');
}

unsigned int int_toNonNegative (int x) /*@*/
{
  llassert (x >= 0);
  return (unsigned) x;
}

int int_log (int x)
{
  int ret = 1;

  while (x > 10)
    {
      ret++;
      x /= 10;
    }

  return ret;
}

/*@-czechfcns@*/
long unsigned int 
longUnsigned_fromInt (int x)
{
  llassert (x >= 0);
  
  return (long unsigned) x;
}

size_t size_fromInt (int x) /*@ensures result==x@*/
{
  size_t res = (size_t) x;

  llassert ((int) res == x);
  return res;
}

size_t size_fromLong (long x) /*@ensures result==x@*/
{
  size_t res = (size_t) x;

  llassert ((long) res == x);
  return res;
}

size_t size_fromLongUnsigned (unsigned long x) /*@ensures result==x@*/
{
  size_t res = (size_t) x;

  llassert ((unsigned long) res == x);
  return res;
}

int size_toInt (size_t x)
{
  int res = (int) x;

  llassert ((size_t) res == x);
  return res;
}

long size_toLong (size_t x)
{
  long res = (long) x;

  llassert ((size_t) res == x);
  return res;
}

/*@=czechfcns@*/

char
char_fromInt (int x)
{
  /*
  ** evans 2001-09-28 - changed assertion in response to Anthony Giorgio's comment 
  ** that the old assertions failed for EBCDIC character set.  Now we just check 
  ** that the result is equal.
  */

  char res = (char) x;
  llassert ((int) res == x);
  return res;
}

/*@-czechfcns@*/
int
longUnsigned_toInt (long unsigned int x)
{
  int res = (int) x;

  llassert ((long unsigned) res == x);
  return res;
}

int
long_toInt (long int x)
{
  int res = (int) x;

  /*@+ignorequals@*/ llassert (res == x); /*@=ignorequals@*/
  return res;
}

/*@+czechfcns@*/

bool mstring_equalPrefix (const char *c1, const char *c2)
{
  llassert (c1 != NULL);
  llassert (c2 != NULL);

  if (strncmp(c1, c2, strlen(c2)) == 0)
    {
      return TRUE;
    }
  else
    {
      return FALSE;
    }
}

bool mstring_equal (/*@null@*/ const char *s1, /*@null@*/ const char *s2)
{
  if (s1 == NULL)
    {
      return (s2 == NULL);
    }
  else
    {
      if (s2 == NULL)
	{
	  return FALSE;
	}
      else
	{
	  return (strcmp(s1, s2) == 0);
	}
    }
}

