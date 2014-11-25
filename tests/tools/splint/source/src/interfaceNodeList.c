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
** interfaceNodeList.c
**
** based on list_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"

/*@only@*/ interfaceNodeList
interfaceNodeList_new ()
{
  interfaceNodeList s = (interfaceNodeList) dmalloc (sizeof (*s));

  s->nelements = 0;
  s->nspacelow = interfaceNodeListGROWLOW;
  s->nspacehigh = interfaceNodeListGROWHI;
  s->elementsroot = (interfaceNode *) dmalloc (sizeof (*s->elements)
					       * interfaceNodeListBASESIZE);
  s->elements = s->elementsroot + interfaceNodeListGROWLOW;

  return (s);
}

static void
interfaceNodeList_grow (interfaceNodeList s)
{
  int i;
  interfaceNode *newelements; 
  
  newelements = (interfaceNode *) dmalloc 
    (sizeof (*newelements) * (s->nelements + interfaceNodeListBASESIZE));

  for (i = 0; i < s->nelements; i++)
    {
      newelements[i + interfaceNodeListGROWLOW] = s->elements[i];
    }
  
  s->nspacelow = interfaceNodeListGROWLOW;
  s->nspacehigh = interfaceNodeListGROWHI; 
  
  sfree (s->elementsroot);  
  s->elementsroot = newelements;

  s->elements = newelements + s->nspacelow;
}

interfaceNodeList
interfaceNodeList_addh (interfaceNodeList s, interfaceNode el)
{
  llassert (interfaceNodeListGROWHI > 0);

  if (s->nspacehigh <= 0)
    interfaceNodeList_grow (s);

  s->nspacehigh--;
  s->elements[s->nelements] = el;
  s->nelements++;

  return s;
}

void 
interfaceNodeList_addl (interfaceNodeList s, /*@keep@*/ interfaceNode el)
{
  llassert (interfaceNodeListGROWLOW > 0);

  if (s->nspacelow <= 0)
    interfaceNodeList_grow (s);

  s->nspacelow--;
  s->elements--;
  s->elements[0] = el;
  s->nelements++;
}

void
interfaceNodeList_free (interfaceNodeList s)
{
  int i;

  for (i = 0; i < s->nelements; i++)
    {
      interfaceNode_free (s->elements[i]);
    }

  sfree (s->elementsroot);
  sfree (s);
}
