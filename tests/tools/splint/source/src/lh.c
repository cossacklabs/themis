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
** lh.c
**
**  MODULE DESCRIPTION:
**
**      This module contains the I/O routines for writing out the .lh file
**	generated from the .lcl file.
**
**  AUTHORS:
**
**      Gary Feldman, Technical Languages and Environments, DECspec project
**      Yang Meng Tan, MIT.
**
**  CREATION DATE:  9 April 91
**
**	The lh.c module controls formatting policy.
*/

# include "splintMacros.nf"
# include "basic.h"
# include "osd.h"
# include "lh.h"
# include "llmain.h"

/*@constant static char TABCH; @*/
# define TABCH 		' '

/*@constant static char TABINCH; @*/
# define TABINCH 	'\t'


/*:private:*/ typedef struct
{
  /*@open@*/ /*@dependent@*/ /*@null@*/ /*@reldef@*/ FILE *f;
  /*@reldef@*/ cstring name;
} outFile;

static bool genLh;
static outFile LhFile;
static bool needIncludeBool = FALSE;

/*
**
**  FORWARD FUNCTIONS
**
*/

/* static int colpos (int startcol, cstring line); */

static cstring lhTypeSpecNode (lclTypeSpecNode p_typespec);
static /*@only@*/ cstring lhTypeExpr (/*@null@*/ typeExpr p_x);
static /*@only@*/ cstring lhDeclaratorNode (declaratorNode p_x);

/*@only@*/ cstring 
lhFunction (lclTypeSpecNode lclTypeSpec, declaratorNode declarator)
{
  cstring s;

  if (!genLh)
    return cstring_undefined;
  
  s = message ("extern %q %q;", lhTypeSpecNode (lclTypeSpec),
	       lhDeclaratorNode (declarator));
  
  return s;
}

static /*@only@*/ cstring
lhDeclaratorNode (declaratorNode x)
{
  return (lhTypeExpr (x->type));
}

static /*@only@*/ cstring lhTypeExpr (/*@null@*/ typeExpr x)
{
  cstring s = cstring_undefined; /* print out types in order of appearance in source */
  paramNodeList params;
  int i;

  if (x != (typeExpr) 0)
    {
      cstring front = cstring_undefined;
      cstring back  = cstring_undefined;

      for (i = x->wrapped; i >= 1; i--)
	{
	  front = cstring_appendChar (front, '(');
	  back  = cstring_appendChar (back, ')');
	}

      switch (x->kind)
	{
	case TEXPR_BASE:
	  s = message ("%q%s", s, ltoken_getRawString (x->content.base));
	  break;
	case TEXPR_PTR:
	  s = message ("%q*%q", s, lhTypeExpr (x->content.pointer));
	  break;
	case TEXPR_ARRAY:
	  s = message ("%q%q[%q]", s, 
		       lhTypeExpr (x->content.array.elementtype),
		       termNode_unparse (x->content.array.size));
	  break;
	case TEXPR_FCN:
	  s = message ("%q%q (", s, lhTypeExpr (x->content.function.returntype));
	  params = x->content.function.args;

	  if (!paramNodeList_empty (params))
	    {
	      s = message ("%q%q", s, 
			   paramNodeList_unparseComments (x->content.function.args));
	    }

	  s = message ("%q)", s);
	  break;
	}
      s = message ("%q%q%q", front, s, back);
    }
  else
    {
      s = cstring_makeLiteral ("?");
    }

  return s;
}

extern void
lhForwardStruct (ltoken t)
{
  if (!genLh)
    return;

  lhOutLine (message ("struct %s;", ltoken_unparse (t)));
}


extern void
lhForwardUnion (ltoken t)
{
  if (!genLh)
    return;

  lhOutLine (message ("union %s;", ltoken_unparse (t)));
}

extern /*@only@*/ cstring 
lhType (typeNode t)
{
  if (!genLh)
    return cstring_undefined;

  if (t->kind == TK_EXPOSED)
    {
      exposedNode n = t->content.exposed;

      if (n != (exposedNode) 0)
	{
	  if (declaratorInvNodeList_size (n->decls) == 0)
	    {
	      /* 
	      ** Forward struct or union declaration
	      */

	      return (cstring_appendChar (lhTypeSpecNode (n->type), ';'));
	    }
	  else
	    {
	      cstring s = cstring_undefined;

	      declaratorInvNodeList_elements (n->decls, d)
		{
		  cstring name = declaratorNode_unparse (d->declarator);
		  cstring pname = declaratorNode_unparseCode (d->declarator); 
		  
		  s = message ("%q\n# ifndef EXPOSED_TYPE_%q\ntypedef %q %q;\n# endif\n", 
			       s, pname, lhTypeSpecNode (n->type), name);
		} end_declaratorInvNodeList_elements;
	      
	      return s;
	    }
	}
    }

  return cstring_undefined;
}

static /*@only@*/ cstring 
lhTypeSpecNode (lclTypeSpecNode typespec)
{
  if (!genLh)
    {
      return cstring_undefined;
    }

  return (lclTypeSpecNode_unparseComments (typespec));
}

/*@only@*/ cstring
lhVarDecl (lclTypeSpecNode lclTypeSpec, initDeclNodeList initDecls,
	   qualifierKind qualifier)
{
  bool first = TRUE;
  cstring s;

  if (!genLh)
    return cstring_undefined;

  s = cstring_makeLiteral ("extern");

  switch (qualifier)
    {
    case QLF_NONE:
      break;
    case QLF_CONST:
      s = message ("%q const", s);
      break;
    case QLF_VOLATILE:
      s = message ("%q volatile", s);
      break;
    default:			/* ignore it */
      break;
    }
  
  s = message ("%q %q ", s, lhTypeSpecNode (lclTypeSpec));

  initDeclNodeList_elements (initDecls, i)
  {
    if (first)
      {
	s = message ("%q %q", s, declaratorNode_unparse (i->declarator));
	first = FALSE;
      }
    else
      {
	s = message ("%q, %q", s, declaratorNode_unparse (i->declarator));
      }
  } end_initDeclNodeList_elements;
  
  return (message ("%q;", s));
}

extern void
lhCleanup (void)
   /*@modifies fileSystem@*/
{
  if (!genLh)
    {
      return;
    }
  else
    {
      llassert (LhFile.f != NULL);
      
      if (LhFile.f == NULL)
	{
	  lldiagmsg (message ("Cannot open lh file for output: %s", LhFile.name));
	}
      else
	{
	  check (fprintf (LhFile.f, "/* Output from %s */\n", LCL_PARSE_VERSION) > 0);
	  check (fileTable_closeFile (context_fileTable (), LhFile.f));
	  LhFile.f = NULL;
	}
    }
}

/* Write an #include of bool.h if we have't done so already.  */
extern void
lhIncludeBool (void)
{
  needIncludeBool = TRUE;
}

void lhInit (inputStream f) /*@globals undef LhFile; @*/
{
  static bool lherror = FALSE;
  
  genLh = context_msgLh ();
  needIncludeBool = FALSE;

  if (!genLh)
    {
      return;
    }
  
  LhFile.name = cstring_concatFree1 (LSLRootName (inputStream_fileName (f)),
				     LH_EXTENSION);
  LhFile.f = fileTable_openWriteUpdateFile (context_fileTable (), LhFile.name);

  if (LhFile.f == NULL)
    {
      genLh = FALSE;
      if (!lherror)
	{
	  lclplainerror (message ("Cannot write temporary file: %s", 
				  LhFile.name));
	  lherror = TRUE;
	}
    } 
} 

void lhOutLine (/*@only@*/ cstring s)
{
  if (genLh)
    {
      llassert (LhFile.f != NULL);
      DPRINTF (("lhOutLine: %s / %s", s, LhFile.name));

      if (cstring_length (s) > 0) 
	{
	  check (fputs (cstring_toCharsSafe (s), LhFile.f) != EOF); 
	}

      check (fputc ('\n', LhFile.f) == (int) '\n'); 
    }

  cstring_free (s);
}

void lhExternals (interfaceNodeList x)
{
  if (genLh)
    {
      llassert (LhFile.f != NULL);

      /*
      ** Need to make sure all standard library includes come first.
      */

      interfaceNodeList_elements (x, el)
	{
	  if (el->kind == INF_IMPORTS)
	    {
	      importNodeList imps = el->content.imports;

	      importNodeList_elements (imps, il)
		{
		  if (il->kind == IMPBRACKET)
		    {
		      lhOutLine (message ("# include <%s.h>", 
					  ltoken_getRawString (il->val)));
		    }
		} end_importNodeList_elements ;
	    }
	} end_interfaceNodeList_elements;

      lhOutLine (cstring_makeLiteral ("# include \"bool.h\""));
      
      interfaceNodeList_elements (x, el)
	{
	  if (el->kind == INF_IMPORTS)
	    {
	      importNodeList imps = el->content.imports;

	      importNodeList_elements (imps, il)
		{
		  if (il->kind != IMPBRACKET)
		    {
		      lhOutLine (message ("# include \"%s.h\"", 
					  ltoken_getRawString (il->val)));
		    }
		} end_importNodeList_elements ;
	    }
	} end_interfaceNodeList_elements;

      lhOutLine (cstring_undefined);
    }
}



