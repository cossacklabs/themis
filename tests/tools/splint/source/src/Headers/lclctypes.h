/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
# ifndef LCLCTYPESX_H
# define LCLCTYPESX_H

/*
** TypeSpecification is an enum defined for each of the valid C type.
*/

typedef enum 
{
  TS_UNKNOWN, TS_VOID, TS_CHAR, TS_INT,
  TS_SIGNED, TS_UNSIGNED, TS_SHORT, TS_LONG,
  TS_FLOAT,  TS_DOUBLE, TS_ENUM, TS_STRUCT, 
  TS_UNION, TS_TYPEDEF
} TypeSpecification;

/*
** TypeSpec is an enum for each of the valid combinations of type specifiers.
*/

typedef enum {
  TYS_NONE,				
  TYS_VOID,				
  TYS_CHAR, TYS_SCHAR, TYS_UCHAR,	
  TYS_SSINT, TYS_USINT,			
  TYS_INT, TYS_SINT, TYS_UINT,		
  TYS_SLINT, TYS_ULINT,			
  TYS_FLOAT, TYS_DOUBLE, TYS_LDOUBLE,	
  TYS_ENUM, TYS_STRUCT, TYS_UNION,	
  TYS_TYPENAME				
  } TypeSpec;

typedef bits lclctype;

extern lclctype fixBits(TypeSpecification p_attr, lclctype p_spec);
# define fixBits(attr, spec)   ((unsigned)(1<<(attr) | (spec)))

extern lsymbol lclctype_toSort(lclctype p_t);
extern lsymbol lclctype_toSortDebug(lclctype p_t);

# else
# error "Multiple include"
# endif
