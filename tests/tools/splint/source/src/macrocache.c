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
** macrocache.c
**
** rep Invariant:
**     no two fileloc's may be equal
**
*/

# include "splintMacros.nf"
# include "basic.h"
# include "llmain.h"

/*@constant int MCEBASESIZE;@*/
# define MCEBASESIZE 8

/*@constant int DNE;@*/
# define DNE -1

/*
** Temporary file used for processing macros.
*/

static /*:open:*/ /*@null@*/ FILE *s_macFile = NULL;

/*
** mcDisable is set to TRUE when a macro is being processed, so
** its contents are not added to the macrocache again, creating
** a nasty infinite loop.
*/

static bool mcDisable = TRUE;
static void macrocache_grow (macrocache p_s);
static int macrocache_exists (macrocache p_s, fileloc p_fl);
static void macrocache_processMacro (macrocache p_m, int p_i);

static /*@only@*/ mce
  mce_create (/*@only@*/ fileloc fl, /*@only@*/ cstring def, bool comment)
{
  mce m = (mce) dmalloc (sizeof (*m));
  m->fl = fl;
  m->def = def; /*< had a copy here! check this carefully */
  m->defined = FALSE;
  m->scomment = comment;
  return m;
}

static void mce_free (/*@only@*/ mce m)
{
  fileloc_free (m->fl);
  cstring_free (m->def);
  sfree (m);
}

/*@only@*/ macrocache
macrocache_create (void)
{
  macrocache s = (macrocache) dmalloc (sizeof (*s));

  s->entries = 0;
  s->nspace = MCEBASESIZE;
  s->contents = (mce *) dmalloc (sizeof (*s->contents) * MCEBASESIZE);

  mcDisable = FALSE;

  return (s);
}

void
macrocache_free (macrocache s)
{
  int i;

  llassert (s_macFile == NULL);

  for (i = 0; i < s->entries; i++)
    {
       mce_free (s->contents[i]);
    }

  sfree (s->contents);
  sfree (s);
}

static void
macrocache_grow (macrocache s)
{
  int i;
  o_mce *oldcontents = s->contents;

  s->nspace = MCEBASESIZE;
  s->contents = (mce *) dmalloc (sizeof (*s->contents) * (s->entries + s->nspace)); 

  for (i = 0; i < s->entries; i++)
    {
       s->contents[i] = oldcontents[i];
    }

  sfree (oldcontents);
}

static void
macrocache_addGenEntry (macrocache s, /*@only@*/ fileloc fl,
			/*@only@*/ cstring def, bool sup)
{
  int i;

  if (mcDisable)
    {
      fileloc_free (fl);
      cstring_free (def);
      return;
    }

  if ((i = macrocache_exists (s, fl)) != DNE)
    {
        if (cstring_equal (def, s->contents[i]->def))
	{
	  fileloc_free (fl);
	  cstring_free (def);

	  return;
	}
      else
	{
	  /*
	  ** macro definition contained macro that is expanded
	  ** replace with def
	  **
	  ** how do we know which is better??
	  */
	  
	  cstring_free (s->contents[i]->def);
	    s->contents[i]->def = def;

	  fileloc_free (fl);
	  return;
	}
    }

  if (s->nspace <= 0) {
    macrocache_grow (s);
  }

  s->nspace--;
    s->contents[s->entries] = mce_create (fl, def, sup);
  s->entries++;
}

void
macrocache_addEntry (macrocache s, /*@only@*/ fileloc fl, /*@only@*/ cstring def)
{
  macrocache_addGenEntry (s, fl, def, FALSE);
}


void
macrocache_addComment (macrocache s, /*@only@*/ fileloc fl, /*@only@*/ cstring def)
{
  DPRINTF (("Macrocache add comment: %s / %s", fileloc_unparse (fl), def));
  macrocache_addGenEntry (s, fl, def, TRUE);
}

static int
macrocache_exists (macrocache s, fileloc fl)
{
  int i;

  for (i = 0; i < s->entries; i++)
    {
        if (fileloc_equal (s->contents[i]->fl, fl))
	return (i);
    }

  return (DNE);
}

/*@only@*/ cstring
macrocache_unparse (macrocache m)
{
  cstring s = cstring_undefined;
  int i;

  for (i = 0; i < m->entries; i++)
    {
        fileloc fl = m->contents[i]->fl;
      cstring def = m->contents[i]->def;
      bool defined = m->contents[i]->defined;
      
      s = message ("%q%q: %s [%s]\n", s, fileloc_unparse (fl), def, 
		   bool_unparse (defined));
    }
  
  return (s);
}

/*
** needs to call lex by hand...yuk!
**
** modifies gc fileloc!
*/

/*
** there's gotta be a better way of doing this!
*/

static void pushString (/*@only@*/ cstring s)
{
  static fileId mtid = fileId_invalid;
  long floc;

  if (s_macFile == NULL)
    {
      cstring fname;
      mtid = fileTable_addMacrosFile (context_fileTable ());
      
      fname = fileTable_fileName (mtid);
      s_macFile = fileTable_createMacrosFile (context_fileTable (), fname); /* , "wb+"); ? **/
      
      if (s_macFile == NULL)
	{
	  llcontbug (message ("Cannot open tmp file %s needed to process macro: %s", 
			      fname, s));
	  cstring_free (s);
	  return;
    	}
    }

  llassert (s_macFile != NULL);

  /* SunOS, others? don't define SEEK_CUR and SEEK_SET */
# ifndef SEEK_CUR 
# define SEEK_CUR 1
# endif
  check (fseek (s_macFile, 0, SEEK_CUR) == 0);

  floc = ftell (s_macFile);

  if (cstring_length (s) > 0) {
    check (fputs (cstring_toCharsSafe (s), s_macFile) != EOF);
  }

  check (fputc ('\n', s_macFile) == (int) '\n');

# ifndef SEEK_SET 
# define SEEK_SET 0
# endif
  check (fseek (s_macFile, floc, SEEK_SET) == 0);

  yyin = s_macFile;
  (void) yyrestart (yyin);
  cstring_free (s);
}

static void
macrocache_processMacro (macrocache m, int i)
{
    fileloc fl = m->contents[i]->fl;
   
    m->contents[i]->defined = TRUE;

  if (!fileId_equal (currentFile (), fileloc_fileId (fl)))
    {
      g_currentloc = fileloc_update (g_currentloc, fl);
      context_enterMacroFile ();
    }
  else
    {
      setLine (fileloc_lineno (fl));
    }

  beginLine ();

  DPRINTF (("Process macro: %s", m->contents[i]->def));

  if (m->contents[i]->scomment)
    {
      pushString (message ("%s%s%s", 
			   cstring_fromChars (BEFORE_COMMENT_MARKER),
			   m->contents[i]->def,
			   cstring_fromChars (AFTER_COMMENT_MARKER)));
      (void) yyparse ();
    }
  else
    {
      bool insup = context_inSuppressRegion ();

      pushString (message ("%s %s", 
			   cstring_makeLiteralTemp (PPMRCODE),
			   m->contents[i]->def));
      (void) yyparse ();

      if (context_inSuppressRegion () && !insup)
	{
	  voptgenerror
	    (FLG_SYNTAX, 
	     message ("Macro ends in ignore region: %s", m->contents[i]->def),
	     fl);
	}
    }
  
  incLine ();  
  context_exitAllClauses ();
  context_exitMacroCache ();
}

extern void macrocache_processUndefinedElements (macrocache m)
{
  fileloc lastfl = fileloc_undefined;
  int i;
 
  mcDisable = TRUE;

  DPRINTF (("Processing undefined elements"));

  if (!context_getFlag (FLG_PARTIAL))
    {
      for (i = 0; i < m->entries; i++) 
	{
	    if (m->contents[i]->defined)
	    {
	      ;
	    }
	  else 
	    { 
	      fileloc fl = m->contents[i]->fl; 
	      
	      if (fileloc_isDefined (lastfl) && fileloc_sameFile (fl, lastfl)) 
		{
		  ;
		}
	      else
		{
		  if (!fileloc_isLib (fl))
		    {
		      displayScan (message ("checking macros %q",
					    fileloc_outputFilename (fl)));
		    }
		  
		  lastfl = fl;
		  cleanupMessages ();
		}
	      
	      macrocache_processMacro (m, i);	
	    }
	}
    }

  mcDisable = FALSE;
}

extern /*@observer@*/ fileloc macrocache_processFileElements (macrocache m, cstring base)
{
  fileloc lastfl = fileloc_undefined;
  int i;
 
  mcDisable = TRUE;

  for (i = 0; i < m->entries; i++) 
    {
        if (m->contents[i]->defined)
	{
	  ;
	}
      else 
        { 
	  fileloc fl = m->contents[i]->fl;  /* should be dependent! */
	  cstring fb = fileloc_getBase (fl);

	  if (cstring_equal (fb, base))
	    {
	      lastfl = fl;
	      macrocache_processMacro (m, i);	
	    }
	}
    }

  mcDisable = FALSE;
  return lastfl;
}

void macrocache_finalize (void)
{
  if (s_macFile != NULL)
    {
      check (fileTable_closeFile (context_fileTable (), s_macFile));
      s_macFile = NULL;
    }
}
