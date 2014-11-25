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
** idDeclList.c
**
** based on list_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"

idDeclList
  idDeclList_singleton (/*@only@*/ idDecl e)
{
  idDeclList s = (idDeclList) dmalloc (sizeof (*s));
  
  s->nelements = 1;
  s->nspace = idDeclListBASESIZE - 1; 
  s->elements = (idDecl *) dmalloc (sizeof (*s->elements) * idDeclListBASESIZE);
  s->elements[0] = e;
  return (s);
}

static void
idDeclList_grow (idDeclList s)
{
  int i;
  idDecl *newelements;
  
  s->nspace += idDeclListBASESIZE; 
  newelements = (idDecl *) dmalloc (sizeof (*newelements) 
				    * (s->nelements + s->nspace));

  for (i = 0; i < s->nelements; i++)
    {
      newelements[i] = s->elements[i]; 
    }
  
  sfree (s->elements); 
  s->elements = newelements;
}

idDeclList idDeclList_add (idDeclList s, /*@only@*/ idDecl el)
{
  if (s->nspace <= 0)
    idDeclList_grow (s);
  
  s->nspace--;
  s->elements[s->nelements] = el;
  s->nelements++;

  return s;
}

/*@only@*/ cstring
idDeclList_unparse (idDeclList s)
{
   int i;
   cstring st = cstring_makeLiteral ("[");

   for (i = 0; i < s->nelements; i++)
     {
       if (i == 0)
	 {
	   st = message ("%q %q", st, idDecl_unparse (s->elements[i]));
	 }
       else
	 st = message ("%q, %q", st, idDecl_unparse (s->elements[i]));
     }
   
   st = message ("%q ]", st);
   return st;
}

void
idDeclList_free (idDeclList s)
{
  int i;
  for (i = 0; i < s->nelements; i++)
    {
      idDecl_free (s->elements[i]);
    }
  
  sfree (s->elements);
  sfree (s);
}
