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
** messageLog.c (from slist_template.c)
*/

# include "splintMacros.nf"
# include "basic.h"

/*@only@*/ messageLog
messageLog_new ()
{
  messageLog s = (messageLog) dmalloc (sizeof (*s));
  
  s->nelements = 0;
  s->nspace = messageLogBASESIZE;
  s->elements = (msgentry *) dmalloc (sizeof (*s->elements) * messageLogBASESIZE);

  return (s);
}

static /*@only@*/ msgentry
msgentry_create (fileloc loc, cstring mess)
{
  msgentry msg = (msgentry) dmalloc (sizeof (*msg));

  msg->loc = fileloc_copy (loc);
  msg->msg = cstring_copy (mess);

  return msg;
}

# ifdef S_SPLINT_S
static /*@unused@*/ cstring msgentry_unparse (msgentry msg) /*@*/
{
  return message ("%q:%s", fileloc_unparse (msg->loc), msg->msg);
}
# endif

static void msgentry_free (/*@only@*/ msgentry msg)
{
  fileloc_free (msg->loc);
  cstring_free (msg->msg);
  sfree (msg);
}

/*
** returns TRUE if m1 < m2
*/

static bool
msgentry_lessthan (msgentry m1, msgentry m2)
{
  return (fileloc_lessthan (m1->loc, m2->loc)
	  || (fileloc_equal (m1->loc, m2->loc) 
	      && (cstring_lessthan (m1->msg, m2->msg))));
}

static bool
msgentry_equal (msgentry m1, msgentry m2)
{
  return (fileloc_equal (m1->loc, m2->loc) &&
	  cstring_equal (m1->msg, m2->msg));
}

/*
** returns highest index of element less than msg
*/

static int
messageLog_index (messageLog s, msgentry msg)
{
  int high;
  int low  = 0;

  llassert (messageLog_isDefined (s));

  high = s->nelements - 1;

  for (low = high; low >= 0; low--)
    {
      if (msgentry_lessthan (s->elements[low], msg))
	{
	  return low;
	}
    }

  return -1;
# if 0      
  while (low < high)
    {
      int mid = (low + high + 1) / 2;

      if (msgentry_lessthan (s->elements[mid], msg)) /* mid < msg */
	{
	  if (high == mid) break;
	  high = mid;
	}
      else
	{
	  if (low == mid) break;
	  low = mid;
	}
    }

  return low - 1;
# endif
}

static void
messageLog_grow (/*@notnull@*/ messageLog s)
{
  int i;
  msgentry *newelements;
  
  s->nspace += messageLogBASESIZE; 
  newelements = (msgentry *) dmalloc (sizeof (*newelements) * (s->nelements + s->nspace));
  
  for (i = 0; i < s->nelements; i++)
    {
      newelements[i] = s->elements[i];
    }
  
  sfree (s->elements);
  s->elements = newelements;
}

bool messageLog_add (messageLog s, fileloc fl, cstring mess)
{
  msgentry msg = msgentry_create (fl, mess);
  int ind, i;

  llassert (messageLog_isDefined (s));

  ind = messageLog_index (s, msg);

  if (ind + 1 < s->nelements)
    {
      if (msgentry_equal (msg, s->elements[ind + 1]))
	{
	  msgentry_free (msg);
	  return FALSE;
	}
    }

  if (s->nspace <= 0) {
    messageLog_grow (s);
  }

  for (i = s->nelements; i > ind + 1; i--)
    {
      s->elements[i] = s->elements[i-1];
    }
  
  s->elements[ind + 1] = msg;
  s->nspace--;
  s->nelements++;

  return TRUE;
}

/*@only@*/ cstring
messageLog_unparse (messageLog s)
{
   int i;
   cstring st = cstring_makeLiteral ("[");

   if (messageLog_isDefined (s))
     {
       for (i = 0; i < s->nelements; i++)
	 {
	   if (i == 0)
	     {
	       st = message ("%q %q", st, fileloc_unparseDirect (s->elements[i]->loc));
	     }
	   else
	     st = message ("%q, %q", st, fileloc_unparseDirect (s->elements[i]->loc));
	 }
     }

   st = message ("%q ]", st);
   return st;
}

void
messageLog_free (messageLog s)
{
  if (s != NULL)
    {
      int i;

      for (i = 0; i < s->nelements; i++)
	{
	  msgentry_free (s->elements[i]);
	}
      
      sfree (s->elements); 
      sfree (s);
    }
}
