/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** fileloc.h
*/

# ifndef FILELOC_H
# define FILELOC_H

# include "fileId.h"

typedef enum 
{ 
  FL_NORMAL, FL_SPEC, FL_LIB, FL_STDLIB, FL_STDHDR, FL_IMPORT, FL_BUILTIN,
  FL_PREPROC, FL_RC, FL_EXTERNAL
} flkind;

struct s_fileloc
{
  flkind  kind;
  fileId  fid;
  int     lineno;
  int     column;
} ;

/* typedef in forwardTypes */
typedef /*@only@*/ fileloc o_fileloc;

extern fileloc fileloc_update (/*@only@*/ fileloc p_old, fileloc p_fnew) 
   /*@modifies p_old@*/ ;

extern fileloc fileloc_create (fileId p_fid, int p_line, int p_col) /*@*/ ;
extern bool fileloc_isSystemFile (fileloc p_f1) /*@*/ ;
extern bool fileloc_isXHFile (fileloc p_f1) /*@*/ ;

extern fileloc fileloc_createSpec (fileId p_fid, int p_line, int p_col) /*@*/ ;
extern fileloc fileloc_createLib (cstring p_ln) /*@*/ ;
extern fileloc fileloc_createRc (cstring p_name) /*@*/ ;
extern fileloc fileloc_decColumn (fileloc p_f, int p_x) /*@*/ ;
extern void fileloc_subColumn (fileloc p_f, int p_x) /*@modifies p_f@*/ ;
extern fileloc fileloc_getBuiltin (void) /*@*/ ;
extern /*@observer@*/ fileloc fileloc_observeBuiltin (void) /*@*/ ;
extern fileloc fileloc_createBuiltin (void) /*@*/ ;
extern fileloc fileloc_createImport (cstring p_fname, int p_lineno) /*@*/ ;

extern void fileloc_destroyMod (void) /*@modifies internalState@*/ ;
extern bool fileloc_isSpecialFile (fileloc p_f) /*@*/ ;
extern bool fileloc_sameBaseFile (fileloc p_f1, fileloc p_f2) /*@*/ ;
extern /*@observer@*/ cstring fileloc_filename (fileloc p_f) /*@*/ ;
extern int fileloc_column (fileloc p_f) /*@*/ ;
extern cstring fileloc_unparse (fileloc p_f) /*@*/ ;
extern cstring fileloc_unparseRaw (cstring p_fname, int p_lineno) /*@*/ ;
extern cstring fileloc_unparseRawCol (cstring p_fname, int p_lineno, int p_col) /*@*/ ;
extern bool fileloc_sameFile (fileloc p_f1, fileloc p_f2) /*@*/ ;
extern bool fileloc_sameFileAndLine (fileloc p_f1, fileloc p_f2) /*@*/ ;
extern void fileloc_free (/*@only@*/ fileloc p_f);
extern void fileloc_reallyFree (/*@only@*/ fileloc p_f);
extern int fileloc_lineno (fileloc p_f) /*@*/ ;
extern bool fileloc_equal (fileloc p_f1, fileloc p_f2) /*@*/ ;
extern bool fileloc_lessthan (fileloc p_f1, fileloc p_f2) /*@*/ ;
extern int fileloc_compare (fileloc p_f1, fileloc p_f2) /*@*/ ;
extern /*@observer@*/ cstring fileloc_getBase (fileloc p_f) /*@*/ ;
extern bool fileloc_isHeader (fileloc p_f) /*@*/ ;
extern bool fileloc_isSpec (fileloc p_f) /*@*/ ;
extern bool fileloc_isRealSpec (fileloc p_f) /*@*/ ;
extern fileloc fileloc_copy (fileloc p_f) /*@*/ ;
extern cstring fileloc_unparseDirect (fileloc p_fl) /*@*/ ;
extern bool fileloc_notAfter (fileloc p_f1, fileloc p_f2) /*@*/ ;
extern bool fileloc_almostSameFile (fileloc p_f1, fileloc p_f2) /*@*/ ;
extern fileloc fileloc_noColumn (fileloc p_f) /*@*/ ;
extern /*@observer@*/ fileloc fileloc_getExternal (void) /*@*/ ;
extern fileloc fileloc_createExternal (void) /*@*/ ;

extern bool fileloc_isExternal (/*@sef@*/ fileloc p_f) /*@*/;
# define fileloc_isExternal(f) \
     (fileloc_isDefined(f) && ((f)->kind == FL_EXTERNAL))

extern /*@falsewhennull@*/ bool fileloc_isDefined (/*@null@*/ fileloc p_f) /*@*/ ;
extern /*@nullwhentrue@*/ bool fileloc_isUndefined (/*@null@*/ fileloc p_f) /*@*/ ;
extern bool fileloc_isInvalid (/*@sef@*/ /*@null@*/ fileloc p_f) /*@*/ ;

/*@constant null fileloc fileloc_undefined; @*/
# define fileloc_undefined      ((fileloc) NULL)
# define fileloc_isDefined(f)   ((f) != fileloc_undefined)
# define fileloc_isUndefined(f) ((f) == fileloc_undefined)
# define fileloc_isInvalid(f)   (!(fileloc_isValid(f)))

extern bool fileloc_isLib (fileloc p_f) /*@*/ ;

extern fileloc fileloc_fromTok (ltoken p_t) /*@*/ ;

/*@constant int UNKNOWN_LINE; @*/
# define UNKNOWN_LINE (0)

/*@constant int UNKNOWN_COLUMN; @*/
# define UNKNOWN_COLUMN (0)

extern /*@unused@*/ bool fileloc_linenoDefined (/*@sef@*/ fileloc p_f) /*@*/ ;
extern /*@unused@*/ bool fileloc_columnDefined (/*@sef@*/ fileloc p_f) /*@*/ ;

# define fileloc_linenoDefined(f) \
  (fileloc_isValid (f) && (f)->lineno != UNKNOWN_LINE)

# define fileloc_columnDefined(f) \
  (fileloc_isValid (f) && (f)->column != UNKNOWN_COLUMN)

extern void fileloc_setColumnUndefined (/*@sef@*/ fileloc p_f) /*@modifies p_f@*/;
# define fileloc_setColumnUndefined(f) \
  (fileloc_isDefined(f) ? (f)->column = UNKNOWN_COLUMN : UNKNOWN_COLUMN)

extern /*@falsewhennull@*/ bool fileloc_isValid (/*@sef@*/ fileloc p_f);
# define fileloc_isValid(f) \
  (fileloc_isDefined(f) && ((f)->lineno >= 0))

extern bool fileloc_isImport (/*@sef@*/ fileloc p_fl);
# define fileloc_isImport(fl)  (fileloc_isDefined(fl) && (fl)->kind == FL_IMPORT)

extern bool fileloc_isPreproc (/*@sef@*/ fileloc p_fl);
# define fileloc_isPreproc(fl)  (fileloc_isDefined(fl) && (fl)->kind == FL_PREPROC)

extern void fileloc_setLineno (/*@sef@*/ fileloc p_f, int p_i) /*@modifies p_f@*/ ;
# define fileloc_setLineno(fl, i) \
      (fileloc_isDefined(fl) ? ((fl)->lineno = (i), (fl)->column = 1) : (i))

extern void fileloc_nextLine (/*@sef@*/ fileloc p_f) /*@modifies p_f@*/ ;
# define fileloc_nextLine(fl) \
      (fileloc_isDefined(fl) ? ((fl)->lineno++, (fl)->column = 1) : 0)

extern void fileloc_addLine (/*@sef@*/ fileloc p_f, int p_i) /*@modifies p_f@*/ ;
# define fileloc_addLine(fl, i) \
      (fileloc_isDefined(fl) ? ((fl)->lineno += (i), (fl)->column = 1) : (i))

extern fileId fileloc_fileId (/*@sef@*/ fileloc p_fl) /*@*/ ;
# define fileloc_fileId(fl) \
      (fileloc_isDefined (fl) ? (fl)->fid : fileId_invalid)

extern void fileloc_setColumn (/*@sef@*/ fileloc p_fl, int p_i) /*@modifies p_fl@*/ ;
# define fileloc_setColumn(fl, i) \
      (fileloc_isDefined (fl) ? ((fl)->column = (i)) : (i))

extern void fileloc_addColumn (/*@sef@*/ fileloc p_fl, int p_i) /*@modifies p_fl@*/ ;
# define fileloc_addColumn(fl, i) \
      (fileloc_isDefined (fl) ? ((fl)->column += (i)) : (i))

extern void fileloc_incColumn (/*@sef@*/ fileloc p_fl) /*@modifies p_fl@*/ ;
# define fileloc_incColumn(fl) \
      (fileloc_isDefined (fl) ? ((fl)->column++) : 0)

extern bool fileloc_isBuiltin (/*@sef@*/ fileloc p_fl) /*@*/ ;
# define fileloc_isBuiltin(fl) \
   (fileloc_isDefined (fl) && ((fl)->kind == FL_BUILTIN))

extern fileloc fileloc_updateFileId (/*@only@*/ fileloc p_old, fileId p_s) /*@*/ ;
extern fileloc fileloc_makePreproc (fileloc p_loc) /*@*/ ;
extern fileloc fileloc_makePreprocPrevious (fileloc p_loc) /*@*/ ;

extern /*@only@*/ cstring fileloc_outputFilename (fileloc) /*@*/ ;

extern bool fileloc_isStandardLibrary (fileloc p_f) /*@*/ ;
extern bool fileloc_isStandardLib (fileloc p_f) /*@*/ ;

extern cstring fileloc_unparseFilename (fileloc p_f) /*@*/ ;
extern bool fileloc_withinLines (fileloc p_f1, fileloc p_f2, int p_n) /*@*/ ;
extern bool fileloc_isUser (fileloc p_f) /*@*/ ;
extern bool fileloc_sameModule (fileloc p_f1, fileloc p_f2) /*@*/ ;

extern bool fileloc_storable (/*@sef@*/ fileloc p_f) /*@*/;
# define fileloc_storable(f) ((f) != g_currentloc || fileloc_isBuiltin (f) || fileloc_isExternal (f))


# else
# error "Multiple include"
# endif






