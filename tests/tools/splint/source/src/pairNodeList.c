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
** pairNodeList.c
**
** based on list_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"

/*@only@*/ pairNodeList
pairNodeList_new ()
{
  pairNodeList s = (pairNodeList) dmalloc (sizeof (*s));
  
  s->nelements = 0;
  s->nspace = pairNodeListBASESIZE;
  s->elements = (pairNode *) 
    dmalloc (sizeof (*s->elements) * pairNodeListBASESIZE);

  return (s);
}

static void
pairNodeList_grow (/*@notnull@*/ pairNodeList s)
{
  int i;
  pairNode *newelements; 

  s->nspace += pairNodeListBASESIZE;
  newelements = (pairNode *) dmalloc (sizeof (*newelements) 
				      * (s->nelements + s->nspace));

  for (i = 0; i < s->nelements; i++)
    {
      newelements[i] = s->elements[i];
    }

  sfree (s->elements); 
  s->elements = newelements;
}

void 
pairNodeList_addh (pairNodeList s, pairNode el)
{
  llassert (pairNodeList_isDefined (s));
  llassert (pairNodeListBASESIZE > 0);

  if (s->nspace <= 0)
    pairNodeList_grow (s);

  s->nspace--;
  s->elements[s->nelements] = el;
  s->nelements++;
}

/*@only@*/ cstring
pairNodeList_unparse (pairNodeList s)
{
  cstring st = cstring_undefined;
  
  if (s != (pairNodeList)0)
    {
      pairNodeList_elements (s, current)
	{
	  if (current != NULL)
	    {
	      st = message ("%q%s %s; ", st, 
			    sort_unparseName (current->sort), 
			    ltoken_getRawString (current->tok));
	    }
	} end_pairNodeList_elements;
    }

  return st;
}

void
pairNodeList_free (/*@only@*/ pairNodeList s)
{
  if (pairNodeList_isDefined (s))
    {
      int i;
      for (i = 0; i < s->nelements; i++)
	{
	  pairNode_free (s->elements[i]); 
	}
      
      sfree (s->elements); 
      sfree (s);
    }
}
