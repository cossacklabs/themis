/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** sort.h
*/

# ifndef sort_H
# define sort_H

/*@constant observer char *BEGINSORTTABLE;@*/
# define BEGINSORTTABLE		"%LCLSortTable"

/*@constant observer char *SORTTABLEEND;@*/
# define SORTTABLEEND		"%LCLSortTableEnd"

typedef enum {
  SRT_FIRST,
  SRT_NONE,        
  SRT_HOF,        
  SRT_PRIM,
  SRT_SYN,
  SRT_PTR,
  SRT_OBJ,
  SRT_ARRAY,
  SRT_VECTOR,
  SRT_STRUCT,
  SRT_TUPLE,
  SRT_UNION,
  SRT_UNIONVAL,
  SRT_ENUM,
  SRT_LAST
} sortKind;

typedef struct s_smemberInfo {
    lsymbol name;
    sort sort;
    /*@reldef@*/ lsymbol sortname; /* useful for sort_imports (yuk!) */
    /*@null@*/ /*@dependent@*/ struct s_smemberInfo *next;
} smemberInfo;

/*@constant null smemberInfo *smemberInfo_undefined; @*/
# define smemberInfo_undefined 	((smemberInfo *)NULL)

typedef struct 
{
  sortKind	kind;  

  /* Handle for this sort in the table of known sorts */
  sort          handle; 

  /*
  ** if SRT_PRIM then name is gotten from LSL traits.
  **  elseif SRT_SYN then it is derived from a user-given typdef name, add _.
  **  elseif SRT_NONE then may contain "_error".
  **  elseif keeps the name of this sort (unique) 
  */
  
  lsymbol	name;    

  /* 
  ** if {STRUCT, UNION, ENUM} and name field is non-nil
  ** then it is the tag name.  Kept to make printing and 
  ** debugging easier, for now.  
  */
  
  /*@reldef@*/ lsymbol tag;
  /*@reldef@*/ bool realtag; 

  /* 
  ** if SRT_SYN then keeps handle for synonym sort,  
  ** elseif {OBJ, PTR, ARRAY, VECTOR} then keeps the element sort.
  ** elseif {TUPLE, UNIONVAL} then keeps the baseSort which is a
  ** record or union sort. 
  */

  sort          baseSort;   

  /*
  ** Only for VECTOR sort, gives its array sort, 
  ** not used otherwise. 
  */

  /*@reldef@*/ sort   objSort; 

  /* First member of struct, union, or enum */
  /*@null@*/ smemberInfo *members;   

  bool export;   /* TRUE if sort is exported by this spec */
  bool mutable;  /* TRUE if sort represents a mutable sort */
  bool abstract; /* TRUE if sort represents an LCL abstract type */
  bool imported; /* TRUE if sort was imported */
} *sortNode;

extern cstring sort_unparse (sort p_s) /*@*/ ;
extern /*@exposed@*/ cstring sort_unparseName (sort p_s) /*@*/ ;
extern sort sort_makeSort (ltoken p_t, lsymbol p_n) /*@*/ ;
extern sort sort_makeSyn (ltoken p_t, sort p_s, lsymbol p_n) /*@*/ ;
extern sort sort_makeFormal (sort p_insort) /*@*/ ;
extern sort sort_makeGlobal (sort p_insort) /*@*/ ;
extern sort sort_makePtr (ltoken p_t, sort p_baseSort) /*@*/ ;
extern sort sort_makePtrN (sort p_s, pointers p_p) /*@*/ ;
extern sort sort_makeVal (sort p_sor) /*@*/ ;
extern sort sort_makeObj (sort p_sor) /*@*/ ;

extern void sort_destroyMod (void) /*@modifies internalState@*/ ;

extern sort sort_makeArr (ltoken p_t, sort p_baseSort) /*@*/ ;
extern sort sort_makeVec (ltoken p_t, sort p_arraySort) /*@*/ ;
extern sort sort_makeMutable (ltoken p_t, lsymbol p_name) /*@*/ ;
extern sort sort_makeImmutable (ltoken p_t, lsymbol p_name) /*@*/ ;

extern sort sort_makeStr (ltoken p_opttagid) /*@*/ ;
extern sort sort_makeUnion (ltoken p_opttagid) /*@*/ ;
extern sort sort_makeEnum (ltoken p_opttagid) /*@*/ ;

extern bool 
  sort_updateStr (sort p_strSort, /*@null@*/ /*@only@*/ smemberInfo *p_info)
  /*@modifies internalState@*/ ;
extern bool
  sort_updateUnion (sort p_unionSort, /*@null@*/ /*@only@*/ smemberInfo *p_info)
  /*@modifies internalState@*/ ;
extern bool 
  sort_updateEnum (sort p_enumSort, /*@null@*/ /*@only@*/ smemberInfo *p_info)
  /*@modifies internalState@*/ ;

extern sort sort_makeTuple (ltoken p_t, sort p_strSort) /*@modifies internalState@*/ ;
extern sort sort_makeUnionVal (ltoken p_t, sort p_unionSort) /*@modifies internalState@*/ ;

extern lsymbol sort_getLsymbol (sort p_sor) /*@*/ ;
extern /*@observer@*/ char *sort_getName (sort p_s) /*@*/ ;
extern /*@observer@*/ sortNode sort_lookup (sort p_sor) /*@*/ ; 
extern /*@observer@*/ sortNode sort_quietLookup (sort p_sor) /*@*/ ;
extern sort sort_lookupName (lsymbol p_name) /*@*/ ;
extern void sort_dump(FILE *p_f, bool p_lco) /*@modifies p_f@*/ ;
extern void sort_init(void) /*@modifies internalState@*/ ;

extern bool sort_compatible(sort p_s1, sort p_s2) /*@*/ ;
extern bool sort_compatible_modulo_cstring(sort p_s1, sort p_s2) /*@*/ ;
extern sort sort_getUnderlying (sort p_s) /*@*/ ;
extern bool sort_mutable (sort p_s) /*@*/ ;
extern sort sort_makeNoSort(void) /*@modifies internalState@*/ ;

extern sort sort_makeHOFSort(sort p_base) /*@*/ ; 
extern bool sort_isHOFSortKind(sort p_s) /*@*/ ; 
extern bool sort_isNoSort(sort p_s) /*@*/ ;
extern bool sort_isValidSort(sort p_s) /*@*/ ;
extern bool sort_setExporting (bool p_flag) /*@modifies internalState@*/ ;

# define sort_isNoSort(s)       ((s) == 0)
/* assume NOSORTHANDLE is #define to 0 in sort.c */

extern /*@unused@*/ void sort_printStats(void) /*@modifies g_warningstream@*/ ;

extern bool sort_equal (sort p_s1, sort p_s2) /*@*/ ;  
extern sort sort_fromLsymbol (lsymbol p_sortid) /*@modifies internalState@*/ ;

extern void sort_import (inputStream p_imported, ltoken p_tok, mapping p_map)
   /*@modifies p_imported, internalState@*/ ;

extern sort g_sortBool;
extern sort g_sortCapBool;
extern sort g_sortInt;
extern sort g_sortChar;
extern sort g_sortCstring;
extern sort g_sortFloat;
extern sort g_sortDouble;

# else
# error "Multiple include"
# endif


