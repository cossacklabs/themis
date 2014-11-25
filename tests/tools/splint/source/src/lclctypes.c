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
** lclctypes.c
**
** This contains definitions used in processing C builtin types.
**
** WARNING: there is a different file ctype.c for handling types
**          in the C checking of splint.  This is only for handling
**          C builtin types in LCL files.
**
**  AUTHORS:
**
**      Gary Feldman, Technical Languages and Environements, DECspec project
**	Joe Wild, Technical Languages and Environements, DECspec project
**
*/

# include "splintMacros.nf"
# include "basic.h"

typedef struct
{
  bits pt;
  TypeSpec ts;
} Lclctype2sortType;

static Lclctype2sortType lclctype2type[] =
{
  { fixBits (TS_VOID, 0), TYS_VOID},
  { fixBits (TS_UNKNOWN, 0), TYS_NONE},
  { fixBits (TS_CHAR, 0), TYS_CHAR},
  { fixBits (TS_SIGNED, fixBits (TS_CHAR, 0)), TYS_SCHAR},
  { fixBits (TS_UNSIGNED, fixBits (TS_CHAR, 0)), TYS_UCHAR},

  { fixBits (TS_SIGNED, fixBits (TS_SHORT, fixBits (TS_INT, 0))), TYS_SSINT},
  { fixBits (TS_SIGNED, fixBits (TS_SHORT, 0)), TYS_SSINT},
  { fixBits (TS_SHORT, fixBits (TS_INT, 0)), TYS_SSINT},
  { fixBits (TS_SHORT, 0), TYS_SSINT},

  { fixBits (TS_UNSIGNED, fixBits (TS_SHORT, fixBits (TS_INT, 0))), TYS_USINT},
  { fixBits (TS_UNSIGNED, fixBits (TS_SHORT, 0)), TYS_USINT},

  { fixBits (TS_SIGNED, fixBits (TS_INT, 0)), TYS_SINT},
  { fixBits (TS_SIGNED, 0), TYS_SINT},

  { fixBits (TS_INT, 0), TYS_INT},
  { 0, TYS_INT},

  { fixBits (TS_UNSIGNED, fixBits (TS_INT, 0)), TYS_UINT},
  { fixBits (TS_UNSIGNED, 0), TYS_UINT},

  { fixBits (TS_SIGNED, fixBits (TS_LONG, fixBits (TS_INT, 0))), TYS_SLINT},
  { fixBits (TS_SIGNED, fixBits (TS_LONG, 0)), TYS_SLINT},
  { fixBits (TS_LONG, fixBits (TS_INT, 0)), TYS_SLINT},
  { fixBits (TS_LONG, 0), TYS_SLINT},

  { fixBits (TS_UNSIGNED, fixBits (TS_LONG, fixBits (TS_INT, 0))), TYS_ULINT},
  { fixBits (TS_UNSIGNED, fixBits (TS_LONG, 0)), TYS_ULINT},

  { fixBits (TS_FLOAT, 0), TYS_FLOAT},
  { fixBits (TS_DOUBLE, 0), TYS_DOUBLE},
  { fixBits (TS_LONG, fixBits (TS_DOUBLE, 0)), TYS_LDOUBLE},

  { fixBits (TS_STRUCT, 0), TYS_STRUCT},
  { fixBits (TS_UNION, 0), TYS_UNION},
  { fixBits (TS_ENUM, 0), TYS_ENUM},
  { fixBits (TS_TYPEDEF, 0), TYS_TYPENAME}
};

lsymbol
lclctype_toSortDebug (bits t)
{
  int i;
  int lsize;
  static ob_mstring OLD_Type2sortName[] =
    {
      "error",			/* TYS_NONE 	*/
      "void",			/* TYS_VOID  	*/
      "char",			/* TYS_CHAR  	*/
      "signed_char",		/* TYS_SCHAR 	*/
      "char",			/* TYS_UCHAR 	*/
      "short_int",		/* TYS_SSINT	*/
      "unsigned_short_int",	/* TYS_USINT	*/
      "int",			/* TYS_INT        */
      "int",			/* TYS_SINT 	*/
      "unsigned_int",		/* TYS_UINT 	*/
      "long_int",		/* TYS_SLINT	*/
      "unsigned_long_int",	/* TYS_ULINT	*/
      "float",			/* TYS_FLOAT	*/
      "double",		        /* TYS_DOUBLE	*/
      "long_double",		/* TYS_LDOUBLE 	*/
      "error",			/* TYS_ENUM 	*/
      "error",			/* TYS_STRUCT 	*/
      "error",			/* TYS_UNION 	*/
      "error"			/* TYS_TYPENAME */
      };

  lsize = size_toInt (sizeof (lclctype2type) / sizeof (lclctype2type[0]));

  for (i = 0; i < lsize; i++)
    {
      if (lclctype2type[i].pt == t)
	{
	  return lsymbol_fromChars (OLD_Type2sortName[(int)lclctype2type[i].ts]);
	}
    }
  return lsymbol_fromChars ("_error");
}


lsymbol
lclctype_toSort (bits t)
{
  int i;
  static ob_mstring Type2sortName[] =
    {
      /* _error must have underscore, LSL/LCL interface convention */
      "_error",		        /* TYS_NONE 	*/
      "void",			/* TYS_VOID  	*/
      "char",			/* TYS_CHAR  	*/
      "char",			/* TYS_SCHAR 	*/
      "char",			/* TYS_UCHAR 	*/
      "int",			/* TYS_SSINT	*/
      "int",			/* TYS_USINT	*/
      "int",			/* TYS_INT	*/
      "int",			/* TYS_SINT 	*/
      "int",			/* TYS_UINT 	*/
      "int",			/* TYS_SLINT	*/
      "int",			/* TYS_ULINT	*/
      "double",		        /* TYS_FLOAT	*/
      "double",		        /* TYS_DOUBLE	*/
      "double",		        /* TYS_LDOUBLE 	*/
      "error",			/* TYS_ENUM 	*/
      "error",			/* TYS_STRUCT 	*/
      "error",			/* TYS_UNION 	*/
      "error"			/* TYS_TYPENAME 	*/
      };

  int lsize = size_toInt (sizeof (lclctype2type) / sizeof (lclctype2type[0]));

  for (i = 0; i < lsize; i++)
    {
      if (lclctype2type[i].pt == t)
	{
	  return lsymbol_fromChars (Type2sortName[(int)lclctype2type[i].ts]);
	}
    }

  return lsymbol_fromChars ("_error");
}
