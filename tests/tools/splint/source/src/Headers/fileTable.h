/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** fileTable.h
*/

# ifndef FILETABLE_H
# define FILETABLE_H

/*@constant int FTBASESIZE; @*/
# define FTBASESIZE 64

/* moved to fileloc.h
**    typedef int fileId;
*/

typedef enum { FILE_NORMAL, FILE_LSLTEMP, FILE_NODELETE,
	       FILE_HEADER, FILE_XH, FILE_MACROS, FILE_METASTATE } fileType;

/*:private:*/ typedef struct
{
  bool     ftemp     BOOLBITS;
  bool     fsystem   BOOLBITS;
  bool     fspecial  BOOLBITS;
  /*@only@*/ cstring fname;
  cstring  basename;
  fileType ftype;
  fileId   fder;
} *ftentry;

typedef /*@only@*/ ftentry o_ftentry;

typedef /*@only@*/ struct {
  /*@null@*/ /*@dependent@*/ /*@exposed@*/ FILE *f;
  /*@only@*/ cstring fname;
} *foentry;

abst_typedef /*@null@*/ struct
{
  int nentries;
  int nspace;
  cstringTable htable;
  /*@reldef@*/ /*@only@*/ o_ftentry *elements;

  /*
  ** Keep track of all the open files, so we can close them on error exits.
  */

  int nopen;
  int nopenspace;
  /*@reldef@*/ /*@only@*/ foentry *openelements; /* evans 2002-07-12: removed reldef */
} *fileTable ;

/*@constant null fileTable fileTable_undefined; @*/
# define fileTable_undefined ((fileTable) NULL)

extern /*@unused@*/ /*@nullwhentrue@*/ bool 
  fileTable_isUndefined (/*@null@*/ fileTable p_f) /*@*/ ;
extern /*@unused@*/ /*@falsewhennull@*/ bool 
  fileTable_isDefined (/*@null@*/ fileTable p_f) /*@*/ ;

# define fileTable_isUndefined(ft) ((ft) == fileTable_undefined)
# define fileTable_isDefined(ft) ((ft) != fileTable_undefined)
extern /*@observer@*/ cstring fileTable_getName (fileTable p_ft, fileId p_fid) /*@*/ ; 
extern /*@observer@*/ cstring fileTable_getNameBase (fileTable p_ft, fileId p_fid) ; 
extern fileId fileTable_addFile (fileTable p_ft, cstring p_name)
   /*@modifies p_ft@*/ ;

extern void fileTable_addStreamFile (fileTable p_ft, /*@dependent@*/ FILE *p_fstream,
				     cstring p_name)
   /*@modifies p_ft@*/ ;

extern fileId fileTable_addHeaderFile (fileTable p_ft, cstring p_name)
   /*@modifies p_ft@*/ ;
extern fileId fileTable_addXHFile (fileTable p_ft, cstring p_name)
   /*@modifies p_ft@*/ ;
extern fileId fileTable_addLibraryFile (fileTable p_ft, cstring p_name)
   /*@modifies p_ft@*/ ;

extern fileId fileTable_addLCLFile (fileTable p_ft, cstring p_name)
   /*@modifies p_ft@*/ ;

extern fileId fileTable_addltemp (fileTable p_ft);

extern /*@notnull@*/ /*@only@*/ fileTable fileTable_create (void) /*@*/ ;
extern fileId fileTable_lookup (fileTable p_ft, cstring p_s) /*@*/ ;
extern fileId fileTable_addCTempFile (fileTable p_ft, fileId p_fid)
   /*@modifies p_ft@*/ ;
extern fileId fileTable_addFileOnly (fileTable p_ft, /*@only@*/ cstring p_name)
   /*@modifies p_ft@*/ ;

extern fileId fileTable_addImportFile (fileTable p_ft, cstring p_name)
   /*@modifies p_ft@*/ ;

extern fileId fileTable_addMacrosFile (fileTable p_ft)
   /*@modifies p_ft@*/ ;

extern fileId fileTable_addMetastateFile (fileTable p_ft, cstring p_name)
   /*@modifies p_ft@*/ ;

extern void fileTable_setFilePath (fileTable p_ft, fileId p_fid, cstring p_path) 
     /*@modifies p_ft@*/ ;

extern /*@observer@*/ cstring fileTable_getRootName (fileTable p_ft, fileId p_fid) /*@*/ ;
extern bool fileTable_isHeader       (fileTable p_ft, fileId p_fid) /*@*/ ;
extern bool fileId_isHeader (fileId p_f) /*@*/ ;
# define fileId_isHeader(f)          (fileTable_isHeader (context_fileTable(), f))

extern bool fileTable_sameBase (fileTable p_ft, fileId p_f1, fileId p_f2);
extern void fileTable_cleanup (fileTable p_ft) /*@modifies fileSystem@*/;
extern fileId fileTable_lookupBase (fileTable p_ft, cstring p_base) /*@modifies p_ft@*/ ;
extern void fileTable_printTemps (fileTable p_ft) /*@modifies g_warningstream@*/ ;
extern /*@unused@*/ /*@only@*/ cstring fileTable_unparse (fileTable p_ft) /*@*/ ;
extern bool fileTable_exists (fileTable p_ft, cstring p_s) /*@*/ ;
extern void fileTable_free (/*@only@*/ fileTable p_f);
extern bool fileTable_isSpecialFile (fileTable p_ft, fileId p_fid) /*@*/ ;
extern bool fileTable_isSystemFile (fileTable p_ft, fileId p_fid) /*@*/ ;
extern bool fileTable_isXHFile (fileTable p_ft, fileId p_fid) /*@*/ ;

extern /*@observer@*/ cstring fileTable_fileName (fileId p_fid) /*@*/ ;
extern /*@observer@*/ cstring fileTable_fileNameBase (fileId p_fid) /*@*/ ;
extern /*@observer@*/ cstring fileTable_rootFileName (fileId p_fid) /*@*/ ;

extern /*@null@*/ /*@open@*/ /*@dependent@*/ FILE *fileTable_createFile (fileTable p_ft, cstring p_fname) 
     /*@modifies p_ft, fileSystem@*/ ;

extern /*@null@*/ /*@open@*/ /*@dependent@*/ FILE *fileTable_createMacrosFile (fileTable p_ft, cstring p_fname) 
     /*@modifies p_ft, fileSystem@*/ ;

# define fileTable_fileName(fid)     (fileTable_getName(context_fileTable(), fid))
# define fileTable_fileNameBase(fid) (fileTable_getNameBase(context_fileTable(), fid))
# define fileTable_rootFileName(fid) (fileTable_getRootName(context_fileTable(), fid))

extern void fileTable_noDelete (fileTable, cstring);
extern bool fileId_baseEqual (/*@sef@*/ fileId p_t1, /*@sef@*/ fileId p_t2) /*@*/ ;
# define fileId_baseEqual(t1,t2) \
  (fileId_equal (t1, t2) || fileTable_sameBase (context_fileTable (), t1, t2))

extern /*@null@*/ /*@open@*/ /*@dependent@*/ FILE *
fileTable_openReadFile (fileTable p_ft, cstring p_fname) 
     /*@modifies p_ft@*/ ;

extern /*@null@*/ /*@open@*/ /*@dependent@*/ FILE *
fileTable_openWriteFile (fileTable p_ft, cstring p_fname) 
     /*@modifies p_ft@*/ ;

extern /*@null@*/ /*@open@*/ /*@dependent@*/ FILE *
fileTable_openWriteUpdateFile (fileTable p_ft, cstring p_fname) 
     /*@modifies p_ft@*/ ;

extern bool fileTable_closeFile (fileTable p_ft, FILE *p_f) 
   /*@ensures closed p_f@*/ 
   /*@modifies p_ft, p_f@*/ ;

extern void fileTable_closeAll (fileTable p_ft)
  /*@modifies p_ft@*/ ;

# else
# error "Multiple include"
#endif

