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
** enumNameList.c
**
** based on list_template.c
**
** where T has T_equal (or change this) and T_unparse
**
** used to be cenum.c
*/

# include "splintMacros.nf"
# include "basic.h"

enumNameList
  enumNameList_new ()
{
  enumNameList s = (enumNameList) dmalloc (sizeof (*s));

  s->nelements = 0;
  s->nspace = enumNameListBASESIZE;
  s->elements = (enumName *)
    dmalloc (sizeof (*s->elements) * enumNameListBASESIZE);

  return (s);
}

/*@only@*/ enumNameList
enumNameList_single (/*@keep@*/ enumName t)
{
  enumNameList s = (enumNameList) dmalloc (sizeof (*s));
  
  s->nelements = 1;
  s->nspace = enumNameListBASESIZE - 1;
  s->elements = (enumName *) dmalloc (sizeof (*s->elements) * enumNameListBASESIZE);
          s->elements[0] = t;

  return (s);
}

bool
enumNameList_match (enumNameList e1, enumNameList e2)
{
  int i;

  if (e1->nelements != e2->nelements) return FALSE;

  for (i = 0; i < e1->nelements; i++)
    {
                              if (!cstring_equal (e1->elements[i], e2->elements[i]))
	return FALSE;
    }
  return TRUE;
}

static void
enumNameList_grow (enumNameList s)
{
  int i;
  enumName *newelements;

  s->nspace += enumNameListBASESIZE;
  newelements = (enumName *) dmalloc (sizeof (*newelements) * (s->nelements + s->nspace));

  if (newelements == (enumName *) 0)
    {
      llfatalerror (cstring_makeLiteral ("enumNameList_grow: out of memory!"));
    }

  for (i = 0; i < s->nelements; i++)
    {
                              newelements[i] = s->elements[i];
    }

  sfree (s->elements);
  s->elements = newelements;
}

void 
enumNameList_addh (enumNameList s, /*@keep@*/ enumName el)
{
  if (s->nspace <= 0)
    enumNameList_grow (s);

  s->nspace--;
    s->elements[s->nelements] = el;
  s->nelements++;
}

enumNameList 
enumNameList_push (/*@returned@*/ enumNameList s, /*@only@*/ enumName el)
{
  enumNameList_addh (s, el);
  return s;
}

/*@only@*/ enumNameList 
enumNameList_copy (enumNameList s)
{
  enumNameList r = enumNameList_new ();

  enumNameList_elements (s, x)
  {
    enumNameList_addh (r, cstring_copy (x));
  } end_enumNameList_elements;

  return r;
}

bool
enumNameList_member (enumNameList s, cstring m)
{
  enumNameList_elements (s, x)
  {
    if (cstring_equal (m, x)) return TRUE;
  } end_enumNameList_elements;

  return FALSE;
}

/*@only@*/ enumNameList
enumNameList_subtract (enumNameList source, enumNameList del)
{
  enumNameList ret = enumNameList_new ();

  enumNameList_elements (source, el)
    {
      if (!enumNameList_member (del, el))
	{
	  enumNameList_addh (ret, cstring_copy (el));
	}
    } end_enumNameList_elements;

  return ret;
}

cstring
enumNameList_unparse (enumNameList s)
{
  int i;
  cstring st = cstring_undefined;

  for (i = 0; i < s->nelements; i++)
    {
      if (i == 0) 
	{
	        	  st = cstring_copy (s->elements[i]);
	}
      else
	{
	        	  st = message ("%q, %s", st, s->elements[i]);
	}
    }

  return st;
}

cstring enumNameList_unparseBrief (enumNameList s)
{
  int i;
  cstring st = cstring_undefined;

  for (i = 0; i < s->nelements; i++)
    {
      if (i == 0)
	{
	        	  st = cstring_copy (s->elements[i]);
	}
      else if (i == 3 && s->nelements > 5)
	{
	  st = message ("%q, ...", st);
	  i = s->nelements - 2;
	}
      else
	{
	        	  st = message ("%q, %s", st, s->elements[i]);
	}
    }
  
  return st;
}

/*@only@*/ cstring
enumNameList_dump (enumNameList s)
{
  int i;
  cstring st = cstring_undefined;

  for (i = 0; i < s->nelements; i++)
    {
      if (i == 0)
	{
	        	  st = cstring_copy (s->elements[i]);
	}
      else
	      	st = message ("%q,%s", st, s->elements[i]);
    }
  return st;
}

/*@only@*/ enumNameList 
enumNameList_undump (d_char *s)
{
  enumNameList e = enumNameList_new ();

  if (**s == '}')
    (*s)++;
  else
    {
      while (TRUE)
	{
	  char *t = strchr (*s, ',');
	  char mt;

	  if (t == NULL)
	    {
	      t = strchr (*s, '}');

	      if (t == NULL)
		{
		  llcontbug (message ("enumNameList_undump: bad line: %s", cstring_fromChars (*s)));
		  return e; 
		}
	    }
	  
	  mt = *t;
	  *t = '\0';
	  
	  enumNameList_addh (e, cstring_fromChars (mstring_copy (*s)));
	  *s = t + 1;
	  if (mt == '}')
	    break;
	}
    }
  return e;
}

void
enumNameList_free (enumNameList s)
{
  int i;

  
  for (i = 0; i < s->nelements; i++)
    {
      cstring_free (s->elements[i]); 
    }

  sfree (s->elements); 
  sfree (s);
}


