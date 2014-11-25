/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** fileLib.h
*/

# ifndef FILELIB_H
# define FILELIB_H

extern bool fileLib_isLCLFile (cstring p_s) /*@*/ ;
extern bool fileLib_isCExtension (cstring p_ext) /*@*/ ;

extern cstring fileLib_addExtension (cstring p_s, cstring p_suffix);
extern cstring fileLib_withoutExtension (cstring p_s, cstring p_suffix) /*@*/ ;

extern cstring fileLib_removePath (cstring p_s) /*@*/ ;
extern cstring fileLib_removePathFree (/*@only@*/ cstring p_s) /*@*/ ;
extern cstring fileLib_removeAnyExtension (cstring p_s) /*@*/ ;
extern /*@only@*/ cstring fileLib_cleanName (/*@only@*/ cstring p_s) /*@*/ ;
extern bool fileLib_hasExtension (cstring p_s, cstring p_ext) /*@*/ ;

extern /*@observer@*/ cstring
  fileLib_getExtension (/*@returned@*/ cstring p_s) /*@*/ ;

/*@constant observer cstring MTS_EXTENSION;@*/
# define MTS_EXTENSION cstring_makeLiteralTemp (".mts")

/*@constant observer cstring LCL_EXTENSION;@*/
# define LCL_EXTENSION cstring_makeLiteralTemp (".lcl")

/*@constant observer cstring LH_EXTENSION; @*/
# define LH_EXTENSION cstring_makeLiteralTemp (".lh")

/*@constant observer cstring C_EXTENSION; @*/
# define C_EXTENSION cstring_makeLiteralTemp (".c")

/*@constant observer cstring XH_EXTENSION;@*/
# define XH_EXTENSION cstring_makeLiteralTemp (".xh")

/* C files that have already been preprocessed */
/*@constant observer cstring PP_EXTENSION;@*/
# define PP_EXTENSION cstring_makeLiteralTemp (".pp")

# endif
