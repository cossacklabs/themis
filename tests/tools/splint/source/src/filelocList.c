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
** filelocList.c (from slist_template.c)
*/

# include "splintMacros.nf"
# include "basic.h"
# include "filelocList.h"

/*
** Invariant:  If any member of the list is fileloc_undefined, then
**             the 0th member is fileloc_undefined.
*/

filelocList
filelocList_new ()
{
  return (filelocList_undefined);
}

static /*@notnull@*/ /*@only@*/ filelocList
filelocList_newEmpty (void)
{
  filelocList s = (filelocList) dmalloc (sizeof (*s));
  
  s->nelements = 0;
  s->free = filelocListBASESIZE;
  s->elements = (fileloc *) dmalloc (sizeof (*s->elements) * filelocListBASESIZE);

  return (s);
}

static void
filelocList_grow (/*@notnull@*/ filelocList s)
{
  int i;
  o_fileloc *oldelements = s->elements;
  
  s->free += filelocListBASESIZE; 
  s->elements = (fileloc *) dmalloc (sizeof (*s->elements) 
				     * (s->nelements + s->free));
    
  for (i = 0; i < s->nelements; i++)
    {
      s->elements[i] = oldelements[i];
    }
  
  sfree (oldelements);
}

filelocList 
filelocList_append (/*@returned@*/ filelocList s, /*@only@*/ filelocList t)
{
  llassert (NOALIAS (s, t));

  if (filelocList_isUndefined (t) || filelocList_isEmpty (t)) return s;

  if (filelocList_isUndefined (s)) 
    {
      s = filelocList_newEmpty ();
    }

  filelocList_elements (t, fl)
    {
      /* Okay to use exposed storage here, t is begin eaten. */

      /*@-exposetrans@*/ /*@-dependenttrans@*/
      s = filelocList_add (s, fl);
      /*@=exposetrans@*/ /*@=dependenttrans@*/
    } end_filelocList_elements;

  sfree (t->elements);
  sfree (t);

  return s;
}

filelocList 
  filelocList_addUndefined (/*@returned@*/ filelocList s)
{
  if (filelocList_isUndefined (s) 
      || s->nelements == 0
      || fileloc_isDefined (s->elements[0]))
    {
      return (filelocList_add (s, fileloc_undefined));
    }
  else
    {
      return s;
    }
}

static bool filelocList_hasUndefinedLoc (filelocList s)
{
  return (filelocList_isDefined (s) 
	  && s->nelements > 0
	  && fileloc_isUndefined (s->elements[0]));
}
  
filelocList 
  filelocList_addDifferentFile (/*@returned@*/ filelocList s,
				fileloc where,
				fileloc loc)
{
  if (filelocList_hasUndefinedLoc (s) || filelocList_size (s) >= 2)
    {
      return s;
    }
  else
    {
      if (fileloc_sameModule (where, loc))
	{
	  if (filelocList_isEmpty (s))
	    {
	      return filelocList_add (s, fileloc_copy (loc));
	    }
	  else
	    {
	      return s;
	    }
	}
      else
	{
	  return filelocList_addUndefined (s);
	}
    }
}

filelocList 
  filelocList_add (/*@returned@*/ filelocList s, /*@only@*/ fileloc el)
{
  if (filelocList_isUndefined (s))
    {
      s = filelocList_newEmpty ();
    }

  if (s->free <= 0)
    {
      filelocList_grow (s);
    }
  
  s->free--;
  s->elements[s->nelements] = el;

  if (fileloc_isUndefined (el))
    {
      s->elements[s->nelements] = s->elements[0];
      s->elements[0] = fileloc_undefined;
    }

  s->nelements++;
  return s;
}

/*@only@*/ cstring
filelocList_unparse (filelocList s)
{
   int i;
   cstring st = cstring_makeLiteral ("[");

   if (filelocList_isDefined (s))
     {
       for (i = 0; i < filelocList_size (s); i++)
	 {
	   if (i == 0)
	     {
	       st = message ("%q %q", st, fileloc_unparse (s->elements[i]));
	     }
	   else
	     st = message ("%q, %q", st, fileloc_unparse (s->elements[i]));
	 }
     }
   
   st = message ("%q ]", st);
   return st;
}

int filelocList_realSize (filelocList s)
{
  int size = 0;

  filelocList_elements (s, el)
    {
      if (fileloc_isDefined (el))
	{
	  size++;
	}
    } end_filelocList_elements;

  return size;
}

cstring filelocList_unparseUses (filelocList s)
{
  int i;
  size_t linelen = 0;
  int maxlen = context_getLineLen () - 3;
  cstring st = cstring_undefined;
  fileId lastFile = fileId_invalid;
  bool parenFormat = context_getFlag (FLG_PARENFILEFORMAT); 

  if (filelocList_isDefined (s))
    {
      bool firstone = TRUE;

      for (i = 0; i < filelocList_size (s); i++)
	{
	  if (fileloc_isDefined (s->elements[i]))
	    {
	      if (firstone)
		{
		  st = fileloc_unparse (s->elements[i]);
		  lastFile = fileloc_fileId (s->elements[i]);
		  linelen = 3 + cstring_length (st);
		  firstone = FALSE;
		}
	      else
		{
		  if (fileId_equal (fileloc_fileId (s->elements[i]), lastFile))
		    {
		      if (linelen + 7 > size_fromInt (maxlen))
			{
			  st = message ("%q\n      ", st);
			  linelen = 6;
			}
		      else
			{
			  st = message ("%q, ", st);
			}
		      
		      if (parenFormat)
			{
			  st = message ("%q(%d,%d)", 
					st, fileloc_lineno (s->elements[i]), 
					fileloc_column (s->elements[i]));
			}
		      else
			{
			  st = message ("%q%d:%d", 
					st, fileloc_lineno (s->elements[i]), 
					fileloc_column (s->elements[i]));
			}
		      
		      linelen += 3 + int_log (fileloc_lineno (s->elements[i])) 
			+ int_log (fileloc_column (s->elements[i]));
		    }
		  else
		    {
		      cstring fl = fileloc_unparse (s->elements[i]);
		      st = message ("%q\n   %s", st, fl);
		      lastFile = fileloc_fileId (s->elements[i]);
		      linelen = 3 + cstring_length (fl);
		      cstring_free (fl);
		    }
		}
	    }
	}
    }
  
  return st;
}

void
filelocList_free (/*@only@*/ filelocList s)
{
  if (filelocList_isDefined (s))
    {
      int i;
      for (i = 0; i < s->nelements; i++)
	{
	  fileloc_free (s->elements[i]); 
	}
      
      sfree (s->elements); 
      sfree (s);
    }
}






