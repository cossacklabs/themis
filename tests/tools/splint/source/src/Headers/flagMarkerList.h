/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** flagMarkerList.h
*/

# ifndef flagMarkerList_H
# define flagMarkerList_H

typedef /*@only@*/ flagMarker o_flagMarker;

abst_typedef struct
{
  int nelements;
  int nspace;
  /*@reldef@*/ /*@relnull@*/ o_flagMarker  *elements;
} *flagMarkerList ;

extern /*@only@*/ flagMarkerList flagMarkerList_new (void) /*@*/ ;

extern /*@unused@*/ /*@only@*/ cstring
  flagMarkerList_unparse (flagMarkerList p_s) /*@*/ ;
extern void flagMarkerList_free (/*@only@*/ flagMarkerList p_s) ;

extern bool flagMarkerList_add (flagMarkerList p_s, /*@only@*/ flagMarker p_fm)
     /*@modifies p_s@*/ ;

extern ynm 
  flagMarkerList_suppressError (flagMarkerList p_s, flagcode p_code, fileloc p_loc) /*@*/ ;

extern void flagMarkerList_checkSuppressCounts (flagMarkerList p_s) 
   /*@modifies g_warningstream@*/ ;

extern bool
  flagMarkerList_inIgnore (flagMarkerList p_s, fileloc p_loc) /*@*/ ;

/*@constant int flagMarkerListBASESIZE;@*/
# define flagMarkerListBASESIZE SMALLBASESIZE

# else
# error "Multiple include"
# endif




