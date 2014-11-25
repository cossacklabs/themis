/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** globalsClause.h
*/

# ifndef GLOBALSCLAUSE_H
# define GLOBALSCLAUSE_H

struct s_globalsClause {
  globSet globs;
  fileloc loc;
} ;

extern globalsClause globalsClause_create (/*@only@*/ lltok, /*@only@*/ globSet) /*@*/ ;
extern /*@observer@*/ globSet globalsClause_getGlobs (globalsClause) /*@*/ ;
extern /*@only@*/ globSet globalsClause_takeGlobs (globalsClause p_gclause) /*@modifies p_gclause@*/ ;

extern /*@observer@*/ fileloc globalsClause_getLoc (globalsClause) /*@*/ ;
# define globalsClause_getLoc(gl) ((gl)->loc)

extern cstring globalsClause_unparse (globalsClause p_node) /*@*/ ;
extern void globalsClause_free (/*@only@*/ globalsClause) ;

# else
# error "Multiple include"
# endif
