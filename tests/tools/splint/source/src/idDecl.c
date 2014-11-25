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
** idDecl.c
*/

# include "splintMacros.nf"
# include "basic.h"

/*@only@*/ idDecl
  idDecl_createClauses (/*@only@*/ cstring s, /*@only@*/ qtype t, /*@only@*/ functionClauseList clauses)
{
  idDecl d = (idDecl) dmalloc (sizeof (*d));

  d->id = s;
  d->typ = t;
  d->clauses = clauses;

  return (d);
}

/*@only@*/ idDecl
  idDecl_create (/*@only@*/ cstring s, /*@only@*/ qtype t)
{
  return idDecl_createClauses (s, t, functionClauseList_undefined);
}

void
idDecl_free (idDecl t)
{
  if (idDecl_isDefined (t))
    {
      /* don't: functionClauseList_free (t->clauses); */ /* evans 2002-01-03: splint catches this now! */
      qtype_free (t->typ);
      cstring_free (t->id);

      /*@-compdestroy@*/ sfree (t); /*@=compdestroy@*/
    }
}

cstring
idDecl_unparse (idDecl d)
{
  if (idDecl_isDefined (d))
    {
      if (functionClauseList_isDefined (d->clauses)) 
	{
	  return (message ("%s : %q / %q", d->id, qtype_unparse (d->typ),
			   functionClauseList_unparse (d->clauses)));
	}
      else
	{
	  return (message ("%s : %q", d->id, qtype_unparse (d->typ)));
	}
    }
  else
    {
      return (cstring_makeLiteral ("<undefined id>"));
    }
}

cstring
idDecl_unparseC (idDecl d)
{
  if (idDecl_isDefined (d))
    {
      return (message ("%q %s", qtype_unparse (d->typ), d->id));
    }
  else
    {
      return (cstring_makeLiteral ("<undefined id>"));
    }
}

/*@observer@*/ cstring
idDecl_observeId (idDecl d)
{
  if (idDecl_isDefined (d))
    {
      return (d->id);
    }
  else
    {
      return cstring_undefined;
    }
}

qtype
idDecl_getTyp (idDecl d)
{
  llassert (idDecl_isDefined (d));

  return d->typ;
}

ctype
idDecl_getCtype (idDecl d)
{
  if (idDecl_isDefined (d))
    {
      return (qtype_getType (d->typ));
    }
  else
    {
      return ctype_unknown;
    }
}

qualList
idDecl_getQuals (idDecl d)
{
  if (idDecl_isDefined (d))
    {
      return (qtype_getQuals (d->typ));
    }
  else
    {
      return qualList_undefined;
    }
}

functionClauseList
idDecl_getClauses (idDecl d)
{
  if (idDecl_isDefined (d))
    {
      return (d->clauses);
    }
  else
    {
      return functionClauseList_undefined;
    }
}

void
idDecl_addQual (idDecl d, qual q)
{
  llassert (idDecl_isDefined (d));

  (void) qtype_addQual (d->typ, q);
}

void
idDecl_setTyp (idDecl d, qtype c)
{
  llassert (idDecl_isDefined (d));

  qtype_free (d->typ);
  d->typ = c;
}

idDecl
idDecl_replaceCtype (/*@returned@*/ idDecl d, ctype c)
{
  llassert (idDecl_isDefined (d));

  DPRINTF (("Replace type: %s / %s", idDecl_unparse (d), ctype_unparse (c)));
  qtype_setType (d->typ, c);
  return d;
}

idDecl
idDecl_fixBase (/*@returned@*/ idDecl t, qtype b)
{
  llassert (idDecl_isDefined (t));

  t->typ = qtype_newQbase (t->typ, b);
  return t;
}

idDecl
idDecl_fixParamBase (/*@returned@*/ idDecl t, qtype b)
{
  qtype q;
  ctype c;

  llassert (idDecl_isDefined (t));

  q = qtype_newQbase (t->typ, b);
  c = qtype_getType (q);

  /*
  ** For some reason, C adds an implicit pointer to function
  ** parameters.  It is "optional" syntax.
  */

  if (ctype_isFunction (c) && !ctype_isPointer (c))
    {
      qtype_setType (q, ctype_makePointer (c));
    }

  t->typ = q;
  /* Splint thinks t->typ is kept. */
  /*@-compmempass@*/ return t; /*@=compmempass@*/
}

idDecl
idDecl_expectFunction (/*@returned@*/ idDecl d)
{
  llassert (idDecl_isDefined (d));

  qtype_setType (d->typ, ctype_expectFunction (qtype_getType (d->typ)));
  return d;
}

/*
** evans 2002-02-09: This is a bit of a kludge, but we 
** need it to fix declarations like int (*p)[];
*/

void
idDecl_notExpectingFunction (/*@returned@*/ idDecl d)
{
  if (idDecl_isDefined (d)) 
    {
      ctype ct = qtype_getType (d->typ);

      if (ctype_isExpFcn (ct))
	{
	  qtype_setType (d->typ, ctype_dontExpectFunction (ct));
	}
    }
}

void
idDecl_addClauses (idDecl d, functionClauseList clauses)
{
  llassert (idDecl_isDefined (d));

  /*
    DRL comment out llassert:
    
    This breaks on sometypes of functionPointers.
    I.e.
    void (*signal (int sig ) @requires g >= 0 @ ) (int) @requires g >= 0 @ ;

    llassert (functionClauseList_isUndefined (d->clauses));

  */

  if (functionClauseList_isUndefined (d->clauses) )
    {
      d->clauses = clauses;
    }
  else
    {
      functionClauseList_free(d->clauses);
      d->clauses = clauses;
    }
}
