/* ;-*-C-*-; 
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
** cttable.i
**
** NOTE: This is not a stand-alone source file, but is included in ctype.c.
**       (This is necessary becuase there is no other way in C to have a
**       hidden scope, besides at the file level.)
*/

/*@access ctype@*/
           
/*
** type definitions and forward declarations in ctbase.i
*/

static void
ctentry_free (/*@only@*/ ctentry c)
{
  ctbase_free (c->ctbase);
  cstring_free (c->unparse);
  sfree (c);
}

static void cttable_reset (void)
   /*@globals cttab@*/
   /*@modifies cttab@*/
{
  if (cttab.entries != NULL) 
    {
      int i;  

      for (i = 0; i < cttab.size; i++)
	{
	  /*drl bee: si*/   ctentry_free (cttab.entries[i]);
	}
      
      /*@-compdestroy@*/ 
      sfree (cttab.entries);
      /*@=compdestroy@*/

      cttab.entries = NULL;
    }

  cttab.size = 0 ;
  cttab.nspace = 0 ;
}

static ctentry
ctentry_makeNew (ctkind ctk, /*@only@*/ ctbase c)
{
  
  return (ctentry_make (ctk, c, ctype_dne, ctype_dne, ctype_dne, cstring_undefined));
}

static /*@only@*/ ctentry
ctentry_make (ctkind ctk,
	      /*@keep@*/ ctbase c, ctype base,
	      ctype ptr, ctype array,
	      /*@keep@*/ cstring unparse) /*@*/ 
{
  ctentry cte = (ctentry) dmalloc (sizeof (*cte));
  cte->kind = ctk;
  cte->ctbase = c;
  cte->base = base;
  cte->ptr = ptr;
  cte->array = array;
  cte->unparse = unparse;
  return cte;
}

static cstring
ctentry_unparse (ctentry c)
{
  return (message 
	  ("%20s [%d] [%d] [%d]",
	   (cstring_isDefined (c->unparse) ? c->unparse : cstring_makeLiteral ("<no name>")),
	   c->base, 
	   c->ptr,
	   c->array));
}

static bool
ctentry_isInteresting (ctentry c)
{
  return (cstring_isNonEmpty (c->unparse));
}

static /*@only@*/ cstring
ctentry_dump (ctentry c)
{
  DPRINTF (("Dumping: %s", ctentry_unparse (c)));

  if (c->ptr == ctype_dne
      && c->array == ctype_dne
      && c->base == ctype_dne)
    {
      return (message ("%d %q&", 
		       ctkind_toInt (c->kind),
		       ctbase_dump (c->ctbase)));
    }
  else if (c->base == ctype_undefined
	   && c->array == ctype_dne)
    {
      if (c->ptr == ctype_dne)
	{
	  return (message ("%d %q!", 
			   ctkind_toInt (c->kind),
			   ctbase_dump (c->ctbase)));
	}
      else
	{
	  return (message ("%d %q^%d", 
			   ctkind_toInt (c->kind),
			   ctbase_dump (c->ctbase),
			   c->ptr));
	}
    }
  else if (c->ptr == ctype_dne
	   && c->array == ctype_dne)
    {
      return (message ("%d %q%d&", 
		       ctkind_toInt (c->kind),
		       ctbase_dump (c->ctbase),
		       c->base));
    }
  else
    {
      return (message ("%d %q%d %d %d", 
		       ctkind_toInt (c->kind),
		       ctbase_dump (c->ctbase),
		       c->base, c->ptr, c->array));
    }
}


static /*@only@*/ ctentry
ctentry_undump (/*@dependent@*/ char *s) /*@requires maxRead(s) >= 2 @*/
{
  int base, ptr, array;
  ctkind kind;
  ctbase ct;

  kind = ctkind_fromInt (reader_getInt (&s));
  ct = ctbase_undump (&s);

  if (reader_optCheckChar (&s, '&'))
    {
      base = ctype_dne;
      ptr = ctype_dne;
      array = ctype_dne;
    }
  else if (reader_optCheckChar (&s, '!'))
    {
      base = ctype_undefined;
      ptr = ctype_dne;
      array = ctype_dne;
    }
  else if (reader_optCheckChar (&s, '^'))
    {
      base = ctype_undefined;
      ptr = reader_getInt (&s);
      array = ctype_dne;
    }
  else
    {
      base = reader_getInt (&s);
      
      if (reader_optCheckChar (&s, '&'))
	{
	  ptr = ctype_dne;
	  array = ctype_dne;
	}
      else
	{
	  ptr = reader_getInt (&s);
	  array = reader_getInt (&s);
	}
    }

  /* can't unparse w/o typeTable */
  return (ctentry_make (kind, ct, base, ptr, array, cstring_undefined));
}

static /*@observer@*/ cstring
ctentry_doUnparse (ctentry c) /*@modifies c@*/
{
  if (cstring_isDefined (c->unparse))
    {
      return (c->unparse);
    }
  else
    {
      cstring s = ctbase_unparse (c->ctbase);

      if (!cstring_isEmpty (s) && !cstring_containsChar (s, '<'))
	{
	  c->unparse = s;
	}
      else
	{
	  cstring_markOwned (s);
	}

      return (s);
    }
}

static /*@observer@*/ cstring
ctentry_doUnparseDeep (ctentry c)
{
  if (cstring_isDefined (c->unparse))
    {
      return (c->unparse);
    }
  else
    {
      c->unparse = ctbase_unparseDeep (c->ctbase);
      return (c->unparse);
    }
}

/*
** cttable operations
*/

static /*@only@*/ cstring
cttable_unparse (void)
{
  int i;
  cstring s = cstring_undefined;

  /*@access ctbase@*/
  for (i = 0; i < cttab.size; i++)
    {
     /*drl bee: si*/    ctentry cte = cttab.entries[i];
      if (ctentry_isInteresting (cte))
	{
	  if (ctbase_isUA (cte->ctbase))
	    {
	      s = message ("%s%d\t%q [%d]\n", s, i, ctentry_unparse (cttab.entries[i]),
			   cte->ctbase->contents.tid);
	    }
	  else
	    {
	      s = message ("%s%d\t%q\n", s, i, ctentry_unparse (cttab.entries[i]));
	    }
	}
    }
  /*@noaccess ctbase@*/
  return (s);
}

void
cttable_print (void)
{
  int i;

  /*@access ctbase@*/
  for (i = 0; i < cttab.size; i++)
    {
   /*drl bee: si*/     ctentry cte = cttab.entries[i];

      if (TRUE) /* ctentry_isInteresting (cte)) */
	{
	  if (ctbase_isUA (cte->ctbase))
	    {
	      fprintf (g_warningstream, "%3d: %s [%d]\n", i, 
		       cstring_toCharsSafe (ctentry_doUnparse (cttab.entries[i])),
		       cte->ctbase->contents.tid);
	    }
	  else
	    {
	      fprintf (g_warningstream, "%3d: %s\n", i, 
		       cstring_toCharsSafe (ctentry_doUnparse (cttab.entries[i])));
	    }
	}
      else
	{
	  /* fprintf (g_warningstream, "%3d: <no name>\n", i); */
	}
    }
  /*@noaccess ctbase@*/
}

/*
** cttable_dump
**
** Output cttable for dump file
*/

static void
cttable_dump (FILE *fout)
{
  bool showdots = FALSE;
  int showdotstride = 0;
  int i;
  
  if (context_getFlag (FLG_SHOWSCAN) && cttab.size > 5000)
    {
      displayScanClose ();
      displayScanOpen (message ("Dumping type table (%d types)", cttab.size));
      showdotstride = cttab.size / 5;
      showdots = TRUE;
    }

  for (i = 0; i < cttab.size; i++)
    {
      cstring s;

   /*drl bee: si*/     s = ctentry_dump (cttab.entries[i]);
      DPRINTF (("[%d] = %s", i, ctentry_unparse (cttab.entries[i])));
      llassert (cstring_length (s) < MAX_DUMP_LINE_LENGTH);
      fputline (fout, cstring_toCharsSafe (s));
      cstring_free (s);

      if (showdots && (i != 0 && ((i - 1) % showdotstride == 0)))
	{
	  (void) fflush (g_warningstream);
	  displayScanContinue (cstring_makeLiteralTemp ("."));
	  (void) fflush (stderr);
	}
    }

  if (showdots)
    {
      displayScanClose ();
      displayScanOpen (cstring_makeLiteral ("Continuing dump "));
    }
  
}

/*
** load cttable from init file
*/

static void cttable_load (FILE *f) 
  /*@globals cttab @*/
  /*@modifies cttab, f @*/
{
  char *s = mstring_create (MAX_DUMP_LINE_LENGTH);
  char *os = s;
  ctentry cte;

  cttable_reset ();

  /*
  DPRINTF (("Loading cttable: "));
  cttable_print ();
  */

  while (reader_readLine (f, s, MAX_DUMP_LINE_LENGTH) != NULL && *s == ';')
    {
      ;
    }
  
  if (mstring_length (s) == (MAX_DUMP_LINE_LENGTH - 1))
    {
      llbug (message ("Library line too long: %s\n", cstring_fromChars (s)));
    }
  
  while (s != NULL && *s != ';' && *s != '\0')
    {
      ctype ct;

      /*drl bee: tcf*/      cte = ctentry_undump (s);
      ct = cttable_addFull (cte);

      DPRINTF (("Type: %d: %s", ct, ctype_unparse (ct)));

      if (ctbase_isConj (cte->ctbase)
	  && !(ctbase_isExplicitConj (cte->ctbase)))
	{
	  ctype_recordConj (ct);
	}

      s = reader_readLine (f, s, MAX_DUMP_LINE_LENGTH);
    }

  sfree (os);

  /*
  DPRINTF (("Done loading cttable: "));
  cttable_print ();
  */
}

/*
** cttable_init
**
** fill up the cttable with basic types, and first order derivations.
** this is done without using our constructors for efficiency reasons
** (probably bogus)
**
*/

/*@access cprim@*/
static void cttable_init (void) 
   /*@globals cttab@*/ /*@modifies cttab@*/
{
  ctkind i;
  cprim  j;
  ctbase ct = ctbase_undefined; 

  llassert (cttab.size == 0);

  /* do for plain, pointer, arrayof */
  for (i = CTK_PLAIN; i <= CTK_ARRAY; i++)	
    {
      for (j = CTX_UNKNOWN; j <= CTX_LAST; j++)
	{
	  if (i == CTK_PLAIN)
	    {
	      if (j == CTX_BOOL)
		{
		  ct = ctbase_createBool (); /* why different? */
		}
	      else if (j == CTX_UNKNOWN)
		{
		  ct = ctbase_createUnknown ();
		}
	      else
		{
		  ct = ctbase_createPrim ((cprim)j);
		}

	      (void) cttable_addFull 
		(ctentry_make (CTK_PLAIN,
			       ct, ctype_undefined, 
			       j + CTK_PREDEFINED, j + CTK_PREDEFINED2,
			       ctbase_unparse (ct)));
	    }
	  else
	    {
	      switch (i)
		{
		case CTK_PTR:
		  ct = ctbase_makePointer (j);
		  /*@switchbreak@*/ break;
		case CTK_ARRAY:
		  ct = ctbase_makeArray (j);
		  /*@switchbreak@*/ break;
		default:
		  llbugexitlit ("cttable_init: base case");
		}
	      
	      (void) cttable_addDerived (i, ct, j);
	    }
	}
    }

  /**** reserve a slot for userBool ****/
  (void) cttable_addFull (ctentry_make (CTK_INVALID, ctbase_undefined, 
					ctype_undefined, ctype_dne, ctype_dne, 
					cstring_undefined));
}

/*@noaccess cprim@*/

static void
cttable_grow ()
{
  int i;
  o_ctentry *newentries ;

  cttab.nspace = CTK_BASESIZE;
  newentries = (ctentry *) dmalloc (sizeof (*newentries) * (cttab.size + cttab.nspace));

  if (newentries == NULL)
    {
      llfatalerror (message ("cttable_grow: out of memory.  Size: %d", 
			     cttab.size));
    }

  for (i = 0; i < cttab.size; i++)
    {
      /*drl bee: dm*/  /*drl bee: si*/  newentries[i] = cttab.entries[i];
    }

  /*@-compdestroy@*/
  sfree (cttab.entries);
  /*@=compdestroy@*/

  cttab.entries = newentries;
/*@-compdef@*/
} /*@=compdef@*/

static ctype
cttable_addDerived (ctkind ctk, /*@keep@*/ ctbase cnew, ctype base)
{
  if (cttab.nspace == 0)
    cttable_grow ();
  
  /*drl bee: si*/  cttab.entries[cttab.size] = 
    ctentry_make (ctk, cnew, base, ctype_dne, ctype_dne, cstring_undefined);

  cttab.nspace--;
  
  return (cttab.size++);
}

static ctype
cttable_addComplex (/*@only@*/ ctbase cnew)
   /*@modifies cttab; @*/
{
  /*@access ctbase@*/
  if (cnew->type != CT_FCN && cnew->type != CT_EXPFCN) 
    {
      ctype i;
      int ctstop = cttable_lastIndex () - DEFAULT_OPTLEVEL;
      
      if (ctstop < LAST_PREDEFINED)
	{
	  ctstop = LAST_PREDEFINED;
	}

      for (i = cttable_lastIndex (); i >= ctstop; i--)	/* better to go from back... */
	{
	  ctbase ctb;
	  
	  ctb = ctype_getCtbase (i);
	  
	  /* is this optimization really worthwhile? */

	  if (ctbase_isDefined (ctb) && ctbase_equivStrict (cnew, ctb))
	    {
	      DPRINTF (("EQUIV!! %s / %s",
			ctbase_unparse (cnew),
			ctbase_unparse (ctb)));

	      ctbase_free (cnew);
	      return i;
	    }
	}
    }
  
  if (cttab.nspace == 0)
    cttable_grow ();
  
  /*drl bee: si*/  cttab.entries[cttab.size] = ctentry_make (CTK_COMPLEX, cnew, ctype_undefined, 
					    ctype_dne, ctype_dne,
					    cstring_undefined);
  cttab.nspace--;
  
  return (cttab.size++);
  /*@noaccess ctbase@*/
}

static ctype
cttable_addFull (ctentry cnew)
{
  if (cttab.nspace == 0)
    {
      cttable_grow ();
    }

  /*drl bee: si*/  cttab.entries[cttab.size] = cnew;
  cttab.nspace--;

  return (cttab.size++);
}

static ctype
cttable_addFullSafe (/*@only@*/ ctentry cnew)
{
  int i;
  ctbase cnewbase = cnew->ctbase;

  llassert (ctbase_isDefined (cnewbase));

  for (i = cttable_lastIndex (); i >= CT_FIRST; i--)
    {
      ctbase ctb = ctype_getCtbase (i);

      if (ctbase_isDefined (ctb) 
	  && ctbase_equiv (cnewbase, ctype_getCtbaseSafe (i)))
	{
	  ctentry_free (cnew);
	  return i;
	}
    }

  if (cttab.nspace == 0)
    cttable_grow ();

  /*drl bee: si*/  cttab.entries[cttab.size] = cnew;

  cttab.nspace--;
  
  return (cttab.size++);
}

