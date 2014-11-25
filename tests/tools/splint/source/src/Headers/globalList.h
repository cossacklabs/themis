/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
typedef varDeclarationNodeList globalList ; 

extern /*@unused@*/ cstring globalList_unparse (globalList p_s);
extern void globalList_free (/*@only@*/ globalList p_s);

# define globalList_free(s) (varDeclarationNodeList_free (s))
# define globalList_unparse(s) (varDeclarationNodeList_unparse(s))
