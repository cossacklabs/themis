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
** cprim.c
*/

# include "splintMacros.nf"
# include "basic.h"

static bool cprim_isReal (cprim c)
{
  return (cprim_isAnyReal (c));
}

static bool cprim_isNumeric (cprim c)
{
  return (cprim_isReal (c) || cprim_isInt (c));
}

cprim
cprim_fromInt (int i)
{
  if (i < CTX_UNKNOWN || i > CTX_LAST)
    {
      llcontbug (message ("cprim_fromInt: out of range: %d", i));
      return CTX_UNKNOWN;
    }
  return (cprim) i;
}


/*
** not symmetric:  c1 := c2 or c2 is passed as c1
**    (if RELAXQUALS, c1 must be "bigger" than c2)
*/

static bool cprim_closeEnoughAux (cprim p_c1, cprim p_c2, bool p_deep);

bool
cprim_closeEnoughDeep (cprim c1, cprim c2) 
{
  /*
  ** If * c2 is passed as * c1
  ** Comparison is slightly different since it is safe to pass int as long,
  ** but not to pass int * as long *!
  **
  ** For deep comparisons, +relaxquals does not permit the long/int break.
  */

  return cprim_closeEnoughAux (c1, c2, TRUE);
}

bool
cprim_closeEnough (cprim c1, cprim c2)
{
  return cprim_closeEnoughAux (c1, c2, FALSE);
}

static bool
cprim_closeEnoughAux (cprim c1, cprim c2, bool deep)
{
  if (c1 == c2) return TRUE;
  
  DPRINTF (("cprim close: %s / %s", cprim_unparse (c1), cprim_unparse (c2)));

  if (c1 == CTX_ANYINTEGRAL)
    {
      if (context_getFlag (FLG_MATCHANYINTEGRAL)
	  || context_getFlag (FLG_IGNOREQUALS))
	{
	  return (cprim_isAnyInt (c2)
		  || (cprim_isAnyChar (c2) && context_msgCharInt ()));
	}
      else if (context_getFlag (FLG_LONGINTEGRAL))
	{
	  return (cprim_closeEnough (CTX_LINT, c2));
	}
      else if (context_getFlag (FLG_LONGUNSIGNEDINTEGRAL))
	{
	  return (cprim_closeEnough (CTX_ULINT, c2));
	}
      else
	{
	  return FALSE;
	}
    }

  if (c1 == CTX_UNSIGNEDINTEGRAL)
    {
      /* We allow signed ints to match any integral if matchanyintegral is set */
      if (context_getFlag (FLG_MATCHANYINTEGRAL)) {
	return (cprim_isAnyInt (c2)
		|| (cprim_isAnyChar (c2) && context_msgCharInt ()));
      }
      
      if (context_getFlag (FLG_IGNOREQUALS))
	{
	  if (context_getFlag (FLG_IGNORESIGNS)) 
	    {
	      return (cprim_isAnyUnsignedInt (c2)
		      || (cprim_isUnsignedChar (c2) && context_msgCharInt ()));
	    }
	  else
	    {
	      return (cprim_isAnyInt (c2)
		      || (cprim_isAnyChar (c2) && context_msgCharInt ()));
	    }
	}
      else if (context_getFlag (FLG_LONGUNSIGNEDUNSIGNEDINTEGRAL))
	{
	  return (cprim_closeEnough (CTX_ULINT, c2));
	}
      else
	{
	  return FALSE;
	}
    }

  if (c1 == CTX_SIGNEDINTEGRAL)
    {
      /* We allow signed ints to match any integral if matchanyintegral is set */
      if (context_getFlag (FLG_MATCHANYINTEGRAL)) {
	return (cprim_isAnyInt (c2)
		|| (cprim_isAnyChar (c2) && context_msgCharInt ()));
      }

      if (context_getFlag (FLG_IGNOREQUALS))
	{
	  return (cprim_isAnyInt (c2)
		  || (cprim_isAnyChar (c2) && context_msgCharInt ()));
	}
      else if (context_getFlag (FLG_LONGSIGNEDINTEGRAL))
	{
	  return (cprim_closeEnough (CTX_LINT, c2));
	}
      else
	{
	  return FALSE;
	}
    }

  if (c2 == CTX_ANYINTEGRAL)
    {
      if (context_getFlag (FLG_MATCHANYINTEGRAL))
	{
	  return (cprim_isAnyInt (c1)
		  || (cprim_isAnyChar (c1) && context_msgCharInt ()));
	}
      else if (context_getFlag (FLG_LONGINTEGRAL))
	{
	  return (cprim_closeEnough (c1, CTX_LINT));
	}
      else if (context_getFlag (FLG_LONGUNSIGNEDINTEGRAL))
	{
	  return (cprim_closeEnough (c1, CTX_ULINT));
	}
      else
	{
	  return FALSE;
	}
    }

  if (c2 == CTX_UNSIGNEDINTEGRAL)
    {
      if (context_getFlag (FLG_MATCHANYINTEGRAL))
	{
	  return (cprim_isAnyInt (c1)
		  || (cprim_isAnyChar (c1) && context_msgCharInt ()));
	}
      else if (context_getFlag (FLG_LONGUNSIGNEDUNSIGNEDINTEGRAL))
	{
	  return (cprim_closeEnough (c1, CTX_ULINT));
	}
      else
	{
	  return FALSE;
	}
    }

  if (c2 == CTX_SIGNEDINTEGRAL)
    {
      if (context_getFlag (FLG_MATCHANYINTEGRAL))
	{
	  return (cprim_isAnyInt (c2)
		  || (cprim_isAnyChar (c2) && context_msgCharInt ()));
	}
      else if (context_getFlag (FLG_LONGSIGNEDINTEGRAL))
	{
	  return (cprim_closeEnough (c1, CTX_LINT));
	}
      else
	{
	  return FALSE;
	}
    }


  DPRINTF (("cprim close: %s / %s", cprim_unparse (c1), cprim_unparse (c2)));

  if (context_getFlag (FLG_RELAXTYPES))
    {
      if (cprim_isNumeric (c1) && cprim_isNumeric (c2)) return TRUE;
    }

  if (context_getFlag (FLG_IGNOREQUALS))
    {
      switch (c1)
	{
	case CTX_CHAR:
	case CTX_UCHAR:
	  if (cprim_isAnyChar (c2) 
	      || (cprim_isAnyInt (c2) && (context_msgCharInt ()))) {
	    return TRUE;
	  } 
	  break;
	case CTX_DOUBLE:
	case CTX_FLOAT:
	case CTX_LDOUBLE:
	  if (c2 == CTX_DOUBLE || c2 == CTX_FLOAT || c2 == CTX_LDOUBLE) {
	    return TRUE;
	  }
	  break;
	case CTX_INT:
	case CTX_LINT:
	case CTX_LLINT:
	case CTX_ULLINT:
	case CTX_SINT:
	case CTX_UINT:
	case CTX_ULINT:
	case CTX_USINT:
	  if (cprim_isAnyInt (c2) 
	      || (cprim_isAnyChar (c2) && context_msgCharInt ())) {
	    return TRUE;
	  }
	  /*@fallthrough@*/ 
	default:
	  ;
	}
    }

  if (context_getFlag (FLG_IGNORESIGNS))
    {
      if (c1 == CTX_UCHAR)  
	{
	  c1 = CTX_CHAR;
	}
      else if (c1 == CTX_UINT)  
	{
	  c1 = CTX_INT;
	}
      else if (c1 == CTX_ULINT) 
	{
	  c1 = CTX_LINT;
	}
      /* 2001-06-10: This fix provided by Jim Zelenka: */
      else if (c1 == CTX_ULLINT) 
	{
	  c1 = CTX_LLINT;
	}
      /* End fix */
      else if (c1 == CTX_USINT)  
	{
	  c1 = CTX_SINT;
	}
      else
	{
	  ;
	}
      
      if (c2 == CTX_UCHAR)  
	{
	  c2 = CTX_CHAR;
	}
      else if (c2 == CTX_UINT)   
	{
	  c2 = CTX_INT;
	}
      else if (c2 == CTX_ULINT) 
	{
	  c2 = CTX_LINT;
	}
      /* 2001-06-10: This fix provided by Jim Zelenka: */
      else if (c2 == CTX_ULLINT)
	{
	  c2 = CTX_LLINT;
	}
      /* End fix */
      else if (c2 == CTX_USINT)  
	{
	  c2 = CTX_SINT;
	}
      else
	{
	  ;
	}
    }

  if (c1 == c2) return TRUE;
  
  if (context_getFlag (FLG_FLOATDOUBLE))
    {
      if (c1 == CTX_FLOAT && c2 == CTX_DOUBLE) 
	{
	  return TRUE;
	}
      if (c2 == CTX_FLOAT && c1 == CTX_DOUBLE)
	{
	  return TRUE;
	}
    }
  
  DPRINTF (("cprim close: %s / %s", cprim_unparse (c1), cprim_unparse (c2)));
  
  if (!deep && context_getFlag (FLG_RELAXQUALS))
    {
      switch (c1)
	{
	case CTX_DOUBLE:
	  return (c2 == CTX_FLOAT);
	case CTX_LDOUBLE:
	  return (c2 == CTX_DOUBLE || c2 == CTX_FLOAT);
	case CTX_SINT:
	  return ((c2 == CTX_CHAR && context_msgCharInt ()) 
		  || (c2 == CTX_INT && context_msgShortInt ())
		  || (c2 == CTX_LINT && context_msgShortInt () && context_msgLongInt ()));

	case CTX_INT:
	  return ((c2 == CTX_SINT
		   || (cprim_isAnyChar (c2) && context_msgCharInt ())
		   || (c2 == CTX_LINT && context_msgLongInt ())));

	case CTX_LLINT:
	  return (c2 == CTX_SINT
		  || c2 == CTX_INT 
		      || c2 == CTX_LINT
		  || (cprim_isAnyChar (c2) && context_msgCharInt ()));
	case CTX_ULLINT:
	  return (c2 == CTX_USINT
		  || c2 == CTX_UINT 
		  || c2 == CTX_ULINT
		  /* 2001-06-10: This fix provided by Jim Zelenka: */
		  || (cprim_isAnyChar (c2) && context_msgCharInt ()));
	case CTX_LINT:
	  return (c2 == CTX_SINT
		  || c2 == CTX_INT 
		  || (cprim_isAnyChar (c2) && context_msgCharInt ()));
	case CTX_UINT:
	  return (c2 == CTX_USINT 
		  || (c2 == CTX_UCHAR && context_msgCharInt ()));
	case CTX_USINT:
	  return (c2 == CTX_UCHAR && context_msgCharInt ());
	case CTX_ULINT:
	  /* 2001-06-10: This fix provided by Jim Zelenka: */
	  return (c2 == CTX_UINT || c2 == CTX_USINT
		  || (c2 == CTX_UCHAR && context_msgCharInt()));
	case CTX_UCHAR:
	  return (c2 == CTX_UINT && context_msgCharInt ());
	case CTX_CHAR:
	  return ((c2 == CTX_INT || c2 == CTX_SINT)
		  && context_msgCharInt ());
	default:
	  return FALSE;
	}
    }
  else
    {
      switch (c1)
	{
	case CTX_DOUBLE:
	case CTX_LDOUBLE:
	  return FALSE;
	case CTX_SINT:
	  if (c2 == CTX_INT && context_msgShortInt ()) {
	    return TRUE;
	  }
	  /*@fallthrough@*/
	case CTX_INT:
	  if (c2 == CTX_INT && context_msgLongInt ()) {
	    return TRUE;
	  }
	  
	  if (c2 == CTX_SINT && context_msgShortInt ()) {
	    return TRUE;
	  }
	  /*@fallthrough@*/
	case CTX_LINT:
	  if (c2 == CTX_INT && context_msgLongInt ()) {
	    return TRUE;
	  }
	  /*@fallthrough@*/
	case CTX_LLINT:
	  return (c2 == CTX_CHAR && context_msgCharInt ());
	case CTX_UINT:
	case CTX_USINT:
	case CTX_ULINT:
	case CTX_ULLINT:
	  return (c2 == CTX_UCHAR && context_msgCharInt ());
	case CTX_UCHAR:
	  return (c2 == CTX_UINT && context_msgCharInt ());
	case CTX_CHAR:
	  return ((c2 == CTX_INT || c2 == CTX_SINT)
		  && context_msgCharInt ());
	default:
	  return FALSE;
	}
    }
}

/*@only@*/ cstring
cprim_unparse (cprim c)
{
  switch (c)
    {
    case CTX_UNKNOWN:
      return cstring_makeLiteral ("-");
    case CTX_VOID:
      return cstring_makeLiteral ("void");
    case CTX_CHAR:
      return cstring_makeLiteral ("char");
    case CTX_UCHAR:
      return cstring_makeLiteral ("unsigned char");
   case CTX_DOUBLE:
      return cstring_makeLiteral ("double");
    case CTX_LDOUBLE:
      return cstring_makeLiteral ("long double");
    case CTX_FLOAT:
      return cstring_makeLiteral ("float");
    case CTX_INT:
      return cstring_makeLiteral ("int");
    case CTX_LINT:
      return cstring_makeLiteral ("long int");
    case CTX_LLINT:
      return cstring_makeLiteral ("long long");
    case CTX_ULLINT:
      return cstring_makeLiteral ("unsigned long long");
    case CTX_SINT:
      return cstring_makeLiteral ("short int");
    case CTX_UINT:
      return cstring_makeLiteral ("unsigned int");
    case CTX_ULINT:
      return cstring_makeLiteral ("unsigned long int");
    case CTX_USINT:
      return cstring_makeLiteral ("unsigned short int");
    case CTX_UNSIGNEDINTEGRAL:
      return cstring_makeLiteral ("arbitrary unsigned integral type");
    case CTX_SIGNEDINTEGRAL:
      return cstring_makeLiteral ("arbitrary signed integral type");
    case CTX_ANYINTEGRAL:
      return cstring_makeLiteral ("arbitrary integral type");
    default:
      return cstring_makeLiteral ("unknown prim");
    }
}

bool cprim_isInt (cprim c) 
{
  return (cprim_isAnyInt (c)
	  || (cprim_isAnyChar (c) && context_msgCharInt ()));
}
    
int cprim_getExpectedBits (cprim c)
{
  /* Any basis to these numbers?  Just guesses for now..., check ISO spec */
  switch (c)
    {
    case CTX_UNKNOWN:
      return 0;
    case CTX_VOID:
      return 0;
    case CTX_CHAR:
      return 8;
    case CTX_UCHAR:
      return 8;
   case CTX_DOUBLE:
      return 64;
    case CTX_LDOUBLE:
      return 128;
    case CTX_FLOAT:
      return 32;
    case CTX_INT:
      return 32;
    case CTX_LINT:
      return 64;
    case CTX_LLINT:
      return 128;
    case CTX_ULLINT:
      return 128;
    case CTX_SINT:
      return 8;
    case CTX_UINT:
      return 32;
    case CTX_ULINT:
      return 64;
    case CTX_USINT:
      return 8;
    case CTX_UNSIGNEDINTEGRAL:
      return 64;
    case CTX_SIGNEDINTEGRAL:
      return 64;
    case CTX_ANYINTEGRAL:
      return 64;
    default:
      return 0;
    }
}
