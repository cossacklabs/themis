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
** fileLib.c
*/

# include "splintMacros.nf"
# include "basic.h"
# include "osd.h"

bool
fileLib_isCExtension (cstring ext)
{
  return (cstring_equalLit (ext, ".c") 
	  || cstring_equalLit (ext, ".C") 
	  || cstring_equalLit (ext, ".h")
	  || cstring_equalLit (ext, ".lh")
	  || cstring_equalLit (ext, ".xh")
	  || cstring_equalLit (ext, ".H")
	  || cstring_equalLit (ext, ".y")
	  || cstring_equalLit (ext, ".l"));
}

bool
fileLib_isLCLFile (cstring s)
{
  return (fileLib_hasExtension (s, LCL_EXTENSION));
}

/*@only@*/ cstring fileLib_withoutExtension (/*@temp@*/ cstring s, cstring suffix)
{
  /*@access cstring@*/
  char *t;
  char *s2;

  if (cstring_isUndefined (s)) {
    return cstring_undefined;
  }

  t = strrchr (s, '.');
  if (t == (char *) 0 || !mstring_equal (t, suffix))
    {
      return mstring_copy (s);
    }

  /*@-mods@*/ 
  *t = '\0';
  s2 = mstring_copy (s);
  *t = '.';
  /*@=mods@*/  /* Modification is undone. */
  return s2;
  /*@noaccess cstring@*/
}

/*@only@*/ cstring fileLib_removePath (cstring s)
{
  /*@access cstring@*/
  char *t;

  if (cstring_isUndefined (s)) {
    return cstring_undefined;
  }

  t = strrchr (s, CONNECTCHAR);

  if (t == NULL) return (mstring_copy (s));
  else return (mstring_copy (t + 1));
  /*@noaccess cstring@*/
}

/*@only@*/ cstring
fileLib_removePathFree (/*@only@*/ cstring s)
{
  /*@access cstring@*/
  char *t;


  if (cstring_isUndefined (s)) {
    return cstring_undefined;
  }

  t = strrchr (s, CONNECTCHAR);

# ifdef ALTCONNECTCHAR
  {
    char *at = strrchr (s, ALTCONNECTCHAR);
    if (t == NULL || (at > t)) {
      t = at;
    }
  }
# endif

  if (t == NULL) 
    {
      return (s);
    }
  else
    {
      char *res = mstring_copy (t + 1);
      mstring_free (s);
      return res;
    }
  /*@noaccess cstring@*/
}

/*@only@*/ cstring
fileLib_removeAnyExtension (cstring s)
{
  /*@access cstring@*/
  char *ret;
  char *t;


  if (cstring_isUndefined (s)) {
    return cstring_undefined;
  } 

  t = strrchr (s, '.');

  if (t == (char *) 0)
    {
      return mstring_copy (s);
    }

  /*@-mods@*/
  *t = '\0';
  ret = mstring_copy (s);
  *t = '.';
  /*@=mods@*/ /* modification is undone */

  return ret;
  /*@noaccess cstring@*/
}

/*@only@*/ cstring
fileLib_addExtension (/*@temp@*/ cstring s, cstring suffix)
{
  /*@access cstring@*/
  llassert (cstring_isDefined (s));

  if (strrchr (s, '.') == (char *) 0)
    {
      /* <<< was mstring_concatFree1 --- bug detected by splint >>> */
      return (cstring_concat (s, suffix));
    }
  else
    {
      return cstring_copy (s);
    }
}

bool fileLib_hasExtension (cstring s, cstring ext)
{
  return cstring_equal (fileLib_getExtension (s), ext);
}

/*@observer@*/ cstring fileLib_getExtension (/*@returned@*/ cstring s)
{
  llassert (cstring_isDefined (s));

  /*@access cstring@*/
  return (strrchr(s, '.'));
  /*@noaccess cstring@*/
}

cstring removePreDirs (cstring s)
{
  /*@access cstring@*/

  llassert (cstring_isDefined (s));

  while (*s == '.' && *(s + 1) == CONNECTCHAR) 
    {
      s += 2;
    }

# if defined(OS2) || defined(MSDOS)
  /* remove remainders from double path delimiters... */
  while (*s == CONNECTCHAR) 
    {
      ++s;
    }
# endif /* !defined(OS2) && !defined(MSDOS) */

  return s;
  /*@noaccess cstring@*/
}

bool isHeaderFile (cstring fname)
{
  cstring ext = fileLib_getExtension (fname);
  
  return (cstring_equalLit (ext, ".h")
	  || cstring_equalLit (ext, ".H")
	  || cstring_equal (ext, LH_EXTENSION));
}

cstring fileLib_cleanName (cstring s)
{
  if (cstring_equalPrefixLit (s, "./")) 
    {
      cstring res = cstring_copySegment (s, 2, cstring_length (s) - 1);
      cstring_free (s);
      return res;
    }

  return s;
}
