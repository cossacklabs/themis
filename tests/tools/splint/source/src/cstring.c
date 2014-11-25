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
** cstring.c
*/

/*
 * Herbert 06/12/2000
 * - use drive spec specials with OS2 like with WIN32
 * - cstring_replaceAll () needed in cpplib.c
 */

# include "splintMacros.nf"
# include "basic.h"
# include "osd.h"

/*@only@*/ /*@notnull@*/ 
cstring cstring_newEmpty (void)
{
  return (cstring_create (0));
}

char cstring_firstChar (cstring s) 
{
  llassert (cstring_isDefined (s));
  llassert (cstring_length (s) > 0);

  return (s[0]);
}

char cstring_getChar (cstring s, size_t n) 
{
  size_t length = cstring_length (s);

  llassert (cstring_isDefined (s));
  llassert (n >= 1 && n <= length);

  return (s[n - 1]);
}

cstring cstring_suffix (cstring s, size_t n) 
{
  llassert (cstring_isDefined (s));
  llassert (n <= cstring_length (s));

  return (s + n);
}

cstring cstring_prefix (cstring s, size_t n) 
   /*@requires maxRead(s) >= n /\ maxSet(s) >= n @*/
   /*@ensures maxRead(result) == n /\ maxSet(result) == n @*/
{
  cstring t;
  char c;
  llassert (cstring_isDefined (s));
  llassert (n <= cstring_length (s));

  c = *(s + n);
  /*@-mods@*/  /* The modifications cancel out. */
  *(s + n) = '\0';
  t = cstring_copy (s);
  *(s + n) = c;
  /*@=mods@*/

  return t;
}

/* effects If s = [0-9]*, returns s as an int.
**         else returns -1.
*/

int cstring_toPosInt (cstring s)
{
  int val = 0;

  cstring_chars (s, c)
    {
      if (isdigit ((unsigned char) c))
	{
	  val = (val * 10) + (int)(c - '0');
	}
      else
	{
	  return -1;
	}
    } end_cstring_chars ; 

  return val;
}

cstring cstring_afterChar (cstring s, char c) 
{
  llassert (cstring_isDefined (s));
  return strchr (s, c);
}

cstring cstring_beforeChar (cstring s, char c)
{
  if (cstring_isDefined (s))
    {
      char *cp = strchr (s, c);

      if (cp != NULL)
	{
	  cstring ret;

	  /*@-mods@*/
	  *cp = '\0';
	  ret = cstring_copy (s);
	  *cp = c;
	  /*@=mods@*/ /* modification is undone */
	  
	  return ret;
	}
    }

  return cstring_undefined;
}

void cstring_setChar (cstring s, size_t n, char c) /*@requires maxRead(s) >= (n - 1) /\ maxSet(s) >= (n - 1) @*/
{
  llassert (cstring_isDefined (s));
  llassert (n > 0 && n <= cstring_length (s));

  s[n - 1] = c;
}

char cstring_lastChar (cstring s) 
{
  size_t length;

  llassert (cstring_isDefined (s));

  length = cstring_length (s);
  llassert (length > 0);

  return (s[length - 1]);
}

/*@only@*/ cstring cstring_copy (cstring s) /*@ensures maxSet(result) == maxRead(s) /\ maxRead(result) == maxRead(s) @*/
{
  if (cstring_isDefined (s))
    {
      return (mstring_copy (s));
    }
  else
    {
      return cstring_undefined;
    }
}

/*@only@*/ cstring cstring_copyLength (char *s, size_t len) /*@requires maxSet(s) >= (len - 1) @*/
{
  char *res = mstring_create (len + 1);

  strncpy (res, s, len);
  res[len] = '\0';
  return res;
}

bool cstring_containsChar (cstring c, char ch)
{
  if (cstring_isDefined (c))
    {
      return (strchr (c, ch) != NULL);
    }
  else
    {
      return FALSE;
    }
}

/*
** Replaces all occurances of old in s with new.
*/

void cstring_replaceAll (cstring s, char old, char snew)
{
  
  llassert (old != snew);

  if (cstring_isDefined (s))
    {
      char *sp = strchr (s, old);

      while (sp != NULL)
	{
	  *sp = snew;
	  sp = strchr (sp, old);
	}

          }
}

void cstring_replaceLit (/*@unique@*/ cstring s, char *old, char *snew) 
   /*@requires maxRead(snew) >= 0 /\ maxRead(old) >= 0 /\ maxRead(old) >= maxRead(snew) @*/
{
  llassert (strlen (old) >= strlen (snew));
  
  if (cstring_isDefined (s))
    {
      char *sp = strstr (s, old);

      while (sp != NULL)
	{
	  int lendiff = size_toInt (strlen (old) - strlen (snew));
	  char *tsnew = snew;

	  llassert (lendiff >= 0);

	  while (*tsnew != '\0')
	    {
	      llassert (*sp != '\0');
	      *sp++ = *tsnew++;
	    }
	  
	  if (lendiff > 0)
	    {
	      while (*(sp + lendiff) != '\0')
		{
		  *sp = *(sp + lendiff);
		  sp++;
		}
	      
	      *sp = '\0';
	    }

	  sp = strstr (s, old);
	}
    }
}

/*
** removes all chars in clist from s
*/

void cstring_stripChars (cstring s, const char *clist)
{
  if (cstring_isDefined (s))
    {
      int i;
      size_t size = cstring_length (s);

      for (i = 0; i < size_toInt (size); i++)
	{
	  	
	  char c = s[i];
	  
	  if (strchr (clist, c) != NULL)
	    {
	      /* strip this char */
	      int j;
	      
	      size--;
	      
	      for (j = i; j < size_toInt (size); j++)
		{
		  s[j] = s[j+1];
		}
	      
	      s[size] = '\0'; 
	      i--;
	    }
	}
    }
}

bool cstring_contains (/*@unique@*/ cstring c, cstring sub)
{
  if (cstring_isDefined (c))
    {
      llassert (cstring_isDefined (sub));
      
      return (strstr (c, sub) != NULL);
    }
  else
    {
      return FALSE;
    }
}

static char lookLike (char c) /*@*/
{
  if (c == 'I' || c == 'l')
    {
      return '1';
    }
  else if (c == 'O' || c == 'o')
    {
      return '0';
    }
  else if (c == 'Z')
    {
      return '2';
    }
  else if (c == 'S')
    {
      return '5';
    }
  else
    {
      return c;
    }
}

cmpcode cstring_genericEqual (cstring s, cstring t,
			      size_t nchars,
			      bool caseinsensitive,
			      bool lookalike) 
  /*@requires maxRead(s) >= nchars /\ maxRead(t) >= nchars @*/
{
  if (s == t) return CGE_SAME;
  else if (cstring_isUndefined (s))
    {
      return cstring_isEmpty (t) ? CGE_SAME : CGE_DISTINCT;
    }
  else if (cstring_isUndefined (t))
    {
      return cstring_isEmpty (s) ? CGE_SAME : CGE_DISTINCT;
    }
  else
    {
      int i = 0;
      bool diffcase = FALSE;
      bool difflookalike = FALSE;

      while (*s != '\0')
	{
	  if (nchars > 0 && i >= size_toInt (nchars))
	    {
	      break;
	    }

	  if (*t == *s)
	    {
	      ; /* no difference */
	    }
	  else if (caseinsensitive 
		   && (toupper ((int) *t) == toupper ((int) *s)))
	    {
	      diffcase = TRUE;
	    }
	  else if (lookalike && (lookLike (*t) == lookLike (*s)))
	    {
	      difflookalike = TRUE;
	    }
	  else 
	    {
	      return CGE_DISTINCT;
	    }
	  i++;
	  s++;
	  t++;
	}

        
      if (*s == '\0' && *t != '\0')
	{
	  return CGE_DISTINCT;
	}

      if (diffcase)
	{
	  return CGE_CASE;
	}
      else if (difflookalike)
	{
	  return CGE_LOOKALIKE;
	}
      else
	{
	  return CGE_SAME;
	}
    }
}



bool cstring_equalFree (/*@only@*/ cstring c1, /*@only@*/ cstring c2)
{
  bool res = cstring_equal (c1, c2);
  cstring_free (c1);
  cstring_free (c2);
  return res;
}

bool cstring_equal (cstring c1, cstring c2)
{
  if (c1 == c2) return TRUE;
  else if (cstring_isUndefined (c1)) return cstring_isEmpty (c2);
  else if (cstring_isUndefined (c2)) return cstring_isEmpty (c1);
  else return (strcmp (c1, c2) == 0);
}

bool cstring_equalLen (cstring c1, cstring c2, size_t len)
{
  if (c1 == c2) return TRUE;
  else if (cstring_isUndefined (c1)) return cstring_isEmpty (c2);
  else if (cstring_isUndefined (c2)) return cstring_isEmpty (c1);
  else return (strncmp (c1, c2, len) == 0);
}

bool cstring_equalCaseInsensitive (cstring c1, cstring c2)
{
  if (c1 == c2) return TRUE;
  else if (cstring_isUndefined (c1)) return cstring_isEmpty (c2);
  else if (cstring_isUndefined (c2)) return cstring_isEmpty (c1);
  else return (cstring_genericEqual (c1, c2, 0, TRUE, FALSE) != CGE_DISTINCT);
}

bool cstring_equalLenCaseInsensitive (cstring c1, cstring c2, size_t len)
{
  if (c1 == c2) return TRUE;
  else if (cstring_isUndefined (c1)) return cstring_isEmpty (c2);
  else if (cstring_isUndefined (c2)) return cstring_isEmpty (c1);
  else return (cstring_genericEqual (c1, c2, len, TRUE, FALSE) != CGE_DISTINCT);
}

bool cstring_equalPrefix (cstring c1, cstring c2)
{
  llassert (c2 != NULL);

  if (cstring_isUndefined (c1)) 
    {
      return (strlen (c2) == 0);
    }

  return (strncmp (c1, c2, strlen (c2)) == 0);
}

bool cstring_equalPrefixLit (cstring c1, const char *c2)
{
  llassert (c2 != NULL);

  if (cstring_isUndefined (c1)) 
    {
      return (strlen (c2) == 0);
    }

  return (strncmp (c1, c2, strlen (c2)) == 0);
}

int cstring_xcompare (cstring *c1, cstring *c2)
{
  return (cstring_compare (*c1, *c2));
}

int cstring_compare (cstring c1, cstring c2)
{
  int res;

  if (c1 == c2)
    {
      res = 0;
    }
  else if (cstring_isUndefined (c1))
    {
      if (cstring_isEmpty (c2))
	{
	  res = 0;
	}
      else
	{
	  res = 1;
	}
    }
  else if (cstring_isUndefined (c2))
    {
      if (cstring_isEmpty (c1))
	{
	  res = 0;
	}
      else
	{
	  res = -1;
	}
    }
  else
    {
      res = strcmp (c1, c2);
    }

    return (res);
}

void cstring_markOwned (/*@owned@*/ cstring s)
{
  sfreeEventually (s);
}

void cstring_free (/*@only@*/ cstring s)
{
  if (cstring_isDefined (s)) 
    {
      /*drl 2/3/2002*/
      s[0] = '\0';
      
      sfree (s);
    }
}

cstring cstring_fromChars (/*@exposed@*/ const char *cp)
{
  return (cstring) cp;
}

/*@exposed@*/ char *cstring_toCharsSafe (cstring s)
{
  static /*@only@*/ cstring emptystring = cstring_undefined;

  if (cstring_isDefined (s))
    {
      return (char *) s;
    }
  else
    {
      if (cstring_isUndefined (emptystring))
	{
	  emptystring = cstring_newEmpty ();
	}

      return emptystring;
    }
}

size_t cstring_length (cstring s)
{
  if (cstring_isDefined (s))
    {
      return strlen (s);
    }

  return 0;
}

cstring
cstring_capitalize (cstring s) /*@requires maxSet(s) >= 0 @*/
{
  if (!cstring_isEmpty (s))
    {
      cstring ret = cstring_copy (s);

      cstring_setChar (ret, 1, (char) toupper ((int) cstring_firstChar (ret)));
      return ret;
    }
  
  return cstring_undefined;
}

cstring
cstring_capitalizeFree (cstring s) /*@requires maxSet(s) >= 0 /\ maxRead(s) >= 0 @*/
{
  if (!cstring_isEmpty (s))
    {
      cstring_setChar (s, 1, (char) toupper ((int) cstring_firstChar (s)));
      return s;
    }
  
  return s;
}

cstring
cstring_clip (cstring s, size_t len)
{
  if (cstring_isUndefined (s) || cstring_length (s) <= len)
    {
      ;
    }
  else
    {
      llassert (s != NULL);
      
      *(s + len) = '\0';
    }
  
  return s;
}

/*@only@*/ cstring
cstring_elide (cstring s, size_t len)
{
  if (cstring_isUndefined (s) || cstring_length (s) <= len)
    {
      return cstring_copy (s);
    }
  else
    {
      cstring sc = cstring_create (len);
      
      strncpy (sc, s, len);
      
      *(sc + len - 1) = '\0';
      *(sc + len - 2) = '.';      
      *(sc + len - 3) = '.';      
      *(sc + len - 4) = '.';      
      
      return sc;
    }
}

/*@only@*/ cstring
cstring_fill (cstring s, size_t n) /*@requires n >= 0 @*/
{
  cstring t = cstring_create (n + 1);
  cstring ot = t;
  size_t len = cstring_length (s);
  size_t i;
  
  if (len > n)
    {
      for (i = 0; i < n; i++)
	{
		  
	  *t++ = *s++;
	}
      *t = '\0';
    }
  else
    {
      for (i = 0; i < len; i++)
	{
	
	  *t++ = *s++;
	}
      for (i = 0; i < n - len; i++)
	{

	  *t++ = ' ';
	}
      *t = '\0';
    }

  return ot;
}

cstring
cstring_downcase (cstring s)
{
  if (cstring_isDefined (s))
    {
      cstring t = cstring_create (strlen (s) + 1);
      cstring ot = t;
      char c;
      
      while ((c = *s) != '\0')
	{
	  if (c >= 'A' && c <= 'Z')
	    {
	      c = c - 'A' + 'a';
	    }
	  *t++ = c;
	  s++;
	}
      *t = '\0';
      
      return ot;
    }
  else
    {
      return cstring_undefined;
    }
}

/*@notnull@*/ cstring 
cstring_appendChar (/*@only@*/ cstring s1, char c)
{
  size_t l = cstring_length (s1);
  char *s;

  s = (char *) dmalloc (sizeof (*s) * (l + 2));

  if (cstring_isDefined (s1))
    {  
      strcpy (s, s1);
      *(s + l) = c;
      *(s + l + 1) = '\0';
      sfree (s1); 
    }
  else
    {
      *(s) = c;
       *(s + 1) = '\0';
    } 

  return s;
}

/*@only@*/ cstring 
cstring_concatFree (cstring s, cstring t)
{
  cstring res = cstring_concat (s, t);
  cstring_free (s);
  cstring_free (t);
  return res;
}

/*@only@*/ cstring 
cstring_concatFree1 (cstring s, cstring t)
{
  cstring res = cstring_concat (s, t);
  cstring_free (s);
  return res;
}

/*@only@*/ cstring 
cstring_concatChars (cstring s, char *t)
{
  cstring res = cstring_concat (s, cstring_fromChars (t));
  cstring_free (s);
  return res;
}

/*@only@*/ cstring 
cstring_concatLength (cstring s1, char *s2, size_t len) /*@requires maxSet(s2) >= (len - 1) @*/
{
  cstring tmp = cstring_copyLength (s2, len);
  cstring res = cstring_concat (s1, tmp);
  cstring_free (tmp);
  cstring_free (s1);

  return res;
}

/*@only@*/ cstring 
cstring_concat (cstring s, cstring t) /*@requires maxSet(s) >= 0 @*/
{
  char *ret = mstring_create (cstring_length (s) + cstring_length (t));

  if (cstring_isDefined (s))
    {
      strcpy (ret, s);
    }
  if (cstring_isDefined (t))
    {
      strcat (ret, t);
    }

  return ret;
}

/*@notnull@*/ /*@only@*/ cstring 
cstring_prependCharO (char c, /*@only@*/ cstring s1)
{
  cstring res = cstring_prependChar (c, s1);

  cstring_free (s1);
  return (res);
}

/*@notnull@*/ /*@only@*/ cstring 
cstring_prependChar (char c, /*@temp@*/ cstring s1)
{
  size_t l = cstring_length (s1);
  char *s = (char *) dmalloc (sizeof (*s) * (l + 2));
  
  *(s) = c;

  if (cstring_isDefined (s1)) 
    {
      /*@-mayaliasunique@*/ 
      strcpy (s + 1, s1);
      /*@=mayaliasunique@*/ 
    }

 *(s + l + 1) = '\0';
  return s;
}

bool
cstring_hasNonAlphaNumBar (cstring s)
{
  int c;

  if (cstring_isUndefined (s)) return FALSE;

 while ((c = (int) *s) != (int) '\0')
    {
      if ((isalnum (c) == 0) && (c != (int) '_')
	  && (c != (int) '.') && (c != (int) CONNECTCHAR))
	{
	  return TRUE;
	}

      s++;
    }
  return FALSE;
}

/*@only@*/ /*@notnull@*/ cstring 
cstring_create (size_t n)
{
  char *s = dmalloc (sizeof (*s) * (n + 1));
  
  *s = '\0';
  return s;
}

/*@only@*/ /*@notnull@*/ cstring
cstring_copySegment (cstring s, size_t findex, size_t tindex)
{
  cstring res = cstring_create (tindex - findex + 1);

  llassert (cstring_isDefined (s));
  llassert (cstring_length (s) > tindex);

  strncpy (res, (s + findex), size_fromInt (size_toInt (tindex - findex) + 1));
  return res;
}

lsymbol cstring_toSymbol (cstring s)
{
  lsymbol res = lsymbol_fromString (s);

  cstring_free (s);
  return res;
}

cstring cstring_bsearch (cstring key, char **table, int nentries)
{
  if (cstring_isDefined (key))
    {
      int low = 0;
      int high = nentries;
      int mid = (high + low + 1) / 2;
      int last = -1;
      cstring res = cstring_undefined;

      while (low <= high && mid < nentries)
	{
	  int cmp;

	  llassert (mid != last);
	  llassert (mid >= 0 && mid < nentries);

	  cmp = cstring_compare (key, table[mid]);
	  
	  if (cmp == 0)
	    {
	      res = table[mid];
	      break;
	    }
	  else if (cmp < 0) /* key is before table[mid] */
	    {
	      high = mid - 1;
	    }
	  else /* key of after table[mid] */
	    {
	      low = mid + 1;
	    }

	  last = mid;
	  mid = (high + low + 1) / 2;
	}

      if (mid != 0 && mid < nentries - 1)
	{
	  llassert (cstring_compare (key, table[mid - 1]) > 0);
	 llassert (cstring_compare (key, table[mid + 1]) < 0);
	}

      return res;
    }
  
  return cstring_undefined;
}

extern /*@observer@*/ cstring cstring_advanceWhiteSpace (cstring s)
{
  if (cstring_isDefined (s)) {
    char *t = s;

   while (*t != '\0' && isspace ((int) *t)) {
      t++;
    }

    return t;
  }
  
  return cstring_undefined;
}

/* changes strings like "sdf" "sdfsd" into "sdfsdfsd"*/
/* This function understands that "sdf\"  \"sdfsdf" is okay*/
static mstring doMergeString (cstring s)
{
  char *ptr;
  mstring ret;
  char * retPtr;
  bool escape;
  
  llassert(cstring_isDefined (s));
  
  ret = mstring_create (cstring_length(s) );

  ptr = s;

  retPtr = ret;
  /*
  llassert(*ptr == '\"');

  *retPtr = *ptr;

  retPtr++;
  ptr++;
  */

  while (*ptr != '\0')
    {
      escape = FALSE;
      
      if (*ptr == '\\')
	{
	  *retPtr = *ptr;
	  
	  if (!escape)
	    escape = TRUE;
	  else
	    /* case of escaped \ ('\\')  */
	    escape = FALSE;
	}
      else if ( (*ptr == '\"') && (!escape) )
	{
	  while ( (ptr[1] != '\"') && (ptr[1] != '\0') )
	    {
	      ptr++;
	    }
	  if (ptr[1] == '\0')
	    {
	      llassert(*ptr == '\"');
	      *retPtr =  '\"';
	      retPtr++;
	      *retPtr = '\0';
	      BADEXIT;

	      /*@notreached@*/ return ret;
	    }
	  else
	    {
	      ptr++;
	    }
	}
      else
	{
	  *retPtr = *ptr;
	}

      retPtr++;
      ptr++;
      
    }/* end while */
  *retPtr = '\0';
  return ret;
}

static mstring doExpandEscapes (cstring s, /*@out@*/ size_t *len)
{
  char *ptr;
  mstring ret;
  char * retPtr;
  
  llassert(cstring_isDefined (s));
  
  ret = mstring_create (cstring_length(s));

  ptr = s;

  retPtr = ret;
  while (*ptr != '\0')
    {
      if (*ptr != '\\')
	{
	  *retPtr = *ptr;
	  retPtr++;
	  ptr++;
	  continue;
	}
      
      if (*ptr == '\\')
	{
	  ptr++;
	  if (*ptr == '\0')
	    {
	      /*not a legal escape sequence but try to handle it in a sesible way*/
	      *retPtr = '\\';
	      retPtr++;
	    }
	  
	  /* Handle Octal escapes  */
	  else if (*ptr >= '0' && *ptr <= '9' )
	    {
	      int total;
	      total = (int)(*ptr - '0');
	      ptr++;
	      /*octal can only be 3 characters long */
	      if (*ptr != '\0' &&  (*ptr >= '0' && *ptr <= '9' ) )
		{
		  total *= 8;
		  ptr++;
		  if (*ptr != '\0' &&  (*ptr >= '0' && *ptr <= '9' ) )
		    {
		      total *= 8;
		      total += (int) (*ptr - '0');
		      ptr++;
		    }
		}
	      
	      *retPtr =  (char) total;
	      retPtr++;
	    }
	  
	  else if (*ptr == 'x')
	    {
	      int total;
	      total = 0;
	      ptr++;
	      if (!(*ptr != '\0' &&
		    ( (*ptr >= '0' && *ptr <= '9' ) ||
		      (toupper(*ptr) >= (int)('A') && toupper(*ptr) <= (int)('F') ) )
		      ))
		{
		  total = (int)'x';
		}
	      else
		{
		  while (*ptr != '\0' &&
		    ( (*ptr >= '0' && *ptr <= '9' ) ||
		      (toupper(*ptr) >= ((int)('A')) && toupper(*ptr) <= ((int)'F') ) )
			 )
		    {
		      total *= 16;
		      if (*ptr >= '0' && *ptr <= '9' )
			total += (int)(*ptr - '0');
		      else
			total += ( (toupper(*ptr) - 'A') + 10);
		      ptr++;
		    }
		}
	      *retPtr =  (char) total;
	      retPtr++;
	    }
	  else
	    {
	      switch ( *ptr )
		{
		case 'a':
		  *retPtr = '\a';
		  retPtr++;
		  /*@switchbreak@*/ break;

		case 'b':
		  *retPtr = '\b';
		  retPtr++;
		  /*@switchbreak@*/ break;

		case 'f':
		  *retPtr = '\f';
		  retPtr++;
		  /*@switchbreak@*/ break;

		case 'n':
		  *retPtr = '\n';
		  retPtr++;
		  /*@switchbreak@*/ break;

		case 'r':
		  *retPtr = '\r';
		  retPtr++;
		  /*@switchbreak@*/ break;

		case 't':
		  *retPtr = '\t';
		  retPtr++;
		  /*@switchbreak@*/ break;
		  /* ' " ? \ */
		  /* we assume invalid sequences are handled somewhere else
		     so we handle an invalid sequence of the form \char by replacing
		     it with char (this is what gcc does) the C standard says a diagnostic is
		     required..*/
		default:
		  *retPtr = *ptr;
		  retPtr++;
		}
	      ptr++;
	    }
	  
	}/*end outer if*/
      
    }/*end while */

  /* add the null character */
  *retPtr = '\0';

  llassert(  (retPtr-ret) >= 0 );
  *len = (size_t)(retPtr - ret);
  return ret;
}


/*this function is like sctring_expandEscapses */
mstring cstring_expandEscapes (cstring s)
{
  size_t len;

  mstring ret;
  ret = doExpandEscapes (s, &len);
  return ret;
}

size_t cstring_lengthExpandEscapes (cstring s)
{
  size_t len;
  mstring tmpStr, tmpStr2;

  tmpStr = doMergeString (s);
  tmpStr2 = doExpandEscapes (tmpStr, &len);
  
  cstring_free(tmpStr);
  cstring_free(tmpStr2);

  return len;
}

cstring cstring_replaceChar(/*@returned@*/ cstring c, char oldChar, char newChar)
{
  char *ptr;
  llassert(oldChar != '\0');
  if (cstring_isUndefined(c) )
    {
      llcontbug(cstring_makeLiteral("cstring_replaceChar called with undefined string"));
      return c;
    }
  
  ptr = c;
  while (*ptr != '\0')
    {
      if (*ptr == oldChar)
	*ptr = newChar;
      ptr++;
    }

  return c;
}








