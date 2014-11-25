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
** filelocStack.c (from slist_template.c)
*/

# include "splintMacros.nf"
# include "basic.h"
# include "filelocStack.h"

static /*@notnull@*/ /*@only@*/ filelocStack
filelocStack_newEmpty (void)
{
  filelocStack s = (filelocStack) dmalloc (sizeof (*s));
  
  s->nelements = 0;
  s->free = filelocStackBASESIZE;
  s->elements = (fileloc *) dmalloc (sizeof (*s->elements) * filelocStackBASESIZE);

  return (s);
}

filelocStack
filelocStack_new ()
{
  return (filelocStack_newEmpty ());
}

static void
filelocStack_grow (/*@notnull@*/ filelocStack s)
{
  o_fileloc *oldelements = s->elements;
  int i;
  
  s->free += filelocStackBASESIZE; 
  s->elements = (fileloc *) dmalloc (sizeof (*s->elements) 
				     * (s->nelements + s->free));
    
  for (i = 0; i < s->nelements; i++)
    {
                  s->elements[i] = oldelements[i];
    }
  
  sfree (oldelements);
}

static void 
  filelocStack_push (/*@returned@*/ filelocStack s, /*@keep@*/ fileloc el)
  /*@modifies s@*/
{
  llassert (filelocStack_isDefined (s));

  if (s->free <= 0)
    {
      filelocStack_grow (s);
    }
  
  s->free--;
    s->elements[s->nelements] = el;
  s->nelements++;
}

fileloc filelocStack_nextTop (filelocStack s)
{
  llassert (filelocStack_isDefined (s) && s->nelements > 1);

  return (s->elements[s->nelements - 2]);
}

void filelocStack_clear (filelocStack s)
{
  if (filelocStack_isDefined (s))
    {
      int i;

      for (i = 0; i < s->nelements; i++)
	{
	        	  fileloc_free (s->elements[i]);
	}

      s->free += s->nelements;
      s->nelements = 0;
    }
}

/*
** Returns TRUE of el is a new file.
*/

bool filelocStack_popPushFile (filelocStack s, fileloc el)
{
  int i;

  llassert (filelocStack_isDefined (s));

  for (i = s->nelements - 1; i >= 0; i--)
    {
                  if (fileloc_sameBaseFile (s->elements[i], el))
	{
	  int j;
	  
	  for (j = i; j < s->nelements; j++)
	    {
	            	      fileloc_free (s->elements[j]);
	    }
      	  s->elements[i] = el;
	  s->nelements = i + 1;
	  return FALSE;
	}
    }

  filelocStack_push (s, el);
  return TRUE;
}

/*@only@*/ cstring
filelocStack_unparse (filelocStack s)
{
   int i;
   cstring st = cstring_makeLiteral ("[");

   if (filelocStack_isDefined (s))
     {
       for (i = s->nelements - 1; i >= 0; i--)
	 {
	   if (i == s->nelements - 1)
	     {
	             	       st = message ("%q %q", st, fileloc_unparse (s->elements[i]));
	     }
	   else
	     {
	             	       st = message ("%q, %q", st, fileloc_unparse (s->elements[i]));
	     }
	 }
     }
   
   st = message ("%q ]", st);
   return st;
}

int filelocStack_includeDepth (filelocStack s)
{
  int depth = 0;
  int i;

  if (filelocStack_isDefined (s))
    {
      /* the zeroth element doesn't count! */
      for (i = s->nelements - 1; i > 0; i--)
	{
	        	  if (!fileloc_isSpecialFile (s->elements[i]))
	    {
	      depth++;
	    }
	}
    }

  return depth;
}

void
filelocStack_printIncludes (filelocStack s)
{
  if (filelocStack_isDefined (s))
    {
      int i;
      bool prep = context_isPreprocessing ();
      
      if (prep)
	{
	  /* need to do this for messages */
	  context_clearPreprocessing ();
	}

      /* don't show last two files pushed */
      for (i = s->nelements - 3; i >= 0; i--)
	{
	        	  if (i == 0 || !fileloc_isSpecialFile (s->elements[i]))
	    {
	            	      llgenindentmsg (cstring_makeLiteral ("Include site"),
			      s->elements[i]);
	    }
	}

      if (prep)
	{
	  context_setPreprocessing ();
	}
    }
}

void
filelocStack_free (/*@only@*/ filelocStack s)
{
  if (filelocStack_isDefined (s))
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






