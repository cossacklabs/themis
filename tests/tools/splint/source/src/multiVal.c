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
** multiVal.c
*/

# include "splintMacros.nf"
# include "basic.h"

/*@only@*/ multiVal multiVal_unknown ()
{
  return multiVal_undefined;
}

static /*@special@*/ /*@notnull@*/ multiVal multiVal_create (mvkind kind)
   /*@defines result->kind@*/
{
  multiVal mv = (multiVal) dmalloc (sizeof (*mv));

  mv->kind = kind;
  return mv;
}

/*@only@*/ multiVal multiVal_makeInt (long x)
{
  multiVal mv = multiVal_create (MVLONG);

  mv->value.ival = x;
  return mv;
}

/*@only@*/ multiVal multiVal_makeChar (char x)
{
  multiVal mv = multiVal_create (MVCHAR);
  mv->value.cval = x;
  return mv;
}

/*@only@*/ multiVal multiVal_makeDouble (double x)
{
  multiVal mv = multiVal_create (MVDOUBLE);

    mv->value.fval = x;
    return mv;
}

/*@only@*/ multiVal multiVal_makeString (/*@only@*/ cstring s)
{
  multiVal mv = multiVal_create (MVSTRING);

  mv->value.sval = s;
  return mv;
}


/*@only@*/ multiVal multiVal_copy (multiVal m)
{
  multiVal r;

  if (multiVal_isUndefined (m))
    {
      return multiVal_undefined;
    }

  r = multiVal_create (m->kind);
      
  switch (m->kind)
    {
    case MVLONG:
      r->value.ival = m->value.ival;
      break;
    case MVCHAR:
      r->value.cval = m->value.cval;
      break;
    case MVDOUBLE:
      r->value.fval = m->value.fval;
      break;
    case MVSTRING:
      r->value.sval = cstring_copy (m->value.sval);
      break;
    }
  
  return r;
}

multiVal multiVal_invert (multiVal m)
{
  if (multiVal_isUndefined (m))
    {
      return multiVal_undefined;
    }
      
  switch (m->kind)
    {
    case MVLONG:
      return multiVal_makeInt (-1 * m->value.ival);
    case MVCHAR:
      BADBRANCHCONT;
      return multiVal_undefined;
    case MVDOUBLE:
      return multiVal_makeDouble (-1.0 * m->value.fval);
    case MVSTRING:
      BADBRANCHCONT;
      return multiVal_undefined;
    }

  BADEXIT;
}

long multiVal_forceInt (multiVal m)
{
  if (multiVal_isInt (m)) {
    return m->value.ival;
  } else {
    llcontbug (message ("Multival is not int: %s", multiVal_unparse (m)));
    return 0;
  }
}

char multiVal_forceChar (multiVal m)
{
  llassert (multiVal_isChar (m));
  
  return m->value.cval;
}

double multiVal_forceDouble (multiVal m)
{
  llassert (multiVal_isDouble (m));

  return m->value.fval;
}

/*@dependent@*/ /*@observer@*/ cstring multiVal_forceString (multiVal m)
{
  llassert (multiVal_isString (m));

  return m->value.sval;
}

bool multiVal_isInt (multiVal m)
{
  return (multiVal_isDefined (m) && m->kind == MVLONG);
}

bool multiVal_isChar (multiVal m)
{
  return (multiVal_isDefined (m) && m->kind == MVCHAR);
}

bool multiVal_isDouble (multiVal m)
{
  return (multiVal_isDefined (m) && m->kind == MVDOUBLE);
}

bool multiVal_isString (multiVal m)
{
  return (multiVal_isDefined (m) && m->kind == MVSTRING);
}

/*@only@*/ cstring multiVal_unparse (multiVal m)
{
  if (multiVal_isDefined (m))
    {
      switch (m->kind)
	{
	case MVLONG:
	  return message ("%d", (int)m->value.ival);
	case MVCHAR:
	  	  return message ("'%h'", m->value.cval);
	case MVDOUBLE:
	  return message ("%f", (float)m->value.fval);
	case MVSTRING:
	  return message ("%s", m->value.sval);
	}
      BADEXIT;
    }
  else
    {
      return (cstring_makeLiteral ("?"));
    }
}

/*@only@*/ cstring multiVal_dump (multiVal m)
{
  if (multiVal_isDefined (m))
    {
      switch (m->kind)
	{
	case MVLONG:
	  return (message ("i%d", (int)m->value.ival));
	case MVCHAR:
	  return (message ("c%d", (int)m->value.cval));
	case MVDOUBLE:
	  return (message ("d%f", (float)m->value.fval));
	case MVSTRING:
	  return (message ("s%s", m->value.sval));
	}
      BADEXIT;
    }
  else
    {
      return (cstring_undefined);
    }
}

/*@only@*/ multiVal multiVal_undump (char **s)
{
  char tchar = **s;

  switch (tchar)
    {
    case 'i':
      (*s)++;
      return multiVal_makeInt (reader_getInt (s));
    case 'c':
      (*s)++;
      return multiVal_makeChar ((char) reader_getInt (s));
    case 'd':
      (*s)++;
      return multiVal_makeDouble (reader_getDouble (s));
    case 's':
      {
	cstring st = cstring_undefined;

	(*s)++;
	while (**s != '#')
	  {
	    st = cstring_appendChar (st, **s);
	    (*s)++;
	  }

	return multiVal_makeString (st);
      }
    case '@':
    case '#':
      return multiVal_unknown ();
    BADDEFAULT;
    }
  
  BADEXIT;
}

int multiVal_compare (multiVal m1, multiVal m2)
{
  if (multiVal_isUndefined (m1))
    {
      if (multiVal_isUndefined (m2)) 
	{
	  return 0;
	}

      else return -1;
    }
  if (multiVal_isUndefined (m2))
    {
      return -1;
    }

  COMPARERETURN (generic_compare (m1->kind, m2->kind));

  switch (m1->kind)
    {
    case MVLONG:   return (generic_compare (m1->value.ival, m2->value.ival));
    case MVCHAR:   return (generic_compare (m1->value.cval, m2->value.cval));
    case MVDOUBLE: return (generic_compare (m1->value.fval, m2->value.fval));
    case MVSTRING: return (cstring_compare (m1->value.sval, m2->value.sval));
    }

  BADEXIT;
}

multiVal multiVal_add (multiVal m1, multiVal m2)
{
  if (multiVal_isUndefined (m1) || multiVal_isUndefined (m2) || m1->kind != m2->kind)
    {
      return multiVal_undefined;
    }

  switch (m1->kind)
    {
    case MVLONG:   return (multiVal_makeInt (m1->value.ival + m2->value.ival));
    case MVCHAR:   return (multiVal_makeChar ((char) (m1->value.cval + m2->value.cval)));
    case MVDOUBLE: return (multiVal_makeDouble (m1->value.fval + m2->value.fval));
    case MVSTRING: return multiVal_undefined;
    }

  BADEXIT;
}

multiVal multiVal_subtract (multiVal m1, multiVal m2)
{
  if (multiVal_isUndefined (m1) || multiVal_isUndefined (m2) || m1->kind != m2->kind)
    {
      return multiVal_undefined;
    }

  switch (m1->kind)
    {
    case MVLONG:   return (multiVal_makeInt (m1->value.ival - m2->value.ival));
    case MVCHAR:   return (multiVal_makeChar ((char) (m1->value.cval - m2->value.cval)));
    case MVDOUBLE: return (multiVal_makeDouble (m1->value.fval - m2->value.fval));
    case MVSTRING: return multiVal_undefined;
    }

  BADEXIT;
}

multiVal multiVal_multiply (multiVal m1, multiVal m2)
{
  if (multiVal_isUndefined (m1) || multiVal_isUndefined (m2) || m1->kind != m2->kind)
    {
      return multiVal_undefined;
    }

  switch (m1->kind)
    {
    case MVLONG:   return (multiVal_makeInt (m1->value.ival * m2->value.ival));
    case MVCHAR:   return (multiVal_makeChar ((char) (m1->value.cval * m2->value.cval)));
    case MVDOUBLE: return (multiVal_makeDouble (m1->value.fval * m2->value.fval));
    case MVSTRING: return multiVal_undefined;
    }

  BADEXIT;
}

multiVal multiVal_divide (multiVal m1, multiVal m2)
{
  if (multiVal_isUndefined (m1) || multiVal_isUndefined (m2) || m1->kind != m2->kind)
    {
      return multiVal_undefined;
    }

  switch (m1->kind)
    {
    case MVLONG: 
      if (m2->value.ival != 0)
	{
	  return (multiVal_makeInt (m1->value.ival / m2->value.ival));
	}
      else
	{
	  return multiVal_undefined;
	}
    case MVCHAR:   
      if (m2->value.cval != (char) 0)
	{
	  return (multiVal_makeChar ((char) (m1->value.cval / m2->value.cval)));
	}
      else
	{
	  return multiVal_undefined;
	}
    case MVDOUBLE:
      return multiVal_undefined; /* Don't attempt to divide floats */
    case MVSTRING: return multiVal_undefined;
    }

  BADEXIT;
}

void multiVal_free (/*@only@*/ multiVal m)
{
  if (multiVal_isDefined (m))
    {
      if (m->kind == MVSTRING)
	{
	  cstring_free (m->value.sval);
	}
      
      sfree (m);
    }
}




