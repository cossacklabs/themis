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
** varDeclarationNodeList.c
**
** based on list_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"

varDeclarationNodeList varDeclarationNodeList_new ()
{
  varDeclarationNodeList s = (varDeclarationNodeList) dmalloc (sizeof (*s));
   
  s->nelements = 0;
  s->nspace = varDeclarationNodeListBASESIZE;
  s->elements = (varDeclarationNode *) 
    dmalloc (sizeof (*s->elements) * varDeclarationNodeListBASESIZE);

  return (s);
}

static void
varDeclarationNodeList_grow (varDeclarationNodeList s)
{
  int i;
  varDeclarationNode *newelements; 

  s->nspace += varDeclarationNodeListBASESIZE;

  newelements = (varDeclarationNode *) dmalloc (sizeof (*newelements) 
						* (s->nelements + s->nspace));

  for (i = 0; i < s->nelements; i++)
    {
      newelements[i] = s->elements[i];
    }

  sfree (s->elements);
  s->elements = newelements;
}

void 
varDeclarationNodeList_addh (varDeclarationNodeList s, /*@keep@*/ varDeclarationNode el)
{
  if (s->nspace <= 0)
    varDeclarationNodeList_grow (s);

  s->nspace--;
  s->elements[s->nelements] = el;
  s->nelements++;
}

/*@only@*/ cstring
varDeclarationNodeList_unparse (varDeclarationNodeList s)
{
  cstring st = cstring_undefined;

  varDeclarationNodeList_elements (s, current)
  {
    if (current->isPrivate)
      st = message ("%q private ", st);

    st = message ("%q%q %q;\n", st, lclTypeSpecNode_unparse (current->type),
		  initDeclNodeList_unparse (current->decls));
  } end_varDeclarationNodeList_elements;

  return st;
}


void
varDeclarationNodeList_free (varDeclarationNodeList s)
{
  int i;
  for (i = 0; i < s->nelements; i++)
    {
      varDeclarationNode_free (s->elements[i]); 
    }

  sfree (s->elements);
  sfree (s);
}
