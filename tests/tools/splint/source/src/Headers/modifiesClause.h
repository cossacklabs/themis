/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** modifiesClause.h
*/

# ifndef MODIFIESCLAUSE_H
# define MODIFIESCLAUSE_H

struct s_modifiesClause {
  bool isnomods;
  fileloc loc;
  sRefSet srs;
} ;

extern modifiesClause modifiesClause_createNoMods (/*@only@*/ lltok) /*@*/ ;

extern bool modifiesClause_isNoMods (modifiesClause) ;
# define modifiesClause_isNoMods(m) ((m)->isnomods)

extern /*@observer@*/ sRefSet modifiesClause_getMods (modifiesClause) ;
extern /*@only@*/ sRefSet modifiesClause_takeMods (modifiesClause) ;

extern /*@observer@*/ fileloc modifiesClause_getLoc (modifiesClause) /*@*/ ;
# define modifiesClause_getLoc(gl) ((gl)->loc)

extern modifiesClause 
   modifiesClause_create (/*@only@*/ lltok, /*@only@*/ sRefSet) /*@*/ ;

extern cstring modifiesClause_unparse (modifiesClause p_node) /*@*/ ;
extern void modifiesClause_free (/*@only@*/ modifiesClause) ;

# else
# error "Multiple include"
# endif
