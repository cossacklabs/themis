/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/

# ifndef NAMECHECKS_H
# define NAMECHECKS_H

extern void checkCppName (uentry p_ue) /*@modifies g_warningstream, p_ue@*/ ;
extern void checkExternalName (uentry p_ue) /*@modifies g_warningstream, p_ue@*/ ;
extern void checkLocalName (uentry p_ue) /*@modifies g_warningstream, p_ue@*/ ;
extern void checkFileScopeName (uentry p_ue) /*@modifies g_warningstream, p_ue@*/ ;
extern void checkPrefix (uentry p_ue) /*@modifies g_warningstream, p_ue@*/ ;
extern void checkAnsiName (uentry p_ue) /*@modifies g_warningstream, p_ue@*/ ;
extern void checkParamNames (uentry p_ue) /*@modifies g_warningstream@*/;

# else
# error "Multiple include"
# endif
