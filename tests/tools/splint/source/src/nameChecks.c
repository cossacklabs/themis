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
** nameChecks.c
*/

# include "splintMacros.nf"
# include "basic.h"
# include "nameChecks.h"

static bool checkCzechName (uentry p_ue, flagcode p_czechflag, bool p_report)
  /*@modifies p_ue, g_warningstream@*/ ;

static bool checkSlovakName (uentry p_ue, flagcode p_slovakflag, bool p_report)
  /*@modifies p_ue, g_warningstream@*/ ;

static cstring czechPrefix (cstring name)
{
  return (cstring_beforeChar (name, '_'));
}

static cstring slovakPrefix (cstring name)
{
  size_t i = 0;

  cstring_chars (name, c)
    {
      if (isupper ((unsigned char) c))
	{
	  return (cstring_prefix (name, i));
	}
      i++;
    } end_cstring_chars;

  return cstring_undefined;
}

static flagcode excludeCodes [] = 
 {
   FLG_MACROVARPREFIXEXCLUDE,
   FLG_TAGPREFIXEXCLUDE,
   FLG_ENUMPREFIXEXCLUDE,
   FLG_FILESTATICPREFIXEXCLUDE,
   FLG_GLOBPREFIXEXCLUDE,
   FLG_TYPEPREFIXEXCLUDE,
   FLG_EXTERNALPREFIXEXCLUDE,
   FLG_UNCHECKEDMACROPREFIXEXCLUDE,
   FLG_LOCALPREFIXEXCLUDE,
   INVALID_FLAG
   } ;

/*@iter excludeFlagCodes (yield flagcode code);@*/
# define excludeFlagCodes(m_c) \
  { int m_i = 0; while (flagcode_isValid (excludeCodes[m_i])) \
      { flagcode m_c = excludeCodes[m_i]; m_i++; 

# define end_excludeFlagCodes }}

static bool matchPrefixChar (int nc, int pc)
{
  if (nc == pc)
    {
      return TRUE;
    }
  else
    {
      switch (pc)
	{
	case PFX_UPPERCASE: 
	  return isupper (nc);
	case PFX_LOWERCASE:
	  return islower (nc);
	case PFX_ANY:
	  return TRUE;
	case PFX_DIGIT:
	  return isdigit (nc);
	case PFX_NOTUPPER:
	  return !isupper (nc);
	case PFX_NOTLOWER:
	  return !islower (nc);
	case PFX_ANYLETTER:
	  return isalpha (nc);
	case PFX_ANYLETTERDIGIT: 
	  return (isdigit (nc) || isalpha (nc));
	default: return FALSE;
	}
    }

  BADEXIT;
}

static bool matchPrefix (cstring name, cstring prefix)
{
  if (cstring_isUndefined (name)
      || cstring_isUndefined (prefix))
    {
      return TRUE;
    }
  else
    {
      size_t namelen = cstring_length (name);
      int last = (int) '\0';
      size_t n = 1;

      cstring_chars (prefix, pc)
	{
	  int nc;

	  if (pc == '*')
	    {
	      n++;

	      while (n <= namelen)
		{
		  nc = (int) cstring_getChar (name, n);

		  if (!matchPrefixChar (nc, last))
		    {
		      return FALSE;
		    }

		  n++;
		}

	      return TRUE;
	    }
	  else
	    {
	      if (n > namelen)
		{
		  if (namelen > 1
		      && (cstring_length (prefix) >= n + 1)
		      && cstring_getChar (prefix, n + 1) == '*')
		    {
		      return TRUE;
		    }
		  else
		    {
		      return FALSE;
		    }
		}
	      
	      nc = (int) cstring_getChar (name, n);
	      
	      if (!matchPrefixChar (nc, (int) pc))
		{
		  return FALSE;
		}
	    }
	
	  last = (int) pc;
	  n++;
	} end_cstring_chars;

      return TRUE;
    }
}

static flagcode
namespaceExcluded (flagcode code) /*@*/ 
{
  switch (code)
    {
    case FLG_MACROVARPREFIXEXCLUDE:
      return (FLG_MACROVARPREFIX);
    case FLG_TAGPREFIXEXCLUDE:
      return (FLG_TAGPREFIX);
    case FLG_ENUMPREFIXEXCLUDE:
      return (FLG_ENUMPREFIX);
    case FLG_FILESTATICPREFIXEXCLUDE:
      return (FLG_FILESTATICPREFIX);
    case FLG_GLOBPREFIXEXCLUDE:
      return (FLG_GLOBPREFIX);
    case FLG_TYPEPREFIXEXCLUDE:
      return (FLG_TYPEPREFIX);
    case FLG_EXTERNALPREFIXEXCLUDE:
      return (FLG_EXTERNALPREFIX);
    case FLG_UNCHECKEDMACROPREFIXEXCLUDE:
      return (FLG_UNCHECKEDMACROPREFIX);
    case FLG_LOCALPREFIXEXCLUDE:
      return (FLG_LOCALPREFIX);
    case FLG_ITERPREFIXEXCLUDE:
      return (FLG_ITERPREFIX);
    case FLG_CONSTPREFIXEXCLUDE:
      return (FLG_CONSTPREFIX);
    BADDEFAULT;
    }
}

static /*@observer@*/ cstring
namespaceName (flagcode flag) /*@*/
{
  switch (flag)
    {
    case FLG_MACROVARPREFIX: 
      return cstring_makeLiteralTemp ("macro variable");
    case FLG_TAGPREFIX:  
      return cstring_makeLiteralTemp ("tag");
    case FLG_ENUMPREFIX:  
      return cstring_makeLiteralTemp ("enum member");
    case FLG_TYPEPREFIX:     
      return cstring_makeLiteralTemp ("user-defined type");
    case FLG_FILESTATICPREFIX:
      return cstring_makeLiteralTemp ("file static");
    case FLG_GLOBPREFIX: 
      return cstring_makeLiteralTemp ("global variable");
    case FLG_EXTERNALPREFIX: 
      return cstring_makeLiteralTemp ("external");
    case FLG_LOCALPREFIX: 
      return cstring_makeLiteralTemp ("local variable");
    case FLG_CONSTPREFIX: 
      return cstring_makeLiteralTemp ("constant");
    case FLG_ITERPREFIX: 
      return cstring_makeLiteralTemp ("iter");
    case FLG_UNCHECKEDMACROPREFIX: 
      return cstring_makeLiteralTemp ("unchecked macro");
    BADDEFAULT;
    }
}

void
checkPrefix (uentry ue)
{
  cstring name = cstring_undefined;
  flagcode flag;

  if (uentry_isExpandedMacro (ue))
    {
      flag = FLG_UNCHECKEDMACROPREFIX;
    }
  else if (uentry_isAnyTag (ue))
    {
      flag = FLG_TAGPREFIX;
    }
  else if (uentry_isEnumConstant (ue))
    {
      flag = FLG_ENUMPREFIX;
    }
  else if (uentry_isDatatype (ue))
    {
      flag = FLG_TYPEPREFIX;
    }
  else if (uentry_isFileStatic (ue))
    {
      flag = FLG_FILESTATICPREFIX;
    }
  else if (uentry_isGlobalVariable (ue))
    {
      flag = FLG_GLOBPREFIX;
    }
  else if (uentry_isVariable (ue))
    {
      if (uentry_isRefParam (ue))
	{
	  return; /* already checked param */
	}

      if (context_inMacro ())
	{
	  if (uentry_isAnyParam (ue))
	    {
	      if (uentry_isYield (ue))
		{
		  flag = FLG_MACROVARPREFIX;
		}
	      else
		{
		  flag = FLG_LOCALPREFIX;
		}
	    }
	  else
	    {
	      flag = FLG_MACROVARPREFIX;
	    }
	}
      else
	{
	  flag = FLG_LOCALPREFIX;
	}
    }
  else if (uentry_isConstant (ue))
    {
      flag = FLG_CONSTPREFIX;
    }
  else if (uentry_isIter (ue))
    {
      flag = FLG_ITERPREFIX;
    }
  else if (uentry_isExported (ue))
    {
      flag = FLG_EXTERNALPREFIX;
    }
  else
    {
      llcontbug (message ("What is it: %q", uentry_unparseFull (ue)));
      return;
    }

  if (flag == FLG_TYPEPREFIX || flag == FLG_GLOBPREFIX
      || flag == FLG_ENUMPREFIX || flag == FLG_CONSTPREFIX)
    {
      if (flag == FLG_ENUMPREFIX)
	{
	  if (!context_getFlag (flag))
	    {
	      flag = FLG_CONSTPREFIX;
	    }
	}

      if (!context_getFlag (flag))
	{
	  flag = FLG_EXTERNALPREFIX;
	}
    }

  if (context_getFlag (flag))
    {
      name = uentry_observeRealName (ue);
      
      if (!matchPrefix (name, context_getString (flag)))
	{
	  llassert (flag != FLG_NAMECHECKS);

	  if (optgenerror2
	      (flag, FLG_NAMECHECKS,
	       message ("%s %s name is not consistent with %s "
			"namespace prefix \"%s\"",
			uentry_ekindName (ue),
			name,
			namespaceName (flag),
			context_getString (flag)),
	       uentry_whereLast (ue)))
	    {
	      uentry_setHasNameError (ue);
	    }
	}
    }  

  excludeFlagCodes (code)
    {
      bool check = FALSE;

      if (context_getFlag (code))
	{
	  /*@-loopswitchbreak@*/
	  switch (code)
	    {
	    case FLG_MACROVARPREFIXEXCLUDE:
	      check = (flag != FLG_MACROVARPREFIX);
	      break;
	    case FLG_TAGPREFIXEXCLUDE:
	      check = (flag != FLG_TAGPREFIX);
	      break;
	    case FLG_ENUMPREFIXEXCLUDE:
	      check = (flag != FLG_ENUMPREFIX);
	      break;
	    case FLG_FILESTATICPREFIXEXCLUDE:
	      check = (flag != FLG_FILESTATICPREFIX);
	      break;
	    case FLG_GLOBPREFIXEXCLUDE:
	      check = (flag != FLG_GLOBPREFIX);
	      break;
	    case FLG_TYPEPREFIXEXCLUDE:
	      check = (flag != FLG_TYPEPREFIX);
	      break;
	    case FLG_EXTERNALPREFIXEXCLUDE:
	      check = (flag != FLG_EXTERNALPREFIX
		       && flag != FLG_GLOBPREFIX
		       && flag != FLG_TYPEPREFIX
		       && flag != FLG_UNCHECKEDMACROPREFIX);
	      break;
	    case FLG_UNCHECKEDMACROPREFIXEXCLUDE:
	      check = (flag != FLG_UNCHECKEDMACROPREFIX);
	      break;
	    case FLG_LOCALPREFIXEXCLUDE:
	      check = (flag != FLG_LOCALPREFIX);
	      break;
	    case FLG_CONSTPREFIXEXCLUDE:
	      check = (flag != FLG_CONSTPREFIX);
	      break;
	    case FLG_ITERPREFIXEXCLUDE:
	      check = (flag != FLG_ITERPREFIX);
	      break;
	    BADDEFAULT;
	    }
	  /*@=loopswitchbreak@*/

	  if (check)
	    {
	      flagcode rcode = namespaceExcluded (code);
	      cstring pstring = context_getString (rcode);

	      if (cstring_isDefined (pstring))
		{
		  if (cstring_isUndefined (name))
		    {
		      name = uentry_observeRealName (ue);
		    }
		  
		  if (matchPrefix (name, context_getString (rcode)))
		    {
		      if (optgenerror2
			  (code, FLG_NAMECHECKS,
			   message
			   ("%s %s name is not a %s (it is a %s), "
			    "but matches the %s "
			    "namespace prefix \"%s\"",
			    uentry_ekindName (ue),
			    name,
			    namespaceName (rcode),
			    namespaceName (flag),
			    namespaceName (rcode),
			    context_getString (rcode)),
			   uentry_whereLast (ue)))
			{
			  uentry_setHasNameError (ue);
			}
		    }
		}
	    }
	} 
    } end_excludeFlagCodes ;
}

static void
checkNationalName (uentry ue)
{
  flagcode czechflag;
  flagcode slovakflag;
  flagcode czechoslovakflag;
  bool gcf, gsf, gcsf;

  
  if (uentry_isFunction (ue)
      || uentry_isIter (ue)
      || uentry_isEndIter (ue))
    {
      czechflag = FLG_CZECHFUNCTIONS;
      slovakflag = FLG_SLOVAKFUNCTIONS;
      czechoslovakflag = FLG_CZECHOSLOVAKFUNCTIONS;
    }
  else if (uentry_isExpandedMacro (ue))
    {
      czechflag = FLG_CZECHMACROS;
      slovakflag = FLG_SLOVAKMACROS;
      czechoslovakflag = FLG_CZECHOSLOVAKMACROS;
    }
  else if (uentry_isVariable (ue))
    {
      if (uentry_isGlobalVariable (ue) && context_getFlag (FLG_GLOBPREFIX))
	{
	  /* prefix checks supercede national naming checks */
	  return;
	}

      czechflag = FLG_CZECHVARS;
      slovakflag = FLG_SLOVAKVARS;
      czechoslovakflag = FLG_CZECHOSLOVAKVARS;
    }
  else if (uentry_isConstant (ue))
    {
      if (uentry_isGlobalVariable (ue) && context_getFlag (FLG_CONSTPREFIX))
	{
	  /* prefix checks supercede national naming checks */
	  return;
	}

      czechflag = FLG_CZECHCONSTANTS;
      slovakflag = FLG_SLOVAKCONSTANTS;
      czechoslovakflag = FLG_CZECHOSLOVAKCONSTANTS;
    }
  else
    {
      if (uentry_isAnyTag (ue) || uentry_isEnumConstant (ue))
	{
	  return; /* no errors for tags */
	}
      
      llassert (uentry_isDatatype (ue));

      czechflag = FLG_CZECHTYPES;
      slovakflag = FLG_SLOVAKTYPES;
      czechoslovakflag = FLG_CZECHOSLOVAKTYPES;
    }

  gcf = context_getFlag (czechflag);
  gsf = context_getFlag (slovakflag);
  gcsf = context_getFlag (czechoslovakflag);

  if (gcf || (uentry_isFunction (ue) 
	      && context_getFlag (FLG_ACCESSCZECH)))
    {
            (void) checkCzechName (ue, czechflag, gcf);
    }

  if (gsf || (uentry_isFunction (ue) 
	      && context_getFlag (FLG_ACCESSSLOVAK)))
    {
      (void) checkSlovakName (ue, slovakflag, gsf);
    }

  if (gcsf)
    {
      if (uentry_isDatatype (ue))
	{
	  /* May not have either _'s or uppercase letter */
	  cstring name = uentry_rawName (ue);
	  int charno = 1;

	  cstring_chars (name, c)
	    {
	      if (isupper ((unsigned char) c))
		{
		  if (optgenerror2
		      (FLG_CZECHOSLOVAKTYPES, FLG_NAMECHECKS,
		       message
		       ("%s %q name violates Czechoslovak naming convention.  "
			"Czechoslovak datatype names should not use uppercase "
			"letters.",
			uentry_ekindName (ue),
			uentry_getName (ue)),
		       uentry_whereLast (ue)))
		    {
		      uentry_setHasNameError (ue);
		    }
		  break;
		}

	      if (c == '_' && charno != 2 && charno != 3)
		{
		  if (optgenerror2
		      (FLG_CZECHOSLOVAKTYPES, FLG_NAMECHECKS,
		       message ("%s %q name violates Czechoslovak naming "
				"convention.  Czechoslovak datatype names "
				"should not use the _ charater.",
				uentry_ekindName (ue),
				uentry_getName (ue)),
		       uentry_whereLast (ue)))
		    {
		      uentry_setHasNameError (ue);
		    }
		  break;
		}
	      
	      charno++;
	    } end_cstring_chars;
	}
      else
	{
	  bool okay = checkCzechName (ue, czechflag, FALSE);
	  
	  /* still need to call, to set access */
	  okay |= checkSlovakName (ue, slovakflag, FALSE);
	  
	  if (!okay)
	    {
	      if (optgenerror2
		  (czechoslovakflag, FLG_NAMECHECKS,
		   message ("%s %q name is not consistent with Czechoslovak "
			    "naming convention.",
			    uentry_ekindName (ue),
			    uentry_getName (ue)),
		   uentry_whereLast (ue)))
		{
		  uentry_setHasNameError (ue);
		}
	    }
	}
    }
}

static bool checkCzechName (uentry ue, flagcode czechflag, bool report)
{
  if (uentry_isDatatype (ue))
    {
      /*
      ** Czech datatypes may not have _'s, except if there are 1 or 2 characters
      ** before the only _.
      */

      cstring name = uentry_rawName (ue);
      int charno = 1;
      
      cstring_chars (name, c)
	{
	  if (c == '_' && charno != 2 && charno != 3)
	    {
	      if (report)
		{
		  if (optgenerror2
		      (FLG_CZECHTYPES, FLG_NAMECHECKS,
		       message 
		       ("%s %q name violates Czech naming convention.  "
			"Czech datatype names should not use the _ charater.",
			uentry_ekindName (ue),
			uentry_getName (ue)),
		       uentry_whereLast (ue)))
		    {
		      uentry_setHasNameError (ue);
		    }
		}

	      return FALSE;
	    }
	  
	  charno++;
	} end_cstring_chars;
    }
  else
    {
      typeIdSet acc = context_fileAccessTypes ();
      cstring pfx = czechPrefix (uentry_rawName (ue));

      if (cstring_isEmpty (pfx))
	{
	  if (uentry_isVariable (ue) || uentry_isConstant (ue))
	    {
	      ctype ct = uentry_getType (ue);
	      
	      if (ctype_isAbstract (ct)
		  && context_hasAccess (ctype_typeId (ct)))
		{
		  if (report)
		    {
		      if (optgenerror2
			  (czechflag, FLG_NAMECHECKS,
			   message ("%s %q name is not consistent with Czech "
				    "naming convention.  The name should "
				    "begin with %s_",
				    uentry_ekindName (ue),
				    uentry_getName (ue),
				    ctype_unparse (ct)),
			   uentry_whereLast (ue)))
			{
			  uentry_setHasNameError (ue);
			}
		    }

		  cstring_free (pfx);
		  return FALSE;
		}
	    }
	  else if (uentry_isFunction (ue) || uentry_isIter (ue))
	    {
	      if (typeIdSet_isEmpty (acc))
		{
		  ; /* okay - should not be czech name */
		}
	      else
		{
		  if (report)
		    {
		      if (optgenerror2
			  (czechflag, FLG_NAMECHECKS,
			   message ("%s %q name is not consistent with Czech "
				    "naming convention.  Accessible types: %q",
				    uentry_ekindName (ue),
				    uentry_getName (ue),
				    typeIdSet_unparse (acc)),
			   uentry_whereLast (ue)))
			{
			  uentry_setHasNameError (ue);
			}
		    }

		  cstring_free (pfx);
		  return FALSE;
		}
	    }
	  else
	    {
	      ;
	    }
	}
      else
	{
	  if (usymtab_existsTypeEither (pfx))
	    {
	      ctype ct = usymtab_lookupAbstractType (pfx);
	      typeId tid;
	      
	      if (ctype_isUA (ct))
		{
		  tid = ctype_typeId (ct);
		  
		  if (ctype_isUser (ct) || context_hasAccess (tid))
		    {
		      ;
		    }
		  else
		    {
		      if (context_getFlag (FLG_ACCESSCZECH)
			  || context_getFlag (FLG_ACCESSCZECHOSLOVAK))
			{
			  if (!uentry_isVar (ue))
			    {
			      uentry_addAccessType (ue, tid);
			    }
			}
		      else
			{
			  if (report)
			    {
			      if (llgenhinterror
				  (czechflag,
				   message 
				   ("%s %q name violates Czech naming "
				    "convention. Czech prefix %s names "
				    "an abstract type that is "
				    "not accessible.",
				    uentry_ekindName (ue),
				    uentry_getName (ue),
				    pfx),
				   cstring_makeLiteral 
				   ("Use +accessczech to allow access to "
				    "type <t> in functions "
				    "named <t>_<name>."), 
				   uentry_whereLast (ue)))
				{
				  uentry_setHasNameError (ue);
				}
			    }
			  
			  cstring_free (pfx);
			  return FALSE;
			}
		    }
		}
	      else if (ctype_isManifestBool (ct))
		{
		  if (context_canAccessBool ())
		    {
		      ;
		    }
		  else
		    {
		      if (context_getFlag (FLG_ACCESSCZECH)
			  || context_getFlag (FLG_ACCESSCZECHOSLOVAK))
			{
			  if (!uentry_isVar (ue))
			    {
			      tid = usymtab_getTypeId (context_getBoolName ());
			      uentry_addAccessType (ue, tid);
			    }
			}
		      else
			{
			  if (report)
			    {
			      if (llgenhinterror
				  (czechflag,
				   message
				   ("%s %q name violates Czech naming "
				    "convention. Type bool is not accessible.",
				    uentry_ekindName (ue),
				    uentry_getName (ue)),
				   cstring_makeLiteral 
				   ("Use +accessczech to allow access to "
				    "type <t> in functions named <t>_<name>."), 
				   uentry_whereLast (ue)))
				{
				  uentry_setHasNameError (ue);
				}
			    }
			  
			  cstring_free (pfx);
			  return FALSE;
			}
		    }
		}
	      else
		{
		  ;
		}
	    }
	  else
	    {
	      if (cstring_equalLit (pfx, "int")
		  || cstring_equalLit (pfx, "char")
		  || cstring_equalLit (pfx, "short")
		  || cstring_equalLit (pfx, "long")
		  || cstring_equalLit (pfx, "unsigned")
		  || cstring_equalLit (pfx, "signed")
		  || cstring_equalLit (pfx, "float")
		  || cstring_equalLit (pfx, "double"))
		{
		  ; /* built-in types */
		}
	      else
		{
		  /* no accessible types, could match module name */
		  
		  if (cstring_equal (pfx, context_moduleName ()))
		    {
		      ;
		    }
		  else
		    {
		      if (report)
			{
			  if (optgenerror2
			      (czechflag, FLG_NAMECHECKS,
			       message 
			       ("%s %q name violates Czech naming convention.  "
				"Czech prefix %s is not the name of a type.",
				uentry_ekindName (ue),
				uentry_getName (ue),
				pfx),
			       uentry_whereLast (ue)))
			    {
			      uentry_setHasNameError (ue);
			    }
			}

		      cstring_free (pfx);		      
		      return FALSE;
		    }
		}
	    }
	}
      cstring_free (pfx);
    }

  return TRUE;
}

static bool checkSlovakName (uentry ue, flagcode slovakflag, bool report)
{
  if (uentry_isDatatype (ue))
    {
      /*
      ** Slovak datatypes may not have uppercase letters.
      */

      if (context_getFlag (FLG_SLOVAK))
	{
	  cstring name = uentry_rawName (ue);

	  cstring_chars (name, c)
	    {
	      if (isupper ((unsigned char) c))
		{
		  if (report)
		    {
		      if (optgenerror2
			  (FLG_SLOVAKTYPES, FLG_NAMECHECKS,
			   message 
			   ("%s %q name violates Slovak naming convention.  "
			    "Slovak datatype names should not use uppercase "
			    "letters.",
			    uentry_ekindName (ue),
			    uentry_getName (ue)),
			   uentry_whereLast (ue)))
			{
			  uentry_setHasNameError (ue);
			}
		    }
		  return FALSE;
		}
	    } end_cstring_chars;
	}
    }
  else
    {
      typeIdSet acc = context_fileAccessTypes ();
      cstring pfx = slovakPrefix (uentry_rawName (ue));
      
      if (cstring_isEmpty (pfx))
	{
	  if (typeIdSet_isEmpty (acc))
	    {
	      ; /* okay - should not be slovak name */
	    }
	  else
	    {
	      if (uentry_isFunction (ue))
		{
		  if (report)
		    {
		      if (optgenerror2
			  (slovakflag, FLG_NAMECHECKS,
			   message ("%s %q name is not consistent with Slovak "
				    "naming convention.  Accessible types: %q",
				    uentry_ekindName (ue),
				    uentry_getName (ue),
				    typeIdSet_unparse (acc)),
			   uentry_whereLast (ue)))
			{
			  uentry_setHasNameError (ue);
			}
		    }
		  
		  cstring_free (pfx);
		  return FALSE;
		}
	      else
		{
		  ctype ct = uentry_getType (ue);
		  
		  if (ctype_isUA (ct))
		    {
		      if (report)
			{
			  if (optgenerror2
			      (slovakflag, FLG_NAMECHECKS,
			       message ("%s %q name is not consistent with "
					"Slovak naming convention.  The "
					"name should begin with %s followed "
					"by an uppercase letter.",
					uentry_ekindName (ue),
					uentry_getName (ue),
					ctype_unparse (ct)),
			       uentry_whereLast (ue)))
			    {
			      uentry_setHasNameError (ue);
			    }	
			}
		      
		      cstring_free (pfx);
		      return FALSE;
		    }
		}
	    }
	}
      else
	{
	  if (usymtab_existsTypeEither (pfx))
	    {
	      ctype ct = usymtab_lookupAbstractType (pfx);
	      typeId tid;
	      
	      if (ctype_isUA (ct))
		{
		  tid = ctype_typeId (ct);
		  
		  if (ctype_isUser (ct) || context_hasAccess (tid))
		    {
		      ;
		    }
		  else
		    {
		      if (context_getFlag (FLG_ACCESSSLOVAK)
			  || context_getFlag (FLG_ACCESSCZECHOSLOVAK))
			{
			  if (!uentry_isVar (ue))
			    {
			      uentry_addAccessType (ue, tid);
			    }
			}
		      else
			{
			  if (report)
			    {
			      if (llgenhinterror
				  (slovakflag,
				   message 
				   ("%s %q name violates Slovak naming "
				    "convention. Slovak prefix %s names "
				    "an abstract type that is not accessible.",
				    uentry_ekindName (ue),
				    uentry_getName (ue),
				    pfx),
				   cstring_makeLiteral 
				   ("Use +accessslovak to allow access to "
				    "type <t> in functions named <t>_<name>."), 
				   uentry_whereLast (ue)))
				{
				  uentry_setHasNameError (ue);
				}
			    }
			  
			  cstring_free (pfx);
			  return FALSE;
			}
		    }
		}
	      else if (ctype_isManifestBool (ct))
		{
		  if (context_canAccessBool ())
		    {
		      ;
		    }
		  else
		    {
		      if (context_getFlag (FLG_ACCESSSLOVAK)
			  || context_getFlag (FLG_ACCESSCZECHOSLOVAK))
			{
			  if (!uentry_isVar (ue))
			    {
			      tid = usymtab_getTypeId (context_getBoolName ());
			      uentry_addAccessType (ue, tid);
			    }
			}
		      else
			{
			  if (report)
			    {
			      if (llgenhinterror
				  (slovakflag,
				   message
				   ("%s %q name violates Slovak naming convention.  "
				    "Type bool is not accessible.",
				    uentry_ekindName (ue),
				    uentry_getName (ue)),
				   cstring_makeLiteral
				   ("Use +accessslovak to allow access to "
				    "type <t> in functions named <t>_<name>."),
				   uentry_whereLast (ue)))
				{
				  uentry_setHasNameError (ue);
				}
			    }
			  
			  cstring_free (pfx);
			  return FALSE;
			}
		    }
		}
	      else
		{
		  ;
		}
	    }
	  else
	    {
	      if (cstring_equalLit (pfx, "int")
		  || cstring_equalLit (pfx, "char")
		  || cstring_equalLit (pfx, "short")
		  || cstring_equalLit (pfx, "long")
		  || cstring_equalLit (pfx, "unsigned")
		  || cstring_equalLit (pfx, "signed")
		  || cstring_equalLit (pfx, "float")
		  || cstring_equalLit (pfx, "double"))
		{
		  ; /* built-in types */
		}
	      else
		{
		  /* no accessible types, could match module name */
		  
		  if (cstring_equal (pfx, context_moduleName ()))
		    {
		      ;
		    }
		  else
		    {
		      if (report)
			{
			  if (optgenerror2
			      (slovakflag, FLG_NAMECHECKS,
			       message 
			       ("%s %q name violates Slovak naming convention.  "
				"Slovak prefix %s is not the name of a type.",
				uentry_ekindName (ue),
				uentry_getName (ue),
				pfx),
			       uentry_whereLast (ue)))
			    {
			      uentry_setHasNameError (ue);
			    }
			}

		      cstring_free (pfx);
		      return FALSE;
		    }
		}
	    }
	}

      cstring_free (pfx);
    }

  return TRUE;
}

void
checkExternalName (uentry ue)
{
  if (!uentry_isStatic (ue) && uentry_hasName (ue))
    {
      checkNationalName (ue);
    }
  else
    {
      ;
    }
}

void
checkLocalName (/*@unused@*/ uentry ue)
{
  /*
  ** No local checks (yet)
  */

  return;
}

void
checkFileScopeName (/*@unused@*/ uentry ue)
{
  /*
  ** No file scope checks (yet)
  */

  /* add a file scope naming convention policy? */

  return;
}

/*
** Checks a name used by user source is not reserved by ANSI 
** (or for future library functions).
**
** The restrictions are described in X3.159-1989: 4.13
*/

/*@constant int NRESERVEDNAMES; @*/
# define NRESERVEDNAMES 201

/*@constant int NCPPNAMES@*/
# define NCPPNAMES 39

void
checkCppName (uentry ue)
{
  cstring name = uentry_observeRealName (ue);

  static ob_mstring cppNames[NCPPNAMES] =
    {
      "and", "and_eq", "asm", 
      "bitand", "bitor", "bool", /* gasp: "bool", is special for splint */
      "catch", "class", "compl", "const_class",
      "delete", "dynamic_cast", "false", "friend",
      "inline", "mutable", "namespace", "new",
      "not", "not_eq",
      "operator", "or", "or_eq", "overload",
      "private", "protected", "public",
      "reinterpret_cast", "static_cast",
      "template", "this", "throw", "true", "try",
      "typeid", "using", "virtual", "xor", "xor_eq"
      } ;
  
  if (cstring_isDefined (cstring_bsearch (name, &cppNames[0],
					  NCPPNAMES)))
    {
      if (optgenerror2
	  (FLG_CPPNAMES, FLG_NAMECHECKS,
	   message ("Name %s is a keyword or reserved word in C++",
		    name),
	   uentry_whereLast (ue)))
	{
	  uentry_setHasNameError (ue);
	}
    }
}

void
checkAnsiName (uentry ue)
{
  bool hasError = FALSE;
  cstring name = uentry_observeRealName (ue);
  size_t length = cstring_length (name);
  char fchar = (length >= 1) ? cstring_firstChar (name) : '\0';
  char schar = (length >= 2) ? cstring_secondChar (name) : '\0';
  char tchar = (length >= 3) ? cstring_getChar (name, 3) : '\0';
  char rchar = (length >= 4) ? cstring_getChar (name, 4) : '\0';

  /* 
  ** reservedNames
  **   taken from Linden, "Expert C Programming", p. 126-8. 
  **   invariant:  must be sorted (case-insensitive, lexicographically)
  **               must end with NULL
  */

  static ob_mstring reservedNames[NRESERVEDNAMES] =
    { 
# include "reservedNames.nf"
    } ;

# if 0
  /*
  ** This code is for checking reservedNames.nf
  */

  {
    int i = 0;
    char *lastname = NULL;
    char *name;
    
    while ((name = reservedNames[i]) != NULL)
      {
	llassertprint (lastname == NULL
		       || strcmp (name, lastname) > 0,
		       ("%s / %s", lastname, name));
	lastname = name;
	i++;
      }
    
    nreservedNames = i - 1;
  }
# endif

  if (fileloc_isSystemFile (uentry_whereLast (ue)) || fileloc_isBuiltin (uentry_whereLast (ue)))
    {
      return;  /* no errors for system files */
    }
  
  if (cstring_isDefined (cstring_bsearch (name, &reservedNames[0],
					  NRESERVEDNAMES)))
    {
      hasError |= optgenerror2
	(FLG_ISORESERVED, FLG_NAMECHECKS,
	 message ("Name %s is reserved for the standard library",
		  name),
	 uentry_whereLast (ue));
    }

  if (uentry_isFileStatic (ue) || uentry_isVisibleExternally (ue) || uentry_isAnyTag (ue)
      || context_getFlag (FLG_ISORESERVEDLOCAL))
    {
      if (fchar == '_')
	{
	  hasError |= optgenerror2
	    (FLG_ISORESERVED, FLG_NAMECHECKS,
	     message 
	     ("Name %s is in the implementation name space (any identifier "
	      "beginning with underscore)", 
	      name),
	     uentry_whereLast (ue));
	}
    }
  else
    {
      /*
      ** ISO 7.1.3:
      ** - All identifiers that begin with an underscore and either an uppercase
      ** letter or another underscore are always reserved for any use.
      */
      
      if (fchar == '_' 
	  && (schar == '_' || isupper ((int) schar)))
	{
	  hasError |= optgenerror2
	    (FLG_ISORESERVED, FLG_NAMECHECKS,
	     message 
	     ("Name %s is in the implementation name space (any identifier "
	      "beginning with underscore and either an uppercase letter or "
	      "another underscore is always reserved for any use)", 
	      name),
	     uentry_whereLast (ue));
	}
    }

  /*
  ** 4.13.1 Errors <errno.h>
  **
  ** Macros that begin with E and a digit or E and an uppercase letter ...
  */
  
  if (fchar == 'E' && (isdigit ((int) schar) 
		       || isupper ((int) schar)))
    {
      hasError |= optgenerror2
	(FLG_ISORESERVED, FLG_NAMECHECKS,
	 message 
	 ("Name %s is reserved for future library extensions. "
	  "Macros beginning with E and a digit or uppercase letter "
	  "may be added to <errno.h>. (ISO99:7.26.3)",
	  name),
	 uentry_whereLast (ue));
    }

  /*
  ** 4.13.3 Localization <locale.h>
  **
  ** Macros that begin with LC_ and an uppercase letter ...
  */
  
  if (length >= 4 
      && ((fchar == 'L')
	  && (schar == 'C')
	  && (tchar == '_'))
      && (isupper ((int) rchar)))
    {
      hasError |= optgenerror2
	(FLG_ISORESERVED, FLG_NAMECHECKS,
	 message
	 ("Name %s is reserved for future library extensions.  "
	  "Macros beginning with \"LC_\" and an uppercase letter may "
	  "be added to <locale.h>. (ISO99:7.26.5)",
	  name),
	 uentry_whereLast (ue));
    }

  /*
  ** 4.13.5 Signal Handling <signal.h>
  **
  ** Macros that begin with either SIG or SIG_ and an uppercase letter or...
  */
  
  if (fchar == 'S' && schar == 'I' && tchar == 'G'
	   && ((rchar == '_' && ((length >= 5 
				  && isupper ((int) cstring_getChar (name, 5)))))
	       || (isupper ((int) rchar))))
    {
      hasError |= optgenerror2
	(FLG_ISORESERVED, FLG_NAMECHECKS,
	 message
	 ("Name %s is reserved for future library extensions.  "
	  "Macros that begin with SIG and an uppercase letter or SIG_ "
	  "and an uppercase letter may be added to "
	  "<signal.h>. (ISO99:7.14)",
	  name),
	 uentry_whereLast (ue));
    }

  /*
  ** evans - 2002-12-16: added this check (even though it is not required by ISO)
  */

  if (fchar == 'S' && schar == 'A' && tchar == '_')
    {
      hasError |= optgenerror2
	(FLG_ISORESERVED, FLG_NAMECHECKS,
	 message 
	 ("Name %s may be defined as a macro by Linux library. "
	  "It is not research by the ISO specification, but may produce conflicts on some systems.",
	  name),
	 uentry_whereLast (ue));
    }

  if ((uentry_isVisibleExternally (ue) && !uentry_isAnyTag (ue))
      || context_getFlag (FLG_ISORESERVEDLOCAL))
    {
      flagcode flg;

      DPRINTF (("Okay...: %s", uentry_unparse (ue)));

      if (uentry_isVisibleExternally (ue) && !uentry_isAnyTag (ue))
	{
	  flg = FLG_ISORESERVED;
	}
      else
	{
	  flg = FLG_ISORESERVEDLOCAL;
	}

      DPRINTF (("ue: %s", uentry_unparseFull (ue)));

      /*
      ** These restrictions only apply to identifiers with global linkage.
      */

      /*
      ** 4.13.2 Character Handling <ctype.h>
      **
      ** Function names that begin with either "is" or "to" and a lowercase letter ...
      */
      
      if (((fchar == 'i' && schar == 's') 
	   || (fchar == 't' && schar == 'o'))
	  && (islower ((int) tchar)))
	{
	  hasError |= optgenerror2
	    (flg, FLG_NAMECHECKS,
	     message
	     ("Name %s is reserved for future library extensions.  "
	      "Functions beginning with \"is\" or \"to\" and a lowercase "
	      "letter may be added to <ctype.h>. (ISO99:7.26.2)",
	      name),
	     uentry_whereLast (ue));

	  DPRINTF (("Externally visible: %s / %s",
		    uentry_unparseFull (ue),
		    bool_unparse (uentry_isVisibleExternally (ue))));
	}
      
      
      /*
      ** 4.13.4 Mathematics <math.h>
      **
      ** The names of all existing functions declared in the <math.h> header, 
      ** suffixed with f or l...
      */

      DPRINTF (("Check name: %s", name));

      if ((cstring_lastChar (name) == 'f' || cstring_lastChar (name) == 'l')
	  && 
	  (((length == 4)
	    && ((cstring_equalPrefixLit (name, "cos") ||
		 cstring_equalPrefixLit (name, "sin") ||
		 cstring_equalPrefixLit (name, "tan") ||
		 cstring_equalPrefixLit (name, "exp") ||
		 cstring_equalPrefixLit (name, "log") ||
		 cstring_equalPrefixLit (name, "pow"))))
	   || ((length == 5)
	       && ((cstring_equalPrefixLit (name, "acos") ||
		    cstring_equalPrefixLit (name, "asin") ||
		    cstring_equalPrefixLit (name, "atan") ||
		    cstring_equalPrefixLit (name, "cosh") ||
		    cstring_equalPrefixLit (name, "sinh") ||
		    cstring_equalPrefixLit (name, "sqrt") ||
		    cstring_equalPrefixLit (name, "ceil") ||
		    cstring_equalPrefixLit (name, "fabs") ||
		    cstring_equalPrefixLit (name, "fmod") ||
		    cstring_equalPrefixLit (name, "tanh") ||
		    cstring_equalPrefixLit (name, "modf"))))
	   || ((length == 6)
	       && ((cstring_equalPrefixLit (name, "atan2") ||
		    cstring_equalPrefixLit (name, "floor") ||
		    cstring_equalPrefixLit (name, "frexp") ||
		    cstring_equalPrefixLit (name, "ldexp") ||
		    cstring_equalPrefixLit (name, "log10"))))))
	{
	  hasError |= optgenerror2
	    (flg, FLG_NAMECHECKS,
	     message
	     ("Name %s is reserved for future library extensions.  "
	      "The names of all existing functions in <math.h> suffixed "
	      "with 'f' or 'l' may be added to <math.h>. (ISO:7.26.1)",
	      name),
	     uentry_whereLast (ue));
	}
      
      /*
      ** 4.13.6 Input/Output <stdio.h>
      **
      ** (nothing to check)
      */
      
      /*
      ** 4.13.7 General Utilities <stdlib.h>
      **
      ** Functions names that begin with str and a lowercase letter may be added to <stdlib.h>.
      */
      
      if (fchar == 's' && schar == 't' && tchar == 'r' 
	  && (islower ((int) rchar)))
	{
	  hasError |= optgenerror2
	    (flg, FLG_NAMECHECKS,
	     message
	     ("Name %s is reserved for future library extensions.  "
	      "Functions that begin with \"str\" and a lowercase letter "
	      "may be added to <stdlib.h> or <string.h>. (ISO99:7.26.9)",
	      name),
	     uentry_whereLast (ue));
	}
      
      /*
      ** 4.13.8 String Handling <string.h>
      **
      ** Function names that begin with str, mem, or wcs and a lowercase letter ...
      **
      ** (Note: already checked "str" above.)
      */
      
      if (((fchar == 'm' && schar == 'e' && tchar == 'm')
	   || (fchar == 'w' && schar == 'c' && tchar == 's'))
	  && (islower ((int) rchar)))
	{
	  hasError |= optgenerror2
	    (flg, FLG_NAMECHECKS,
	     message
	     ("Name %s is reserved for future library extensions.  "
	      "Functions that begin with \"mem\" or \"wcs\" and a "
	      "lowercase letter may be added to <string.h>. (ISO:7.26.11)",
	      name),
	     uentry_whereLast (ue));
	}
    }
  else
    {
      DPRINTF (("Not checked: [%s] %s", bool_unparse (uentry_isVisibleExternally (ue)),
		uentry_unparseFull (ue)));
    }

  if (hasError)
    {
      uentry_setHasNameError (ue);
    }
}

void checkParamNames (uentry ue)
{
  cstring fpfx = context_getString (FLG_DECLPARAMPREFIX);
  bool noformal = context_getFlag (FLG_DECLPARAMNAME);

  llassert (uentry_isFunction (ue));
  
  if (cstring_isDefined (fpfx) || noformal)
    {
      uentryList params = uentry_getParams (ue);
      
      uentryList_elements (params, p)
	{
	  if (uentry_hasName (p))
	    {
	      if (noformal && !cstring_isDefined (fpfx))
		{
		  if (optgenerror2
		      (FLG_DECLPARAMNAME, FLG_NAMECHECKS,
		       message ("Declaration parameter has name: %q",
				uentry_getName (p)),
		       uentry_whereLast (p)))
		    {
		      uentry_setHasNameError (p);
		    }
		}
	      else
		{
		  cstring pname = uentry_observeRealName (p);
		  
		  if (!cstring_equalPrefix (pname, fpfx))
		    {
		      if (context_getFlag (FLG_NAMECHECKS))
			{
			  if (optgenerror2
			      (FLG_DECLPARAMPREFIX, FLG_NAMECHECKS,
			       message ("Declaration parameter name %s does not begin "
					"with protoparamprefix (%s)",
					pname, fpfx),
			       uentry_whereLast (p)))
			    {
			      uentry_setHasNameError (p);
			    }
			}
		    }
		}
	    }
	} end_uentryList_elements ;
    }
}

/* not yet checked: POSIX p. 527 - applications should not declare any symbols that end _MAX */
