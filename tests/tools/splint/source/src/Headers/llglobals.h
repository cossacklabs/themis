/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
# ifndef LLGLOBALS_H
# define LLGLOBALS_H

extern /*@only@*/ lsymbolSet g_currentImports;
extern /*@only@*/ symtable g_symtab;

# else
# error "Multiple include"
# endif
