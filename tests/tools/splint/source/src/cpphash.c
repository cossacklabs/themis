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
** cpphash.c
**
** Pre-processor hash table.  Derived from gnu cpp.
*/

/* Part of CPP library.  (Macro hash table support.)
   Copyright (C) 1986, 87, 89, 92-95, 1996 Free Software Foundation, Inc.
   Written by Per Bothner, 1994.
   Based on CCCP program by by Paul Rubin, June 1986
   Adapted to ANSI C, Richard Stallman, Jan 1987

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

 In other words, you are welcome to use, share and improve this program.
 You are forbidden to forbid anyone else to use, share and improve
 what you give them.   Help stamp out software-hoarding!  */

# include "splintMacros.nf"
# include "basic.h"
# include <string.h>
# include "cpplib.h"
# include "cpphash.h"

typedef /*@null@*/ /*@only@*/ hashNode o_hashNode;
typedef /*@null@*/ /*@only@*/ hashNode n_hashNode;

static o_hashNode hashtab[CPP_HASHSIZE]; 
static o_hashNode ohashtab[CPP_HASHSIZE];

static void hashNode_delete (/*@null@*/ /*@only@*/ hashNode);

/* p_prev need not be defined, but isn't defined by hashNode_copy */

/*@function static unsigned int hashStep (unsigned, char) modifies nothing ; @*/
# define hashStep(old, c) (((old) << 2) + (unsigned int) (c))

/*@function static unsigned int makePositive (unsigned int) modifies nothing ; @*/
# define makePositive(v) ((v) & 0x7fffffff) /* make number positive */

static /*@null@*/ hashNode hashNode_copy (/*@null@*/ hashNode, 
					  /*@null@*/ /*@dependent@*/ n_hashNode *p_hdr, 
					  /*@dependent@*/ /*@null@*/ /*@special@*/ hashNode p_prev) 
     /*@*/ ;

void cppReader_saveHashtab ()
{
  int i;

  for (i = 0; i < CPP_HASHSIZE; i++) 
    {
      ohashtab[i] = hashNode_copy (hashtab[i], &ohashtab[i], NULL);
    }
}

void cppReader_restoreHashtab ()
{
  int i;

  for (i = 0; i < CPP_HASHSIZE; i++) {
    /* hashNode_delete (hashtab[i]); */
    hashtab[i] = hashNode_copy (ohashtab[i], &hashtab[i], NULL);
  }  
}

static void hashNode_delete (/*@only@*/ /*@null@*/ hashNode node) 
{
  if (node == NULL) 
    {
      ;
    } 
  else 
    {
      hashNode_delete (node->next);
      
      if (node->type == T_MACRO)
	{
	  DEFINITION *d = node->value.defn;
	  struct reflist *ap, *nextap;
	  
	  for (ap = d->pattern; ap != NULL; ap = nextap)
	    {
	      nextap = ap->next;
	      sfree (ap);
	    }
	  
	  if (d->nargs >= 0) 
	    {
	      sfree (d->args.argnames);
	    }
	  
	  sfree (d);
	}
      
      cstring_free (node->name);
      sfree (node); 
    }
}

/*@null@*/ hashNode hashNode_copy (hashNode node, hashNode *hdr, 
				   /*@dependent@*/ hashNode prev)
{
  if (node == NULL) 
    {
      return NULL;
    } 
  else 
    {
      hashNode res = dmalloc (sizeof (*res));
      
      res->next = hashNode_copy (node->next, hdr, res);
      res->prev = prev;
      
      res->bucket_hdr = hdr;
      res->type = node->type;
      res->length = node->length;
      res->name = cstring_copy (node->name);
      
      if (node->type == T_MACRO)
	{
	  DEFINITION *d = node->value.defn;
	  DEFINITION *nd = dmalloc (sizeof (*nd));
	  
	  res->value.defn = nd;
	  nd->nargs = d->nargs;
	  
	  nd->length = d->length;
	  nd->predefined = d->predefined;
	  nd->expansion = d->expansion; 
	  nd->line = d->line;
	  nd->file = d->file; 
	  
	  if (d->pattern != NULL) 
	    {
	      struct reflist *ap, *nextap;
	      struct reflist **last = &nd->pattern;
	      
	      for (ap = d->pattern; ap != NULL; ap = nextap) 
		{
		  struct reflist *npattern = dmalloc (sizeof (* (nd->pattern)));
		  
		  nextap = ap->next;
		  
		  if (ap == d->pattern) 
		    {
		      *last = npattern;
		      /*@-branchstate@*/ 
		    } /*@=branchstate@*/ /* npattern is propagated through loop */
		  
		  last = & (npattern->next);
		  npattern->next = NULL; /* will get filled in */
		  npattern->stringify = d->pattern->stringify;
		  npattern->raw_before = d->pattern->raw_before;
		  npattern->raw_after = d->pattern->raw_after;
		  npattern->rest_args = d->pattern->rest_args;
		  npattern->argno = d->pattern->argno;
		  /*@-mustfree@*/ 
		}
	      /*@=mustfree@*/
	    } 
	  else 
	    {
	      nd->pattern = NULL;
	    }
	  
	  if (d->nargs >= 0) 
	    {
	      llassert (d->args.argnames != NULL);
	      
	      nd->args.argnames = mstring_copy (d->args.argnames);
	    } 
	  else 
	    {
	      /*
	      ** This fix found by:
	      **
	      **    Date: Mon, 31 May 1999 15:10:50 +0900 (JST)
	      **    From: "N.Komazaki" <koma@focs.sei.co.jp>
	      */

	      /*! why doesn't splint report an error for this? */
	      nd->args.argnames = mstring_createEmpty ();
	    }
	} 
      else 
	{
	  if (node->type == T_CONST) 
	    {
	      res->value.ival = node->value.ival;
	    } 
	  else if (node->type == T_PCSTRING) 
	    {
	      res->value.cpval = mstring_copy (node->value.cpval);
	      llassert (res->value.cpval != NULL);
	    } 
	  else 
	    {
	      res->value = node->value;
	    }
	}
      
      /*@-uniondef@*/ /*@-compdef@*/ /* res->prev is not defined */
      return res;
      /*@=uniondef@*/ /*@=compdef@*/
    }
}

/* Return hash function on name.  must be compatible with the one
   computed a step at a time, elsewhere  */

int
cpphash_hashCode (const char *name, size_t len, int hashsize)
{
  unsigned int r = 0;

  while (len-- != 0)
    {
      r = hashStep (r, *name++);
    }

  return (int) (makePositive (r) % hashsize);
}

/*
** Find the most recent hash node for name name (ending with first
** non-identifier char) cpphash_installed by install
**
** If len is >= 0, it is the length of the name.
** Otherwise, compute the length by scanning the entire name.
**
** If hash is >= 0, it is the precomputed hash code.
** Otherwise, compute the hash code.  
*/

/*@null@*/ hashNode cpphash_lookup (char *name, int len, int hash)
{
  const char *bp;
  hashNode bucket;

  if (len < 0)
    {
      for (bp = name; isIdentifierChar (*bp); bp++) 
	{
	  ;
	}

      len = bp - name;
    }

  if (hash < 0)
    {
      hash = cpphash_hashCode (name, size_fromInt (len), CPP_HASHSIZE);
    }

  bucket = hashtab[hash];

  while (bucket != NULL) 
    {
      if (bucket->length == size_fromInt (len) && 
	  cstring_equalLen (bucket->name, cstring_fromChars (name), size_fromInt (len))) 
	{
	  return bucket;
	}
      
      bucket = bucket->next;
    }

  return NULL;
}

/*@null@*/ hashNode cpphash_lookupExpand (char *name, int len, int hash, bool forceExpand)
{
  hashNode node = cpphash_lookup (name, len, hash);

  DPRINTF (("Lookup expand: %s", name));

  if (node != NULL) 
    {
      if (node->type == T_MACRO)
	{
	  DEFINITION *defn = (DEFINITION *) node->value.defn;
	
	  DPRINTF (("Check macro..."));

	  if (defn->noExpand && !forceExpand) {
	    DPRINTF (("No expand!"));
	    return NULL;
	  }
	}
    }
  
  return node;
}

/*
 * Delete a hash node.  Some weirdness to free junk from macros.
 * More such weirdness will have to be added if you define more hash
 * types that need it.
 */

/* Note that the DEFINITION of a macro is removed from the hash table
   but its storage is not freed.  This would be a storage leak
   except that it is not reasonable to keep undefining and redefining
   large numbers of macros many times.
   In any case, this is necessary, because a macro can be #undef'd
   in the middle of reading the arguments to a call to it.
   If #undef freed the DEFINITION, that would crash.  */

void
cppReader_deleteMacro (hashNode hp)
{
  if (hp->prev != NULL) 
    {
      /*@-mustfree@*/
      hp->prev->next = hp->next;
      /*@=mustfree@*/
      /*@-branchstate@*/ 
    } /*@=branchstate@*/ 
  
  if (hp->next != NULL) 
    {
      hp->next->prev = hp->prev;
    }
  
  /* make sure that the bucket chain header that
     the deleted guy was on points to the right thing afterwards.  */

  llassert (hp != NULL);
  llassert (hp->bucket_hdr != NULL);

  if (hp == *hp->bucket_hdr) {
    *hp->bucket_hdr = hp->next;
  }
  
  if (hp->type == T_MACRO)
    {
      DEFINITION *d = hp->value.defn;
      struct reflist *ap, *nextap;

      for (ap = d->pattern; ap != NULL; ap = nextap)
	{
	  nextap = ap->next;
	  sfree (ap);
	}

      if (d->nargs >= 0)
	{
	  sfree (d->args.argnames);
	}
    }

  /*@-dependenttrans@*/ /*@-exposetrans@*/ /*@-compdestroy@*/ 
  sfree (hp); 
  /*@=dependenttrans@*/ /*@=exposetrans@*/ /*@=compdestroy@*/
}

/* Install a name in the main hash table, even if it is already there.
     name stops with first non alphanumeric, except leading '#'.
   caller must check against redefinition if that is desired.
   cppReader_deleteMacro () removes things installed by install () in fifo order.
   this is important because of the `defined' special symbol used
   in #if, and also if pushdef/popdef directives are ever implemented.

   If LEN is >= 0, it is the length of the name.
   Otherwise, compute the length by scanning the entire name.

   If HASH is >= 0, it is the precomputed hash code.
   Otherwise, compute the hash code.  */

hashNode cpphash_install (char *name, int len, enum node_type type, 
			     int ivalue, char *value, int hash)
{
  hashNode hp;
  int bucket;
  char *p;

  DPRINTF (("Install: %s / %d", name, len));

  if (len < 0) {
    p = name;

    while (isIdentifierChar (*p))
      {
	p++;
      }

    len = p - name;
  }

  if (hash < 0) 
    {
      hash = cpphash_hashCode (name, size_fromInt (len), CPP_HASHSIZE);
    }

  hp = (hashNode) dmalloc (sizeof (*hp));
  bucket = hash;
  hp->bucket_hdr = &hashtab[bucket];

  hp->next = hashtab[bucket];
  hp->prev = NULL;

  if (hp->next != NULL) 
    {
      hp->next->prev = hp;
    }

  hashtab[bucket] = hp;

  hp->type = type;
  hp->length = size_fromInt (len);

  if (hp->type == T_CONST)
    {
      hp->value.ival = ivalue;
      llassert (value == NULL);
    }
  else
    {
      hp->value.cpval = value;
    }
  
  hp->name = cstring_clip (cstring_fromCharsNew (name), size_fromInt (len));

  DPRINTF (("Name: *%s*", hp->name));
  /*@-mustfree@*/ /*@-uniondef@*/ /*@-compdef@*/ /*@-compmempass@*/
  return hp;
  /*@=mustfree@*/ /*@=uniondef@*/ /*@=compdef@*/ /*@=compmempass@*/
}

hashNode cpphash_installMacro (char *name, size_t len, 
				 struct definition *defn, int hash)
{
  DPRINTF (("install macro: %s", name));
  return cpphash_install (name, size_toInt (len), T_MACRO, 0, (char  *) defn, hash);
}

void
cppReader_hashCleanup (void)
{
  int i;

  for (i = CPP_HASHSIZE; --i >= 0; )
    {
      while (hashtab[i] != NULL)
	{
	  cppReader_deleteMacro (hashtab[i]);
	}
    }
}
