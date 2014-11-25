/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** mtDeclarationPieces.h
*/

# ifndef mtDeclarationPieces_H
# define mtDeclarationPieces_H

struct s_mtDeclarationPieces {
  mtDeclarationPiece thisPiece;
  /*@null@*/ mtDeclarationPieces rest;
} ;

/* mtDeclarationPieces defined in forwardTypes.h */

/*@constant null mtDeclarationPieces mtDeclarationPieces_undefined; @*/
# define mtDeclarationPieces_undefined NULL

extern /*@falsewhennull@*/ bool mtDeclarationPieces_isDefined(mtDeclarationPieces) /*@*/ ;
# define mtDeclarationPieces_isDefined(p_h) ((p_h) != mtDeclarationPieces_undefined)

extern /*@nullwhentrue@*/ bool mtDeclarationPieces_isUndefined(mtDeclarationPieces) /*@*/ ;
# define mtDeclarationPieces_isUndefined(p_h) ((p_h) == mtDeclarationPieces_undefined)

extern mtDeclarationPieces mtDeclarationPieces_create (void) /*@*/ ;

extern /*@only@*/ mtDeclarationPieces 
mtDeclarationPieces_append (/*@only@*/ mtDeclarationPieces p_node,
			    /*@only@*/ mtDeclarationPiece)
     /*@modifies p_node*/ ;

extern /*@exposed@*/ mtDeclarationPiece
mtDeclarationPieces_findPiece (mtDeclarationPieces, mtPieceKind) /*@*/ ;

extern cstring mtDeclarationPieces_unparse (mtDeclarationPieces p_node) /*@*/ ;
extern void mtDeclarationPieces_free (/*@only@*/ mtDeclarationPieces p_node) ;

# else
# error "Multiple include"
# endif
