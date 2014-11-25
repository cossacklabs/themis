/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/
# ifndef POINTERS_H
# define POINTERS_H

struct s_pointers {
  qualList quals;
  /*@null@*/ pointers rest;
} ;

/*@constant null pointers pointers_undefined; @*/
# define pointers_undefined ((pointers) NULL)

extern /*@nullwhentrue@*/ bool pointers_isUndefined (/*@special@*/ pointers p_e) /*@*/ ;
extern /*@falsewhennull@*/ bool pointers_isDefined (/*@special@*/ pointers p_e) /*@*/ ;

# define pointers_isUndefined(e) ((e) == pointers_undefined)
# define pointers_isDefined(e)   ((e) != pointers_undefined)

extern pointers pointers_create (lltok p_tok) /*@*/ ;
extern pointers pointers_createMods (lltok p_tok, /*@only@*/ qualList p_quals) /*@*/ ;
extern pointers pointers_createMt (mttok p_tok) /*@*/ ;
extern pointers pointers_createLt (ltoken p_tok) /*@*/ ;
extern pointers pointers_createModsLt (ltoken p_tok, /*@only@*/ qualList p_quals) /*@*/ ;
extern pointers pointers_createModsMt (mttok p_tok, /*@only@*/ qualList p_quals) /*@*/ ;
extern pointers pointers_extend (/*@returned@*/ pointers p_p1, /*@only@*/ pointers p_p2) /*@modifies p_p1@*/ ;
extern /*@observer@*/ pointers pointers_getRest (pointers) /*@*/ ;

extern int pointers_depth (pointers p_p) /*@*/ ;
extern cstring pointers_unparse (pointers) /*@*/ ;
extern void pointers_free (/*@only@*/ pointers) ;

# else
# error "Multiple include"
# endif



