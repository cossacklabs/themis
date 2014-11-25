/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** invariant: line1 <= line2
**            if line1 == line2, col1 <= col2.
*/

# ifndef FLAGMARKER_H
# define FLAGMARKER_H

typedef enum
{
  FMK_LOCALSET,
  FMK_IGNOREON,
  FMK_IGNORECOUNT,
  FMK_IGNOREOFF,
  FMK_SUPPRESS
} flagMarkerKind ;

typedef struct
{
  flagMarkerKind kind;
  flagcode code;
  
  /*@reldef@*/ union {
    ynm set;
    int nerrors;
  } info;

  fileloc loc;
} *flagMarker ;

extern bool flagMarker_isLocalSet (flagMarker p_c) /*@*/ ;
# define flagMarker_isLocalSet(c) ((c)->kind == FMK_LOCALSET)

extern bool flagMarker_isSuppress (flagMarker p_c) /*@*/ ;
# define flagMarker_isSuppress(c) ((c)->kind == FMK_SUPPRESS)

extern bool flagMarker_isIgnoreOn (flagMarker p_c) /*@*/ ;
# define flagMarker_isIgnoreOn(c) ((c)->kind == FMK_IGNOREON)

extern bool flagMarker_isIgnoreOff (flagMarker p_c) /*@*/ ;
# define flagMarker_isIgnoreOff(c) ((c)->kind == FMK_IGNOREOFF)

extern bool flagMarker_isIgnoreCount (flagMarker p_c) /*@*/ ;
# define flagMarker_isIgnoreCount(c) ((c)->kind == FMK_IGNORECOUNT)

extern flagMarker
  flagMarker_createLocalSet (flagcode p_code, ynm p_set, fileloc p_loc) /*@*/ ;

extern flagMarker
  flagMarker_createIgnoreOn (fileloc p_loc) /*@*/ ;

extern flagMarker
  flagMarker_createIgnoreOff (fileloc p_loc) /*@*/ ;

extern flagMarker
  flagMarker_createIgnoreCount (int p_count, fileloc p_loc) /*@*/ ;

extern flagMarker
  flagMarker_createSuppress (flagcode p_code, fileloc p_loc) /*@*/ ;

extern void flagMarker_free (/*@only@*/ flagMarker p_c) ;

extern bool flagMarker_sameFile (flagMarker p_c, fileloc p_loc) /*@*/ ;
extern /*@only@*/ cstring flagMarker_unparse (flagMarker p_c) /*@*/ ;

extern bool flagMarker_beforeMarker (flagMarker p_c, fileloc p_loc) /*@*/ ;

extern bool flagMarker_equal (flagMarker p_f1, flagMarker p_f2) /*@*/ ;

extern ynm flagMarker_getSet (flagMarker p_f) /*@*/ ; 
extern flagcode flagMarker_getCode (flagMarker p_f) /*@*/ ; 
extern int flagMarker_getCount (flagMarker p_f) /*@*/ ; 

extern /*@observer@*/ fileloc flagMarker_getLoc (flagMarker p_f) /*@*/ ;  

# define flagMarker_getLoc(f)  ((f)->loc)

# else
# error "Multiple include"
# endif
