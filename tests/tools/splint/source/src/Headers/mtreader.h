/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** mtreader.h
*/

# ifndef MTREADER_H
# define MTREADER_H

extern void mtreader_readFile (/*@only@*/ cstring p_infile) 
     /*@modifies internalState@*/ ;

extern void mtreader_processDeclaration (/*@only@*/ mtDeclarationNode p_decl)
     /*@modifies internalState@*/ ;

extern void mtreader_processGlobalDeclaration (/*@only@*/ mtDeclarationNode p_decl)
     /*@modifies internalState@*/ ;

# else
# error "Multiple include"
# endif
