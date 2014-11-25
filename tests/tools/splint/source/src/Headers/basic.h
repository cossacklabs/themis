/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/

# ifndef BASIC_H
# define BASIC_H

# ifdef HAVE_CONFIG_H
# ifdef WIN32
/* BCC32 Contributed by Scott Frazer */
# ifdef BCC32
# include "../../bcc32/bcc32_config.h"
# else
/* Win32 uses its own hand-generated config file */
# include "../../winconfig.h"
# endif 
# else 
# include "../../config.h"
# endif
# else
# error "No config.h file!"
# endif

# include <stdlib.h>
# include <stdio.h>

# if !defined (WIN32) &&  !(defined (OS2) && defined (__IBMC__))
/* Microsoft VC++ still doesn't support ISO C99... */
# include <stdbool.h>
# endif

#if defined (OS2) && defined (__IBMC__)
extern int snprintf (char *str, size_t count, const char *fmt, ...);
#endif

# include <string.h>
# include <ctype.h>
# include <float.h>
# include <limits.h>
# include "general.h"
# include "ynm.h"
# include "message.h" 
# include "fileloc.h"
# include "globals.h"
# include "cpp.h"
# include "cstringSList.h"
# include "cstringList.h"
# include "flag_codes.h"
# include "flags.h"
# include "flagSpec.h"
# include "qual.h"
# include "lltok.h"
# include "clause.h"
# include "globalsClause.h"
# include "modifiesClause.h"
# include "warnClause.h"
# include "functionClause.h"
# include "functionClauseList.h"
# include "llerror.h" 
# include "fileLib.h"
# include "inputStream.h"
# include "qualList.h"
# include "pointers.h"
# include "code.h"
# include "mapping.h"
# include "sort.h"
# include "lclctypes.h"
# include "paramNode.h"
# include "paramNodeList.h"
# include "lsymbol.h"
# include "abstract.h"
# include "symtable.h"
# include "exprNodeList.h"
# include "cprim.h"
# include "cstringTable.h"
# include "genericTable.h"
# include "filelocList.h"
# include "enumNameList.h"
# include "enumNameSList.h"
# include "varKinds.h"
# include "sRefSet.h"
# include "ekind.h"
# include "usymId.h"
# include "typeId.h"
# include "usymIdSet.h"
# include "sRefList.h"
# include "uentryList.h"
# include "globSet.h"
# include "ctypeList.h"
# include "aliasTable.h"
# include "reader.h"
# include "usymtab.h"
# include "lctype.h"
# include "qtype.h"
# include "idDecl.h"
# include "multiVal.h"
# include "stateClause.h"
# include "stateClauseList.h"
# include "uentry.h"
# include "stateInfo.h"
# include "stateValue.h"
# include "valueTable.h"
# include "sRef.h"
# include "guardSet.h"
# include "constraintTerm.h"
# include "constraintExprData.h"
# include "constraintExpr.h"
# include "constraint.h"
# include "constraintList.h"
# include "exprNode.h"
# include "exprData.h"
# include "typeIdSet.h"
# include "idDeclList.h"
# include "clabstract.h"
# include "sRefSetList.h"
# include "flagMarker.h"
# include "flagMarkerList.h"
# include "macrocache.h"
# include "fileTable.h"
# include "messageLog.h"
# include "clauseStack.h"
# include "stateCombinationTable.h"
# include "mtincludes.h"
# include "functionConstraint.h"
# include "fileIdList.h"
# include "context.h"
# include "constants.h"
# include "llglobals.h"

# else
# error "Multiple include"
# endif

